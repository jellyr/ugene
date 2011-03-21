/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2011 UniPro <ugene@unipro.ru>
 * http://ugene.unipro.ru
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "BioStruct3DGLWidget.h"
#include "BioStruct3DGLRender.h"
#include "BioStruct3DColorScheme.h"
#include "GLFrameManager.h"
#include "ExportImageDialog.h"
#include "SettingsDialog.h"
#include "MolecularSurfaceRenderer.h"

#include "StructuralAlignmentDialog.h"

#include <U2Core/BioStruct3D.h>
#include <U2View/AnnotatedDNAView.h>
#include <U2View/ADVSequenceObjectContext.h>
#include <U2Core/AnnotationSettings.h>
#include <U2Core/AnnotationTableObject.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/BioStruct3DObject.h>
#include <U2Core/GObjectUtils.h>
#include <U2Core/GObjectRelationRoles.h>
#include <U2Core/Log.h>
#include <U2Core/AppContext.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/Counter.h>
#include <U2Algorithm/MolecularSurfaceFactoryRegistry.h>
#include <U2Algorithm/MolecularSurface.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/AnnotationSelection.h>
#include <time.h>

#include <U2Core/AppContext.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/DocumentModel.h>
#include <U2Algorithm/StructuralAlignmentAlgorithm.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmRegistry.h>
#include <U2Algorithm/StructuralAlignmentAlgorithmFactory.h>

#include <QtGui/QMouseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QColorDialog>
#include <QtGui/QFileDialog>
#include <QtGui/QImageWriter>
#include <QtCore/QTime>

#include "gl2ps/gl2ps.h"

// disable "unsafe functions" deprecation warnings on MS VS
#ifdef Q_OS_WIN
#pragma warning(disable : 4996)
#endif

namespace U2 {

int BioStruct3DGLWidget::widgetCount = 0;

static QColor DEFAULT_BACKGROUND_COLOR = Qt::black;
static QColor DEFAULT_SELECTION_COLOR = Qt::yellow;

static QColor DEFAULT_LEFT_EYE_COLOR = QColor(0, 255, 255);
static QColor DEFAULT_RIGHT_EYE_COLOR = QColor(255, 0, 0);

static bool DEFAULT_ANAGLYPH = false;

static int DEFAULT_RENDER_DETAIL_LEVEL = 100;
static int DEFAULT_EYES_SHIFT = 160;
static int DEFAULT_SHADING_LEVEL = 50;

const QString BioStruct3DGLWidget::BACKGROUND_COLOR_NAME("BackgroundColor");
const QString BioStruct3DGLWidget::PRODUCT_NAME("Unipro Ugene");
const QString BioStruct3DGLWidget::PLUGIN_NAME("BioStruct3D Viewer Plugin");
const QString BioStruct3DGLWidget::COLOR_SCHEME_NAME("ColorScheme");
const QString BioStruct3DGLWidget::RENDERER_NAME("GLRenderer");
// unused
//const QString BioStruct3DGLWidget::OBJECT_ID_NAME("OBJECT_ID");

const QString BioStruct3DGLWidget::SELECTION_COLOR_NAME("SelectionColor");

const QString BioStruct3DGLWidget::LEFT_EYE_COLOR_NAME("LeftEyeColor");
const QString BioStruct3DGLWidget::RIGHT_EYE_COLOR_NAME("RightEyeColor");

const QString BioStruct3DGLWidget::RENDER_DETAIL_LEVEL_NAME("RenderDetailLevel");
const QString BioStruct3DGLWidget::SHADING_LEVEL_NAME("Shading Unselected Regions Level");
const QString BioStruct3DGLWidget::ANAGLYPH_NAME("Anaglyph");
const QString BioStruct3DGLWidget::EYES_SHIFT_NAME("EyesShift");

BioStruct3DGLWidget::BioStruct3DGLWidget(BioStruct3DObject* obj, const AnnotatedDNAView* view, GLFrameManager* manager, QWidget *parent /* = 0*/)
    : QGLWidget(parent),
    dnaView(view),
    contexts(),
    frameManager(manager), molSurface(NULL),
    spinAngle(0), displayMenu(NULL)
{
    GCOUNTER( cvar, tvar, "BioStruct3DGLWidget" );

    QString currentModelID = obj->getBioStruct3D().pdbId;
    setObjectName(currentModelID + "-" + QString("%1").arg(++widgetCount));
    //TODO: ? setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

    connectExternalSignals();

    backgroundColor = DEFAULT_BACKGROUND_COLOR;
    selectionColor = DEFAULT_SELECTION_COLOR;
    eyesShift = DEFAULT_EYES_SHIFT;

    anaglyphRenderTextureLeft = 0;
    anaglyphRenderTextureRight = 0;
    tempAnaglyphRenderTexture = 0;

    anaglyph = DEFAULT_ANAGLYPH;
    anaglyphAvailable = true;
    firstResize = true;

    leftEyeColor = DEFAULT_LEFT_EYE_COLOR;
    rightEyeColor = DEFAULT_RIGHT_EYE_COLOR;

    renderDetailLevel = DEFAULT_RENDER_DETAIL_LEVEL;
    unselectedShadingLevel = DEFAULT_SHADING_LEVEL;

    emptyTextureData = NULL;

    currentColorSchemeName = BioStruct3DColorSchemeRegistry::defaultFactoryName();
    currentGLRendererName = BioStruct3DGLRendererRegistry::defaultFactoryName();

    addBiostruct(obj);

    createActions();
    createMenus();

    createSelectModelsActions();
    createSelectModelsMenu();

    loadColorSchemes();
    loadGLRenderers();

    /*setupRenderer(currentGLRendererName);
    setupColorScheme(currentColorSchemeName);*/

    // Set view settings
    // shoud be separate function
    float scaleFactor = 2.5;
    float maxDistFromCenter = contexts.first().biostruct->getMaxDistFromCenter();
    float camZ = scaleFactor * maxDistFromCenter;

    cameraClipNear = (camZ - maxDistFromCenter) * 0.66f;
    cameraClipFar = (camZ + maxDistFromCenter) * 1.2f;

    glFrame.reset( new GLFrame(this, cameraClipNear, cameraClipFar, camZ) );
    frameManager->addGLFrame(glFrame.get());
    saveDefaultSettings();

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::BIOSTRUCTURE_3D).icon);
}

BioStruct3DGLWidget::~BioStruct3DGLWidget() {
    uiLog.trace("Biostruct3DGLWdiget "+objectName()+" deleted");
}

void BioStruct3DGLWidget::initializeGL()
{
    setLightPosition(Vector3D(0, 0.0, 1.0));
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0 };
    GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0 };
    GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0 };
    GLfloat mat_shininess[] = { 90.0 };

    qglClearColor(backgroundColor);
    glShadeModel (GL_SMOOTH);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPostion);
    glEnable(GL_BLEND);                                         // Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void BioStruct3DGLWidget::resizeGL(int width, int height)
{
    glDeleteTextures(1, &anaglyphRenderTextureLeft);
    glDeleteTextures(1, &anaglyphRenderTextureRight);
    glDeleteTextures(1, &tempAnaglyphRenderTexture);

    anaglyphRenderTextureLeft = getEmptyTexture(width, height);
    anaglyphRenderTextureRight = getEmptyTexture(width, height);
    tempAnaglyphRenderTexture =  getEmptyTexture(width, height);

    hasGlErrors();

    if (firstResize && (width>0) && (height>0)) {
        // test anaglyph mode
        anaglyph = true;
        draw(); // draw() checks for openGL errors and sets 'anaglyphAvailable' flag

        // return anaglyph mode to the default value
        anaglyph = DEFAULT_ANAGLYPH;

        firstResize = false;
    }
    glFrame->updateViewPort(width, height);
}

void BioStruct3DGLWidget::paintGL()
{
    if (!isVisible())
        return;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}

float BioStruct3DGLWidget::getEyesShiftMult() const
{
    return eyesShift/100.0f;
}

float BioStruct3DGLWidget::getRenderDetailLevel() const
{
    return renderDetailLevel/100.0f;
}

void BioStruct3DGLWidget::ViewOrtho()                                               // Set Up An Ortho View
{
    glMatrixMode(GL_PROJECTION);                                // Select Projection
    glPushMatrix();                                             // Push The Matrix
    glLoadIdentity();                                           // Reset The Matrix
    glOrtho( 0, width() , height(), 0, -1, 1 );                         // Select Ortho Mode (640x480)
    glMatrixMode(GL_MODELVIEW);                                 // Select Modelview Matrix
    glPushMatrix();                                             // Push The Matrix
    glLoadIdentity();                                           // Reset The Matrix
}

void BioStruct3DGLWidget::ViewPerspective()                                         // Set Up A Perspective View
{
    glMatrixMode( GL_PROJECTION );                              // Select Projection
    glPopMatrix();                                              // Pop The Matrix
    glMatrixMode( GL_MODELVIEW );                               // Select Modelview
    glPopMatrix();                                              // Pop The Matrix
}

GLuint BioStruct3DGLWidget::getEmptyTexture(int textureWidth, int textureHeight)    // Create An Empty Texture
{
    GLuint txtnumber;                                                    // Texture ID

    glGenTextures(1, &txtnumber);                                        // Create 1 Texture
    glBindTexture(GL_TEXTURE_2D, txtnumber);                            // Bind The Texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, textureWidth, textureHeight, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, emptyTextureData);                       // Build Texture Using Information In data
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

    return txtnumber;                                                    // Return The Texture ID
}

void BioStruct3DGLWidget::drawTexturesAnaglyph(GLuint anaglyphRenderTextureLeft, GLuint anaglyphRenderTextureRight)                                // Draw The Image
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers
    drawTexture(anaglyphRenderTextureLeft, rightEyeColor.red(), rightEyeColor.green(), rightEyeColor.blue(), 0.5f, false); // colored left image
    if (hasGlErrors()) { return; }

    glBindTexture(GL_TEXTURE_2D, tempAnaglyphRenderTexture);                    // Bind To The Blur Texture
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width(), height(), 0);// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
    if (hasGlErrors()) { return; }

    drawTexture(anaglyphRenderTextureRight, leftEyeColor.red(), leftEyeColor.green(), leftEyeColor.blue(), 0.5f, false); // colored right image
    drawTexture(tempAnaglyphRenderTexture, 255, 255, 255, 1.0f, true);
}

void BioStruct3DGLWidget::drawTexture(GLuint anaglyphRenderTexture, int red, int green, int blue, float alpha, bool alphaOnly)                              // Draw The Blurred Image
{
    glEnable(GL_TEXTURE_2D);                                    // Enable 2D Texture Mapping

    GLboolean blendEnabled = glIsEnabled(GL_BLEND);
    if (blendEnabled) {
        if (alphaOnly){
            glBlendFunc(GL_DST_ALPHA, GL_DST_ALPHA); // the second image is this, it has needed alpha color
        }
        else {
            glBlendFunc(GL_ONE, GL_ONE);
        }
    }
    if (hasGlErrors()) { return; }

    glBindTexture(GL_TEXTURE_2D, anaglyphRenderTexture);                    // Bind To The Blur Texture
    if (hasGlErrors()) { return; }
    ViewOrtho();                                                // Switch To An Ortho View
    if (hasGlErrors()) { return; }

    glColor4f((float)red/255, (float)green/255, (float)blue/255, alpha);                    // Set The Alpha Value (Starts At 0.2)
    glBegin(GL_QUADS);                                          // Begin Drawing Quads
        glTexCoord2f(0, 1);                     // Texture Coordinate   ( 0, 1 )
        glVertex2f(0,0);                                    // First Vertex     (   0,   0 )

        glTexCoord2f(0, 0);                     // Texture Coordinate   ( 0, 0 )
        glVertex2f(0, (float)height());                                 // Second Vertex    (   0, 480 )

        glTexCoord2f(1, 0);                     // Texture Coordinate   ( 1, 0 )
        glVertex2f((float)width(), (float)height());                                // Third Vertex     ( 640, 480 )

        glTexCoord2f(1, 1);                     // Texture Coordinate   ( 1, 1 )
        glVertex2f((float)width(), 0);                                  // Fourth Vertex    ( 640,   0 )
    glEnd();                                                    // Done Drawing Quads
    if (hasGlErrors()) { return; }

    ViewPerspective();                                          // Switch To A Perspective View
    if (hasGlErrors()) { return; }

    glDisable(GL_TEXTURE_2D);                                   // Disable 2D Texture Mapping
    glBindTexture(GL_TEXTURE_2D,0);                             // Unbind The Blur Texture
    if (hasGlErrors()) { return; }
}

static Vector3D calcRotationCenter(const QList<BioStruct3DRendererContext> &contexts) {
    Vector3D c;
    foreach (const BioStruct3DRendererContext &ctx, contexts) {
        c += ctx.biostruct->getCenter();
    }
    return c/contexts.length();
}

void BioStruct3DGLWidget::drawAll()
{
    glEnable(GL_DEPTH_TEST);                                    // Enable Depth Testing

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Vector3D rotCenter = calcRotationCenter(contexts);

    glTranslatef(glFrame->getCameraPosition().x, glFrame->getCameraPosition().y, 0);

    glMultMatrixf(glFrame->getRotationMatrix() );
    glTranslatef(-rotCenter.x ,-rotCenter.y, -rotCenter.z);
    clock_t t1 =  clock();

    foreach (const BioStruct3DRendererContext &ctx, contexts) {
        ctx.renderer->drawBioStruct3D();
    }

    if(NULL != molSurface.get())
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        surfaceRenderer->drawSurface(*molSurface);
        glCullFace(GL_BACK);
        surfaceRenderer->drawSurface(*molSurface);
        glDisable(GL_CULL_FACE);
    }

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_DEPTH_TEST);                                   // Disable Depth Testing

    // bug-2858: This profiling wrong when anaglyph enabled
    // This profiling doesn't consider creation time of
    // BioStruct3DGLRenderer and BioStruct3DColorScheme which may be crucial
    clock_t t2 = clock();
    perfLog.trace("BioStruct3D structure rendering time " + QString::number((float)( t2 - t1)/ CLOCKS_PER_SEC)+ " s");
}

bool BioStruct3DGLWidget::hasGlErrors() {

    GLenum lastGlError = glGetError();

    bool hasAnyGlError = (lastGlError != GL_NO_ERROR);
    if (anaglyphAvailable) {
        anaglyphAvailable = !hasAnyGlError; // don't change anaglyph mode availability if some error happened at least once
    }

    if (hasAnyGlError) {
        uiLog.trace(
            QString("OpenGL error: ") + QString((char*)gluErrorString(lastGlError)) +
            QString(", returned error code ") + QString::number(lastGlError) +
            QString(", GL_NO_ERROR = ") + QString::number(GL_NO_ERROR)
        );

        anaglyph = false;
    }

    return hasAnyGlError;
}

void BioStruct3DGLWidget::draw() {

    float eyesShift = 5*getEyesShiftMult()*glFrame->getCameraPosition().z/200;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);      //clear color and depth buffers

    if (!anaglyph) {
        gluLookAt(0.0, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        drawAll();
        hasGlErrors();
    }
    else {

        gluLookAt(eyesShift, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        drawAll();
        if (hasGlErrors()) { return; }

        glBindTexture(GL_TEXTURE_2D, anaglyphRenderTextureRight);                   // Bind To The Anaglyph Texture
        if (hasGlErrors()) { return; }
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width(), height(), 0);// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
        if (hasGlErrors()) { return; }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (hasGlErrors()) { return; }
        glLoadIdentity();
        if (hasGlErrors()) { return; }

        gluLookAt(-eyesShift, 0.0, glFrame->getCameraPosition().z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        if (hasGlErrors()) { return; }
        drawAll();

        if (hasGlErrors()) { return; }

        glBindTexture(GL_TEXTURE_2D, anaglyphRenderTextureLeft);                    // Bind To The Anaglyph Texture
        if (hasGlErrors()) { return; }
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width(), height(), 0);// Copy Our ViewPort To The Blur Texture (From 0,0 To 128,128... No Border)
        if (hasGlErrors()) { return; }

        drawTexturesAnaglyph(anaglyphRenderTextureLeft, anaglyphRenderTextureRight);
    }
}

Vector3D BioStruct3DGLWidget::getTrackballMapping(int x, int y)
{
    Vector3D pos;
    /* project x,y onto a hemisphere centered within width, height */
    pos.x = (2.0f*x - width()) / width();
    pos.y = (height() - 2.0f*y) / height();
    pos.z = 0;
    float d = pos.length();
    d = (d < 1.0) ? d : 1.0;
    pos.z = sqrtf(1.0f - d*d);
    pos.normalize();

    return pos;
} 

void BioStruct3DGLWidget::contextMenuEvent(QContextMenuEvent *event) {
    QMenu menu;
    foreach(QAction* action, getDisplayMenu()->actions()) {
        menu.addAction(action);
    }

    menu.addAction(closeAction);

    menu.exec(event->globalPos());
}



void BioStruct3DGLWidget::setLightPosition( const Vector3D& pos )
{
    lightPostion[0] = pos.x;
    lightPostion[1] = pos.y;
    lightPostion[2] = pos.z;
    lightPostion[3] = 1.0;
}

const QList<ADVSequenceObjectContext*> BioStruct3DGLWidget::getSequenceContexts() const
{
    return dnaView->getSequenceContexts();
}

void BioStruct3DGLWidget::sl_onSequenceSelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed)
{
    if (!isVisible())
        return;

    DNASequenceSelection* selection = qobject_cast<DNASequenceSelection*>(s);
    const DNASequenceObject* seqObj = selection->getSequenceObject();
    
    const BioStruct3D &biostruct = *contexts.first().biostruct;

    // bug-2857: GObject relations shoud be used
    if (!seqObj->getGObjectName().startsWith(biostruct.pdbId)) {
        return;
    }
    
    if (seqObj) {
        // TODO: this is correct only for first biostruct
        int chainId = getSequenceChainId(seqObj);
        assert(biostruct.moleculeMap.contains(chainId));

        contexts.first().colorScheme->updateSelectionRegion(chainId, added, removed);
        updateAllColorSchemes();
        update();
    }
}

int BioStruct3DGLWidget::getChainIdForAnnotationObject(AnnotationTableObject* ao) 
{
    if ( this->chainIdCache.contains(ao) ) {
        return chainIdCache.value(ao);
    } else {
        int chainID = -1;
        QList<GObjectRelation> relations = ao->findRelatedObjectsByRole(GObjectRelationRole::SEQUENCE);
        assert(relations.count() == 1);
        GObjectRelation relation = relations.first();
        Document *biostrucDoc = contexts.first().obj->getDocument();
        GObject* obj = biostrucDoc->findGObjectByName(relation.ref.objName);
        DNASequenceObject* seqObj = qobject_cast<DNASequenceObject*>(obj);
        if (seqObj) {
            chainID = getSequenceChainId(seqObj);
            chainIdCache.insert(ao, chainID);
        } 
        return chainID;
    }
}

QVariantMap BioStruct3DGLWidget::getState()
{
    QVariantMap state;
    glFrame->writeStateToMap(state);

    state[COLOR_SCHEME_NAME] = QVariant::fromValue(currentColorSchemeName);
    state[RENDERER_NAME] = QVariant::fromValue(currentGLRendererName);
    // unused
    //state[OBJECT_ID_NAME] = QVariant::fromValue(getBioStruct3DObjectName());

    state[BACKGROUND_COLOR_NAME] = QVariant::fromValue(backgroundColor);
    state[SELECTION_COLOR_NAME] = QVariant::fromValue(selectionColor);

    state[LEFT_EYE_COLOR_NAME] = QVariant::fromValue(leftEyeColor);
    state[RIGHT_EYE_COLOR_NAME] = QVariant::fromValue(rightEyeColor);

    state[RENDER_DETAIL_LEVEL_NAME] = QVariant::fromValue(renderDetailLevel);
    state[SHADING_LEVEL_NAME] = QVariant::fromValue(unselectedShadingLevel);
    state[ANAGLYPH_NAME] = QVariant::fromValue(anaglyph);
    state[EYES_SHIFT_NAME] = QVariant::fromValue(eyesShift);

    return state;
}

void BioStruct3DGLWidget::setState( const QVariantMap& state )
{
    // bug-2859: correct save/restore current selection.
    if (state.isEmpty()) {
        return;
    }
    glFrame->setState(state);

    backgroundColor = state.value(BACKGROUND_COLOR_NAME, DEFAULT_BACKGROUND_COLOR).value<QColor>();
    setBackgroundColor(backgroundColor);

    selectionColor = state.value(SELECTION_COLOR_NAME, DEFAULT_SELECTION_COLOR).value<QColor>();

    leftEyeColor = state.value(LEFT_EYE_COLOR_NAME, DEFAULT_LEFT_EYE_COLOR).value<QColor>();
    rightEyeColor = state.value(RIGHT_EYE_COLOR_NAME, DEFAULT_RIGHT_EYE_COLOR).value<QColor>();

    renderDetailLevel = state.value(RENDER_DETAIL_LEVEL_NAME, DEFAULT_RENDER_DETAIL_LEVEL).value<int>();
    anaglyph = state.value(ANAGLYPH_NAME, DEFAULT_ANAGLYPH).value<bool>();
    eyesShift = state.value(EYES_SHIFT_NAME, DEFAULT_EYES_SHIFT).value<int>();

    QString previousColorSchemeName = currentColorSchemeName;
    QString previousGLRendererName = currentGLRendererName;

    currentColorSchemeName = state.value(COLOR_SCHEME_NAME, BioStruct3DColorSchemeRegistry::defaultFactoryName()).value<QString>();
    currentGLRendererName = state.value(RENDERER_NAME, BioStruct3DGLRendererRegistry::defaultFactoryName()).value<QString>();

    if (previousColorSchemeName != currentColorSchemeName){
        setupColorScheme(currentColorSchemeName);
    }

    unselectedShadingLevel = state.value(SHADING_LEVEL_NAME, DEFAULT_SHADING_LEVEL).value<int>();
    setUnselectedShadingLevel(unselectedShadingLevel);

    if (previousGLRendererName != currentGLRendererName) {
        setupRenderer(currentGLRendererName);
    }

    resizeGL(width(), height());
    updateGL();
}

void BioStruct3DGLWidget::setupColorScheme(const QString &name) {
    QList<BioStruct3DRendererContext>::iterator i = contexts.begin();
    for (; i != contexts.end(); ++i) {
        BioStruct3DRendererContext &ctx = *(i);

        // TODO: this situation may be potentialy dangerous
        // if renderer starts draw right now, maybe SharedPointer will be good solution
        BioStruct3DColorScheme *scheme = BioStruct3DColorSchemeRegistry::createColorScheme(name, ctx.obj);
        assert(scheme);

        scheme->setSelectionColor(selectionColor);
        scheme->setUnselectedShadingLevel((double)unselectedShadingLevel/100.0);

        ctx.colorScheme = QSharedPointer<BioStruct3DColorScheme>(scheme);
        ctx.renderer->setColorScheme(scheme);
    }
}

void BioStruct3DGLWidget::setupRenderer(const QString &name) {
    QList<BioStruct3DRendererContext>::iterator i = contexts.begin();
    for (; i != contexts.end(); ++i) {
        BioStruct3DRendererContext &ctx = *(i);

        // TODO: this situation may be potentialy dangerous
        // if renderer starts draw right now, maybe SharedPointer will be good solution
        BioStruct3DGLRenderer *rend = BioStruct3DGLRendererRegistry::createRenderer(name, *ctx.biostruct, ctx.colorScheme.data(), ctx.shownModelsIndexes, this);
        assert(rend);
        ctx.renderer = QSharedPointer<BioStruct3DGLRenderer>(rend);
    }
}

void BioStruct3DGLWidget::updateAllColorSchemes() {
    foreach (const BioStruct3DRendererContext &ctx, contexts) {
        ctx.renderer->updateColorScheme();
    }
}

void BioStruct3DGLWidget::setBackgroundColor(QColor backgroundColor)
{
    this->backgroundColor=backgroundColor;
    qglClearColor(backgroundColor);
}

void BioStruct3DGLWidget::zoom( float delta )
{
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.get(), syncLock);
    foreach( GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->performZoom(delta);
        frame->updateViewPort();
        frame->updateGL();
    } 
}

void BioStruct3DGLWidget::shift( float deltaX, float deltaY)
{
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.get(), syncLock);
    foreach( GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->performShift(deltaX, deltaY);
        frame->updateViewPort();
        frame->updateGL();
    } 
}

void BioStruct3DGLWidget::saveDefaultSettings()
{
    glFrame->writeStateToMap(defaultsSettings);
    defaultsSettings[COLOR_SCHEME_NAME] = QVariant::fromValue(currentColorSchemeName);
    defaultsSettings[RENDERER_NAME] = QVariant::fromValue(currentGLRendererName);
}

void BioStruct3DGLWidget::restoreDefaultSettigns()
{   
    assert(!defaultsSettings.isEmpty());
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.get(), syncLock);
    foreach( GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->setState(defaultsSettings);
        frame->updateViewPort();
        frame->updateGL();
    } 
}

void BioStruct3DGLWidget::showModel(int modelId, bool show) {
    BioStruct3DRendererContext &ctx = contexts.first();

    // this function uses modelId - key from BioStruct3D::modelMap
    int idx = ctx.biostruct->modelMap.keys().indexOf(modelId);
    assert(idx != -1);

    if (show && !ctx.shownModelsIndexes.contains(idx)) {
        ctx.shownModelsIndexes.append(idx);
    }
    else if (!show) {
        ctx.shownModelsIndexes.removeAll(idx);
    }
    ctx.renderer->getShownModelsIndexes() = ctx.shownModelsIndexes;
}

void BioStruct3DGLWidget::showAllModels(bool show) {
    BioStruct3DRendererContext &ctx = contexts.first();

    ctx.shownModelsIndexes.clear();
    if (show) {
        int numModels = ctx.biostruct->modelMap.size();
        for (int i = 0; i < numModels; ++i) {
            ctx.shownModelsIndexes.append(i);            
        }
    }
    ctx.renderer->getShownModelsIndexes() = ctx.shownModelsIndexes;
}

void BioStruct3DGLWidget::sl_selectModel(QAction *action) {
    int modelId = action->text().toInt();
    bool show = action->isChecked();

    // in exclusive mode exact one model shown at the same time
    if (selectModelActions->isExclusive()) {
        showAllModels(false);
        showModel(modelId, true);
    }
    else {
        showModel(modelId, show);
    }

    contexts.first().renderer->updateShownModels();
    updateGL();
}

void BioStruct3DGLWidget::sl_selectAllModels() {
    selectModelsExclusiveAction->setChecked(false);
    selectModelActions->setExclusive(false);

    foreach (QAction *action, selectModelActions->actions()) {
        action->setChecked(true);
    }

    showAllModels(true);
    contexts.first().renderer->updateShownModels();
    updateGL();
}

void BioStruct3DGLWidget::sl_selectModelsExclusive() {
    if (selectModelsExclusiveAction->isChecked()) {
        QAction *first = 0;
        foreach (QAction *action, selectModelActions->actions()) {
            if (!first && action->isChecked()) {
                first = action;
            }
            action->setChecked(false);
        }

        if (!first) {
            first = selectModelActions->actions().first();
        }

        int firstId = first->text().toInt();
        showAllModels(false);
        showModel(firstId, true);

        selectModelActions->setExclusive(true);
        first->setChecked(true);
    }
    else {
        selectModelActions->setExclusive(false);
    }

    contexts.first().renderer->updateShownModels();
    updateGL();
}

void BioStruct3DGLWidget::writeImage2DToFile( int format, int options, int nbcol, const char *fileName )
{
    FILE *fp = NULL;
    const char* FOPEN_ARGS = "wb";
    const QByteArray title(fileName);
    int state = GL2PS_OVERFLOW, buffsize = 0;
    GLint viewport[4];
    int sort = GL2PS_SIMPLE_SORT;
        
    fp = fopen(fileName, FOPEN_ARGS);

    if(!fp){
        QMessageBox::warning(this, tr("Error"),tr("Unable to open file %1 for writing").arg(fileName));
        return;
    }
    
    glGetIntegerv(GL_VIEWPORT,viewport);
    
    if (format == GL2PS_EPS) {
        // hack -> make widget aspect ratio 1:1
        if (width() > height()) {
            int size = height();
            resize(size, size);
        }
    }

    while(state == GL2PS_OVERFLOW){
        buffsize += 2048*2048;
        gl2psBeginPage(title.constData(), "Unipro UGENE BioStruct3D Viewer plugin", viewport, format, sort, options,
            GL_RGBA, 0, NULL, nbcol, nbcol, nbcol, buffsize, fp, fileName);
        paintGL();
        state = gl2psEndPage();
    }

    fclose(fp);
    
    if (format == GL2PS_EPS) {
        // restore sizes 
        updateGeometry();
    }
  
}

int BioStruct3DGLWidget::getSequenceChainId(const DNASequenceObject* seqObj)
{
    const QVariantMap &info = seqObj->getDNASequence().info;
    int id = info.value(DNAInfo::CHAIN_ID, qVariantFromValue(-1)).toInt();

    return id;
}

void BioStruct3DGLWidget::loadColorSchemes()
{
    currentColorSchemeName = BioStruct3DColorSchemeRegistry::defaultFactoryName();

    // highlight default color scheme in menu
    QList<QAction*>::iterator iter;
    QList<QAction*> schemeActions = colorSchemeActions->actions();
    for (iter = schemeActions.begin(); iter != schemeActions.end(); ++iter) {
        if ((*iter)->text() == currentColorSchemeName) {
            (*iter)->setChecked(true);
            break;
        }
    }
    assert(iter != schemeActions.end());    
}

void BioStruct3DGLWidget::loadGLRenderers()
{
    currentGLRendererName =  BioStruct3DGLRendererRegistry::defaultFactoryName();

    // highlight default renderer in menu
    QList<QAction*>::iterator iter;
    QList<QAction*> renderActions = rendererActions->actions();
    for (iter = renderActions.begin(); iter != renderActions.end(); ++iter) {
        if ((*iter)->text() == currentGLRendererName) {
            (*iter)->setChecked(true);
            break;
        }
    }
    assert(iter != renderActions.end());

    QString surfaceRendererName = ConvexMapRenderer::ID;
    surfaceRenderer.reset(MolecularSurfaceRendererRegistry::createMSRenderer(surfaceRendererName));
}

bool BioStruct3DGLWidget::isSyncModeOn()
{
    Qt::KeyboardModifiers km = QApplication::keyboardModifiers();
    bool synchronizationMode = km.testFlag(Qt::ShiftModifier) || frameManager->getSyncLock();
    synchronizationMode &=  frameManager->getGLFrames().count() > 1 ;
    return synchronizationMode;
}

void BioStruct3DGLWidget::setUnselectedShadingLevel(int shading) {
    foreach (const BioStruct3DRendererContext &ctx, contexts) {
        ctx.colorScheme->setUnselectedShadingLevel((double)shading/100.0);
    }
    updateAllColorSchemes();
}

QMenu* BioStruct3DGLWidget::getDisplayMenu()
{
    assert(displayMenu != NULL);
    return displayMenu;
}

void BioStruct3DGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = getTrackballMapping(event->x(), event->y());
}

void BioStruct3DGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    Vector3D rotCenter = contexts.first().biostruct->getCenter();

    if (event->buttons() & Qt::LeftButton) {
        Vector3D curPos = getTrackballMapping(event->x(), event->y());
        Vector3D delta = curPos - lastPos;

        if (delta.x || delta.y || delta.z) {
            rotAngle = 90.0f*delta.length();
            rotAxis =  vector_cross(lastPos,curPos);

            bool syncLock = isSyncModeOn();
            QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.get(), syncLock);
            foreach( GLFrame* frame, frames) {
                frame->makeCurrent();

                if (event->modifiers() & Qt::CTRL)
                    frame->performShift(delta.x, delta.y);
                else
                    frame->rotateCamera(rotAxis, rotAngle);

                frame->updateGL();
            }
        }

        lastPos = curPos;
    }
}

void BioStruct3DGLWidget::wheelEvent ( QWheelEvent * event )
{
    float numDegrees =  event->delta() / 8;
    zoom(numDegrees / 10);
}

void BioStruct3DGLWidget::createSelectModelsActions() {
    selectModelActions = 0;
    const BioStruct3DRendererContext &ctx = contexts.first();
    const BioStruct3D &biostruct = *ctx.biostruct;

    if (biostruct.modelMap.size() > 1) {
        selectModelActions = new QActionGroup(this);
        connect(selectModelActions, SIGNAL(triggered(QAction *)), this, SLOT(sl_selectModel(QAction *)));

        foreach (int modelId, biostruct.modelMap.keys()) {
            QAction *action = new QAction(QString::number(modelId), selectModelActions);
            action->setCheckable(true);

            if (ctx.shownModelsIndexes.contains(biostruct.modelMap.keys().indexOf(modelId))) {
                action->setChecked(true);
            }
        }        
        selectModelsExclusiveAction = new QAction(tr("Exclusive"), this);
        selectModelsExclusiveAction->setCheckable(true);
        selectModelsExclusiveAction->setChecked(true);
        selectModelActions->setExclusive(true);
        connect(selectModelsExclusiveAction, SIGNAL(triggered()), this, SLOT(sl_selectModelsExclusive()));

        selectAllModelsAction = new QAction(tr("Select all"), this);
        connect(selectAllModelsAction, SIGNAL(triggered()), this, SLOT(sl_selectAllModels()));
    }
}

void BioStruct3DGLWidget::createActions()
{
    QAction* action = NULL;

    animationTimer = new QTimer(this);
    animationTimer->setInterval(20); // fixed interval
    connect(animationTimer, SIGNAL(timeout()), this, SLOT(sl_updateAnnimation()));

    rendererActions = new QActionGroup(this);
    connect(rendererActions, SIGNAL(triggered(QAction *)), this, SLOT(sl_selectGLRenderer(QAction *)));

    foreach(const QString &key, BioStruct3DGLRendererRegistry::factoriesNames()) {
        action  = new QAction(key, rendererActions);
        action->setCheckable(true);
    }

    colorSchemeActions = new QActionGroup(this);
    connect(colorSchemeActions, SIGNAL(triggered(QAction *)), this, SLOT(sl_selectColorScheme(QAction *)));
    foreach(const QString &key, BioStruct3DColorSchemeRegistry::factoriesNames()) {
        action = new QAction(key, colorSchemeActions);
        action->setCheckable(true);
    }

    molSurfaceRenderActions = new QActionGroup(this);
    connect(molSurfaceRenderActions, SIGNAL(triggered(QAction *)), this, SLOT(sl_selectSurfaceRenderer(QAction*)));
    foreach(const QString &key, MolecularSurfaceRendererRegistry::factoriesNames()) {
        action = new QAction(key, molSurfaceRenderActions);
        action->setCheckable(true);
        if (key == ConvexMapRenderer::ID) {
            action->setChecked(true);
        }
    }

    molSurfaceTypeActions = new QActionGroup(this);
    foreach(QString key, AppContext::getMolecularSurfaceFactoryRegistry()->getSurfNameList())
    {
        action = new QAction(key, molSurfaceTypeActions);
        connect(action, SIGNAL(triggered()), this, SLOT(sl_showSurface()));
        action->setCheckable(true);
        bool hasConstraints = AppContext::getMolecularSurfaceFactoryRegistry()->getSurfaceFactory(key)->hasConstraints(*contexts.first().biostruct);
        action->setEnabled(!hasConstraints);

    }
    action = new QAction(tr("Off"), molSurfaceTypeActions);
    connect(action, SIGNAL(triggered()), this, SLOT(sl_hideSurface()));
    action->setCheckable(true);
    action->setChecked(true);

    spinAction = new QAction(tr("Spin"), this);
    spinAction->setCheckable(true);
    connect(spinAction, SIGNAL(triggered()), this, SLOT(sl_acitvateSpin()));

    settingsAction = new QAction(tr("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(sl_settings()));

    closeAction = new QAction(tr("close_action"), this);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(sl_closeWidget()));

    exportImageAction = new QAction(tr("Export image"), this);
    connect(exportImageAction, SIGNAL(triggered()), this, SLOT(sl_exportImage()));

    alignWithAction = new QAction(tr("Align With..."), this);
    connect(alignWithAction, SIGNAL(triggered()), this, SLOT(sl_alignWith()));
    // bug-2855 Disabled for now
    alignWithAction->setVisible(false);

    connect(AppContext::getTaskScheduler(), SIGNAL(si_stateChanged(Task*)), SLOT(sl_onTaskFinished(Task*)));
}

void BioStruct3DGLWidget::createSelectModelsMenu() {
    if (selectModelActions) {
        modelsMenu = new QMenu(tr("Models"));
        modelsMenu->addAction(selectModelsExclusiveAction);
        modelsMenu->addAction(selectAllModelsAction);
        modelsMenu->addActions(selectModelActions->actions());

        displayMenu->addMenu(modelsMenu);
    }
}

void BioStruct3DGLWidget::createMenus()
{
    // Renderer selection
    selectRendererMenu = new QMenu(tr("Render style"));
    selectRendererMenu->addActions(rendererActions->actions());

    // Color scheme selection
    selectColorSchemeMenu = new QMenu(tr("Color Scheme"));
    selectColorSchemeMenu->addActions(colorSchemeActions->actions());

    // Molecular surface
    QMenu* surfaceMenu = new QMenu(tr("Molecular Surface Render Style"));
    surfaceMenu->addActions(molSurfaceRenderActions->actions());

    QMenu *surfaceTypeMenu = new QMenu(tr("Molecular Surface"));
    surfaceTypeMenu->addActions(molSurfaceTypeActions->actions());

    // Display (context) menu
    displayMenu = new QMenu(this);
    displayMenu->addMenu(selectRendererMenu);
    displayMenu->addMenu(selectColorSchemeMenu);

    displayMenu->addMenu(surfaceMenu);
    displayMenu->addMenu(surfaceTypeMenu);
    displayMenu->addAction(spinAction);
    displayMenu->addAction(settingsAction);
    displayMenu->addAction(exportImageAction);
    displayMenu->addAction(alignWithAction);
}

void BioStruct3DGLWidget::connectExternalSignals()
{
    AnnotationSettingsRegistry* asr = AppContext::getAnnotationsSettingsRegistry();
    connect(asr, SIGNAL(si_annotationSettingsChanged(const QStringList& )), this, SLOT(sl_updateRenderSettings(const QStringList& )) );

    const QList<ADVSequenceObjectContext*> seqContexts = getSequenceContexts();
    foreach (ADVSequenceObjectContext* ctx, seqContexts) {
        connect(ctx->getSequenceSelection(),
            SIGNAL(si_selectionChanged(LRegionsSelection*, const QVector<U2Region>&, const QVector<U2Region>&)),
            SLOT(sl_onSequenceSelectionChanged(LRegionsSelection*, const QVector<U2Region>& , const QVector<U2Region>&)));

        connect(dnaView->getAnnotationsSelection(),
            SIGNAL(si_selectionChanged(AnnotationSelection* , const QList<Annotation*>&, const QList<Annotation*>&)),
            SLOT(sl_onAnnotationSelectionChanged(AnnotationSelection* , const QList<Annotation*>&, const QList<Annotation*>&)));
    }
}

void BioStruct3DGLWidget::sl_selectColorScheme(QAction* action) {
    QString schemeName = action->text();

    currentColorSchemeName = schemeName;
    setupColorScheme(schemeName);

    GLFrame* frame = frameManager->getGLWidgetFrame(this);
    frame->makeCurrent();
    frame->updateGL();
}

void BioStruct3DGLWidget::sl_updateRenderSettings(const QStringList& list )
{
    Q_UNUSED(list);
    sl_selectColorScheme(colorSchemeActions->checkedAction());
}

void BioStruct3DGLWidget::sl_onAnnotationSelectionChanged( AnnotationSelection* as, const QList<Annotation*>& added, const QList<Annotation*>& removed )
{
    Q_UNUSED(as);
    if (!isVisible())
        return;

    QVector<U2Region> empty;
    QList<GObject*> seqObjects = dnaView->getSequenceGObjectsWithContexts();

    const BioStruct3D &biostruct = *contexts.first().biostruct;
    BioStruct3DColorScheme *scheme = contexts.first().colorScheme.data();

    foreach (Annotation* annotation, added) {
        if (annotation->getLocation()->isEmpty()) { //TODO: there must be no such annotations!
            continue;
        }

        AnnotationTableObject* ao = annotation->getGObject();
        int chainId  = getChainIdForAnnotationObject(ao);
        if (chainId != -1) {
            assert(biostruct.moleculeMap.contains(chainId));
            scheme->updateSelectionRegion(chainId, annotation->getRegions(), empty);
        }
    }

    foreach (Annotation* annotation, removed) {
        if (annotation->getLocation()->isEmpty() ) { //TODO: there must be no such annotations!
            continue;
        }
        AnnotationTableObject* ao = annotation->getGObject();
        int chainId = getChainIdForAnnotationObject(ao);
        if (chainId != -1) {
            assert(biostruct.moleculeMap.contains(chainId));
            scheme->updateSelectionRegion(chainId, empty, annotation->getRegions());
        }
    }

    updateAllColorSchemes();
    update();
}

void BioStruct3DGLWidget::sl_acitvateSpin()
{
    if (spinAction->isChecked()) {
        animationTimer->start();
    } else {
        animationTimer->stop();
    }

    updateGL();

}

void BioStruct3DGLWidget::sl_updateAnnimation()
{
    static float velocity = 0.05f;
    spinAngle = velocity* animationTimer->interval();
    Vector3D rotAxis(0,1,0);
    bool syncLock = isSyncModeOn();
    QList<GLFrame*> frames = frameManager->getActiveGLFrameList(glFrame.get(), syncLock);

    foreach( GLFrame* frame, frames) {
        frame->makeCurrent();
        frame->rotateCamera(rotAxis, spinAngle);
        frame->updateGL();
    }
    updateGL();
}

void BioStruct3DGLWidget::sl_selectGLRenderer(QAction* action)
{
    QString rendererName = action->text();
    currentGLRendererName = rendererName;
    setupRenderer(currentGLRendererName);

    GLFrame* frame = frameManager->getGLWidgetFrame(this);
    frame->makeCurrent();
    frame->updateGL();
}

void BioStruct3DGLWidget::sl_settings()
{
    BioStruct3DSettingsDialog dialog;

    dialog.setWidget(this);

    dialog.setBackgroundColor(backgroundColor);
    dialog.setSelectionColor(selectionColor);
    dialog.setRenderDetailLevel(renderDetailLevel);
    dialog.setShadingLevel(unselectedShadingLevel);

    dialog.setAnaglyphAvailability(anaglyphAvailable);
    dialog.setAnaglyph(anaglyph);
    dialog.setEyesShift(eyesShift);
    dialog.setGlassesColorScheme(leftEyeColor, rightEyeColor);

    QVariantMap previousState = getState();

    if (QDialog::Accepted == dialog.exec())
    {
        backgroundColor=dialog.getBackgroundColor();
        selectionColor = dialog.getSelectionColor();
        unselectedShadingLevel = dialog.getShadingLevel();

        foreach (const BioStruct3DRendererContext &ctx, contexts) {
            ctx.colorScheme->setSelectionColor(selectionColor);
        }
        setUnselectedShadingLevel(unselectedShadingLevel);

        renderDetailLevel=dialog.getRenderDetailLevel();
        anaglyph = dialog.getAnaglyph();

        eyesShift = dialog.getEyesShift();

        leftEyeColor = dialog.getLeftEyeColor();
        rightEyeColor = dialog.getRightEyeColor();

        this->makeCurrent();
        setBackgroundColor(backgroundColor);

        updateGL();
    }
    else
    {
        setState(previousState);
    }

}

void BioStruct3DGLWidget::sl_closeWidget()
{
    hide();
    emit si_widgetClosed(this);
}

void BioStruct3DGLWidget::sl_exportImage()
{
    ExportImageDialog dialog(this);
    dialog.exec();
}

void BioStruct3DGLWidget::sl_showSurface()
{
    QList<SharedAtom> atoms;
    int index = contexts.first().shownModelsIndexes.first();
    foreach (const SharedMolecule &mol, contexts.first().biostruct->moleculeMap) {
        const Molecule3DModel& model = mol->models.at(index);
        atoms += model.atoms;
    }

    QString surfaceType = qobject_cast<QAction *>( sender() )->text();
    surfaceCalcTask = new MolecularSurfaceCalcTask(surfaceType, atoms);
    AppContext::getTaskScheduler()->registerTopLevelTask(surfaceCalcTask);
}

void BioStruct3DGLWidget::sl_hideSurface()
{
    molSurface.reset();

    makeCurrent();
    updateGL();
}

void BioStruct3DGLWidget::sl_selectSurfaceRenderer(QAction*  action)
{
    QString msRendererName = action->text();
    surfaceRenderer.reset(MolecularSurfaceRendererRegistry::createMSRenderer(msRendererName));

    makeCurrent();
    updateGL();
}

void BioStruct3DGLWidget::sl_onTaskFinished( Task* task )
{
    if ( surfaceCalcTask != task || surfaceCalcTask->getState()!= Task::State_Finished) {
        return;
    }

    molSurface.reset();
    molSurface = surfaceCalcTask->getCalculatedSurface();
    
    makeCurrent();
    updateGL();
}

void BioStruct3DGLWidget::addBiostruct(BioStruct3DObject *obj) {
    assert(obj);

    BioStruct3DRendererContext ctx(obj);

    // initially show only first model
    ctx.shownModelsIndexes.append(0);

    BioStruct3DColorScheme *colorScheme = BioStruct3DColorSchemeRegistry::createColorScheme(currentColorSchemeName, ctx.obj);
    assert(colorScheme);
    ctx.colorScheme = QSharedPointer<BioStruct3DColorScheme>(colorScheme);
    ctx.colorScheme->setSelectionColor(selectionColor);
    ctx.colorScheme->setUnselectedShadingLevel((double)unselectedShadingLevel/100.0);

    BioStruct3DGLRenderer *renderer = BioStruct3DGLRendererRegistry::createRenderer(currentGLRendererName, *ctx.biostruct, ctx.colorScheme.data(), ctx.shownModelsIndexes, this);
    assert(renderer);
    ctx.renderer = QSharedPointer<BioStruct3DGLRenderer>(renderer);

    contexts.append(ctx);
}

void BioStruct3DGLWidget::sl_alignWith() {
    // bug-2855 This code is here temporary only for testing,
    // and should be removed bebore release
    QList<BioStruct3DObject*> biostructs;

    Project *proj = AppContext::getProject();
    if (proj) {
        foreach (Document *doc, proj->getDocuments()) {
            assert(doc);
            if (doc->isLoaded()) {
                foreach (GObject *obj, doc->findGObjectByType(GObjectTypes::BIOSTRUCTURE_3D)) {
                    biostructs << qobject_cast<BioStruct3DObject*>(obj);
                }
            }
        }
    }

    StructuralAlignmentAlgorithmRegistry *reg = AppContext::getStructuralAlignmentAlgorithmRegistry();
    if (reg->getFactoriesIds().isEmpty()) {
        QMessageBox::warning(0, "Error", "No available algorithms");
        return;
    }

    StructuralAlignmentAlgorithmFactory *fac = reg->getAlgorithmFactory(reg->getFactoriesIds().first());
    StructuralAlignmentAlgorithm *alg = fac->create();
    assert(alg);

    int refModel = contexts.first().shownModelsIndexes.first();
    StructuralAlignmentDialog dlg(biostructs, contexts.first().obj, refModel);
    if (dlg.exec() == QDialog::Accepted) {
        BioStruct3DObject *refo = static_cast<BioStruct3DObject*>( dlg.reference->itemData(dlg.reference->currentIndex()).value<void*>() );
        BioStruct3DObject *alto = static_cast<BioStruct3DObject*>( dlg.alter->itemData(dlg.alter->currentIndex()).value<void*>() );

        const BioStruct3D &ref = refo->getBioStruct3D();
        const BioStruct3D &alt = alto->getBioStruct3D();

        int altModel = dlg.altModel->itemData(dlg.altModel->currentIndex()).value<int>();

        //StructuralAlignment result = alg->align(ref, alt);
        addBiostruct(alto);
        contexts.last().shownModelsIndexes = QList<int>() << altModel;
        contexts.last().renderer->getShownModelsIndexes() = contexts.last().shownModelsIndexes;
        contexts.last().renderer->updateShownModels();
    }
}
} // namespace U2


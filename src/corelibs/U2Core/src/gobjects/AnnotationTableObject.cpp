/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2015 UniPro <ugene@unipro.ru>
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

#include <QCoreApplication>

#include <U2Core/AnnotationTableObjectConstraints.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/GHints.h>
#include <U2Core/Timer.h>
#include <U2Core/U2DbiUtils.h>
#include <U2Core/U2FeatureUtils.h>
#include <U2Core/U2ObjectDbi.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include "AnnotationTableObject.h"
#include "GObjectTypes.h"

namespace U2 {

AnnotationTableObject::AnnotationTableObject( const QString &objectName, const U2DbiRef &dbiRef,
    const QVariantMap &hintsMap)
    : GObject( GObjectTypes::ANNOTATION_TABLE, objectName, hintsMap )
{
    U2OpStatusImpl os;
    const QString folder = hintsMap.value(DocumentFormat::DBI_FOLDER_HINT, U2ObjectDbi::ROOT_FOLDER).toString();
    U2AnnotationTable table = U2FeatureUtils::createAnnotationTable( objectName, dbiRef, folder, os );
    SAFE_POINT_OP( os, );
    entityRef = U2EntityRef( dbiRef, table.id );
    rootFeatureId = table.rootFeature;
    dataLoaded = true;

    ref();
}

AnnotationTableObject::AnnotationTableObject( const QString &objectName,
    const U2EntityRef &tableRef, const QVariantMap &hintsMap )
    : GObject( GObjectTypes::ANNOTATION_TABLE, objectName, hintsMap )
{
    entityRef = tableRef;
}

AnnotationTableObject::~AnnotationTableObject() {
    if (dataLoaded) {
        deref();
    }
}

QList<Annotation> AnnotationTableObject::getAnnotations() const {
    QList<Annotation> result;

    ensureDataLoaded();

    U2OpStatusImpl os;
    const QList<U2Feature> subfeatures = U2FeatureUtils::getSubAnnotations(rootFeatureId, entityRef.dbiRef, os);
    SAFE_POINT_OP(os, result);

    foreach (const U2Feature &feature, subfeatures) {
        if (!feature.name.isEmpty()) {
            result << Annotation(feature.id, const_cast<AnnotationTableObject *>(this));
        }
    }

    return result;
}

bool AnnotationTableObject::hasAnnotations() const {
    ensureDataLoaded();

    U2OpStatusImpl os;
    const qint64 subFeaturesCount = U2FeatureUtils::countOfChildren(rootFeatureId, Root, entityRef.dbiRef, U2Feature::Annotation, os);
    SAFE_POINT_OP(os, false);
    return 0 != subFeaturesCount;
}

AnnotationGroup AnnotationTableObject::getRootGroup( ) {
    ensureDataLoaded();
    return AnnotationGroup( rootFeatureId, this );
}

void AnnotationTableObject::addAnnotation( const AnnotationData &a, const QString &groupName ) {
    ensureDataLoaded();

    AnnotationGroup rootGroup( rootFeatureId, this );
    AnnotationGroup subgroup = rootGroup.getSubgroup( groupName.isEmpty( )
        ? a.name : groupName, true );

    subgroup.addAnnotation( a );

    setModified( true );
}

void AnnotationTableObject::addAnnotations( const QList<AnnotationData> &annotations, U2OpStatus &os,
    const QString &groupName )
{
    if ( annotations.isEmpty( ) || os.isCanceled( ) ) {
        return;
    }

    ensureDataLoaded();

    AnnotationGroup rootGroup( rootFeatureId, this );
    AnnotationGroup group( rootGroup );
    QList<Annotation> resultAnnotations;

    const U2Feature rootFeature = U2FeatureUtils::getFeatureById( rootFeatureId, U2Feature::Group, entityRef.dbiRef, os );
    SAFE_POINT_OP( os, );

    if ( groupName.isEmpty( ) ) {
        QString previousGroupName;
        foreach ( const AnnotationData &a, annotations ) {
            if (os.isCanceled()) {
                return;
            }
            const QString groupName = a.name;
            if ( groupName != previousGroupName ) {
                group = rootGroup.getSubgroup( groupName, true );
                previousGroupName = groupName;
            }
            resultAnnotations << group.addAnnotation( a );
            QCoreApplication::processEvents( );
        }
    } else {
        group = rootGroup.getSubgroup( groupName, true );
        foreach ( const AnnotationData &a, annotations ) {
            if (os.isCanceled()) {
                return;
            }
            resultAnnotations << group.addAnnotation( a );
            QCoreApplication::processEvents( );
        }
     }

    setModified( true );
}

void AnnotationTableObject::removeAnnotation( const Annotation &a ) {
    ensureDataLoaded();

    setModified( true );
    emit si_onAnnotationsRemoved( QList<Annotation>( ) << a );

    removeAnnotationFromDb( a );
}

void AnnotationTableObject::removeAnnotations( const QList<Annotation> &annotations ) {
    ensureDataLoaded();

    emit si_onAnnotationsRemoved( annotations );
    foreach ( const Annotation &a, annotations ) {
        removeAnnotationFromDb( a );
    }
    setModified( true );
}

GObject * AnnotationTableObject::clone(const U2DbiRef &ref, U2OpStatus &os, const QVariantMap &hints ) const {
    ensureDataLoaded();

    GHintsDefaultImpl gHints(getGHintsMap());
    gHints.setAll(hints);

    DbiOperationsBlock opBlock(ref, os);
    Q_UNUSED(opBlock);
    CHECK_OP(os, NULL);

    AnnotationTableObject *cln = new AnnotationTableObject( getGObjectName( ), ref, gHints.getMap() );
    cln->setIndexInfo( getIndexInfo( ) );

    AnnotationGroup rootGroup = const_cast<AnnotationTableObject *>(this)->getRootGroup();
    QStringList subgroupPaths;
    rootGroup.getSubgroupPaths(subgroupPaths);
    AnnotationGroup clonedRootGroup = cln->getRootGroup();
    foreach (const QString &groupPath, subgroupPaths) {
        AnnotationGroup originalGroup = rootGroup.getSubgroup(groupPath, false);
        SAFE_POINT(originalGroup != rootGroup, "Unexpected annotation group", NULL);

        AnnotationGroup clonedGroup = clonedRootGroup.getSubgroup(groupPath, true);
        foreach (const Annotation &a, originalGroup.getAnnotations()) {
            clonedGroup.addAnnotation(a.getData());
        }
    }

    return cln;
}

QList<Annotation> AnnotationTableObject::getAnnotationsByName(const QString &name) const {
    QList<Annotation> result;

    ensureDataLoaded();

    U2OpStatusImpl os;
    QList<U2Feature> features = U2FeatureUtils::getFeaturesByName(rootFeatureId, name, U2Feature::Annotation, entityRef.dbiRef, os);
    SAFE_POINT_OP(os, result);

    return convertFeaturesToAnnotations(features);
}

QList<Annotation> AnnotationTableObject::getAnnotationsByRegion(const U2Region &region, bool contains) const {
    QList<Annotation> result;

    ensureDataLoaded();

    U2OpStatusImpl os;
    const QList<U2Feature> features = U2FeatureUtils::getAnnotatingFeaturesByRegion(rootFeatureId, entityRef.dbiRef, region, os, contains);
    SAFE_POINT_OP(os, result);

    return convertFeaturesToAnnotations(features);
}

bool AnnotationTableObject::checkConstraints(const GObjectConstraints *c) const {
    const AnnotationTableObjectConstraints *ac = qobject_cast<const AnnotationTableObjectConstraints *>(c);
    SAFE_POINT(NULL != ac, "Invalid feature constraints!", false);

    ensureDataLoaded();

    U2OpStatusImpl os;
    QList<U2Feature> allSubfeatures = U2FeatureUtils::getSubAnnotations(rootFeatureId, entityRef.dbiRef, os);
    SAFE_POINT_OP(os, false);

    const int fitSize = ac->sequenceSizeToFit;
    SAFE_POINT(0 < fitSize, "Invalid sequence length provided!", false);
    foreach (const U2Feature &feature, allSubfeatures) {
        SAFE_POINT(0 <= feature.location.region.startPos, "Invalid annotation region!", false);
        if (feature.location.region.endPos() > fitSize) {
            return false;
        }
    }

    return true;
}

void AnnotationTableObject::setGObjectName( const QString &newName ) {
    ensureDataLoaded();

    U2OpStatusImpl os;
    U2FeatureUtils::renameAnnotationTable( entityRef, newName, os );
    SAFE_POINT_OP( os, );

    GObject::setGObjectName( newName );
}

void AnnotationTableObject::ref( ) const {
    U2OpStatusImpl os;
    ensureDataLoaded(os);
    SAFE_POINT_OP(os, );

    U2FeatureUtils::refAnnotationTable(rootFeatureId, entityRef.dbiRef, os);
    SAFE_POINT_OP(os, );
}

void AnnotationTableObject::deref( ) const {
    U2OpStatusImpl os;
    U2FeatureUtils::derefAnnotationTable(rootFeatureId, entityRef.dbiRef, os);
    SAFE_POINT_OP(os, );
}

//////////////////////////////////////////////////////////////////////////
// Direct features interface (without sync with annotations)

U2DataId AnnotationTableObject::getRootFeatureId( ) const {
    ensureDataLoaded();

    return rootFeatureId;
}

void AnnotationTableObject::removeAnnotationFromDb( const Annotation &a ) {
    SAFE_POINT( this == a.getGObject( ), "Annotation belongs to another object!", );
    SAFE_POINT( a.hasValidId( ), "Invalid feature ID detected!", );

    ensureDataLoaded();

    U2OpStatusImpl os;
    U2FeatureUtils::removeFeature( a.id, U2Feature::Annotation, entityRef.dbiRef, os );
    SAFE_POINT_OP( os, );
}

QList<Annotation> AnnotationTableObject::convertFeaturesToAnnotations(
    const QList<U2Feature> &features ) const
{
    QList<Annotation> results;

    ensureDataLoaded();

    U2OpStatusImpl os;
    foreach ( const U2Feature &feature, features ) {
        if ( U2Feature::Annotation == feature.featureClass && !feature.name.isEmpty( ) ) {
            // this case corresponds to complete annotations, not to partial (e.g. joins, orders)
            results << Annotation( feature.id, const_cast<AnnotationTableObject *>( this ) );
        }
    }
    return results;
}

void AnnotationTableObject::emit_onAnnotationsAdded( const QList<Annotation> &l ) {
    emit si_onAnnotationsAdded( l );
}

void AnnotationTableObject::emit_onAnnotationModified( const AnnotationModification &md ) {
    emit si_onAnnotationModified( md );
}

void AnnotationTableObject::emit_onGroupCreated( const AnnotationGroup &g ) {
    emit si_onGroupCreated( g );
}

void AnnotationTableObject::emit_onGroupRemoved( const AnnotationGroup &p,
    const AnnotationGroup &g )
{
    emit si_onGroupRemoved( p, g );
}

void AnnotationTableObject::emit_onGroupRenamed( const AnnotationGroup &g ) {
    emit si_onGroupRenamed( g );
}

void AnnotationTableObject::emit_onAnnotationsInGroupRemoved( const QList<Annotation> &l,
    const AnnotationGroup &gr )
{
    emit si_onAnnotationsInGroupRemoved( l, gr );
}

void AnnotationTableObject::loadDataCore( U2OpStatus &os ) {
    U2AnnotationTable table = U2FeatureUtils::getAnnotationTable( entityRef, os );
    CHECK_OP( os, );
    rootFeatureId = table.rootFeature;
}

} // namespace U2

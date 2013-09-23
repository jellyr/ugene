/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2013 UniPro <ugene@unipro.ru>
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

#ifndef _U2_WORKFLOW_UTILS_H_
#define _U2_WORKFLOW_UTILS_H_

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <U2Lang/Dataset.h>
#include <U2Lang/Descriptor.h>
#include <U2Lang/ActorModel.h>
#include <U2Lang/IntegralBusModel.h>
#include <U2Lang/Schema.h>
#include <U2Lang/WorkflowContext.h>

class QListWidgetItem;

namespace U2 {
class Descriptor;
class DocumentFormat;
typedef QMap<int, QVariant> ValidateError;

using namespace Workflow;

class U2LANG_EXPORT WorkflowUtils : public QObject {
    Q_OBJECT
public:
    static QString getRichDoc(const Descriptor& d);
    static void setQObjectProperties(QObject&, const QVariantMap&);
    static QString getDropUrl(QList<DocumentFormat*>& fs, const QMimeData* md);
    static QStringList expandToUrls(const QString&);

    static const QStringList WD_FILE_EXTENSIONS;
    static const QString WD_XML_FORMAT_EXTENSION;
    static const QString HREF_PARAM_ID;

    #define ACTOR_REF (Qt::UserRole)
    #define PORT_REF (Qt::UserRole + 1)
    #define TEXT_REF (Qt::UserRole + 3)

    // used in GUI schema validating
    static bool validate(const Workflow::Schema &s, QList<QListWidgetItem*> &errs);
    // used in cmdline schema validating
    static bool validate( const Workflow::Schema &s, QStringList &errs);
    
    static QList<Descriptor> findMatchingTypes(DataTypePtr set, DataTypePtr elementDataType);
    static QStringList findMatchingTypesAsStringList(DataTypePtr set, DataTypePtr elementDatatype);
    static QStringList candidatesAsStringList(const QList<Descriptor> &candidates);
    static QList<Descriptor> findMatchingCandidates(DataTypePtr from, DataTypePtr to, const Descriptor & key);
    static QList<Descriptor> findMatchingCandidates(DataTypePtr from, DataTypePtr elementDatatype);
    static Descriptor getCurrentMatchingDescriptor(const QList<Descriptor> & candidates, DataTypePtr to, const Descriptor & key,
        const QStrStrMap & bindings);
    static DataTypePtr getToDatatypeForBusport(IntegralBusPort * p);
    static DataTypePtr getFromDatatypeForBusport(IntegralBusPort * p, DataTypePtr to);
    
    // find schema with 'name' in common folders or from settings
    static QString findPathToSchemaFile(const QString & name);

    static void getLinkedActorsId(Actor *a, QList<QString> &linkedActors); //get list of ID's of all linked actors

    static bool isPathExist(const Port *src, const Port *dest);
    
    static QString getStringForParameterDisplayRole(const QVariant & value);
    
    static Actor * findActorByParamAlias(const QList<Actor*> & procs, const QString & alias, QString & attrName, bool writeLog = true);

    static Descriptor getSlotDescOfDatatype(const DataTypePtr & dt);
    
    static QString getParamIdFromHref(const QString& href);

    static void print(const QString &slotString, const QVariant &data, DataTypePtr type, WorkflowContext *context);

    static bool validateSchemaForIncluding(const Schema &s, QString &error);

    static void extractPathsFromBindings(QStrStrMap &busMap, SlotPathMap &pathMap);

    static void applyPathsToBusMap(QStrStrMap &busMap, const SlotPathMap &pathMap);

    static bool startExternalProcess(QProcess *process, const QString &program, const QStringList &arguments);

    static QStringList getDatasetsUrls(const QList<Dataset> &sets);

    static QStringList getAttributeUrls(Attribute *attr);

    static Actor * actorById(const QList<Actor*> &actors, const ActorId &id);

    static QMap<Descriptor, DataTypePtr> getBusType(Port *inPort);

    static bool isBindingValid(const QString &srcSlotId, const QMap<Descriptor, DataTypePtr> &srcBus,
        const QString &dstSlotId, const QMap<Descriptor, DataTypePtr> &dstBus);

    /** Returns the string which is not contained by @uniqueStrs list
     * Result is created from @str rolling @sep + number suffix
     */
    static QString createUniqueString(const QString &str, const QString &sep, const QStringList &uniqueStrs);

    /** if path == "default" then nothing is changed. Returns the new path */
    static QString updateExternalToolPath(const QString &toolName, const QString &path);

    static QString externalToolError(const QString &toolName);
    static QString externalToolInvalidError(const QString &toolName);

    static void schemaFromFile(const QString &url, Schema *schema, Metadata *meta, U2OpStatus &os);

    /** Returns "true" if the attribute is used to keep input/output URL(s) */
    static bool isUrlAttribute(Attribute *attr, Actor *actor);

private:
    static QStringList initExtensions();
    static bool validate(const Workflow::Schema &s, QList<ValidateError> &infoList);
    
}; // WorkflowUtils

/** 
 * provides utility functions for ActorDocument purposes
 */
class U2LANG_EXPORT PrompterBaseImpl : public ActorDocument, public Prompter {
    Q_OBJECT
public:
    PrompterBaseImpl(Actor* p = 0) : ActorDocument(p) {}

    static bool isWildcardURL(const QString& url) {return url.indexOf(QRegExp("[*?\\[\\]]")) >= 0;}
    
    virtual ActorDocument * createDescription(Actor*) = 0;
    
    QString getURL(const QString& id, bool * empty = NULL );
    QString getScreenedURL(IntegralBusPort* input, const QString& id, const QString& slot);
    QString getRequiredParam(const QString& id);
    QVariant getParameter(const QString& id);
    QString getProducers(const QString& port, const QString& slot);
    QString getProducersOrUnset(const QString &port, const QString &slot);
    static QString getHyperlink(const QString& id, const QString& val);
    static QString getHyperlink(const QString& id, int val);
    static QString getHyperlink(const QString& id, qreal val);

    virtual QString composeRichDoc() = 0;
    virtual void update(const QVariantMap& cfg) {map = cfg; sl_actorModified();}

protected slots:
    virtual void sl_actorModified() {
        /*Actor* a = qobject_cast<Actor*>(sender());
        if (!a) {
            a = qobject_cast<Port*>(sender())->owner();
        }*/
        setHtml(QString("<center><b>%1</b></center><hr>%2")
            .arg(target->getLabel()).arg(composeRichDoc()));
    }
protected:
    QVariantMap map;
    
}; // PrompterBaseImpl

/**
 * template realization of Prompter and ActorDocument in one entity
 * represents creating description, updating description and displaying description facilities
 *
 * only classes that inherit ActorDocument can be used as a template argument
 * provides 
 */
template <typename T>
class PrompterBase : public PrompterBaseImpl {
public:
    PrompterBase(Actor* p = 0, bool listenInputs = true) : PrompterBaseImpl(p), listenInputs(listenInputs) {}
    virtual ActorDocument* createDescription(Actor* a) {
        T* doc = new T(a);
        doc->connect(a, SIGNAL(si_labelChanged()), SLOT(sl_actorModified()));
        doc->connect(a, SIGNAL(si_modified()), SLOT(sl_actorModified()));
        if (listenInputs) {
            foreach(Workflow::Port* input, a->getInputPorts()) {
                doc->connect(input, SIGNAL(bindingChanged()), SLOT(sl_actorModified()));
            }
        }

        foreach(Workflow::Port* input, a->getOutputPorts()) {
            doc->connect(input, SIGNAL(bindingChanged()), SLOT(sl_actorModified()));
        }
        //}
        return doc;
    }
protected:
    bool listenInputs;
    
}; // PrompterBase

}//namespace

#endif


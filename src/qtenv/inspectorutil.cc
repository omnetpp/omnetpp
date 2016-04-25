//==========================================================================
//  INSPECTORUTIL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "inspectorutil.h"
#include <QDebug>
#include <QMenu>
#include <QApplication>
#include <QClipboard>
#include <omnetpp/cobject.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cmessage.h>
#include <qtenv.h>
#include <qtenv/inspector.h>
#include <inspectorfactory.h>
#include "preferencesdialog.h"

namespace omnetpp {
namespace qtenv {

QVector<int> InspectorUtil::supportedInspTypes(cObject *object)
{
    using namespace qtenv;
    QVector<int> insp_types;
    cRegistrationList *a = inspectorfactories.getInstance();
    for (int i = 0; i < a->size(); i++) {
        InspectorFactory *ifc = static_cast<InspectorFactory *>(a->get(i));
        if (ifc->supportsObject(object)) {
            int k;
            for (k = 0; k < insp_types.size(); k++)
                if (insp_types[k] == ifc->getInspectorType())
                    break;

            if (k == insp_types.size())  // not yet in vector, insert
                insp_types.push_front(ifc->getInspectorType());
        }
    }

    return insp_types;
}

Inspector *InspectorUtil::getContainingInspector(QWidget *widget) {
    QWidget *curr = widget;
    while (curr) {
        Inspector *insp = dynamic_cast<Inspector *>(curr);
        if (insp)
            return insp;
        curr = curr->parentWidget();
    }
    return nullptr;
}

void InspectorUtil::fillInspectorContextMenu(QMenu *menu, cObject *object, Inspector *insp)
{
    // add "Go Info" if applicable
    QString name = object->getFullName();
    if(insp && object != insp->getObject() && insp->supportsObject(object))
    {
        QAction *action = menu->addAction("Go Into '" + name + "'", insp, SLOT(goUpInto()));
        action->setData(QVariant::fromValue(object));
        menu->addSeparator();
    }

    // add inspector types supported by the object
    for (int type : supportedInspTypes(object)) {
        QString label;
        switch (type) {
            case INSP_DEFAULT:
                label = "Open Best View";
                break;

            case INSP_OBJECT:
                label = "Open Details";
                break;

            case INSP_GRAPHICAL:
                label = "Open Graphical View";
                break;

            case INSP_MODULEOUTPUT:
                label = "Open Component Log";
                break;

            case INSP_OBJECTTREE:
                label = "Open Object Tree";
                break;

            default:
                qDebug() << "Unsupported inspector type " << type << " in context menu";
        }
        label += QString(" for '") + name + "'";
        QAction *action = menu->addAction(label, getQtenv(), SLOT(inspect()));
        action->setData(QVariant::fromValue(ActionDataPair(object, type)));
    }

    // add "run until" menu items
    if (dynamic_cast<cSimpleModule *>(object) || dynamic_cast<cModule *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Next Event in Module '") + name + "'", getQtenv(), SLOT(runUntilModule()));
        action->setData(QVariant::fromValue(ActionDataTriplet(ActionDataPair(object, Qtenv::RUNMODE_NORMAL), insp)));
        action = menu->addAction(QString("Fast Run Until Next Event in Module '") + name + "'", getQtenv(), SLOT(runUntilModule()));
        action->setData(QVariant::fromValue(ActionDataTriplet(ActionDataPair(object, Qtenv::RUNMODE_FAST), insp)));
    }

    cMessage *msg = dynamic_cast<cMessage *>(object);
    if (msg) {
        QAction *action;
        if (msg->isScheduled()) {
            menu->addSeparator();
            action = menu->addAction(QString("Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_NORMAL)));
            action = menu->addAction(QString("Fast Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_FAST)));
            action = menu->addAction(QString("Express Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_EXPRESS)));
        }
        menu->addSeparator();
        action = menu->addAction(QString("Exclude Messages Like '") + name + "' From Animation", getQtenv(), SLOT(excludeMessage()));
        action->setData(QVariant::fromValue(object));
    }

    cComponent *comp = dynamic_cast<cComponent *>(object);
    if (comp) {
        menu->addSeparator();
        QMenu *subMenu = menu->addMenu(QString("Set Log Level for '") + name + "' and children...");

        QAction *action;
        QActionGroup *logLevelActionGroup = new QActionGroup(menu);

        #define INSPECTORUTIL_ADD_LOGLEVEL(LEVEL) \
            /* Capitalize action's text */ \
            QString text_ ## LEVEL = #LEVEL; \
            text_ ## LEVEL = text_ ## LEVEL.toLower(); \
            text_ ## LEVEL[0] = text_ ## LEVEL[0].toUpper(); \
            /* Set action */ \
            action = subMenu->addAction(text_ ## LEVEL, getQtenv(), SLOT(setComponentLogLevel())); \
            action->setData(QVariant::fromValue(ActionDataPair(comp, LOGLEVEL_ ## LEVEL))); \
            action->setCheckable(true); \
            action->setChecked(comp->getLogLevel() == LOGLEVEL_ ## LEVEL); \
            action->setActionGroup(logLevelActionGroup);

        INSPECTORUTIL_ADD_LOGLEVEL(TRACE);
        INSPECTORUTIL_ADD_LOGLEVEL(DEBUG);
        INSPECTORUTIL_ADD_LOGLEVEL(DETAIL);
        INSPECTORUTIL_ADD_LOGLEVEL(INFO);
        INSPECTORUTIL_ADD_LOGLEVEL(WARN);
        INSPECTORUTIL_ADD_LOGLEVEL(ERROR);
        INSPECTORUTIL_ADD_LOGLEVEL(FATAL);
        subMenu->addSeparator();
        INSPECTORUTIL_ADD_LOGLEVEL(OFF);

        #undef INSPECTORUTIL_ADD_LOGLEVEL
    }

    // add utilities menu
    menu->addSeparator();
    QMenu *subMenu = menu->addMenu(QString("Utilities for '") + name + "'");
    QAction *action = subMenu->addAction("Copy Pointer With Cast (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_PTRWITHCAST)));
    action = subMenu->addAction("Copy Pointer Value (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_PTR)));
    subMenu->addSeparator();
    action = subMenu->addAction("Copy Full Path", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_FULLPATH)));
    action = subMenu->addAction("Copy Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_FULLNAME)));
    action = subMenu->addAction("Copy Class Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_CLASSNAME)));
}



QMenu *InspectorUtil::createInspectorContextMenu(cObject* object, Inspector *insp)
{
    QVector<cObject*> obj;
    obj.push_back(object);
    return createInspectorContextMenu(obj, insp);
}

QMenu *InspectorUtil::createInspectorContextMenu(QVector<cObject*> objects, Inspector *insp)
{
    QMenu *menu = new QMenu();
    menu->setFont(getQtenv()->getBoldFont());
    //TODO Is it needed?
    // If there are more than one ptrs, remove the inspector object's own ptr:
    // when someone right-clicks a submodule icon, we don't want the compound
    // module to be in the list.
    if(insp && objects.size() > 1)
    {
        cObject *object = insp->getObject();
        if(objects.indexOf(object) >= 0 && objects.indexOf(object) < objects.size())
        objects.remove(objects.indexOf(object));
    }

    if(objects.size() == 1)
    {
        fillInspectorContextMenu(menu, objects[0], insp);
    }
    else if(objects.size() > 1)
    {
        //then create a submenu for each object
        for(cObject *object : objects)
        {
            const char *name = object->getFullName();
            const char *shortTypeName = getObjectShortTypeName(object);
            QString infoStr = shortTypeName + QString(", ") + object->info().c_str();
            if(infoStr.size() > 30)
            {
                infoStr.truncate(30);
                infoStr += "...";
            }

            std::string baseClass = getObjectBaseClass(object);
            QString label;
            if(baseClass == "cGate")
            {
                cGate *nextGate = static_cast<cGate*>(object)->getNextGate();
                const char *nextGateName = object->getFullName();
                cObject *owner = object->getOwner();
                const char *ownerName = owner->getFullName();
                cObject *nextGateOwner = nextGate->getOwner();
                const char *nextGateOwnerName = nextGateOwner->getFullName();

                label = ownerName + QString(".") + name + " --> " + nextGateOwnerName + "." + nextGateName;
            }
            else if(baseClass == "cMessage")
            {
                const char *shortInfo = getMessageShortInfoString(static_cast<cMessage*>(object));
                label = label + name + " (" + shortTypeName + ", " + shortInfo + ")";
            }
            else
                label = label + name + " (" + infoStr + ")";

            QMenu *subMenu = menu->addMenu(label);
            fillInspectorContextMenu(subMenu, object, insp);
        }
    }

    if(insp)
    {
        cObject *object = insp->getObject();
        if(object)
        {
            cObject *parent = dynamic_cast<cComponent *>(object) ? ((cComponent *)object)->getParentModule() : object->getOwner();
            if(parent && insp->supportsObject(parent))
            {
                menu->addSeparator();
                QAction *action = menu->addAction("Go Up", insp, SLOT(goUpInto()));
                action->setData(QVariant::fromValue(parent));
            }
        }
    }

    return menu;
}

void InspectorUtil::copyToClipboard(cObject *object, int what)
{
    switch (what) {
        case InspectorUtil::COPY_PTR: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString(ss.str().c_str()));
            break;
        }

        case InspectorUtil::COPY_PTRWITHCAST: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString("((") + object->getClassName() + " *)" + ss.str().c_str() + ")");
            break;
        }

        case InspectorUtil::COPY_FULLPATH:
            setClipboard(object->getFullPath().c_str());
            break;

        case InspectorUtil::COPY_FULLNAME:
            setClipboard(object->getFullName());
            break;

        case InspectorUtil::COPY_CLASSNAME:
            setClipboard(object->getClassName());
            break;
    }
}

void InspectorUtil::setClipboard(QString str)
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->clear();
    clipboard->setText(str);
}

void InspectorUtil::preferencesDialog(eTab defaultPage)
{
    PreferencesDialog *prefDialog = new PreferencesDialog(defaultPage);
    prefDialog->exec();
    delete prefDialog;
}

QString InspectorUtil::getInspectMenuLabel(int typeCode)
{
    switch(typeCode)
    {
        case INSP_DEFAULT:
            return "Open Best View";
        case INSP_OBJECT:
            return "Open Details";
        case INSP_GRAPHICAL:
            return "Open Graphical View";
        case INSP_MODULEOUTPUT:
            return"Open Component Log";
    }
    return "";
}

} // namespace qtenv
} // namespace omnetpp

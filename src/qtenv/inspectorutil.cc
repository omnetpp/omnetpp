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
#include <QMenu>
#include <omnetpp/cobject.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cmessage.h>
#include <qtenv.h>
#include <qtenv/inspector.h>
#include <inspectorfactory.h>

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

void InspectorUtil::fillInspectorContextMenu(QMenu *menu, cObject *object, Inspector *insp)
{
    //TODO
    //global contextmenurules

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
        }
        label += QString(" for '") + name + "'";
        QAction *action = menu->addAction(label, insp, SLOT(onClickOpenInspector()));
        action->setData(QVariant::fromValue(ActionDataPair(object, type)));
    }

    // add "run until" menu items
    if (dynamic_cast<cSimpleModule *>(object) || dynamic_cast<cModule *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Next Event in Module '") + name + "'", insp, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_NORMAL)));
        action = menu->addAction(QString("Fast Run Until Next Event in Module '") + name + "'", insp, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_FAST)));
    }

    if (dynamic_cast<cMessage *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Delivery of Message '") + name + "'", insp, SLOT(onClickRunMessage()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_NORMAL)));
        action = menu->addAction(QString("Fast Run Until Delivery of Message '") + name + "'", insp, SLOT(onClickRunMessage()));
        action->setData(QVariant::fromValue(ActionDataPair(object, Qtenv::RUNMODE_FAST)));
        action = menu->addAction(QString("Express Run Until Delivery of Message '") + name + "'", insp, SLOT(onClickRunMessage()));
        menu->addSeparator();
        action = menu->addAction(QString("Exclude Messages Like '") + name + "' From Animation", insp, SLOT(onClickExcludeMessage()));
        action->setData(QVariant::fromValue(object));
    }

    // add utilities menu
    menu->addSeparator();
    QMenu *subMenu = menu->addMenu(QString("Utilities for '") + name + "'");
    QAction *action = subMenu->addAction("Copy Pointer With Cast (for Debugger)", insp, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_PTRWITHCAST)));
    action = subMenu->addAction("Copy Pointer Value (for Debugger)", insp, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_PTR)));
    subMenu->addSeparator();
    action = subMenu->addAction("Copy Full Path", insp, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_FULLPATH)));
    action = subMenu->addAction("Copy Name", insp, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_FULLNAME)));
    action = subMenu->addAction("Copy Class Name", insp, SLOT(onClickUtilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, COPY_CLASSNAME)));
}

QMenu *InspectorUtil::createInspectorContextMenu(QVector<cObject*> objects, Inspector *insp)
{
    QMenu *menu = new QMenu();

    //TODO Is it needed?
    // If there are more than one ptrs, remove the inspector object's own ptr:
    // when someone right-clicks a submodule icon, we don't want the compound
    // module to be in the list.
    if(objects.size() > 1)
    {
        cObject *object = insp->getObject();
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

            const char *baseClass = getObjectBaseClass(object);
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
    return menu;
}

} // namespace qtenv
} // namespace omnetpp

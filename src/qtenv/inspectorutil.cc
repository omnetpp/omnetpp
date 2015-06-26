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

QMenu *InspectorUtil::fillInspectorContextMenu(cObject *object, Inspector *insp)
{
    //TODO
    //global contextmenurules

    QMenu *menu = new QMenu();

    // add "Go Info" if applicable
    QString name = object->getFullName();
    if(insp && object != insp->getObject() && insp->supportsObject(object))
    {
        QAction *action = menu->addAction("Go Into '" + name + "'", insp, SLOT(goInto()));
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

    return menu;
}

void InspectorUtil::createContextMenu(std::vector<cObject*> objects)
{
    if(objects.size() == 1)
    {
        //fillInspectorContextMenu .popup $insp $ptrs
    }// elseif {[llength $ptrs] > 1} {
//    # then create a submenu for each object
//    foreach ptr $ptrs {
//        set submenu .popup.$ptr
//        catch {destroy $submenu}
//        menu $submenu -tearoff 0
//        set name [opp_getobjectfullname $ptr]
//        set shorttypename [opp_getobjectshorttypename $ptr]
//        set infostr "$shorttypename, [opp_getobjectinfostring $ptr]"
//        if {[string length $infostr] > 30} {
//            set infostr [string range $infostr 0 29]...
//        }
//        set baseclass [opp_getobjectbaseclass $ptr]
//        if {$baseclass == "cGate" } {
//            set nextgateptr [opp_getobjectfield $ptr "nextGate"]
//            set nextgatename [opp_getobjectfullname $nextgateptr]
//            set ownerptr [opp_getobjectowner $ptr]
//            set ownername [opp_getobjectfullname $ownerptr]
//            set nextgateownerptr [opp_getobjectowner $nextgateptr]
//            set nextgateownername [opp_getobjectfullname $nextgateownerptr]

//            set label "$ownername.$name --> $nextgateownername.$nextgatename"
//        } elseif {$baseclass == "cMessage" } {
//            set shortinfo [opp_getmessageshortinfostring $ptr]
//            set label "$name ($shorttypename, $shortinfo)"
//        } else {
//            set label "$name ($infostr)"
//        }
//        fillInspectorContextMenu $submenu $insp $ptr
//        .popup add cascade -label $label -menu $submenu
//    }
//}

//if [opp_isinspector $insp] {
//   set ptr [opp_inspector_getobject $insp]
//   if [opp_isnotnull $ptr] {
//      set parentptr [opp_getobjectparent $ptr]
//      if {[opp_isnotnull $parentptr] && [opp_inspector_supportsobject $insp $parentptr]} {
//          .popup add separator
//          .popup add command -label "Go Up" -command "opp_inspector_setobject $insp $parentptr"
//      }
//   }
//}

//return .popup
//}
}

} // namespace qtenv
} // namespace omnetpp

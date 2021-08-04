//==========================================================================
//  INSPECTORUTIL.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <QtCore/QDebug>
#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <common/stringutil.h>
#include <omnetpp/cobject.h>
#include <omnetpp/csimplemodule.h>
#include <omnetpp/cmodule.h>
#include <omnetpp/cmessage.h>
#include "qtenv.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "inspectorutil.h"
#include "preferencesdialog.h"
#include "genericobjectinspector.h"
#include "mainwindow.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

QVector<InspectorType> InspectorUtil::supportedInspTypes(cObject *object)
{
    using namespace qtenv;
    QVector<InspectorType> insp_types;
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

Inspector *InspectorUtil::getContainingInspector(QWidget *widget)
{
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
    // add "Go Into" and "View in Embedded" if applicable
    QString name = object->getFullName();
    if (insp && object != insp->getObject()) {
        if (insp->supportsObject(object)) {
            menu->addAction("Go Into '" + name + "'", insp, SLOT(goUpInto()))
                ->setData(QVariant::fromValue(object));
        }

        if (insp != getQtenv()->getMainObjectInspector())
            menu->addAction("View '" + name + "' in Object Inspector", getQtenv()->getMainObjectInspector(), SLOT(goUpInto()))
                ->setData(QVariant::fromValue(object));
    }

    // the GenericObjectInspector relies on this always being here, so it's unconditional
    // but separators at the ends are skipped when rendering, and multiple separators are merged, so it's fine
    menu->addSeparator();

    // add inspector types supported by the object
    for (InspectorType type : supportedInspTypes(object)) {
        QString label = getInspectMenuLabel(type);
        if (label.isEmpty())
            qDebug() << "Unsupported inspector type " << type << " in context menu";
        label += QString(" for '") + name + "'";
        QAction *action = menu->addAction(label, getQtenv(), SLOT(inspect()));
        action->setData(QVariant::fromValue(InspectActionData{object, type}));
    }

    // add "run until" menu items
    if (dynamic_cast<cSimpleModule *>(object) || dynamic_cast<cModule *>(object)) {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Next Event in Module '") + name + "'", getQtenv(), SLOT(runUntilModule()));
        action->setData(QVariant::fromValue(RunUntilNextEventActionData{object, RUNMODE_NORMAL, insp}));
        action = menu->addAction(QString("Fast Run Until Next Event in Module '") + name + "'", getQtenv(), SLOT(runUntilModule()));
        action->setData(QVariant::fromValue(RunUntilNextEventActionData{object, RUNMODE_FAST, insp}));
    }

    cMessage *msg = dynamic_cast<cMessage *>(object);
    if (msg) {
        QAction *action;
        if (msg->isScheduled()) {
            menu->addSeparator();
            action = menu->addAction(QString("Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(RunUntilActionData{object, RUNMODE_NORMAL}));
            action = menu->addAction(QString("Fast Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(RunUntilActionData{object, RUNMODE_FAST}));
            action = menu->addAction(QString("Express Run Until Delivery of Message '") + name + "'", getQtenv(), SLOT(runUntilMessage()));
            action->setData(QVariant::fromValue(RunUntilActionData{object, RUNMODE_EXPRESS}));
        }
        menu->addSeparator();
        action = menu->addAction(QString("Exclude Messages Like '") + name + "' From Animation", getQtenv(), SLOT(excludeMessage()));
        action->setData(QVariant::fromValue(object));
    }

    cComponent *comp = dynamic_cast<cComponent *>(object);
    if (comp) {
        menu->addSeparator();
        QMenu *subMenu = menu->addMenu(QString("Set Log Level for '") + name + "' and children...");
        QActionGroup *logLevelActionGroup = new QActionGroup(menu);

        addLoglevel(LOGLEVEL_TRACE, "Trace", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_DEBUG, "Debug", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_DETAIL, "Detail", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_INFO, "Info", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_WARN, "Warn", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_ERROR, "Error", comp, logLevelActionGroup, subMenu);
        addLoglevel(LOGLEVEL_FATAL, "Fatal", comp, logLevelActionGroup, subMenu);
        subMenu->addSeparator();
        addLoglevel(LOGLEVEL_OFF, "Off", comp, logLevelActionGroup, subMenu);
    }

    // add utilities menu
    menu->addSeparator();
    QMenu *subMenu = menu->addMenu(QString("Utilities for '") + name + "'");
    QAction *action = subMenu->addAction("Copy Pointer With Cast (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_PTRWITHCAST}));
    action = subMenu->addAction("Copy Pointer Value (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_PTR}));
    subMenu->addSeparator();

    action = subMenu->addAction("Copy Full Path", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_FULLPATH}));
    action = subMenu->addAction("Copy Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_FULLNAME}));
    action = subMenu->addAction("Copy Class Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_CLASSNAME}));

    menu->addSeparator();
    menu->addAction("Find Objects in '" + name + "'", getQtenv()->getMainWindow(), SLOT(on_actionFindInspectObjects_triggered()))
            ->setData(QVariant::fromValue(object));
}

void InspectorUtil::addLoglevel(LogLevel level, QString levelInStr, cComponent *comp, QActionGroup *logLevelActionGroup,
                                QMenu *subMenu)
{
    QAction *action = subMenu->addAction(levelInStr, getQtenv(), SLOT(setComponentLogLevel()));
    action->setData(QVariant::fromValue(ComponentLogActionData{comp, level}));
    action->setCheckable(true);
    action->setChecked(comp->getLogLevel() == level);
    action->setActionGroup(logLevelActionGroup);
}

QMenu *InspectorUtil::createInspectorContextMenu(cObject *object, Inspector *insp)
{
    QVector<cObject *> obj;
    obj.push_back(object);
    return createInspectorContextMenu(obj, insp);
}

QMenu *InspectorUtil::createInspectorContextMenu(QVector<cObject *> objects, Inspector *insp)
{
    QMenu *menu = new QMenu();
    QFont font = getQtenv()->getBoldFont();
    menu->setStyleSheet("font: " + QString::number(font.pointSize()) + "pt \"" + font.family() + "\"");

    // If there are more than one ptrs, remove the inspector object's own ptr:
    // when someone right-clicks a submodule icon, we don't want the compound
    // module to be in the list.
    if (insp && objects.size() > 1) {
        cObject *object = insp->getObject();
        if (objects.indexOf(object) >= 0 && objects.indexOf(object) < objects.size())
            objects.remove(objects.indexOf(object));
    }

    if (objects.size() == 1) {
        fillInspectorContextMenu(menu, objects[0], insp);
    }
    else if (objects.size() > 1) {
        const int maxObjects = 20;

        // then create a submenu for each object
        for (int i = 0; i < std::min(maxObjects, objects.size()); ++i) {
            cObject *object = objects[i];

            const char *name = object->getFullName();
            const QString &shortTypeName = getObjectShortTypeName(object);
            QString infoStr = shortTypeName + QString(", ") + object->str().c_str();
            if (infoStr.size() > 30) {
                infoStr.truncate(30);
                infoStr += "...";
            }

            std::string baseClass = getObjectBaseClass(object);
            QString label;
            if (baseClass == "cGate") {
                cGate *nextGate = static_cast<cGate *>(object)->getNextGate();
                const char *nextGateName = nextGate->getFullName();
                cObject *owner = object->getOwner();
                const char *ownerName = owner->getFullName();
                cObject *nextGateOwner = nextGate->getOwner();
                const char *nextGateOwnerName = nextGateOwner->getFullName();

                label = ownerName + QString(".") + name + " --> " + nextGateOwnerName + "." + nextGateName;
            }
            else if (baseClass == "cMessage") {
                QString shortInfo = getMessageShortInfoString(static_cast<cMessage *>(object));
                label = label + name + " (" + shortTypeName + ", " + shortInfo + ")";
            }
            else
                label = label + name + " (" + infoStr + ")";

            QMenu *subMenu = menu->addMenu(label);
            fillInspectorContextMenu(subMenu, object, insp);
        }
        if (objects.size() > maxObjects) {
            menu->addAction("Too many objects, only showing "
                            + QString::number(maxObjects) + " of "
                            + QString::number(objects.size()))->setEnabled(false);
        }
    }

    if (insp) {
        cObject *object = insp->getObject();
        if (object) {
            cObject *parent = dynamic_cast<cComponent *>(object) ? ((cComponent *)object)->getParentModule() : object->getOwner();
            if (parent && insp->supportsObject(parent)) {
                menu->addSeparator();
                QAction *action = menu->addAction("Go Up", insp, SLOT(goUpInto()));
                action->setData(QVariant::fromValue(parent));
            }
        }
    }

    return menu;
}

void InspectorUtil::copyToClipboard(cObject *object, eCopy what)
{
    switch (what) {
        case COPY_PTR: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString(ss.str().c_str()));
            break;
        }

        case COPY_PTRWITHCAST: {
            void *address = static_cast<void *>(object);
            std::stringstream ss;
            ss << address;
            setClipboard(QString("((") + object->getClassName() + " *)" + ss.str().c_str() + ")");
            break;
        }

        case COPY_FULLPATH:
            setClipboard(object->getFullPath().c_str());
            break;

        case COPY_FULLNAME:
            setClipboard(object->getFullName());
            break;

        case COPY_CLASSNAME:
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
    PreferencesDialog *prefDialog = new PreferencesDialog(defaultPage, getQtenv()->getMainWindow());
    prefDialog->exec();
    delete prefDialog;
}

QString InspectorUtil::getInspectMenuLabel(InspectorType type)
{
    switch(type) {
        case INSP_DEFAULT: return "Open Best View";
        case INSP_OBJECT: return "Open Details";
        case INSP_GRAPHICAL: return "Open Graphical View";
        case INSP_LOG: return "Open Component Log";
        case INSP_OBJECTTREE: return "Open Object Tree";
        default: return "";
    }
}

QString InspectorUtil::formatDouble(double num)
{
    // Don't use 'g' format because it ignores precision for the scientific notation.
    // The other two formats don't omit trailing zeroes.
    return QString::asprintf("%.16g", num);
}

}  // namespace qtenv
}  // namespace omnetpp


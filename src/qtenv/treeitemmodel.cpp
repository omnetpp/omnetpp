#include "treeitemmodel.h"
#include "envir/visitor.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/cqueue.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/coutvector.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/ccanvas.h"
#include "inspectorfactory.h"
#include "qtenv.h"
#include "tkutil.h"
#include <QIcon>
#include <QMainWindow>

#include <QDebug>

TreeItemModel::TreeItemModel(QObject *parent) :
    QAbstractItemModel(parent),
    rootNode(nullptr)
{
}

void TreeItemModel::setRootObject(cObject *root)
{
    beginResetModel();
    if(root == nullptr)
    {}//qDebug() << "TreeItemModel::setRootObject: root is null";}//TODO log: TreeItemModel::setRootObject: root is null
    rootNode = root;
    endResetModel();
}

QModelIndex TreeItemModel::index(int row, int, const QModelIndex &parent) const
{
    //qDebug() << "index" << row << 1;
    if(!rootNode)
        return QModelIndex();

    cObject *node = getObjectFromIndex(parent);

    return createIndex(row, 0, node);
}

cObject *TreeItemModel::getObjectFromIndex(const QModelIndex &index) const
{
    if(!index.isValid())
        return rootNode;

    cObject *parent = static_cast<cObject*>(index.internalPointer());
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();
    return visitor.getArraySize() > index.row() ? objs[index.row()] : nullptr;
}

QModelIndex TreeItemModel::parent(const QModelIndex &index) const
{
    //qDebug() << "parent";

    cObject *parent = index.isValid() ? static_cast<cObject*>(index.internalPointer()) :
                                        rootNode;
    if(!parent)
        return QModelIndex();
    cObject *grandParent = parent->getOwner();
    if(!grandParent)
        return QModelIndex();

    int row = findObjectInParent(parent, grandParent);
    if(row != -1)
        return createIndex(row, 0, grandParent);

    return QModelIndex();
}

int TreeItemModel::findObjectInParent(cObject *obj, cObject *parent) const
{
    cCollectChildrenVisitor visitor(parent);
    visitor.process(parent);
    cObject **objs = visitor.getArray();

    for(size_t i = 0; i < visitor.getArraySize(); ++i)
        if(objs[i] == obj)
            return i;

    return -1;
}

int TreeItemModel::rowCount(const QModelIndex &parent) const
{
    //qDebug() << "RowCount";
    cObject *node = getObjectFromIndex(parent);
    if(!node)
        return 0;
    cCountChildrenVisitor visitor(node);
    visitor.process(node);
    //qDebug() << visitor.getCount();
    return visitor.getCount();
}

QVariant TreeItemModel::data(const QModelIndex &index, int role) const
{
    //qDebug() << "data" << index;
    cObject *node = getObjectFromIndex(index);
    if(!node)
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        long id = -1;
        if (dynamic_cast<cModule *>(node))
            id = dynamic_cast<cModule *>(node)->getId();
        else if (dynamic_cast<cMessage *>(node))
            id = dynamic_cast<cMessage *>(node)->getId();

        QString text = node->getFullName() + QString(" (") + qtenv::getObjectShortTypeName(node) + ")" +
                        (id>=0 ? "(id=" + QString::number(id) + QString(")") : "");
        return QVariant(text);
    }
    else if(role == Qt::DecorationRole)
        return QVariant(QIcon(":/objects/icons/objects/" + getObjectIcon(node)));

    return QVariant();
}

bool TreeItemModel::hasChildren(const QModelIndex &parent) const
{
    cObject *parentNode = getObjectFromIndex(parent);
    if(!parentNode)
        return false;

    cHasChildrenVisitor visitor(parentNode);
    visitor.process(parentNode);
    return visitor.getResult();
}

QString TreeItemModel::getObjectIcon(cObject *object) const
{
    QString iconName;
    if (object == NULL)
        iconName = "none_vs.png";
    else if (dynamic_cast<cModule *>(object) && ((cModule *)object)->isPlaceholder())
        iconName = "placeholder_vs.png";
    else if (dynamic_cast<cSimpleModule *>(object))
        iconName = "simple_vs.png";
    else if (dynamic_cast<cModule *>(object))
        iconName = "compound_vs.png";
    else if (dynamic_cast<cWatchBase *>(object))
        iconName = "cogwheel_vs.png";
    else if (dynamic_cast<cMessage *>(object))
        iconName = "message_vs.png";
    else if (dynamic_cast<cArray *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cQueue *>(object))
        iconName = "queue_vs.png";
    else if (dynamic_cast<cGate *>(object))
        iconName = "gate_vs.png";
    else if (dynamic_cast<cPar *>(object))
        iconName = "param_vs.png";
    else if (dynamic_cast<cChannel *>(object))
        iconName = "chan_vs.png";
    else if (dynamic_cast<cOutVector *>(object))
        iconName = "outvect_vs.png";
    else if (dynamic_cast<cStatistic *>(object))
        iconName = "stat_vs.png";
    else if (dynamic_cast<cFigure *>(object))
        iconName = "figure_vs.png";
    else if (dynamic_cast<cCanvas *>(object))
        iconName = "canvas_vs.png";
    else if (dynamic_cast<cSimulation *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cMessageHeap *>(object))
        iconName = "container_vs.png";
    else if (dynamic_cast<cRegistrationList *>(object))
        iconName = "container_vs.png";
    else
        iconName = "cogwheel_vs.png";

    return iconName;
}

QMenu *TreeItemModel::getContextMenu(QModelIndex &index, QMainWindow *mainWindow)
{
    //global contextmenurules
    QMenu *menu = new QMenu();

    cObject *object = getObjectFromIndex(index);
    const char *name = object->getFullName();

    //add inspector types supported by the object
    for(int type : supportedInspTypes(object))
    {
        QString label;
        switch (type)
        {
            case qtenv::INSP_DEFAULT: label = "Open Best View"; break;
            case qtenv::INSP_OBJECT: label = "Open Details"; break;
            case qtenv::INSP_GRAPHICAL: label = "Open Graphical View"; break;
            case qtenv::INSP_MODULEOUTPUT: label = "Open Component Log"; break;
        }
        label += QString(" for '") + name + "'";
        QAction *action = menu->addAction(label, mainWindow, SLOT(onClickOpenInspector()));
        action->setData(QVariant::fromValue(ActionDataPair(object, type)));
    }

    //add "run until" menu items
    if(dynamic_cast<cSimpleModule*>(object) || dynamic_cast<cModule*>(object))
    {
        menu->addSeparator();
        QAction *action = menu->addAction(QString("Run Until Next Event in Module '") + name + "'", mainWindow, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, qtenv::Qtenv::RUNMODE_NORMAL)));
        action = menu->addAction(QString("Fast Run Until Next Event in Module '") + name + "'", mainWindow, SLOT(onClickRun()));
        action->setData(QVariant::fromValue(ActionDataPair(object, qtenv::Qtenv::RUNMODE_FAST)));
    }

//    if {$baseclass=="cMessage"} {
//        $menu add separator
//        $menu add command -label "Run Until Delivery of Message '$name'" -command "runUntilMsg $ptr normal"
//        $menu add command -label "Fast Run Until Delivery of Message '$name'" -command "runUntilMsg $ptr fast"
//        $menu add command -label "Express Run Until Delivery of Message '$name'" -command "runUntilMsg $ptr express"
//        $menu add separator
//        $menu add command -label "Exclude Messages Like '$name' From Animation" -command "excludeMessageFromAnimation $ptr"
//    }

//    # add utilities menu
//    set submenu .copymenu$ptr
//    catch {destroy $submenu}
//    menu $submenu -tearoff 0
//    $menu add separator
//    $menu add cascade -label "Utilities for '$name'" -menu $submenu
//    $submenu add command -label "Copy Pointer With Cast (for Debugger)" -command [list copyToClipboard $ptr ptrWithCast]
//    $submenu add command -label "Copy Pointer Value (for Debugger)" -command [list copyToClipboard $ptr ptr]
//    $submenu add separator
//    $submenu add command -label "Copy Full Path" -command [list copyToClipboard $ptr fullPath]
//    $submenu add command -label "Copy Name" -command [list copyToClipboard $ptr fullName]
//    $submenu add command -label "Copy Class Name" -command [list copyToClipboard $ptr className]

//    # add further menu items
//    set name [opp_getobjectfullpath $ptr]
//    set allcategories "mqsgvo"
//    set first 1
//    foreach key $contextmenurules(keys) {
//       #debug "trying $contextmenurules($key,label): opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) $name.$contextmenurules($key,name) 1"
//       # check context matches
//       if ![opp_patmatch $name $contextmenurules($key,context)] {
//           continue
//       }
//       # check we have such object
//       # Note: we have to quote the pattern for the object matcher, because $name might contain spaces
//       set objlist [opp_getsubobjectsfilt $ptr $allcategories $contextmenurules($key,class) "\"$name.$contextmenurules($key,name)\"" 1 ""]
//       if {$objlist!={}} {
//           if {$first} {
//               set first 0
//               $menu add separator
//           }
//           $menu add command -label "$contextmenurules($key,label)..." -command "inspectContextMenuRules $ptr $key"
//       }
//    }

    return menu;
}

QVector<int> TreeItemModel::supportedInspTypes(cObject *object)
{
    using namespace qtenv;
    QVector<int> insp_types;
    cRegistrationList *a = inspectorfactories.getInstance();
    for (int i=0; i<a->size(); i++)
    {
       InspectorFactory *ifc = static_cast<InspectorFactory *>(a->get(i));
       if (ifc->supportsObject(object))
       {
           int k;
           for (k=0; k<insp_types.size(); k++)
               if (insp_types[k] == ifc->getInspectorType())
                  break;
           if (k==insp_types.size()) // not yet in vector, insert
               insp_types.push_front(ifc->getInspectorType());
       }
    }

    return insp_types;
}

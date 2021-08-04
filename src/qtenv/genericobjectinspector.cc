//==========================================================================
//  GENERICOBJECTINSPECTOR.CC - part of
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

#include <cstring>
#include <cmath>
#include <utility>
#include <vector>
#include "omnetpp/cpacket.h"
#include "omnetpp/cregistrationlist.h"
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "genericobjectinspector.h"
#include "genericobjecttreemodel.h"
#include "genericobjecttreenodes.h"
#include "highlighteritemdelegate.h"
#include "displayupdatecontroller.h"
#include "inspectorutil.h"
#include "envir/objectprinter.h"
#include "envir/visitor.h"
#include <QtWidgets/QTreeView>
#include <QtCore/QDebug>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>

#define emit

using namespace omnetpp;
using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

void _dummy_for_genericobjectinspector() {}

class GenericObjectInspectorFactory : public InspectorFactory
{
  public:
    GenericObjectInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return true; }
    InspectorType getInspectorType() override { return INSP_OBJECT; }
    double getQualityAsDefault(cObject *object) override { return 1.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new GenericObjectInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(GenericObjectInspectorFactory);

//---- GenericObjectInspector implementation ----

const std::vector<std::string> GenericObjectInspector::containerTypes = {
    "cArray", "cQueue", "cFutureEventSet", "cSimpleModule",
    "cModule", "cChannel", "cRegistrationList", "cCanvas"
};

const QString GenericObjectInspector::PREF_MODE = "mode";

GenericObjectInspector::GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    treeView = new QTreeView(this);

    // various cosmetics
    treeView->setHeaderHidden(true);
    treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    treeView->setUniformRowHeights(true);

    auto delegate = new HighlighterItemDelegate(treeView);
    treeView->setItemDelegate(delegate);
    // pausing the animation (and simulation) while editing is in progress
    auto duc = getQtenv()->getDisplayUpdateController();
    connect(delegate, SIGNAL(editorCreated()), duc, SLOT(pause()));
    connect(delegate, SIGNAL(editorDestroyed()), duc, SLOT(resume()));

    // these enable horizontal scrolling
    treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    treeView->header()->setStretchLastSection(true);
    treeView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QToolBar *toolbar = new QToolBar();

    if (!isTopLevel) {
        // aligning right
        QWidget *spacer = new QWidget();
        spacer->setAutoFillBackground(true);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->addWidget(spacer);
    }

    QActionGroup *modeActionGroup = new QActionGroup(this);
    // mode selection
    toGroupedModeAction = toolbar->addAction(QIcon(":/tools/treemode_grouped"), "Grouped", this, SLOT(toGroupedMode()));
    toGroupedModeAction->setCheckable(true);
    toGroupedModeAction->setActionGroup(modeActionGroup);
    toFlatModeAction = toolbar->addAction(QIcon(":/tools/treemode_flat"), "Flat", this, SLOT(toFlatMode()));
    toFlatModeAction->setCheckable(true);
    toFlatModeAction->setActionGroup(modeActionGroup);
    toInheritanceModeAction = toolbar->addAction(QIcon(":/tools/treemode_inher"), "Inheritance", this, SLOT(toInheritanceMode()));
    toInheritanceModeAction->setCheckable(true);
    toInheritanceModeAction->setActionGroup(modeActionGroup);
    toolbar->addSeparator();
    toChildrenModeAction = toolbar->addAction(QIcon(":/tools/treemode_children"), "Children", this, SLOT(toChildrenMode()));
    toChildrenModeAction->setCheckable(true);
    toChildrenModeAction->setActionGroup(modeActionGroup);
    toolbar->addSeparator();
    toPacketModeAction = toolbar->addAction(QIcon(":/tools/treemode_packet"), "Packet", this, SLOT(toPacketMode()));
    toPacketModeAction->setCheckable(true);
    toPacketModeAction->setActionGroup(modeActionGroup);

    toolbar->addSeparator();

    if (isTopLevel) {
        addTopLevelToolBarActions(toolbar);

        // this is to fill the remaining space on the toolbar, replacing the ugly default gradient on Mac
        QWidget *stretch = new QWidget(toolbar);
        stretch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        stretch->setAutoFillBackground(true);
        toolbar->addWidget(stretch);
    }
    else {
        goBackAction = toolbar->addAction(QIcon(":/tools/back"), "Back", this, SLOT(goBack()));
        goForwardAction = toolbar->addAction(QIcon(":/tools/forward"), "Forward", this, SLOT(goForward()));
        goUpAction = toolbar->addAction(QIcon(":/tools/parent"), "Go to parent", this, SLOT(inspectParent()));
    }

    toolbar->setAutoFillBackground(true);

    layout->addWidget(toolbar);
    layout->addWidget(treeView, 1);
    layout->setMargin(0);
    layout->setSpacing(0);
    parent->setMinimumSize(20, 20);

    copyLineAction = new QAction("Copy &Line", this);
    copyLineAction->setShortcut(QKeySequence::Copy);
    copyLineAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    // lambda because it is easier than binding the parameter value
    connect(copyLineAction, &QAction::triggered, [this]() { copySelectedLineToClipboard(false); });
    addAction(copyLineAction);

    copyLineHighlightedAction = new QAction("&Copy Value", this);
    copyLineHighlightedAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_C);
    copyLineHighlightedAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    // lambda because it is easier than binding the parameter value
    connect(copyLineHighlightedAction, &QAction::triggered, [this]() { copySelectedLineToClipboard(true); });
    addAction(copyLineHighlightedAction);

    QAction *cycleSubtreeModeAction = new QAction("Cycle Subtree Mode", this);
    cycleSubtreeModeAction->setShortcut(Qt::CTRL + Qt::Key_B);
    cycleSubtreeModeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(cycleSubtreeModeAction, &QAction::triggered, this, &GenericObjectInspector::cycleSelectedSubtreeMode);
    addAction(cycleSubtreeModeAction);

    proxyModel = new PropertyFilteredGenericObjectTreeModel(this);

    mode = (Mode)getPref(PREF_MODE, QVariant::fromValue(0), false).toInt();

    doSetMode(mode);
    recreateModel();

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(onTreeViewActivated(QModelIndex)));

    // getting the data into any items newly brought into view
    connect(treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));
}

void GenericObjectInspector::recreateModel(bool keepNodeModeOverrides)
{
    GenericObjectTreeModel *newSourceModel;

    // The following two lines are a workaround for a crash introduced in Qt 5.11.
    // See: QTBUG-44962, QTBUG-67948, QTBUG-68427. These were fixed in 5.11.1, so
    // 5.11 is the only affected version. But let's keep this here, it's not a big deal...
    delete proxyModel;
    proxyModel = new PropertyFilteredGenericObjectTreeModel(this);

    GenericObjectTreeModel::NodeModeOverrideMap newNodeModeOverrides = sourceModel != nullptr && keepNodeModeOverrides
        ? sourceModel->getNodeModeOverrides() : GenericObjectTreeModel::NodeModeOverrideMap{};

    if (mode == Mode::PACKET) {
        newSourceModel = new GenericObjectTreeModel(object, Mode::FLAT, newNodeModeOverrides, this);
        proxyModel->setRelevantProperty("packetData");
    } else {
        newSourceModel = new GenericObjectTreeModel(object, mode, newNodeModeOverrides, this);
        proxyModel->setRelevantProperty("");
    }

    proxyModel->setSourceModel(newSourceModel);
    treeView->setModel(proxyModel);

    // expanding the top level item
    treeView->expand(proxyModel->index(0, 0, QModelIndex()));

    delete sourceModel;
    sourceModel = newSourceModel;

    gatherVisibleDataIfSafe();

    connect(sourceModel, SIGNAL(dataEdited(const QModelIndex&)), this, SLOT(onDataEdited()));
}

void GenericObjectInspector::doSetMode(Mode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        setPref(PREF_MODE, (int)mode, false);
    }

    toGroupedModeAction->setChecked(mode == Mode::GROUPED);
    toFlatModeAction->setChecked(mode == Mode::FLAT);
    toInheritanceModeAction->setChecked(mode == Mode::INHERITANCE);
    toChildrenModeAction->setChecked(mode == Mode::CHILDREN);
    toPacketModeAction->setChecked(mode == Mode::PACKET);
}

void GenericObjectInspector::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::XButton1: goBack(); break;
        case Qt::XButton2: goForward(); break;
        default: /* shut up, compiler! */ break;
    }
}

void GenericObjectInspector::resizeEvent(QResizeEvent *event)
{
    Inspector::resizeEvent(event);
    gatherVisibleDataIfSafe();
}

void GenericObjectInspector::closeEvent(QCloseEvent *event)
{
    setPref(PREF_MODE, (int)mode);
    Inspector::closeEvent(event);
}

void GenericObjectInspector::onTreeViewActivated(const QModelIndex &index)
{
    auto object = sourceModel->getCObjectPointerToInspect(proxyModel->mapToSource(index));
    if (!object)
        return;

    InspectorFactory *factory = findInspectorFactoryFor(object, INSP_DEFAULT);
    if (!factory) {
        getQtenv()->confirm(Qtenv::INFO, opp_stringf("Class '%s' has no associated inspectors.", object->getClassName()).c_str());
        return;
    }

    int preferredType = factory->getInspectorType();
    if (preferredType != INSP_OBJECT)
        getQtenv()->inspect(object);
    else
        setObject(object);
}

void GenericObjectInspector::onDataEdited()
{
    getQtenv()->callRefreshDisplaySafe();
    getQtenv()->callRefreshInspectors();
}

void GenericObjectInspector::gatherVisibleDataIfSafe()
{
    bool changed = gatherMissingDataIfSafe();
    if (changed) {
        // because properly doing it is super slow
        treeView->dataChanged(QModelIndex(), QModelIndex());
        treeView->resizeColumnToContents(0); // and this is needed because of it
    }
}

void GenericObjectInspector::createContextMenu(QPoint pos)
{
    QModelIndex sourceIndex = proxyModel->mapToSource(treeView->indexAt(pos));
    TreeNode *node = static_cast<TreeNode*>(sourceIndex.internalPointer());

    if (node) {
        QMenu *menu;

        cObject *object = sourceModel->getCObjectPointer(sourceIndex);
        if (object) {
            QVector<cObject *> objects;
            objects.push_back(object);
            cObject *objectToInspect = sourceModel->getCObjectPointerToInspect(sourceIndex);
            if (objectToInspect != nullptr && objectToInspect != object)
                objects.push_back(objectToInspect);
            menu = InspectorUtil::createInspectorContextMenu(objects, this);
            menu->addSeparator();
        }
        else {
            menu = new QMenu(this);
        }

        // finding the first separator, so we can insert the submenu where we want it
        auto actions = menu->actions();
        QAction *firstSep = nullptr;
        for (auto action : actions) {
            if (action->isSeparator()) {
                firstSep = action;
                break;
            }
        }

        QMenu *subtreeModeSubmenu = menu->addMenu("Switch Subtree Mode...");

        if (firstSep)
            menu->insertMenu(firstSep, subtreeModeSubmenu);
        else
            menu->addSeparator(); // this one will be _after_ the submenu

        menu->insertSeparator(subtreeModeSubmenu->menuAction()); // this one will be _before_ the submenu

        std::string nodeId = node->getNodeIdentifier().toStdString();
        const auto& overrides = sourceModel->getNodeModeOverrides();
        int nodeModeOverride = containsKey(overrides, nodeId) ? (int)overrides.at(nodeId) : -1;
        for (auto p : std::vector<std::pair<const char *, Mode>>{
            {"Grouped", Mode::GROUPED}, {"Flat", Mode::FLAT},
            {"Inheritance", Mode::INHERITANCE}, {"Children", Mode::CHILDREN}
            // PACKET can't be put here because it is never set on the sourceModel
        }) {
            QAction *action = subtreeModeSubmenu->addAction(p.first, [this, sourceIndex, p](bool checked) {
                sourceModel->setData(sourceIndex, checked ? (int)p.second : -1, (int)GenericObjectTreeModel::DataRole::NODE_MODE_OVERRIDE);
                gatherVisibleDataIfSafe();
            });
            action->setCheckable(true);
            action->setChecked(nodeModeOverride == (int)p.second);
        }

        subtreeModeSubmenu->addSeparator();
        subtreeModeSubmenu->addAction("Reset All", [this]{
            QSet<QString> expanded = getExpandedNodes();
            recreateModel(false);
            expandNodes(expanded);
        });

        menu->addAction(copyLineAction);
        menu->addAction(copyLineHighlightedAction);

        menu->exec(treeView->mapToGlobal(pos));
        delete menu;
    }
}

void GenericObjectInspector::copySelectedLineToClipboard(bool onlyHighlightedPart)
{
    QModelIndexList selection = treeView->selectionModel()->selectedIndexes();

    if (!selection.isEmpty()) {
        TreeNode *node = static_cast<TreeNode*>(proxyModel->mapToSource(selection.first()).internalPointer());
        QString text = node->getData(Qt::DisplayRole).toString();

        if (onlyHighlightedPart) {
            // extracting the "highlighted" blue region - the "value"
            HighlightRange range = node->getData(Qt::UserRole).value<HighlightRange>();
            text = text.mid(range.start, range.length);
        }

        QApplication::clipboard()->setText(text, QClipboard::Clipboard);
    }
}

void GenericObjectInspector::cycleSelectedSubtreeMode()
{
    QModelIndexList selection = treeView->selectionModel()->selectedIndexes();
    if (!selection.isEmpty()) {
        QModelIndex sourceIndex = proxyModel->mapToSource(selection.first());
        TreeNode *node = static_cast<TreeNode*>(sourceIndex.internalPointer());

        Mode currMode = node->getMode();
        Mode nextMode;

        switch (currMode) {
            case Mode::GROUPED:     nextMode = Mode::FLAT;        break;
            case Mode::FLAT:        nextMode = Mode::INHERITANCE; break;
            case Mode::INHERITANCE: nextMode = Mode::CHILDREN;    break;
            case Mode::CHILDREN:    nextMode = Mode::GROUPED;     break;
            case Mode::PACKET:      ASSERT(false);                break;   // this is never seen by the source model or the nodes
        };

        sourceModel->setData(sourceIndex, (int)nextMode, (int)GenericObjectTreeModel::DataRole::NODE_MODE_OVERRIDE);

        // have to get the selection again, it was invalidated
        treeView->expand(treeView->selectionModel()->selectedIndexes().first());
        gatherVisibleDataIfSafe();
    }
}

bool GenericObjectInspector::gatherMissingDataIfSafe()
{
    bool changed = false;
    if (getQtenv()->inspectorsAreFresh())
        changed = gatherMissingData();
    return changed;
}

bool GenericObjectInspector::updateData()
{
    bool changed = false;
    QModelIndexList indices = getVisibleNodes();
    for (auto i : indices) {
        if (i.isValid()) {
            QModelIndex sourceIndex = proxyModel->mapToSource(i);
            if (sourceIndex.isValid()) {
                TreeNode *node = static_cast<TreeNode *>(sourceIndex.internalPointer());
                if (node->updateData()) {
                    changed = true;
                    // we should do this here, but we don't because it is super slow
                    //emit dataChanged(i, i);
                }
            }
        }
    }
    return changed;
}

QString GenericObjectInspector::getSelectedNode()
{
    QModelIndexList selection = treeView->selectionModel()->selectedIndexes();

    if (selection.isEmpty())
        return "";

    TreeNode *node = static_cast<TreeNode*>(proxyModel->mapToSource(selection.first()).internalPointer());
    return node->getNodeIdentifier();
}

void GenericObjectInspector::selectNode(const QString &identifier)
{
    QModelIndexList visible = getVisibleNodes();

    for (auto v : visible) {
        TreeNode *node = static_cast<TreeNode*>(proxyModel->mapToSource(v).internalPointer());
        if (node->getNodeIdentifier() == identifier) {
            treeView->clearSelection();
            treeView->selectionModel()->select(v, QItemSelectionModel::Select | QItemSelectionModel::Rows);
            treeView->setCurrentIndex(v);
            break;
        }
    }
}

QSet<QString> GenericObjectInspector::getExpandedNodes()
{
    return getExpandedNodes(proxyModel->index(0, 0, QModelIndex()));
}


QSet<QString> GenericObjectInspector::getExpandedNodes(const QModelIndex &index)
{
    QSet<QString> result;
    if (treeView->isExpanded(index)) {
        result.insert(static_cast<TreeNode *>(proxyModel->mapToSource(index).internalPointer())->getNodeIdentifier());
        int numChildren = proxyModel->rowCount(index);
        for (int i = 0; i < numChildren; ++i) {
            result.unite(getExpandedNodes(proxyModel->index(i, 0, index)));
        }
    }
    return result;
}

void GenericObjectInspector::expandNodes(const QSet<QString> &ids)
{
    bool wasAnimated = treeView->isAnimated();
    treeView->setAnimated(false); // the last expanded node was animated without this, we don't need that
    QModelIndex rootIndex = proxyModel->index(0, 0, QModelIndex());
    expandNodes(ids, rootIndex);
    treeView->setAnimated(wasAnimated); // restoring the view to how it was before
}


void GenericObjectInspector::expandNodes(const QSet<QString> &ids, const QModelIndex &index)
{
    if (ids.contains(static_cast<TreeNode *>(proxyModel->mapToSource(index).internalPointer())->getNodeIdentifier())) {
        treeView->expand(index);

        int numChildren = proxyModel->rowCount(index);
        for (int i = 0; i < numChildren; ++i)
            expandNodes(ids, proxyModel->index(i, 0, index));
    }
}

QModelIndexList GenericObjectInspector::getVisibleNodes()
{
    QModelIndexList indices;

    QModelIndex topIndex = treeView->indexAt(treeView->rect().topLeft());
    QModelIndex bottomIndex = treeView->indexAt(treeView->rect().bottomLeft());

    for (QModelIndex i = topIndex; i != bottomIndex; i = treeView->indexBelow(i))
        indices.append(i);

    if (bottomIndex.isValid())
        indices.append(bottomIndex);

    return indices;
}

bool GenericObjectInspector::gatherMissingData()
{
    bool changed = false;
    QModelIndexList indices = getVisibleNodes();
    for (auto i : indices) {
        TreeNode *node = static_cast<TreeNode *>(proxyModel->mapToSource(i).internalPointer());
        if (node->gatherDataIfMissing()) {
            // not doing it, super slow, see caller
            //emit dataChanged(i, i);
            changed = true;
        }
    }
    return changed;
}

void GenericObjectInspector::setMode(Mode mode)
{
    if (this->mode != mode) {
        doSetMode(mode);
        recreateModel();
    }
}

void GenericObjectInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    if (!obj) {
        recreateModel();
        return;
    }

    QSet<QString> expanded = getExpandedNodes();

    bool isPacket = dynamic_cast<cPacket *>(obj);
    toPacketModeAction->setEnabled(isPacket);

    if (!isPacket && mode == Mode::PACKET) {
        bool isContainerLike = contains(containerTypes, std::string(getObjectBaseClass(obj)));
        doSetMode(isContainerLike ? Mode::CHILDREN : Mode::GROUPED);
    }

    recreateModel();

    expandNodes(expanded);
}

void GenericObjectInspector::refresh()
{
    Inspector::refresh();
    if (object) {
        QString selected = getSelectedNode();

        QSet<QString> expanded = getExpandedNodes();
        sourceModel->refreshTreeStructure();

        expandNodes(expanded);
        if (!selected.isEmpty())
            selectNode(selected);

        updateData();

        // this is a hack, proper item-wise datachanged is super slow
        treeView->dataChanged(QModelIndex(), QModelIndex());
        treeView->resizeColumnToContents(0);
    }
}

}  // namespace qtenv
}  // namespace omnetpp

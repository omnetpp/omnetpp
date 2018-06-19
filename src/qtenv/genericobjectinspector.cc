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
#include <QTreeView>
#include <QDebug>
#include <QGridLayout>
#include <QMessageBox>
#include <QApplication>
#include <QClipboard>

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
    toGroupedModeAction = toolbar->addAction(QIcon(":/tools/treemode_grouped"), "Switch to grouped mode", this, SLOT(toGroupedMode()));
    toGroupedModeAction->setCheckable(true);
    toGroupedModeAction->setActionGroup(modeActionGroup);
    toFlatModeAction = toolbar->addAction(QIcon(":/tools/treemode_flat"), "Switch to flat mode", this, SLOT(toFlatMode()));
    toFlatModeAction->setCheckable(true);
    toFlatModeAction->setActionGroup(modeActionGroup);
    toInheritanceModeAction = toolbar->addAction(QIcon(":/tools/treemode_inher"), "Switch to inheritance mode", this, SLOT(toInheritanceMode()));
    toInheritanceModeAction->setCheckable(true);
    toInheritanceModeAction->setActionGroup(modeActionGroup);
    toolbar->addSeparator();
    toChildrenModeAction = toolbar->addAction(QIcon(":/tools/treemode_children"), "Switch to children mode", this, SLOT(toChildrenMode()));
    toChildrenModeAction->setCheckable(true);
    toChildrenModeAction->setActionGroup(modeActionGroup);
    toolbar->addSeparator();
    toPacketModeAction = toolbar->addAction(QIcon(":/tools/treemode_packet"), "Switch to packet mode", this, SLOT(toPacketMode()));
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
        goUpAction = toolbar->addAction(QIcon(":/tools/parent"), "Go to parent module", this, SLOT(inspectParent()));
    }

    toolbar->setAutoFillBackground(true);

    layout->addWidget(toolbar);
    layout->addWidget(treeView, 1);
    layout->setMargin(0);
    layout->setSpacing(0);
    parent->setMinimumSize(20, 20);

    proxyModel = new PropertyFilteredGenericObjectTreeModel(this);

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

void GenericObjectInspector::recreateModel()
{
    GenericObjectTreeModel *newSourceModel;

    // The following two lines are a workaround for a crash introduced in Qt 5.11.
    // See: QTBUG-44962, QTBUG-67948, QTBUG-68427. These were fixed in 5.11.1, so
    // 5.11 is the only affected version. But let's keep this here, it's not a big deal...
    delete proxyModel;
    proxyModel = new PropertyFilteredGenericObjectTreeModel(this);

    if (mode == Mode::PACKET) {
        newSourceModel = new GenericObjectTreeModel(object, Mode::FLAT, this);
        proxyModel->setRelevantProperty("packetData");
    } else {
        newSourceModel = new GenericObjectTreeModel(object, mode, this);
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
        setPref(PREF_MODE, (int)mode);
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
    auto object = sourceModel->getCObjectPointer(proxyModel->mapToSource(index));
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
    cObject *object = sourceModel->getCObjectPointer(proxyModel->mapToSource(treeView->indexAt(pos)));
    if (object) {
        QVector<cObject *> objects;
        objects.push_back(object);
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(treeView->mapToGlobal(pos));
        delete menu;
    }
    else {
        TreeNode *node = static_cast<TreeNode*>(proxyModel->mapToSource(treeView->indexAt(pos)).internalPointer());
        if (node) {
            QString text = node->getData(Qt::DisplayRole).toString();

            // extractiong the "highlighted" blue region - the value
            HighlightRange range = node->getData(Qt::UserRole).value<HighlightRange>();
            text = text.mid(range.start, range.length);

            QMenu *menu = new QMenu(this);
            // The QMenu::addAction overload taking a lambda directly was only
            // added in Qt 5.6, but we only require 5.4, so doing it this way.
            QAction *copyAction = menu->addAction("&Copy Value");
            connect(copyAction, &QAction::triggered, [text]() {
                QApplication::clipboard()->setText(text, QClipboard::Clipboard);
            });
            menu->exec(treeView->mapToGlobal(pos));
            delete menu;
        }
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
        TreeNode *node = static_cast<TreeNode *>(proxyModel->mapToSource(i).internalPointer());
        if (node->updateData()) {
            changed = true;
            // we should do this here, but we don't because it is super slow
            //emit dataChanged(i, i);
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
            result.unite(getExpandedNodes(index.child(i, 0)));
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
            expandNodes(ids, index.child(i, 0));
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
        doSetMode((Mode)getPref(PREF_MODE, (int)Mode::GROUPED).toInt());
        recreateModel();
        return;
    }

    QSet<QString> expanded = getExpandedNodes();

    bool isContainerLike = contains(containerTypes, std::string(getObjectBaseClass(obj)));
    auto defaultMode = isContainerLike ? Mode::CHILDREN : Mode::GROUPED;

    Mode mode = (Mode)getPref(PREF_MODE, (int)defaultMode).toInt();

    bool isPacket = dynamic_cast<cPacket *>(obj);
    if (!isPacket && mode == Mode::PACKET)
        mode = defaultMode;
    toPacketModeAction->setEnabled(isPacket);

    doSetMode(mode);
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

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

#include <cmath>
#include <utility>
#include <vector>
#include <QtWidgets/QScrollBar>
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
#include <QtGui/QActionGroup>
#include <QtGui/QClipboard>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

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

const QString GenericObjectInspector::PREF_SORT_BY_NAME = "sortbyname";

GenericObjectInspector::GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    treeView = new QTreeView(this);

    // various cosmetics
    treeView->setHeaderHidden(true);
    treeView->setAttribute(Qt::WA_MacShowFocusRect, false);
    treeView->setUniformRowHeights(true);
    treeView->setSelectionMode(QAbstractItemView::SingleSelection);

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
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->addWidget(spacer);
    }

    sortByNameAction = toolbar->addAction(QIcon(":/tools/sort"), "Sort by Name", [this]() {
        setSortByName(!sortByName);
    });
    sortByNameAction->setCheckable(true);
    sortByNameAction->setToolTip("Sort fields alphabetically (children are always unsorted)");

    toolbar->addSeparator();

    if (isTopLevel) {
        addTopLevelToolBarActions(toolbar);
    }
    else {
        goBackAction = toolbar->addAction(QIcon(":/tools/back"), "Back", this, SLOT(goBack()));
        goForwardAction = toolbar->addAction(QIcon(":/tools/forward"), "Forward", this, SLOT(goForward()));
        goUpAction = toolbar->addAction(QIcon(":/tools/parent"), "Go to parent", this, SLOT(inspectParent()));
    }

    toolbar->setAutoFillBackground(true);

    layout->addWidget(toolbar);
    layout->addWidget(treeView, 1);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    parent->setMinimumSize(20, 20);

    copyLineAction = new QAction("Copy &Line", this);
    copyLineAction->setShortcut(QKeySequence::Copy);
    copyLineAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(copyLineAction, &QAction::triggered, [this]() { copySelectedLineToClipboard(false); });
    addAction(copyLineAction);

    copyLineHighlightedAction = new QAction("&Copy Value", this);
    copyLineHighlightedAction->setShortcut((int)Qt::CTRL | (int)Qt::SHIFT | Qt::Key_C);
    copyLineHighlightedAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(copyLineHighlightedAction, &QAction::triggered, [this]() { copySelectedLineToClipboard(true); });
    addAction(copyLineHighlightedAction);

    QAction *toggleDetailsAction = new QAction("Toggle Details", this);
    toggleDetailsAction->setShortcut((int)Qt::CTRL | Qt::Key_T);
    toggleDetailsAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(toggleDetailsAction, &QAction::triggered, this, &GenericObjectInspector::toggleSelectedNodeDetails);
    addAction(toggleDetailsAction);

    QAction *cycleDetailsModeAction = new QAction("Cycle Details Mode", this);
    cycleDetailsModeAction->setShortcut((int)Qt::CTRL | Qt::Key_D);
    cycleDetailsModeAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
    connect(cycleDetailsModeAction, &QAction::triggered, this, &GenericObjectInspector::cycleSelectedNodeDetailsMode);
    addAction(cycleDetailsModeAction);

    sortByName = getPref(PREF_SORT_BY_NAME, true, false).toBool();
    sortByNameAction->setChecked(sortByName);

    recreateModel();

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(onTreeViewActivated(QModelIndex)));

    // getting the data into any items newly brought into view
    connect(treeView, SIGNAL(expanded(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView, SIGNAL(collapsed(QModelIndex)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));
    connect(treeView->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(gatherVisibleDataIfSafe()));

    // --- subtree mode overlay: strip of mode buttons ---
    treeView->viewport()->setMouseTracking(true);
    treeView->viewport()->installEventFilter(this);

    subtreeModeOverlay = new QWidget(treeView->viewport());
    subtreeModeOverlay->setAutoFillBackground(true); // the buttons must not show the row text through
    subtreeModeOverlay->hide();
    subtreeModeOverlay->setAttribute(Qt::WA_TransparentForMouseEvents, false);

    // Make the button of the current mode stand out: the native checked look is
    // barely visible in some themes. Partially transparent so that dark icons
    // remain readable in the light theme.
    QColor checkedColor = treeView->palette().color(QPalette::Highlight);
    checkedColor.setAlpha(150);
    subtreeModeOverlay->setStyleSheet(QString("QToolButton:checked { background: rgba(%1, %2, %3, %4); border: none; border-radius: 3px; }")
            .arg(checkedColor.red()).arg(checkedColor.green()).arg(checkedColor.blue()).arg(checkedColor.alpha()));

    QHBoxLayout *overlayLayout = new QHBoxLayout(subtreeModeOverlay);
    overlayLayout->setContentsMargins(4, 0, 2, 0);
    overlayLayout->setSpacing(1);

    struct ModeButtonInfo {
        Mode mode;
        DetailsMode detailsMode;
        const char *icon;
        const char *tooltip;
    };
    for (const ModeButtonInfo& info : std::vector<ModeButtonInfo>{
        {Mode::CHILDREN, DetailsMode::GROUPED, ":/tools/treemode_children", "Children mode: child objects (Ctrl+T, Ctrl+Click: toggle Children/Details)"},
        {Mode::DETAILS, DetailsMode::GROUPED, ":/tools/treemode_grouped", "Details/Grouped: fields organized in groups (Ctrl+D: cycle Details modes)"},
        {Mode::DETAILS, DetailsMode::FLAT, ":/tools/treemode_flat", "Details/Flat: fields in a single list (Ctrl+D: cycle Details modes)"},
        {Mode::DETAILS, DetailsMode::INHERITANCE, ":/tools/treemode_inher", "Details/Inheritance: fields organized by declaring class (Ctrl+D: cycle Details modes)"},
        {Mode::DETAILS, DetailsMode::PACKET, ":/tools/treemode_packet", "Details/Packet: packet data fields only (Ctrl+D: cycle Details modes)"}
    }) {
        QToolButton *button = new QToolButton(subtreeModeOverlay);
        button->setIcon(QIcon(info.icon));
        button->setToolTip(info.tooltip);
        button->setIconSize(QSize(16, 16));
        button->setFixedSize(22, 22);
        button->setAutoRaise(true);
        button->setCheckable(true);
        overlayLayout->addWidget(button);

        connect(button, &QToolButton::clicked, [this, info](bool) {
            if (!getQtenv()->inspectorsAreFresh())
                return;
            QPoint viewportPos = treeView->viewport()->mapFromGlobal(QCursor::pos());
            QModelIndex index = treeView->indexAt(viewportPos);
            if (index.isValid())
                switchNodeMode(index, info.mode, info.detailsMode);
        });

        modeButtons.push_back({info.mode, info.detailsMode, button});
    }
    overlayLayout->addStretch(); // keep the buttons left-aligned when the overlay is wider than needed

    subtreeModeOverlay->adjustSize();

    // Hide overlay when scrolling (row positions change)
    connect(treeView->verticalScrollBar(), &QScrollBar::valueChanged, [this](int) { subtreeModeOverlay->hide(); });
    connect(treeView->horizontalScrollBar(), &QScrollBar::valueChanged, [this](int) { subtreeModeOverlay->hide(); });
}

bool GenericObjectInspector::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == treeView->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            if (me->button() == Qt::LeftButton && (me->modifiers() & Qt::ControlModifier)) {
                if (!getQtenv()->inspectorsAreFresh())
                    return true;
                QModelIndex proxyIndex = treeView->indexAt(me->pos());
                if (proxyIndex.isValid()) {
                    // Defer the toggle to avoid model changes (row insertions/removals)
                    // while Qt is still processing the mouse event.
                    QPersistentModelIndex persistent(proxyIndex);
                    QTimer::singleShot(0, this, [this, persistent]() {
                        if (persistent.isValid())
                            toggleNodeDetails(QModelIndex(persistent));
                    });
                    return true; // consume the event
                }
            }
        }
        else if (event->type() == QEvent::MouseMove) {
            QMouseEvent *me = static_cast<QMouseEvent *>(event);
            updateSubtreeModeOverlay(me->pos());
        }
        else if (event->type() == QEvent::Resize) {
            if (subtreeModeOverlay->isVisible()) {
                QPoint viewportPos = treeView->viewport()->mapFromGlobal(QCursor::pos());
                updateSubtreeModeOverlay(viewportPos);
            }
        }
        else if (event->type() == QEvent::Leave) {
            subtreeModeOverlay->hide();
        }
    }
    return false; // don't consume
}

void GenericObjectInspector::updateSubtreeModeOverlay(const QPoint &viewportPos)
{
    // Between simulation events and the next inspector refresh, tree nodes
    // may reference simulation objects that have already been deleted.
    // Avoid accessing node data in that window.
    if (!getQtenv()->inspectorsAreFresh()) {
        subtreeModeOverlay->hide();
        return;
    }

    QModelIndex sourceIndex = treeView->indexAt(viewportPos);
    if (!sourceIndex.isValid()) {
        subtreeModeOverlay->hide();
        return;
    }

    TreeNode *node = static_cast<TreeNode *>(sourceIndex.internalPointer());

    // Only show on nodes that have a class descriptor with fields and are
    // not opaque — toggling mode on a FieldGroupNode, SuperClassNode,
    // TextNode, opaque leaf, or a node with no fields doesn't make sense.
    cClassDescriptor *desc = node->getNodeClassDescriptor();
    if (!desc || desc->getFieldCount() == 0 || node->getMode() == Mode::OPAQUE) {
        subtreeModeOverlay->hide();
        return;
    }

    // Check the button of the current mode; Packet mode is only offered for packets
    bool nodeIsCPacket = dynamic_cast<cPacket *>(node->getCObjectPointer()) != nullptr;
    for (const ModeButton& mb : modeButtons) {
        bool isCurrent = node->getMode() == mb.mode && (mb.mode != Mode::DETAILS || node->getDetailsMode() == mb.detailsMode);
        mb.button->setChecked(isCurrent);
        mb.button->setEnabled(mb.detailsMode != DetailsMode::PACKET || nodeIsCPacket);
    }

    subtreeModeOverlay->adjustSize();

    // Position overlay flush-right in the row rect. Leave room for the vertical
    // scrollbar even when it is not shown, otherwise the overlay would jump
    // sideways under the mouse whenever a mode switch makes the scrollbar
    // appear or disappear. (Transient scrollbars don't take up room.) The overlay
    // still extends to the right edge, so that its background covers the
    // row (and its selection highlight) in the room left for the scrollbar.
    QRect rowRect = treeView->visualRect(sourceIndex);
    int viewportWidth = treeView->viewport()->width();
    int x = viewportWidth - subtreeModeOverlay->sizeHint().width();
    QScrollBar *vScrollBar = treeView->verticalScrollBar();
    if (!vScrollBar->isVisible() && !treeView->style()->styleHint(QStyle::SH_ScrollBar_Transient, nullptr, vScrollBar))
        x -= vScrollBar->sizeHint().width();
    int y = rowRect.top();
    subtreeModeOverlay->setGeometry(x, y, viewportWidth - x, rowRect.height());
    subtreeModeOverlay->raise();
    subtreeModeOverlay->show();
}

void GenericObjectInspector::recreateModel(bool keepNodeModeOverrides)
{
    GenericObjectTreeModel *newSourceModel;

    // Hide the hover overlay; its index will be stale after the model is replaced.
    if (subtreeModeOverlay) {
        subtreeModeOverlay->hide();
    }

    GenericObjectTreeModel::NodeModeOverrideMap newNodeModeOverrides = sourceModel != nullptr && keepNodeModeOverrides
        ? sourceModel->getNodeModeOverrides() : GenericObjectTreeModel::NodeModeOverrideMap{};

    newSourceModel = new GenericObjectTreeModel(object, sortByName, newNodeModeOverrides, true, this);

    treeView->setModel(newSourceModel);

    // expanding the top level item
    treeView->expand(newSourceModel->index(0, 0, QModelIndex()));

    delete sourceModel;
    sourceModel = newSourceModel;

    gatherVisibleDataIfSafe();

    connect(sourceModel, SIGNAL(dataEdited(const QModelIndex&)), this, SLOT(onDataEdited()));
}

void GenericObjectInspector::setSortByName(bool sorted)
{
    if (sortByName != sorted) {
        sortByName = sorted;
        sortByNameAction->setChecked(sortByName);
        setPref(PREF_SORT_BY_NAME, sortByName, false);
        QSet<QString> expanded = getExpandedNodes();
        int vScrollPos = treeView->verticalScrollBar()->value();
        int hScrollPos = treeView->horizontalScrollBar()->value();
        recreateModel();
        expandNodes(expanded);
        treeView->verticalScrollBar()->setValue(vScrollPos);
        treeView->horizontalScrollBar()->setValue(hScrollPos);
    }
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
    setPref(PREF_SORT_BY_NAME, sortByName);
    Inspector::closeEvent(event);
}

void GenericObjectInspector::onTreeViewActivated(const QModelIndex &index)
{
    auto object = sourceModel->getCObjectPointerToInspect(index);
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
    QModelIndex sourceIndex = treeView->indexAt(pos);
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

        // finding the first separator, so we can insert items where we want them
        auto actions = menu->actions();
        QAction *firstSep = nullptr;
        for (auto action : actions) {
            if (action->isSeparator()) {
                firstSep = action;
                break;
            }
        }

        bool isOpaque = node->getMode() == Mode::OPAQUE;
        cClassDescriptor *nodeDesc = node->getNodeClassDescriptor();
        bool hasNoFields = nodeDesc && nodeDesc->getFieldCount() == 0;
        bool detailsUnavailable = isOpaque || hasNoFields;

        // "Show Details" toggle item
        bool inDetails = node->getMode() == Mode::DETAILS;
        QAction *showDetailsAction = new QAction("Show Details\tCtrl+Click", menu);
        showDetailsAction->setCheckable(true);
        showDetailsAction->setChecked(inDetails);
        showDetailsAction->setEnabled(!detailsUnavailable);
        connect(showDetailsAction, &QAction::triggered, [this, sourceIndex](bool) {
            toggleNodeDetails(sourceIndex);
        });

        if (firstSep)
            menu->insertAction(firstSep, showDetailsAction);
        else
            menu->addAction(showDetailsAction);

        menu->insertSeparator(showDetailsAction); // separator before "Show Details"

        std::string nodeId = node->getNodeIdentifier().toStdString();
        bool nodeIsCPacket = dynamic_cast<cPacket *>(sourceModel->getCObjectPointer(sourceIndex)) != nullptr;

        // "Details Mode" radio submenu (only meaningful when in Details mode)
        QMenu *detailsModeSubmenu = new QMenu("Details Mode\tCtrl+D", menu);

        if (firstSep)
            menu->insertMenu(firstSep, detailsModeSubmenu);
        else
            menu->addMenu(detailsModeSubmenu);

        DetailsMode nodeDetailsMode = node->getDetailsMode();
        QActionGroup *detailsModeGroup = new QActionGroup(detailsModeSubmenu);
        for (auto p : std::vector<std::pair<const char *, DetailsMode>>{
            {"Grouped", DetailsMode::GROUPED}, {"Flat", DetailsMode::FLAT},
            {"Inheritance", DetailsMode::INHERITANCE}, {"Packet", DetailsMode::PACKET}
        }) {
            QAction *action = detailsModeSubmenu->addAction(p.first, [this, sourceIndex, p](bool checked) {
                if (checked) {
                    sourceModel->setNodeMode(sourceIndex, Mode::DETAILS, p.second);
                    gatherVisibleDataIfSafe();
                }
            });
            action->setCheckable(true);
            action->setActionGroup(detailsModeGroup);
            action->setChecked(nodeDetailsMode == p.second);
            if (p.second == DetailsMode::PACKET && !nodeIsCPacket)
                action->setEnabled(false);
        }

        detailsModeSubmenu->addSeparator();
        detailsModeSubmenu->addAction("Reset All Overrides", [this]{
            QSet<QString> expanded = getExpandedNodes();
            recreateModel(false);
            expandNodes(expanded);
        });

        detailsModeSubmenu->setEnabled(inDetails && !detailsUnavailable);

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
        TreeNode *node = static_cast<TreeNode*>(selection.first().internalPointer());
        QString text = node->getData(Qt::DisplayRole).toString();

        if (onlyHighlightedPart) {
            // extracting the "highlighted" blue region - the "value"
            HighlightRange range = node->getData(Qt::UserRole).value<HighlightRange>();
            text = text.mid(range.start, range.length);
        }

        QApplication::clipboard()->setText(text, QClipboard::Clipboard);
    }
}

void GenericObjectInspector::toggleSelectedNodeDetails()
{
    QModelIndexList selection = treeView->selectionModel()->selectedIndexes();
    if (!selection.isEmpty())
        toggleNodeDetails(selection.first());
}

void GenericObjectInspector::cycleSelectedNodeDetailsMode()
{
    QModelIndexList selection = treeView->selectionModel()->selectedIndexes();
    if (selection.isEmpty())
        return;

    QModelIndex sourceIndex = selection.first();
    if (!sourceIndex.isValid())
        return;

    TreeNode *node = static_cast<TreeNode*>(sourceIndex.internalPointer());

    if (node->getMode() != Mode::DETAILS)
        return;

    bool isCPacket = dynamic_cast<cPacket *>(node->getCObjectPointer()) != nullptr;

    // Cycle: GROUPED -> FLAT -> INHERITANCE -> PACKET -> GROUPED
    // (skip PACKET for non-cPacket nodes)
    DetailsMode currentDetailsMode = node->getDetailsMode();
    DetailsMode newDetailsMode;
    switch (currentDetailsMode) {
        case DetailsMode::GROUPED:     newDetailsMode = DetailsMode::FLAT; break;
        case DetailsMode::FLAT:        newDetailsMode = DetailsMode::INHERITANCE; break;
        case DetailsMode::INHERITANCE: newDetailsMode = isCPacket ? DetailsMode::PACKET : DetailsMode::GROUPED; break;
        case DetailsMode::PACKET:      newDetailsMode = DetailsMode::GROUPED; break;
        default:                       newDetailsMode = DetailsMode::GROUPED; break;
    }

    subtreeModeOverlay->hide();

    sourceModel->setNodeMode(sourceIndex, Mode::DETAILS, newDetailsMode);

    if (sourceIndex.isValid())
        treeView->expand(sourceIndex);
    gatherVisibleDataIfSafe();

    QPoint viewportPos = treeView->viewport()->mapFromGlobal(QCursor::pos());
    updateSubtreeModeOverlay(viewportPos);
}

void GenericObjectInspector::toggleNodeDetails(const QModelIndex &sourceIndex)
{
    if (!sourceIndex.isValid())
        return;

    TreeNode *node = static_cast<TreeNode*>(sourceIndex.internalPointer());

    bool inDetails = node->getMode() == Mode::DETAILS;
    if (inDetails) {
        switchNodeMode(sourceIndex, Mode::CHILDREN);
    }
    else {
        DetailsMode dm = dynamic_cast<cPacket *>(node->getCObjectPointer()) ? DetailsMode::PACKET : DetailsMode::GROUPED;
        switchNodeMode(sourceIndex, Mode::DETAILS, dm);
    }
}

void GenericObjectInspector::switchNodeMode(const QModelIndex &sourceIndex, Mode mode, DetailsMode detailsMode)
{
    if (!sourceIndex.isValid())
        return;

    sourceModel->setNodeMode(sourceIndex, mode, detailsMode);

    subtreeModeOverlay->hide();

    // Expand the node so the effect of the new mode is immediately visible.
    if (sourceIndex.isValid())
        treeView->expand(sourceIndex);
    gatherVisibleDataIfSafe();

    // Re-show the overlay at the current cursor position.
    QPoint viewportPos = treeView->viewport()->mapFromGlobal(QCursor::pos());
    updateSubtreeModeOverlay(viewportPos);
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
            TreeNode *node = static_cast<TreeNode *>(i.internalPointer());
            if (node->updateData()) {
                changed = true;
                // we should do this here, but we don't because it is super slow
                //Q_EMIT dataChanged(i, i);
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

    TreeNode *node = static_cast<TreeNode*>(selection.first().internalPointer());
    return node->getNodeIdentifier();
}

void GenericObjectInspector::selectNode(const QString &identifier)
{
    QModelIndexList visible = getVisibleNodes();

    for (auto v : visible) {
        TreeNode *node = static_cast<TreeNode*>(v.internalPointer());
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
    return getExpandedNodes(sourceModel->index(0, 0, QModelIndex()));
}


QSet<QString> GenericObjectInspector::getExpandedNodes(const QModelIndex &index)
{
    QSet<QString> result;
    if (treeView->isExpanded(index)) {
        result.insert(static_cast<TreeNode *>(index.internalPointer())->getNodeIdentifier());
        int numChildren = sourceModel->rowCount(index);
        for (int i = 0; i < numChildren; ++i) {
            result.unite(getExpandedNodes(sourceModel->index(i, 0, index)));
        }
    }
    return result;
}

void GenericObjectInspector::expandNodes(const QSet<QString> &ids)
{
    bool wasAnimated = treeView->isAnimated();
    treeView->setAnimated(false); // the last expanded node was animated without this, we don't need that
    QModelIndex rootIndex = sourceModel->index(0, 0, QModelIndex());
    expandNodes(ids, rootIndex);
    treeView->setAnimated(wasAnimated); // restoring the view to how it was before
}


void GenericObjectInspector::expandNodes(const QSet<QString> &ids, const QModelIndex &index)
{
    if (ids.contains(static_cast<TreeNode *>(index.internalPointer())->getNodeIdentifier())) {
        treeView->expand(index);

        int numChildren = sourceModel->rowCount(index);
        for (int i = 0; i < numChildren; ++i)
            expandNodes(ids, sourceModel->index(i, 0, index));
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
        TreeNode *node = static_cast<TreeNode *>(i.internalPointer());
        if (node->gatherDataIfMissing()) {
            // not doing it, super slow, see caller
            //Q_EMIT dataChanged(i, i);
            changed = true;
        }
    }
    return changed;
}

void GenericObjectInspector::doSetObject(cObject *obj)
{
    Inspector::doSetObject(obj);

    if (!obj) {
        recreateModel();
        return;
    }

    QSet<QString> expanded = getExpandedNodes();

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

    if (subtreeModeOverlay->isVisible()) {
        QPoint viewportPos = treeView->viewport()->mapFromGlobal(QCursor::pos());
        updateSubtreeModeOverlay(viewportPos);
    }
}

}  // namespace qtenv
}  // namespace omnetpp

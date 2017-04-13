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
#include "omnetpp/cregistrationlist.h"
#include "common/stringutil.h"
#include "qtenv.h"
#include "inspectorfactory.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "genericobjectinspector.h"
#include "genericobjecttreemodel.h"
#include "inspectorutil.h"
#include "envir/objectprinter.h"
#include "envir/visitor.h"
#include <QTreeView>
#include <QDebug>
#include <QGridLayout>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextLayout>
#include <QMessageBox>

#define emit

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

//---- HighlighterItemDelegate declaration ----

// uses a QTextLayout to highlight a part of the displayed text
// which is given by a HighlightRegion, returned by the tree model
class HighlighterItemDelegate : public QStyledItemDelegate
{
  public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex& index) const;
};

//---- GenericObjectInspector implementation ----

const std::vector<std::string> GenericObjectInspector::containerTypes = {
    "cArray", "cQueue", "cFutureEventSet", "cSimpleModule",
    "cModule", "cChannel", "cRegistrationList", "cCanvas"
};

const QString GenericObjectInspector::PREF_MODE = "mode";

GenericObjectInspector::GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    treeView = new QTreeView(this);
    treeView->setHeaderHidden(true);
    treeView->setItemDelegate(new HighlighterItemDelegate());
    treeView->setAttribute(Qt::WA_MacShowFocusRect, false);

    QVBoxLayout *layout = new QVBoxLayout(this);
    QToolBar *toolbar = new QToolBar();

    if (!isTopLevel) {
        // aligning right
        QWidget *spacer = new QWidget();
        spacer->setAutoFillBackground(true);
        spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        toolbar->addWidget(spacer);
    }

    addModeActions(toolbar);
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

    doSetMode(mode);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(onTreeViewActivated(QModelIndex)));
}

GenericObjectInspector::~GenericObjectInspector()
{
    delete model;
}

void GenericObjectInspector::addModeActions(QToolBar *toolbar)
{
    // mode selection
    toGroupedModeAction = toolbar->addAction(QIcon(":/tools/treemode_grouped"), "Switch to grouped mode", this, SLOT(toGroupedMode()));
    toGroupedModeAction->setCheckable(true);
    toFlatModeAction = toolbar->addAction(QIcon(":/tools/treemode_flat"), "Switch to flat mode", this, SLOT(toFlatMode()));
    toFlatModeAction->setCheckable(true);
    toChildrenModeAction = toolbar->addAction(QIcon(":/tools/treemode_children"), "Switch to children mode", this, SLOT(toChildrenMode()));
    toChildrenModeAction->setCheckable(true);
    toInheritanceModeAction = toolbar->addAction(QIcon(":/tools/treemode_inher"), "Switch to inheritance mode", this, SLOT(toInheritanceMode()));
    toInheritanceModeAction->setCheckable(true);
}

void GenericObjectInspector::recreateModel()
{
    GenericObjectTreeModel *newModel = new GenericObjectTreeModel(object, mode, this);
    treeView->setModel(newModel);
    treeView->reset();

    // expanding the top level item
    treeView->expand(newModel->index(0, 0, QModelIndex()));

    delete model;
    model = newModel;

    connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onDataChanged()));
}

void GenericObjectInspector::doSetMode(Mode mode)
{
    if (this->mode != mode) {
        this->mode = mode;
        setPref(PREF_MODE, (int)mode);
    }

    toGroupedModeAction->setChecked(mode == Mode::GROUPED);
    toFlatModeAction->setChecked(mode == Mode::FLAT);
    toChildrenModeAction->setChecked(mode == Mode::CHILDREN);
    toInheritanceModeAction->setChecked(mode == Mode::INHERITANCE);

    recreateModel();
}

void GenericObjectInspector::mousePressEvent(QMouseEvent *event)
{
    switch (event->button()) {
        case Qt::XButton1: goBack(); break;
        case Qt::XButton2: goForward(); break;
        default: /* shut up, compiler! */ break;
    }
}

void GenericObjectInspector::closeEvent(QCloseEvent *event)
{
    Inspector::closeEvent(event);
    setPref(PREF_MODE, (int)mode);
}

void GenericObjectInspector::onTreeViewActivated(QModelIndex index)
{
    auto object = model->getCObjectPointer(index);
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

void GenericObjectInspector::onDataChanged()
{
    getQtenv()->callRefreshDisplaySafe();
    getQtenv()->callRefreshInspectors();
}

void GenericObjectInspector::createContextMenu(QPoint pos)
{
    cObject *object = model->getCObjectPointer(treeView->indexAt(pos));
    if (object) {
        QVector<cObject *> objects;
        objects.push_back(object);
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(treeView->mapToGlobal(pos));
        delete menu;
    }
}

void GenericObjectInspector::setMode(Mode mode)
{
    if (this->mode != mode)
        doSetMode(mode);
}

void GenericObjectInspector::doSetObject(cObject *obj)
{
    QSet<QString> expanded = model->getExpandedNodesIn(treeView);

    Inspector::doSetObject(obj);

    auto defaultMode = Mode::GROUPED;

    if (obj
        && obj != getObject()
        && std::count(containerTypes.begin(), containerTypes.end(), getObjectBaseClass(obj))) {
        defaultMode = Mode::CHILDREN;
    }

    // will recreate the model for the new object
    doSetMode((Mode)getPref(PREF_MODE, (int)defaultMode).toInt());

    model->expandNodesIn(treeView, expanded);
}

void GenericObjectInspector::refresh()
{
    Inspector::refresh();
    doSetObject(object);
}

//---- HighlighterItemDelegate implementation ----

void HighlighterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // drawing the selection background and focus rectangle, but no text
    QStyledItemDelegate::paint(painter, option, QModelIndex());

    const int spacing = 3;

    // selecting the palette to use, depending on the item state
    QPalette::ColorGroup group = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (group == QPalette::Normal && !(option.state & QStyle::State_Active))
        group = QPalette::Inactive;

    painter->save();

    // getting the icon for the object, and if found, offsetting the text and drawing the icon
    int textOffset = spacing;
    auto iconData = index.data(Qt::DecorationRole);
    if (iconData.isValid()) {
        textOffset += option.decorationSize.width() + spacing * 2;
        QIcon icon = iconData.value<QIcon>();
        painter->drawImage(option.rect.topLeft()
                             + QPoint(spacing, option.rect.height() / 2
                                               - option.decorationSize.height() / 2),
                           icon.pixmap(option.decorationSize).toImage());
    }

    // Text from item
    QString text = index.data(Qt::DisplayRole).toString();

    // the formatted regions
    QList<QTextLayout::FormatRange> formats;

    QTextLayout::FormatRange f;

    // this sets the color of the whole text depending on whether the item is selected or not
    f.format.setForeground(option.palette.brush(group,
        (option.state & QStyle::State_Selected) ? QPalette::HighlightedText : QPalette::Text));
    f.start = 0;
    f.length = text.length();
    formats.append(f);

    // no highlighting on selected items, it was not well readable
    if (!(option.state & QStyle::State_Selected)) {
        // and then adding another format region to highlight the range specified by the model
        HighlightRange range = index.data(Qt::UserRole).value<HighlightRange>();
        f.start = range.start;
        f.length = range.length;
        f.format.setForeground(QBrush(QColor(0, 0, 255)));
        // f.format.setFontWeight(QFont::Bold); // - just causes complications everywhere (elision, editor width, etc.)
        formats.append(f);
    }

    QTextLayout layout;

    // applying the format ranges
    layout.setAdditionalFormats(formats);

    layout.setFont(painter->font());
    layout.setText(option.fontMetrics.elidedText(text, option.textElideMode, option.rect.width() - textOffset - 3));
    // this is the standard layout procedure in a single line case
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(option.rect.width());
    layout.endLayout();

    // the layout is complete, now we just draw it on the appropriate position
    layout.draw(painter, option.rect.bottomLeft() + QPoint(textOffset, -option.fontMetrics.height()));
    painter->restore();
}

void HighlighterItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // setting the initial geometry which covers the entire line
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    // extracting the value from the index
    QString wholeText = index.data().toString();
    QString editorText = index.data(Qt::EditRole).toString();

    // The editor will cover the "highlighted" (blue) part of the text.
    // This is not perfect semantically, but not an entirely wrong guess,
    // since in editable items usually only the value itself is highlighted.
    // And even if we get it wrong, not much depends on it, it will just look weird.
    // (Experimented with substring search, that failed when integer values
    // were found where the matching array index was... And we could add
    // another UserRole, and get it from the model that way, but why bother...)
    HighlightRange range = index.data(Qt::UserRole).value<HighlightRange>();
    // this is where the editor should start
    int editorLeft = option.fontMetrics.width(wholeText.left(range.start));

    // and this is how wide it should be
    int editorWidth = option.fontMetrics.width(editorText);

    // moving the editor horizontally and setting its width as computed
    auto geom = editor->geometry();
    geom.translate(editorLeft, 0);
    geom.setWidth(std::max(20, editorWidth));  // so empty values can be edited too
    editor->setGeometry(geom);
}

void HighlighterItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex& index) const
{
    try {
        QStyledItemDelegate::setModelData(editor, model, index);
    }
    catch (std::exception& e) {
        QMessageBox::warning(editor, "Error editing item: " + index.data().toString(), e.what(), QMessageBox::StandardButton::Ok);
    }
}

}  // namespace qtenv
}  // namespace omnetpp


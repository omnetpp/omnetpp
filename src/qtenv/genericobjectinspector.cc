//==========================================================================
//  GENERICOBJECTINSPECTOR.CC - part of
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

#include <cstring>
#include <cmath>
#include "omnetpp/cregistrationlist.h"
#include "qtenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "moduleinspector.h"
#include "loginspector.h"
#include "genericobjectinspector.h"
#include "genericobjecttreemodel.h"
#include "inspectorutil.h"
#include "inspectorlistbox.h"
#include "inspectorlistboxview.h"
#include "envir/objectprinter.h"
#include "envir/visitor.h"
#include <QTreeView>
#include <QDebug>
#include <QGridLayout>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTextLayout>
#include <QMessageBox>
#include <QTableWidget>
#include <QLabel>

#define emit

namespace omnetpp {
namespace qtenv {

void _dummy_for_genericobjectinspector() {}

class GenericObjectInspectorFactory : public InspectorFactory
{
  public:
    GenericObjectInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return true; }
    int getInspectorType() override { return INSP_OBJECT; }
    double getQualityAsDefault(cObject *object) override { return 1.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new GenericObjectInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(GenericObjectInspectorFactory);

// ---- HighlighterItemDelegate declaration ----

// uses a QTextLayout to highlight a part of the displayed text
// which is given by a HighlightRegion, returned by the tree model
class HighlighterItemDelegate : public QStyledItemDelegate {
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};


// ---- GenericObjectInspector implementation ----

GenericObjectInspector::GenericObjectInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f) {
    icon = new QLabel(this);
    name = new QLabel("N/A", this);

    treeView = new QTreeView(this);
    treeView->setHeaderHidden(true);
    treeView->setItemDelegate(new HighlighterItemDelegate());

    listModel = new InspectorListBox();
    InspectorListBoxView *list = new InspectorListBoxView(this);
    list->setModel(listModel);
    list->setColumnWidth(0, 75);
    list->setColumnWidth(1, 75);
    list->setColumnWidth(2, 0);

    connect(list, SIGNAL(activated(QModelIndex)), this, SLOT(onListActivated(QModelIndex)));

    tabs = new QTabWidget(this);
    tabs->addTab(treeView, "Fields");
    tabs->addTab(list, "Contents");
    tabs->setDocumentMode(true); // makes it prettier

    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);

    auto titleLayout = new QHBoxLayout();
    titleLayout->setMargin(0);

    titleLayout->addWidget(icon);
    titleLayout->addWidget(name, 1);

    layout->addLayout(titleLayout);
    layout->addWidget(tabs, 1);
    layout->setMargin(0);
    parent->setMinimumSize(20, 20);

    model = new GenericObjectTreeModel(nullptr, this);

    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(treeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(createContextMenu(QPoint)));
    connect(treeView, SIGNAL(activated(QModelIndex)), this, SLOT(onTreeViewActivated(QModelIndex)));
}

GenericObjectInspector::~GenericObjectInspector() {
    delete model;
}

void GenericObjectInspector::mousePressEvent(QMouseEvent *event) {
    switch (event->button()) {
    case Qt::XButton1: goBack();      break;
    case Qt::XButton2: goForward();   break;
    default: /* shut up, compiler! */ break;
    }
}

void GenericObjectInspector::onTreeViewActivated(QModelIndex index) {
    auto object = model->getCObjectPointer(index);
    if (object)
        emit objectPicked(object);
}

void GenericObjectInspector::onListActivated(QModelIndex index) {
    setObject(index.data(Qt::UserRole).value<cObject*>());
}

void GenericObjectInspector::onDataChanged() {
    getQtenv()->refreshInspectors();
}

void GenericObjectInspector::createContextMenu(QPoint pos) {
    cObject *object = model->getCObjectPointer(treeView->indexAt(pos));
    if (object) {
        QVector<cObject*> objects;
        objects.push_back(object);
        QMenu *menu = InspectorUtil::createInspectorContextMenu(objects, this);
        menu->exec(mapToGlobal(pos));
        delete menu;
    }
}

void GenericObjectInspector::doSetObject(cObject *obj) {
    Inspector::doSetObject(obj);

    GenericObjectTreeModel *newModel = new GenericObjectTreeModel(obj, this);
    treeView->setModel(newModel);
    treeView->reset();

    // expanding the top level item
    treeView->expand(newModel->index(0, 0, QModelIndex()));

    delete model;
    model = newModel;

    connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(onDataChanged()));

    QVector<cObject*> children;

    if (obj) {
        cCollectChildrenVisitor visitor(obj);
        visitor.process(obj);

        for (int i = 0; i < visitor.getArraySize(); ++i)
            children.push_back(visitor.getArray()[i]);

        name->setText(obj->getFullName());
        icon->setPixmap(QPixmap(":/objects/icons/objects/" + getObjectIcon(obj)));
    } else {
        name->setText("N/A");
        icon->setPixmap(QPixmap(":/objects/icons/objects/none_vs"));
    }

    listModel->setObjects(children);
    tabs->setTabText(1, QString("Contents (%1)").arg(children.size()));
}

void GenericObjectInspector::refresh() {
    Inspector::refresh();

    QSet<QString> expanded = model->getExpandedNodesIn(treeView);

    doSetObject(object);

    model->expandNodesIn(treeView, expanded);
}

// ---- HighlighterItemDelegate implementation ----

void HighlighterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // drawing the selection background and focus rectangle, but no text
    QStyledItemDelegate::paint(painter, option, QModelIndex());

    // selecting the palette to use, depending on the item state
    QPalette::ColorGroup group = option.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
    if (group == QPalette::Normal && !(option.state & QStyle::State_Active))
        group = QPalette::Inactive;

    painter->save();

    // getting the icon for the object, and if found, offsetting the text and drawing the icon
    int textOffset = 0;
    auto iconData = index.data(Qt::DecorationRole);
    if (iconData.isValid()) {
        textOffset += option.decorationSize.width();
        QIcon icon = iconData.value<QIcon>();
        painter->drawImage(option.rect.topLeft(), icon.pixmap(option.decorationSize).toImage());
    }

    //Text from item
    QString text = index.data(Qt::DisplayRole).toString();
    QTextLayout layout;
    layout.setText(option.fontMetrics.elidedText(text, option.textElideMode, option.rect.width() - textOffset - 3));
    // this is the standard layout procedure in a single line case
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(option.rect.width());
    layout.endLayout();

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

    // applying the format ranges
    layout.setAdditionalFormats(formats);

    // the layout is complete, now we just draw it on the appropriate position
    layout.draw(painter, option.rect.translated(3 + textOffset, 1).topLeft());
    painter->restore();
}

void HighlighterItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // setting the initial geometry which covers the entire line
    QStyledItemDelegate::updateEditorGeometry(editor, option, index);

    // extracting the value from the index
    QString wholeText = index.data().toString();
    QString editorText = index.data(Qt::EditRole).toString();

    // searching for the start of the value - if not found, it will be 2... which would still work
    int startIndex = wholeText.indexOf(" = ") + 3;

    // this is where the editor should start
    int editorLeft = option.fontMetrics.width(wholeText.left(startIndex));

    // and this is how wide it should be
    int editorWidth = option.fontMetrics.width(editorText);

    // moving the editor horizontally and setting its width as computed
    auto geom = editor->geometry();
    geom.translate(editorLeft, 0);
    geom.setWidth(qMax(20, editorWidth)); // so empty values can be edited too
    editor->setGeometry(geom);
}

void HighlighterItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    try {
        QStyledItemDelegate::setModelData(editor, model, index);
    } catch (std::exception &e) {
        QMessageBox::warning(editor, "Error editing item: " + index.data().toString(), e.what(), QMessageBox::StandardButton::Ok);
    }
}

} // namespace qtenv
} // namespace omnetpp


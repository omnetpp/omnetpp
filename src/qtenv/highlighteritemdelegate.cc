//==========================================================================
//  HIGHLIGHTERITEMDELEGATE.CC - part of
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

#include "highlighteritemdelegate.h"
#include "genericobjecttreemodel.h" // for HighlightRange
#include <QtGui/QPainter>
#include <QtGui/QTextLayout>
#include <QtWidgets/QMessageBox>

#define emit // technicalities...

namespace omnetpp {
namespace qtenv {

void HighlighterItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    // drawing the selection background and focus rectangle, but no text
    // adapted from void QStyledItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        QStyle *style = option.widget->style();
        opt.text = ""; // we will do it manually later
        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, option.widget);
    }

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
    QVector<QTextLayout::FormatRange> formats;

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
        f.format.setForeground(option.palette.brush(group, QPalette::Link));
        formats.append(f);
    }

    QTextLayout layout;

    // applying the format ranges
    layout.setFormats(formats);

    layout.setFont(option.font);
    layout.setText(option.fontMetrics.elidedText(text, option.textElideMode, option.rect.width() - textOffset - 3));
    // this is the standard layout procedure in a single line case
    layout.beginLayout();
    QTextLine line = layout.createLine();
    line.setLineWidth(option.rect.width());
    layout.endLayout();

    // the layout is complete, now we just draw it on the appropriate position
    layout.draw(painter, option.rect.topLeft() + QPoint(textOffset, option.rect.height() / 2 - layout.boundingRect().height() / 2));
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

QWidget *HighlighterItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    emit editorCreated();
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void HighlighterItemDelegate::destroyEditor(QWidget *editor, const QModelIndex& index) const
{
    emit editorDestroyed();
    QStyledItemDelegate::destroyEditor(editor, index);
}

}  // namespace qtenv
}  // namespace omnetpp

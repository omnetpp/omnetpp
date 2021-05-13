//==========================================================================
//  HIGHLIGHTERITEMDELEGATE.H - part of
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

#ifndef __OMNETPP_QTENV_HIGHLIGHTERITEMDELEGATE_H
#define __OMNETPP_QTENV_HIGHLIGHTERITEMDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include "qtenvdefs.h"

namespace omnetpp {
namespace qtenv {

// uses a QTextLayout to highlight a part of the displayed text
// which is given by a HighlightRange, returned by the tree model
class QTENV_API HighlighterItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

  signals:
    void editorCreated() const;
    void editorDestroyed() const;
  public:
    using QStyledItemDelegate::QStyledItemDelegate;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex& index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const;
    void destroyEditor(QWidget *editor, const QModelIndex &index) const;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_HIGHLIGHTERITEMDELEGATE_H

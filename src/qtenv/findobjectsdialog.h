//==========================================================================
//  FINDOBJECTSDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_FINDOBJECTSDIALOG_H
#define __OMNETPP_QTENV_FINDOBJECTSDIALOG_H

#include <QtWidgets/QDialog>
#include <QtCore/QItemSelection>
#include <QtCore/QModelIndex>
#include "objectlistmodel.h"
#include "qtenvdefs.h"

namespace Ui {
class FindObjectsDialog;
}

namespace omnetpp {

class cObject;

namespace qtenv {

class ObjectListView;

class QTENV_API FindObjectsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FindObjectsDialog(cObject *ptr, QWidget *parent = nullptr);
    ~FindObjectsDialog();

public Q_SLOTS:
    void invalidate();

private Q_SLOTS:
    void refresh();
    void inspect(QModelIndex index);
    void onListBoxSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onFontChanged();

private:
    Ui::FindObjectsDialog *ui;

    static QString classNamePatternHelp;
    static QString namePatternHelp;

    void setPref(const QString& key, const QVariant& value);
    QVariant getPref(const QString& key, const QVariant& defaultValue = QVariant());

    ObjectListModel *listModel; // the model of the object list
    ObjectListView *listView; // the list widget itself

    QStringList getClassNames();
    void checkPattern(const char *pattern);
    cObject **getSubObjectsFilt(cObject *object, const char *classNamePattern, const char *objFullPathPattern, int maxCount, int &num);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_FINDOBJECTSDIALOG_H

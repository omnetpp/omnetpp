//==========================================================================
//  FILTEREDOBJECTLISTDIALOG.H - part of
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

#ifndef __OMNETPP_QTENV_FILTEREDOBJECTLISTDIALOG_H
#define __OMNETPP_QTENV_FILTEREDOBJECTLISTDIALOG_H

#include <QDialog>
#include <QItemSelection>
#include <QModelIndex>

namespace Ui {
class FilteredObjectListDialog;
}

namespace omnetpp {

class cObject;

namespace qtenv {

class InspectorListBoxView;

class FilteredObjectListDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FilteredObjectListDialog(cObject *ptr, QWidget *parent = 0);
    ~FilteredObjectListDialog();

    void setSearchEdit(cObject *obj);

private slots:
    void refresh();
    void inspect(QModelIndex index);
    void onListBoxSelectionChanged(QItemSelection selected, QItemSelection deselected);
    void onFontChanged();

public slots:
    virtual void done(int r);

private:
    Ui::FilteredObjectListDialog *ui;

    static QString classNamePattern;
    static QString namePattern;

    InspectorListBoxView *inspectorListBoxView;

    QStringList getClassNames();
    void checkPattern(const char *pattern);
    cObject **getSubObjectsFilt(cObject *object, const char *classNamePattern, const char *objFullPathPattern, int maxCount, int &num);
};

} // namespace qtenv
} // namespace omnetpp

#endif // __OMNETPP_QTENV_FILTEREDOBJECTLISTDIALOG_H

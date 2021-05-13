//==========================================================================
//  FILEEDITOR.H - part of
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

#ifndef __OMNETPP_QTENV_FILEEDITOR_H
#define __OMNETPP_QTENV_FILEEDITOR_H

#include <QtWidgets/QDialog>
#include <QtCore/QFile>
#include "qtenvdefs.h"

namespace Ui {
class fileEditor;
}

class QMenu;

namespace omnetpp {
namespace qtenv {

class TextViewerWidget;

class QTENV_API FileEditor : public QDialog
{
    Q_OBJECT

private slots:
    void save();
    void find();
    void onCustomContextMenuRequested(const QPoint &pos);
    void wrapLines();
    void findNext();

public:
    explicit FileEditor(QWidget *parent = 0);
    ~FileEditor();

    void setFile(QString fileName);
    void show();
    void reject() override;

private:
    Ui::fileEditor *ui;
    QFile file;
    QMenu *contextMenu = nullptr;

    QAction *wrapLinesAction;
    QAction *copySelectionAction;
    QAction *findAction;
    QAction *findNextAction;
    QAction *saveAction;

    QString searchString;
    int findOptions = 0;

    void addToolBar();
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_FILEEDITOR_H

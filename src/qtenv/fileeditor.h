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
#include "qtenvdefs.h"

namespace Ui {
class fileEditor;
}

class QMenu;

namespace omnetpp {
namespace qtenv {

class QTENV_API FileEditor : public QDialog
{
    Q_OBJECT

private Q_SLOTS:
    void save();
    void find();
    void onCustomContextMenuRequested(const QPoint &pos);
    void wrapLines();
    void findNext();
    void updateFont();

public:
    explicit FileEditor(QWidget *parent = nullptr);
    ~FileEditor();

    void setFile(const QString &fileName);
    void setContent(const QString &content);
    void setTitle(const QString &title) { setWindowTitle(title); }
    void setDisplayFilename(const QString &title);
    void setReadOnly(bool readOnly) { this->readOnly = readOnly; updateSaveActionState(); }

    bool readFile();
    void show();
    void reject() override;

private:
    Ui::fileEditor *ui;
    QString fileName;
    bool readOnly = false;
    bool hasFile = false;

    QMenu *contextMenu = nullptr;

    QAction *wrapLinesAction;
    QAction *copySelectionAction;
    QAction *findAction;
    QAction *findNextAction;
    QAction *saveAction;

    QString searchString;
    int findOptions = 0;

    void addToolBar();
    void updateSaveActionState();  // Helper to update Save action state
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_FILEEDITOR_H

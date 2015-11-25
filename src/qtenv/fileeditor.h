//==========================================================================
//  FILEEDITOR.H - part of
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

#ifndef FILEEDITOR_H
#define FILEEDITOR_H

#include <QDialog>
#include <QFile>

namespace Ui {
class fileEditor;
}

class QMenu;

namespace omnetpp {
namespace qtenv {

class TextViewerWidget;

class FileEditor : public QDialog
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
    QMenu *contextMenu;
    QAction *wrapLinesAction;
    QString searchString;
    int findOptions;

    void addToolBar();
};

} // namespace qtenv
} // namespace omnetpp

#endif // FILEEDITOR_H

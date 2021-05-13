//==========================================================================
//  VIDEORECORDINGDIALOG.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2018 Andras Varga
  Copyright (C) 2006-2018 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_VIDEORECORDINGDIALOG_H
#define __OMNETPP_QTENV_VIDEORECORDINGDIALOG_H

#include <QtWidgets/QDialog>
#include "mainwindow.h"

namespace Ui {
class VideoRecordingDialog;
}

namespace omnetpp {
namespace qtenv {

class QTENV_API VideoRecordingDialog : public QDialog
{
    Q_OBJECT

    QString configRun;

    QString extension;
    QString pixelFormat;
    QRect cropArea; // "null" if full window recording is chosen

    const QString PREF_EXTENSION = "video-recording-extension";
    const QString PREF_PIXELFORMAT = "video-recording-pixelformat";
    const QString PREF_CROPAREA = "video-recording-croparea";

    const QString PREFVAL_WINDOW = "whole-window";
    const QString PREFVAL_NETWORK = "network-area";
    const QString PREFVAL_BORDER = "with-border";
    const QString PREFVAL_PADDING = "with-padding";

    static QRect adjustModuleCropArea(QRect modRect, MainWindow *mainWindow, ModuleInspector *moduleInspector);

public:
    explicit VideoRecordingDialog(MainWindow *mainWindow, QString configRun);
    ~VideoRecordingDialog() override;

    QString makeEncodingCommand();
    void updateCommandLabel();

    QSize sizeHint() const override { return QSize(500, 700); }

    void accept() override;

private:
    Ui::VideoRecordingDialog *ui;
};

}  // namespace qtenv
}  // namespace omnetpp

#endif // __OMNETPP_QTENV_VIDEORECORDINGDIALOG_H

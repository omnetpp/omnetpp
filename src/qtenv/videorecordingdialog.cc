//==========================================================================
//  VIDEORECORDINGDIALOG.CC - part of
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

#include "videorecordingdialog.h"
#include "ui_videorecordingdialog.h"

#include <QtGui/QClipboard>

#include "moduleinspector.h"

namespace omnetpp {
namespace qtenv {

VideoRecordingDialog::VideoRecordingDialog(MainWindow *mainWindow, QString configRun) :
    QDialog(mainWindow),
    configRun(configRun),
    ui(new Ui::VideoRecordingDialog)
{
    ui->setupUi(this);

    connect(ui->mp4, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            extension = "mp4";
            updateCommandLabel();
        }
    });
    connect(ui->mkv, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            extension = "mkv";
            updateCommandLabel();
        }
    });
    connect(ui->sharper, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            pixelFormat = "yuv444p";
            updateCommandLabel();
        }
    });
    connect(ui->compatible, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            pixelFormat = "yuv420p";
            updateCommandLabel();
        }
    });

    connect(ui->window, &QRadioButton::toggled, [this](bool checked) {
        if (checked) {
            cropArea = QRect();
            updateCommandLabel();
        }
    });

    ModuleInspector *moduleInspector = getQtenv()->getMainModuleInspector();

    connect(ui->padding, &QRadioButton::toggled, [this, mainWindow, moduleInspector](bool checked) {
        if (checked) {
            QRect modRect = moduleInspector->getModuleRect(true, 10);
            cropArea = adjustModuleCropArea(modRect, mainWindow, moduleInspector);
            updateCommandLabel();
        }
    });

    connect(ui->border, &QRadioButton::toggled, [this, mainWindow, moduleInspector](bool checked) {
        if (checked) {
            QRect modRect = moduleInspector->getModuleRect(true, 0);
            cropArea = adjustModuleCropArea(modRect, mainWindow, moduleInspector);
            updateCommandLabel();
        }
    });

    connect(ui->network, &QRadioButton::toggled, [this, mainWindow, moduleInspector](bool checked) {
        if (checked) {
            QRect modRect = moduleInspector->getModuleRect(false, 0);
            cropArea = adjustModuleCropArea(modRect, mainWindow, moduleInspector);
            updateCommandLabel();
        }
    });

    connect(ui->copyButton, &QPushButton::pressed, [this]{
        QApplication::clipboard()->setText(makeEncodingCommand());
    });

    ui->topLabel->setText(ui->topLabel->text().replace("${BASE}", configRun + "_"));
    adjustSize();

    extension = getQtenv()->getPref(PREF_EXTENSION, "mp4").toString();
    pixelFormat = getQtenv()->getPref(PREF_PIXELFORMAT, "yuv420p").toString();

    if (extension == "mp4") ui->mp4->setChecked(true);
    if (extension == "mkv") ui->mkv->setChecked(true);

    if (pixelFormat == "yuv420p") ui->compatible->setChecked(true);
    if (pixelFormat == "yuv444p") ui->sharper->setChecked(true);

    QString areaPref = getQtenv()->getPref(PREF_CROPAREA, PREFVAL_WINDOW).toString();

    if (areaPref == PREFVAL_WINDOW) ui->window->setChecked(true);
    if (areaPref == PREFVAL_NETWORK) ui->network->setChecked(true);
    if (areaPref == PREFVAL_BORDER) ui->border->setChecked(true);
    if (areaPref == PREFVAL_PADDING) ui->padding->setChecked(true);

    updateCommandLabel();
}

VideoRecordingDialog::~VideoRecordingDialog()
{
    delete ui;
}

QString VideoRecordingDialog::makeEncodingCommand()
{
    QString cropArg = "";

    float scaleFactor;
    scaleFactor = getQtenv()->getMainWindow()->devicePixelRatioF();

    if (!cropArea.isNull())
        cropArg = QString("-filter:v \"crop=%1:%2:%3:%4\" ")
                .arg(cropArea.width()*scaleFactor).arg(cropArea.height()*scaleFactor)
                .arg(cropArea.x()*scaleFactor).arg(cropArea.y()*scaleFactor);

    // not using QString::arg here, because it has % in it
    return "ffmpeg -r 30 -f image2 -i \"frames/" + configRun + "_%04d.png\" "
            + cropArg + "-vcodec libx264 " "-pix_fmt " + pixelFormat + " "
            + configRun + "." + extension;
}

void VideoRecordingDialog::updateCommandLabel()
{
    ui->commandLabel->setText("<span style=\"font-family:'monospace'; font-weight:600;\">"
                               + makeEncodingCommand() + "</span>");
}

QRect VideoRecordingDialog::adjustModuleCropArea(QRect modRect, MainWindow *mainWindow, ModuleInspector *moduleInspector)
{
    // first clip the modRect to the contentsRect
    modRect = modRect.intersected(moduleInspector->contentsRect());

    // then map the module rectangle from widget space to window space
    modRect.moveTopLeft(moduleInspector->mapTo(mainWindow, modRect.topLeft()));

    // finally make sure its size is divisible by 2
    if (modRect.width() % 2)
        modRect.adjust(0, 0, 1, 0);
    if (modRect.height() % 2)
        modRect.adjust(0, 0, 0, 1);

    return modRect;
}

void VideoRecordingDialog::accept()
{
    if (ui->mp4->isChecked()) getQtenv()->setPref(PREF_EXTENSION, "mp4");
    if (ui->mkv->isChecked()) getQtenv()->setPref(PREF_EXTENSION, "mkv");

    if (ui->compatible->isChecked()) getQtenv()->setPref(PREF_PIXELFORMAT, "yuv420p");
    if (ui->sharper->isChecked()) getQtenv()->setPref(PREF_PIXELFORMAT, "yuv444p");

    if (ui->window->isChecked()) getQtenv()->setPref(PREF_CROPAREA, PREFVAL_WINDOW);
    if (ui->network->isChecked()) getQtenv()->setPref(PREF_CROPAREA, PREFVAL_NETWORK);
    if (ui->border->isChecked()) getQtenv()->setPref(PREF_CROPAREA, PREFVAL_BORDER);
    if (ui->padding->isChecked()) getQtenv()->setPref(PREF_CROPAREA, PREFVAL_PADDING);

    QDialog::accept();
}

}  // namespace qtenv
}  // namespace omnetpp

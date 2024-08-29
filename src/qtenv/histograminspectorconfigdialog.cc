//==========================================================================
//  HISTOGRAMINSPECTORCONFIGDIALOG.CC - part of
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

#include "histograminspectorconfigdialog.h"
#include "ui_histograminspectorconfigdialog.h"
#include <QtWidgets/QPushButton>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>
#include "common/stringutil.h"
#include "inspectorutil.h"

namespace omnetpp {
using namespace common;
namespace qtenv {

HistogramInspectorConfigDialog::HistogramInspectorConfigDialog(HistogramView::DrawingStyle style, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramInspectorConfigDialog)
{
    ui->setupUi(this);

    ui->styleComboBox->setCurrentIndex(style);

    QPushButton *applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(onApplyButtonClicked()));
}

HistogramInspectorConfigDialog::~HistogramInspectorConfigDialog()
{
    delete ui;
}

void HistogramInspectorConfigDialog::onApplyButtonClicked()
{
    if (checkInput())
        Q_EMIT applyButtonClicked();
}

HistogramView::DrawingStyle HistogramInspectorConfigDialog::getDrawingStyle() const
{
    return (HistogramView::DrawingStyle)ui->styleComboBox->currentIndex();
}

bool HistogramInspectorConfigDialog::hasMinY() const
{
    return !ui->yMinLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMinY() const
{
    return opp_atof(ui->yMinLineEdit->text().toLatin1());
}

void HistogramInspectorConfigDialog::setMinY(double minY)
{
    ui->yMinLineEdit->setText(InspectorUtil::formatDouble(minY));
}

bool HistogramInspectorConfigDialog::hasMaxY() const
{
    return !ui->yMaxLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMaxY() const
{
    return opp_atof(ui->yMaxLineEdit->text().toLatin1());
}

void HistogramInspectorConfigDialog::setMaxY(double maxY)
{
    ui->yMaxLineEdit->setText(InspectorUtil::formatDouble(maxY));
}

bool HistogramInspectorConfigDialog::hasMinX() const
{
    return !ui->xMinLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMinX() const
{
    return opp_atof(ui->xMinLineEdit->text().toLatin1());
}

void HistogramInspectorConfigDialog::setMinX(double minX)
{
    ui->xMinLineEdit->setText(InspectorUtil::formatDouble(minX));
}

bool HistogramInspectorConfigDialog::hasMaxX() const
{
    return !ui->xMaxLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMaxX() const
{
    return opp_atof(ui->xMaxLineEdit->text().toLatin1());
}

void HistogramInspectorConfigDialog::setMaxX(double maxX)
{
    ui->xMaxLineEdit->setText(InspectorUtil::formatDouble(maxX));
}

void HistogramInspectorConfigDialog::accept()
{
    if (checkInput())
        QDialog::accept();
}

bool HistogramInspectorConfigDialog::checkInput()
{
    bool ok = true;

    std::string message;

    try {
        bool hasMinX = this->hasMinX();
        double minX = hasMinX ? getMinX() : 0.0;
        bool hasMaxX = this->hasMaxX();
        double maxX = hasMaxX ? getMaxX() : 0.0;

        bool hasMinY = this->hasMinY();
        double minY = hasMinY ? getMinY() : 0.0;
        bool hasMaxY = this->hasMaxY();
        double maxY = hasMaxY ? getMaxY() : 0.0;

        if (hasMaxY && hasMinY && maxY <= minY) {
            message = "Y range: max must be greater than min";
            ok = false;
        }

        if (hasMaxX && hasMinX && maxX <= minX) {
            message = "X range: max must be greater than min";
            ok = false;
        }

    } catch (opp_runtime_error& e) {
        message = e.what();
        ok = false;
    }

    if (!ok)
        QMessageBox::critical(this, "Invalid input", message.c_str());

    return ok;
}

}  // namespace qtenv
}  // namespace omnetpp


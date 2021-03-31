//==========================================================================
//  OUTPUTVECTORINSPECTORCONFIGDIALOG.CC - part of
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

#include "outputvectorinspectorconfigdialog.h"
#include "ui_outputvectorinspectorconfigdialog.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QMessageBox>
#include "common/stringutil.h"
#include "qtenv.h"
#include "inspectorutil.h"

#define emit

namespace omnetpp {
using namespace common;
namespace qtenv {

OutputVectorInspectorConfigDialog::OutputVectorInspectorConfigDialog(VectorPlotItem::PlottingMode plottingMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputVectorInspectorConfigDialog)
{
    ui->setupUi(this);
    ui->styleComboBox->setCurrentIndex(plottingMode);

    connect(ui->yMinLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));
    connect(ui->yMaxLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));
    connect(ui->timeScaleLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));

    QPushButton *applyButton = ui->okCancelButtonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(onApplyButtonClicked()));
}

OutputVectorInspectorConfigDialog::~OutputVectorInspectorConfigDialog()
{
    delete ui;
}

void OutputVectorInspectorConfigDialog::onTextChanged()
{
    QPalette pal;
    pal.setColor(QPalette::Base, Qt::white);
    static_cast<QWidget *>(sender())->setPalette(pal);
}

VectorPlotItem::PlottingMode OutputVectorInspectorConfigDialog::getPlottingMode() const
{
    return (VectorPlotItem::PlottingMode)ui->styleComboBox->currentIndex();
}

bool OutputVectorInspectorConfigDialog::hasMinY() const
{
    return !ui->yMinLineEdit->text().isEmpty();
}

double OutputVectorInspectorConfigDialog::getMinY() const
{
    return opp_atof(ui->yMinLineEdit->text().toLatin1());
}

void OutputVectorInspectorConfigDialog::setMinY(const double minY)
{
    ui->yMinLineEdit->setText(InspectorUtil::formatDouble(minY));
}

bool OutputVectorInspectorConfigDialog::hasMaxY() const
{
    return !ui->yMaxLineEdit->text().isEmpty();
}

double OutputVectorInspectorConfigDialog::getMaxY() const
{
    return opp_atof(ui->yMaxLineEdit->text().toLatin1());
}

void OutputVectorInspectorConfigDialog::setMaxY(const double maxY)
{
    ui->yMaxLineEdit->setText(InspectorUtil::formatDouble(maxY));
}

bool OutputVectorInspectorConfigDialog::hasTimeScale() const
{
    return !ui->timeScaleLineEdit->text().isEmpty();
}

double OutputVectorInspectorConfigDialog::getTimeScale() const
{
    return opp_atof(ui->timeScaleLineEdit->text().toLatin1());
}

void OutputVectorInspectorConfigDialog::setTimeScale(const double timeScale)
{
    ui->timeScaleLineEdit->setText(InspectorUtil::formatDouble(timeScale));
}

void OutputVectorInspectorConfigDialog::onApplyButtonClicked()
{
    if (checkInput())
        emit applyButtonClicked();
}

void OutputVectorInspectorConfigDialog::accept()
{
    if (checkInput())
        QDialog::accept();
}

bool OutputVectorInspectorConfigDialog::checkInput()
{
    bool ok = true;

    std::string message;

    try {
        bool hasMinY = this->hasMinY();
        double minY = hasMinY ? getMinY() : 0.0;
        bool hasMaxY = this->hasMaxY();
        double maxY = hasMaxY ? getMaxY() : 0.0;

        bool hasTimeScale = this->hasTimeScale();
        double timeScale = hasTimeScale ? getTimeScale() : 0.0;

        if (hasMaxY && hasMinY && maxY <= minY) {
            message = "Y range: max must be greater than min";
            ok = false;
        }

        if (hasTimeScale && timeScale < 0) {
            message = "Time span must be positive";
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


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
#include <QPushButton>
#include "qtenv.h"
#include "inspectorutil.h"

#define emit

static int DOUBLE_MAX_PRECISION = 12;
static QColor ERROR_COLOR(216, 108, 112);

namespace omnetpp {
namespace qtenv {

OutputVectorInspectorConfigDialog::OutputVectorInspectorConfigDialog(VectorPlotItem::PlottingMode plottingMode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OutputVectorInspectorConfigDialog)
{
    ui->setupUi(this);
    ui->styleComboBox->setCurrentIndex(plottingMode);

    QDoubleValidator *validator = new QDoubleValidator();
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setBottom(0);
    ui->timeScaleLineEdit->setValidator(validator);

    validator = new QDoubleValidator();
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->yMinLineEdit->setValidator(validator);
    ui->yMaxLineEdit->setValidator(validator);

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
    return ui->yMinLineEdit->text().toDouble();
}

void OutputVectorInspectorConfigDialog::setMinY(const double minY)
{
    ui->yMinLineEdit->setText(InspectorUtil::doubleToQString(minY, DOUBLE_MAX_PRECISION));
}

bool OutputVectorInspectorConfigDialog::hasMaxY() const
{
    return !ui->yMaxLineEdit->text().isEmpty();
}

double OutputVectorInspectorConfigDialog::getMaxY() const
{
    return ui->yMaxLineEdit->text().toDouble();
}

void OutputVectorInspectorConfigDialog::setMaxY(const double maxY)
{
    ui->yMaxLineEdit->setText(InspectorUtil::doubleToQString(maxY, DOUBLE_MAX_PRECISION));
}

bool OutputVectorInspectorConfigDialog::hasTimeScale() const
{
    return !ui->timeScaleLineEdit->text().isEmpty();
}

double OutputVectorInspectorConfigDialog::getTimeScale() const
{
    return ui->timeScaleLineEdit->text().toDouble();
}

void OutputVectorInspectorConfigDialog::setTimeScale(const double timeScale)
{
    ui->timeScaleLineEdit->setText(InspectorUtil::doubleToQString(timeScale, DOUBLE_MAX_PRECISION));
}

void OutputVectorInspectorConfigDialog::onApplyButtonClicked()
{
    if (inputCheck())
        emit applyButtonClicked();
}

void OutputVectorInspectorConfigDialog::accept()
{
    if (inputCheck())
        QDialog::accept();
}

bool OutputVectorInspectorConfigDialog::inputCheck()
{
    QPalette pal;
    pal.setColor(QPalette::Base, Qt::white);
    ui->timeScaleLineEdit->setPalette(pal);
    ui->yMaxLineEdit->setPalette(pal);
    ui->yMinLineEdit->setPalette(pal);

    pal.setColor(QPalette::Base, ERROR_COLOR);
    bool ok = true;

    if (hasTimeScale() && getTimeScale() == 0) {
        ui->timeScaleLineEdit->setPalette(pal);
        ok = false;
    }

    if (hasMinY() && hasMaxY() && (getMinY() >= getMaxY())) {
        ui->yMaxLineEdit->setPalette(pal);
        ui->yMinLineEdit->setPalette(pal);
        ok = false;
    }

    return ok;
}

}  // namespace qtenv
}  // namespace omnetpp


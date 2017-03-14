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
#include <QPushButton>
#include <QDebug>
#include "inspectorutil.h"

static int DOUBLE_MAX_PRECISION = 12;
static QColor ERROR_COLOR(216, 108, 112);

#define emit

namespace omnetpp {
namespace qtenv {

HistogramInspectorConfigDialog::HistogramInspectorConfigDialog(HistogramView::DrawingStyle style, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HistogramInspectorConfigDialog)
{
    ui->setupUi(this);

    ui->styleComboBox->setCurrentIndex(style);

    QDoubleValidator *validator = new QDoubleValidator();
    validator->setNotation(QDoubleValidator::StandardNotation);
    ui->yMinLineEdit->setValidator(validator);
    ui->yMaxLineEdit->setValidator(validator);
    ui->xMaxLineEdit->setValidator(validator);
    ui->xMinLineEdit->setValidator(validator);

    connect(ui->yMinLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));
    connect(ui->yMaxLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));
    connect(ui->xMinLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));
    connect(ui->xMaxLineEdit, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged()));

    QPushButton *applyButton = ui->buttonBox->button(QDialogButtonBox::Apply);
    connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(onApplyButtonClicked()));
}

HistogramInspectorConfigDialog::~HistogramInspectorConfigDialog()
{
    delete ui;
}

void HistogramInspectorConfigDialog::onTextChanged()
{
    QPalette pal;
    pal.setColor(QPalette::Base, Qt::white);
    static_cast<QWidget *>(sender())->setPalette(pal);
}

void HistogramInspectorConfigDialog::onApplyButtonClicked()
{
    if (checkInput())
        emit applyButtonClicked();
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
    return ui->yMinLineEdit->text().toDouble();
}

void HistogramInspectorConfigDialog::setMinY(const double minY)
{
    ui->yMinLineEdit->setText(InspectorUtil::doubleToQString(minY, DOUBLE_MAX_PRECISION));
}

bool HistogramInspectorConfigDialog::hasMaxY() const
{
    return !ui->yMaxLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMaxY() const
{
    return ui->yMaxLineEdit->text().toDouble();
}

void HistogramInspectorConfigDialog::setMaxY(const double maxY)
{
    ui->yMaxLineEdit->setText(InspectorUtil::doubleToQString(maxY, DOUBLE_MAX_PRECISION));
}

bool HistogramInspectorConfigDialog::hasMinX() const
{
    return !ui->xMinLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMinX() const
{
    return ui->xMinLineEdit->text().toDouble();
}

void HistogramInspectorConfigDialog::setMinX(const double minX)
{
    ui->xMinLineEdit->setText(InspectorUtil::doubleToQString(minX, DOUBLE_MAX_PRECISION));
}

bool HistogramInspectorConfigDialog::hasMaxX() const
{
    return !ui->xMaxLineEdit->text().isEmpty();
}

double HistogramInspectorConfigDialog::getMaxX() const
{
    return ui->xMaxLineEdit->text().toDouble();
}

void HistogramInspectorConfigDialog::setMaxX(const double maxX)
{
    ui->xMaxLineEdit->setText(InspectorUtil::doubleToQString(maxX, DOUBLE_MAX_PRECISION));
}

void HistogramInspectorConfigDialog::accept()
{
    if (checkInput())
        QDialog::accept();
}

bool HistogramInspectorConfigDialog::checkInput()
{
    bool ok = true;
    if (hasMaxY() && hasMinY() && getMaxY() <= getMinY()) {
        QPalette pal;
        pal.setColor(QPalette::Base, ERROR_COLOR);
        ui->yMinLineEdit->setPalette(pal);
        ui->yMaxLineEdit->setPalette(pal);

        ok = false;
    }

    if (hasMaxX() && hasMinX() && getMaxX() <= getMinX()) {
        QPalette pal;
        pal.setColor(QPalette::Base, ERROR_COLOR);
        ui->xMinLineEdit->setPalette(pal);
        ui->xMaxLineEdit->setPalette(pal);

        ok = false;
    }

    return ok;
}

}  // namespace qtenv
}  // namespace omnetpp


//==========================================================================
//  AREASELECTORDIALOG.CC - part of
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

#include "areaselectordialog.h"
#include "ui_areaselectordialog.h"

#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

AreaSelectorDialog::AreaSelectorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AreaSelectorDialog)
{
    ui->setupUi(this);

    QString areaPref = getQtenv()->getPref(PREF_KEY_AREA, PREF_VALUE_AREA_BOUNDING_BOX).toString();
    ui->boundingBox->setChecked(areaPref == PREF_VALUE_AREA_BOUNDING_BOX);
    ui->moduleRectangle->setChecked(areaPref == PREF_VALUE_AREA_MODULE_RECTANGLE);
    ui->viewport->setChecked(areaPref == PREF_VALUE_AREA_VIEWPORT);

    int margin = getQtenv()->getPref(PREF_KEY_MARGIN, 20).toInt();
    ui->margin->setValue(margin);
}

AreaSelectorDialog::~AreaSelectorDialog()
{
    delete ui;
}

AreaSelectorDialog::Area AreaSelectorDialog::getArea()
{
    if (ui->boundingBox->isChecked())
        return BOUNDING_BOX;
    if (ui->moduleRectangle->isChecked())
        return MODULE_RECTANGLE;
    if (ui->viewport->isChecked())
        return VIEWPORT;

    throw omnetpp::cRuntimeError("AreaSelectorDialog::getArea(): None of the 3 RadioButtons are checked, this should never happen.");
}

int AreaSelectorDialog::getMargin()
{
    return ui->margin->value();
}

void AreaSelectorDialog::accept()
{
    if (ui->boundingBox->isChecked())
        getQtenv()->setPref(PREF_KEY_AREA, PREF_VALUE_AREA_BOUNDING_BOX);
    if (ui->moduleRectangle->isChecked())
        getQtenv()->setPref(PREF_KEY_AREA, PREF_VALUE_AREA_MODULE_RECTANGLE);
    if (ui->viewport->isChecked())
        getQtenv()->setPref(PREF_KEY_AREA, PREF_VALUE_AREA_VIEWPORT);

    getQtenv()->setPref(PREF_KEY_MARGIN, ui->margin->value());

    QDialog::accept();
}

}  // namespace qtenv
}  // namespace omnetpp

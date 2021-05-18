//==========================================================================
//  LOGFINDDIALOG.CC - part of
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

#include "logfinddialog.h"
#include "ui_logfinddialog.h"
#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

LogFindDialog::LogFindDialog(QWidget *parent, QString lastText, TextViewerWidget::FindOptions options) :
    QDialog(parent),
    ui(new Ui::LogFindDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    ui->text->setText(lastText);
    ui->text->selectAll();
    ui->caseCheckBox->setChecked(options.testFlag(TextViewerWidget::FIND_CASE_SENSITIVE));
    ui->regexpCheckBox->setChecked(options.testFlag(TextViewerWidget::FIND_REGULAR_EXPRESSION));
    ui->wholeWordsCheckBox->setChecked(options.testFlag(TextViewerWidget::FIND_WHOLE_WORDS));
    ui->backwardsCheckBox->setChecked(options.testFlag(TextViewerWidget::FIND_BACKWARDS));
}

LogFindDialog::~LogFindDialog()
{
    delete ui;
}

QString LogFindDialog::getText()
{
    return ui->text->text();
}

TextViewerWidget::FindOptions LogFindDialog::getOptions()
{
    TextViewerWidget::FindOptions options;

    if (ui->regexpCheckBox->isChecked())
        options |= TextViewerWidget::FIND_REGULAR_EXPRESSION;
    if (ui->caseCheckBox->isChecked())
        options |= TextViewerWidget::FIND_CASE_SENSITIVE;
    if (ui->wholeWordsCheckBox->isChecked())
        options |= TextViewerWidget::FIND_WHOLE_WORDS;
    if (ui->backwardsCheckBox->isChecked())
        options |= TextViewerWidget::FIND_BACKWARDS;

    return options;
}

}  // namespace qtenv
}  // namespace omnetpp


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
#include <QtWidgets/QPushButton>
#include <QtCore/QRegExp>
#include "qtenv.h"

namespace omnetpp {
namespace qtenv {

LogFindDialog::LogFindDialog(QWidget *parent, QString lastText, SearchFlags options) :
    QDialog(parent),
    ui(new Ui::LogFindDialog)
{
    ui->setupUi(this);
    setFont(getQtenv()->getBoldFont());

    ui->text->setText(lastText);
    ui->text->selectAll();
    ui->caseCheckBox->setChecked(options & FIND_CASE_SENSITIVE);
    ui->regexpCheckBox->setChecked(options & FIND_REGULAR_EXPRESSION);
    ui->wholeWordsCheckBox->setChecked(options & FIND_WHOLE_WORDS);
    ui->backwardsCheckBox->setChecked(options & FIND_BACKWARDS);

    connect(ui->text, SIGNAL(textChanged(QString)), this, SLOT(updateRegExpValidation()));
    connect(ui->regexpCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateRegExpValidation()));
    updateRegExpValidation();
}

void LogFindDialog::updateRegExpValidation()
{
    QRegExp regexp;
    bool isValid = !ui->regexpCheckBox->isChecked() || (regexp = QRegExp(ui->text->text())).isValid();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
    if (!isValid) {
        if (regexpErrorLabel == nullptr) {
            regexpErrorLabel = new QLabel(regexp.errorString(), this);
            regexpErrorLabel->setAlignment(Qt::AlignTop);
            regexpErrorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            ui->verticalLayout->insertWidget(1, regexpErrorLabel);
        }
        regexpErrorLabel->setText("Invalid regular expression: " + regexp.errorString());
    }
    else {
        delete regexpErrorLabel;
        regexpErrorLabel = nullptr;
    }
}

LogFindDialog::~LogFindDialog()
{
    delete ui;
}

QString LogFindDialog::getText()
{
    return ui->text->text();
}

SearchFlags LogFindDialog::getOptions()
{
    SearchFlags options;

    if (ui->regexpCheckBox->isChecked())
        options |= FIND_REGULAR_EXPRESSION;
    if (ui->caseCheckBox->isChecked())
        options |= FIND_CASE_SENSITIVE;
    if (ui->wholeWordsCheckBox->isChecked())
        options |= FIND_WHOLE_WORDS;
    if (ui->backwardsCheckBox->isChecked())
        options |= FIND_BACKWARDS;

    return options;
}

}  // namespace qtenv
}  // namespace omnetpp


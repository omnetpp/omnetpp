#include "stopdialog.h"
#include "ui_stopdialog.h"
#include "qtenv.h"
#include <QKeyEvent>

StopDialog::StopDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StopDialog)
{
    ui->setupUi(this);

    stopDialogAutoupdate();
}

StopDialog::~StopDialog()
{
    delete ui;
}

void StopDialog::onClickStop()
{
    qtenv::getTkenv()->setStopSimulationFlag();
    close();
}

void StopDialog::stopDialogAutoupdate()
{
    ui->updateButton->setDisabled(ui->checkBox->isChecked());
    if(ui->checkBox->isChecked())
        qtenv::getTkenv()->opt->autoupdateInExpress = true;
    else
        qtenv::getTkenv()->opt->autoupdateInExpress = false;
}

void StopDialog::onClickUpdate()
{
    qtenv::getTkenv()->refreshInspectors();
}

void StopDialog::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Enter:
        case Qt::Key_Escape:
        case Qt::Key_F8:
            qtenv::getTkenv()->setStopSimulationFlag();
            break;
    default:
        break;
    }
}

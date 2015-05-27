#ifndef STOPDIALOG_H
#define STOPDIALOG_H

#include <QDialog>

namespace Ui {
class StopDialog;
}

class StopDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StopDialog(QWidget *parent = 0);
    ~StopDialog();

public slots:
    void onClickStop();
    void onClickUpdate();
    void stopDialogAutoupdate();
protected:
    virtual void keyPressEvent(QKeyEvent *e);

private:
    Ui::StopDialog *ui;
};

#endif // STOPDIALOG_H

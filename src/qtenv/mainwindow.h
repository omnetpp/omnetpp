#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtenvdefs.h"
#include <QMainWindow>

class QGraphicsScene;

NAMESPACE_BEGIN
class cCanvas;
namespace qtenv {
class Qtenv;
};
NAMESPACE_END

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QGraphicsScene *scene;

    explicit MainWindow(qtenv::Qtenv *env, QWidget *parent = 0);

    void displayText(const char* t);

    ~MainWindow();

private slots:
    void on_actionOneStep_triggered();
    void on_actionQuit_triggered();
    void on_actionRun_triggered();
    void on_actionSetUpConfiguration_triggered();
    void on_actionStop_triggered();

private:
    //enum Mode{};
    Ui::MainWindow *ui;
    qtenv::Qtenv *env;

    bool isRunning();
    bool checkRunning();
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qtenvdefs.h"
#include <QMainWindow>

class QGraphicsScene;
class QStandardItem;
class TreeItemModel;
class cObject;

NAMESPACE_BEGIN
class cCanvas;
namespace qtenv {
class Qtenv;
}

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
    void on_actionFastRun_triggered();
    void on_actionExpressRun_triggered();

    void on_actionRunUntil_triggered();

private:
    enum Mode { STEP, NORMAL, FAST, EXPRESS, NOT_RUNNING};
    Ui::MainWindow *ui;
    qtenv::Qtenv *env;
    TreeItemModel *treeModel;

    bool isRunning();
    bool checkRunning();
    void setGuiForRunmode(Mode mode, bool untilMode = false);
    void runSimulation(Mode mode);
};

#endif // MAINWINDOW_H

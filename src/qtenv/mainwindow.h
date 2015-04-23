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

public slots:
    void exitOmnetpp();
    void setUpConfiguration();//tkenv: newRun
    void oneStep();
    void runSimulation();
    void stopSimulation();

private:
    Ui::MainWindow *ui;
    qtenv::Qtenv *env;
};

#endif // MAINWINDOW_H

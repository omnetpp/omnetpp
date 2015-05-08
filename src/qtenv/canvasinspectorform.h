#ifndef CANVASINSPECTORFORM_H
#define CANVASINSPECTORFORM_H

#include <QMainWindow>

class QGraphicsScene;

namespace qtenv {
class CanvasInspector;
}

namespace Ui {
class CanvasInspectorForm;
}

class CanvasInspectorForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CanvasInspectorForm(qtenv::CanvasInspector *inspector, QWidget *parent = 0);
    ~CanvasInspectorForm();

    QGraphicsScene *getScene();

private slots:
    void closeEvent(QCloseEvent * event);

private:
    Ui::CanvasInspectorForm *ui;
    qtenv::CanvasInspector *insp;
};

#endif // CANVASINSPECTORFORM_H

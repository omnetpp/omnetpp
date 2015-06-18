#ifndef __OMNETPP_QTENV_CANVASINSPECTORFORM_H
#define __OMNETPP_QTENV_CANVASINSPECTORFORM_H

#include <QMainWindow>
#include "qtenvdefs.h"

class QGraphicsScene;

namespace Ui {
class CanvasInspectorForm;
}

namespace omnetpp {
namespace qtenv {

class CanvasInspector;

class CanvasInspectorForm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CanvasInspectorForm(CanvasInspector *inspector, QWidget *parent = 0);
    ~CanvasInspectorForm();

    QGraphicsScene *getScene();

private slots:
    void closeEvent(QCloseEvent * event);

private:
    Ui::CanvasInspectorForm *ui;
    CanvasInspector *insp;
};

} // namespace qtenv
} // namespace omnetpp

#endif

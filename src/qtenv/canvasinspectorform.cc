#include "canvasinspectorform.h"
#include "ui_canvasinspectorform.h"
#include "canvasinspector.h"
#include "qtenv.h"

#include <QDebug>

namespace omnetpp {
namespace qtenv {

CanvasInspectorForm::CanvasInspectorForm(CanvasInspector *inspector, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CanvasInspectorForm),
    insp(inspector)
{
    ui->setupUi(this);
    ui->graphicsView->setScene(new QGraphicsScene());
}

CanvasInspectorForm::~CanvasInspectorForm()
{
    delete ui;
}

QGraphicsScene *CanvasInspectorForm::getScene()
{
    return ui->graphicsView->scene();
}

void CanvasInspectorForm::closeEvent(QCloseEvent*)
{
    qDebug() << "Close";
    getTkenv()->deleteInspector(insp);
    QWidget::close();
}

} // namespace qtenv
} // namespace omnetpp

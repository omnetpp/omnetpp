#ifndef MODULEGRAPHICSVIEW_H
#define MODULEGRAPHICSVIEW_H

#include <map>
#include <QPointF>
#include <QGraphicsView>

class QGraphicsPixmapItem;

namespace omnetpp {

class cModule;
class cGate;

namespace qtenv {

class ModuleInspector;

class ModuleGraphicsView : public QGraphicsView
{
private:
    cModule *object;
    int32_t layoutSeed;
    bool notDrawn;

    struct Point {double x,y;};
    typedef std::map<cModule*,Point> PositionMap;
    PositionMap submodPosMap;  // recalculateLayout() fills this map

    std::map<int, QGraphicsPixmapItem*> submoduleGraphicsItems;
    ModuleInspector *eventListener;

    void recalculateLayout();
    void redrawFigures();
    void redrawModules();
    void redrawNextEventMarker();
    void redrawMessages();
    void refreshSubmodules();
    void adjustSubmodulesZOrder();

    void refreshLayout();
    void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeysLayout,
            double& x, double& y, double& sx, double& sy);
    void drawSubmodule(cModule *submod, double x, double y);
    void drawEnclosingModule(cModule *parentModule);
    void drawConnection(cGate *gate);

    QPointF getSubmodCoords(cModule *mod);

protected:
    void mouseDoubleClickEvent(QMouseEvent * event);

public:
    ModuleGraphicsView();

    void relayoutAndRedrawAll();
    void setObject(cModule *obj);
    void setEventListener(ModuleInspector *insp);
    QGraphicsItem *getItemAt(qreal x, qreal y);

    void clear();
};

} // namespace qtenv
} // namespace omnetpp

#endif // MODULEGRAPHICSVIEW_H

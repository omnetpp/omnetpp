#ifndef GRAPHICSPATHARROWITEM_H
#define GRAPHICSPATHARROWITEM_H

#include <QGraphicsPathItem>

class GraphicsPathArrowItem : public QGraphicsPathItem
{
public:
    GraphicsPathArrowItem() {}

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
};

#endif // GRAPHICSPATHARROWITEM_H

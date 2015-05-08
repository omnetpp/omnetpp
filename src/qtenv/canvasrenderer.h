#ifndef CANVASRENDERER_H
#define CANVASRENDERER_H

#include "omnetpp/ccanvas.h"
#include <QGraphicsScene>

namespace qtenv {

class FigureRenderingHints;
class FigureRenderer;

class CanvasRenderer
{
protected:
    QGraphicsScene *scene;
    cCanvas *canvas;    // NULL is allowed
    uint64_t enabledTagBits, exceptTagBits;

protected:
    void assertCanvas();
    virtual FigureRenderer *getRendererFor(cFigure *figure);
    virtual void drawFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints);
    virtual void refreshFigureRec(cFigure *figure, const cFigure::Transform& parentTransform, FigureRenderingHints *hints, bool ancestorTransformChanged = false);
    virtual bool fulfillsTagFilter(cFigure *figure);

public:
    CanvasRenderer() :
        canvas(NULL), enabledTagBits(0), exceptTagBits(0) {}
    virtual ~CanvasRenderer() {}

    virtual void setQtCanvas(QGraphicsScene *scene, cCanvas *canvas);
    virtual void setCanvas(cCanvas *canvas);
    virtual bool hasCanvas() {return canvas != NULL;}
    virtual void redraw(FigureRenderingHints *hints);
    virtual void refresh(FigureRenderingHints *hints);

    // tag-based filtering
    virtual std::string getAllTags();
    virtual std::string getEnabledTags();
    virtual std::string getExceptTags();
    virtual void setEnabledTags(const char* tags);
    virtual void setExceptTags(const char* tags);
};

}

#endif // CANVASRENDERER_H

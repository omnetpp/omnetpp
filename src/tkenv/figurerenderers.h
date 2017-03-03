//==========================================================================
//  FIGURERENDERERS.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_TKENV_FIGURERENDERERS_H
#define __OMNETPP_TKENV_FIGURERENDERERS_H

#include "omnetpp/ccanvas.h"
#include "tkdefs.h"

namespace omnetpp {
namespace tkenv {

struct TKENV_API FigureRenderingHints
{
        double zoom;
        double iconMagnification;
        std::string defaultFont;
        int defaultFontSize;
        bool showSubmoduleLabels;
        bool showArrowheads;
        bool showMessageLabels;

        FigureRenderingHints() :
            zoom(1), iconMagnification(1), defaultFont("Arial"), defaultFontSize(12),
            showSubmoduleLabels(true), showArrowheads(true), showMessageLabels(true) {}
};

class TKENV_API FigureRenderer : public cObject // for because Register_Class() takes cObject*
{
    private:
        static std::map<std::string, FigureRenderer*> rendererCache;
        char bufferSpace[512];
        char *bufferFreePtr;
    protected:
        void initBufs() {bufferFreePtr = bufferSpace;}
        char *getBuf(int len) {char *ret = bufferFreePtr; bufferFreePtr += len; ASSERT(bufferFreePtr < bufferSpace+sizeof(bufferSpace)); return ret;}
        char *point(const cFigure::Point& p);
        char *point2(const cFigure::Point& p1, const cFigure::Point& p2);
        char *bounds(const cFigure::Rectangle& bounds);
        char *matrix(const cFigure::Transform& transform);
        char *color(const cFigure::Color& color);
        char *itoa(int i);
        char *dtoa(double d);
        static int round(double d) { return (int)floor(d+0.5);}
        void lineStyle(cFigure::LineStyle style, int& argc, const char *argv[]);
        void capStyle(cFigure::CapStyle style, int& argc, const char *argv[]);
        void joinStyle(cFigure::JoinStyle style, int& argc, const char *argv[]);
        void arrowHead(bool isStart, cFigure::Arrowhead arrowHead, int lineWidth, double zoom, int& argc, const char *argv[]);
        void fillRule(cFigure::FillRule fill, int& argc, const char *argv[]);
        void interpolation(cFigure::Interpolation interpolation, int& argc, const char *argv[]);
        void anchor(cFigure::Anchor anchor, int& argc, const char *argv[]);
        void textanchor(cFigure::Anchor anchor, int& argc, const char *argv[]);
        void font(const cFigure::Font& font, FigureRenderingHints *hints, int& argc, const char *argv[]);
        const char *resolveIcon(Tcl_Interp *interp, const char *iconName, int& outWidth, int& outHeight);
        std::string polygonPath(const std::vector<cFigure::Point>& points, bool smooth);
        std::string polylinePath(const std::vector<cFigure::Point>& points, bool smooth);
        std::string arcPath(const cFigure::Rectangle& rect, double start, double end);
        std::string pieSlicePath(const cFigure::Rectangle& rect, double start, double end);
        std::string ringPath(const cFigure::Rectangle& rect, double innerRx, double innerRy);
        void moveItemsAbove(Tcl_Interp *interp, Tcl_CmdInfo *cmd, const char *tag, const char *referenceTag);
        void removeItems(Tcl_Interp *interp, Tcl_CmdInfo *cmd, const char *tag);

    public:
        FigureRenderer() : bufferFreePtr(nullptr) {}
        virtual ~FigureRenderer() {}
        static FigureRenderer *getRendererFor(cFigure *figure);
        virtual void render(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints) = 0;
        virtual void refresh(cFigure *figure, int8_t what, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints) = 0;
        virtual void remove(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd) = 0;
};

class TKENV_API AbstractCanvasItemFigureRenderer : public FigureRenderer
{
    protected:
        virtual const char *getItemType() = 0;
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) = 0;
        virtual void addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[]);
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) = 0;
    public:
        virtual void render(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void refresh(cFigure *figure, int8_t what, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void remove(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd) override;
};

class TKENV_API NullRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "pline";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override {return "0 0 0 0";}
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override {}
};

class TKENV_API GroupFigureRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "group";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override {return "0 0 0 0";}
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override {}
};

class TKENV_API AbstractLineFigureRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API LineFigureRenderer : public AbstractLineFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "pline";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API ArcFigureRenderer : public AbstractLineFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API PolylineFigureRenderer : public AbstractLineFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API AbstractShapeRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API RectangleFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "prect";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API OvalFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "ellipse";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API RingFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API PieSliceFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API PolygonFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API PathFigureRenderer : public AbstractShapeRenderer
{
    protected:
        virtual const char *getItemType() override {return "path";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[]) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API AbstractTextFigureRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "ptext";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API TextFigureRenderer : public AbstractTextFigureRenderer
{
};

class TKENV_API LabelFigureRenderer : public AbstractTextFigureRenderer
{
    protected:
        virtual void addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[]) override;
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API AbstractImageFigureRenderer : public AbstractCanvasItemFigureRenderer
{
    protected:
        virtual const char *getItemType() override {return "pimage";}
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API ImageFigureRenderer : public AbstractImageFigureRenderer
{
    protected:
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
};

class TKENV_API IconFigureRenderer : public ImageFigureRenderer
{
    protected:
        virtual void addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[]) override;
        virtual std::string getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints) override;

};

class TKENV_API PixmapFigureRenderer : public AbstractImageFigureRenderer
{
    protected:
        virtual void addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints) override;
        virtual void ensureImage(Tcl_Interp *interp, const char *imageName, const cFigure::Pixmap& pixmap);
};

} // namespace tkenv
}  // namespace omnetpp

#endif

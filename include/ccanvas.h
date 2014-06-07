//==========================================================================
//   CCANVAS.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCANVAS_H
#define __CCANVAS_H

#include "cownedobject.h"

NAMESPACE_BEGIN

class cCanvas;
class cProperty;
class cProperties;

//TODO:
// remove zOrder --> cannot be implemented efficiently with either Tk or GEF!
// use flags field for booleans
// work out details for Point, Color, Font
// changed flags
// utility functions to cFigure: getBoundingBox(), translate(), etc
// recognize color names
// add "arc" items!
//
//TODO later:
// add alpha to Color
// add "constraints" (classes that compute and set attrs of figures, to serve e.g. as anchors)
// rotation support? (needed for transformations)
// figure hierarchy (figure coords to be relative to the parent)
// add cTransformationFigure (wrong name propably)
// layer hierarchy? (~named layer groups)

// getName() should return layer name
// getOwner() should return the canvas
class SIM_API cLayer : public cNamedObject
{
    private:
        cCanvas *canvas;
        int zOrder;
    public:
        cLayer() : canvas(NULL), zOrder(0) {}
        cCanvas *getCanvas() const {return canvas;}
        void setCanvas(cCanvas *canvas) {this->canvas = canvas;}
        int getZOrder() const {return zOrder;}
        void setZOrder(int order) {zOrder = order;}
};

// getName() should return id (NED property index)
// getOwner() should return the canvas (or another figure)
class SIM_API cFigure : public cNamedObject
{
        friend class cCanvas; // for setCanvas()
    public:
        //TODO move these declarations into cCanvas?
        struct Point {
            double x, y;
            Point() : x(0), y(0) {}
            Point(double x, double y) : x(x), y(y) {}
        };

        struct Color {
            uint8_t red, green, blue; // later: alpha
            Color() : red(0), green(0), blue(0) {}
            Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}
        };

        static const Color BLACK;
        static const Color WHITE;
        static const Color GREY;
        static const Color RED;
        static const Color GREEN;
        static const Color BLUE;
        static const Color YELLOW;
        static const Color CYAN;
        static const Color MAGENTA;

        struct Font {
            std::string typeface;
            int pointSize;
            uint8_t style;
            Font() : pointSize(8), style(FONT_NONE) {}
        };

        enum FontStyle { FONT_NONE=0, FONT_BOLD=1, FONT_ITALIC=2, FONT_UNDERLINE=4 };
        enum LineStyle { LINE_SOLID, LINE_DOTTED, LINE_DASHED };
        enum CapStyle { CAP_BUTT, CAP_SQUARE, CAP_ROUND };
        enum JoinStyle { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND };
        enum ArrowHead { ARROW_NONE, ARROW_SIMPLE, ARROW_TRIANGLE, ARROW_BARBED };
        enum Anchor {ANCHOR_CENTER, ANCHOR_N, ANCHOR_E, ANCHOR_S, ANCHOR_W, ANCHOR_NW, ANCHOR_NE, ANCHOR_SE, ANCHOR_SW };
        enum Alignment { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER }; // note: JUSTIFY is not supported by Tk

    private:
        cCanvas *canvas;
        bool visible;
        cLayer *layer; // may be NULL
        std::vector<std::string> tags; //TODO stringpool
        bool hasZOrder;
        int zOrder;

    protected:
        Point parsePoint(cProperty *property, const char *key, int index);
        std::vector<Point> parsePoints(cProperty *property, const char *key);
        bool parseBool(const char *s);
        Color parseColor(const char *s);
        LineStyle parseLineStyle(const char *s);
        CapStyle parseCapStyle(const char *s);
        JoinStyle parseJoinStyle(const char *s);
        ArrowHead parseArrowHead(const char *s);
        Anchor parseAnchor(const char *s);
        Alignment parseAlignment(const char *s);

    public:
        cFigure() : canvas(NULL), visible(true), layer(NULL), hasZOrder(false), zOrder(0) {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y) = 0;
        cCanvas *getCanvas() const {return canvas;}
        void setCanvas(cCanvas *canvas) {this->canvas = canvas;}
        bool isVisible() const {return visible;}
        void setVisible(bool visible) {this->visible = visible;}
        cLayer *getLayer() const {return layer;}
        void setLayer(cLayer *layer) {this->layer = layer;}
        const std::vector<std::string>& getTags() const {return tags;}
        void setTags(const std::vector<std::string>& tags) {this->tags = tags;}
        int getZOrder() const {return hasZOrder ? zOrder : layer ? layer->getZOrder() : 0;}
        void setZOrder(int zOrder) {this->zOrder = zOrder; hasZOrder = true;}
        void unsetZOrder() {hasZOrder = false;}
};

class SIM_API cAbstractLineFigure : public cFigure
{
    private:
        Color lineColor;
        LineStyle lineStyle;
        int lineWidth;
        ArrowHead startArrowHead, endArrowHead;
    public:
        cAbstractLineFigure() : lineColor(BLACK), lineStyle(LINE_SOLID), lineWidth(1), startArrowHead(ARROW_NONE), endArrowHead(ARROW_NONE) {}
        virtual void parse(cProperty *property);
        const Color& getLineColor() const  {return lineColor;}
        void setLineColor(const Color& lineColor)  {this->lineColor = lineColor;}
        int getLineWidth() const  {return lineWidth;}
        void setLineWidth(int lineWidth)  {this->lineWidth = lineWidth;}
        LineStyle getLineStyle() const  {return lineStyle;}
        void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle;}
        ArrowHead getStartArrowHead() const  {return startArrowHead;}
        void setStartArrowHead(ArrowHead startArrowHead)  {this->startArrowHead = startArrowHead;}
        ArrowHead getEndArrowHead() const  {return endArrowHead;}
        void setEndArrowHead(ArrowHead endArrowHead)  {this->endArrowHead = endArrowHead;}
};

class SIM_API cLineFigure : public cAbstractLineFigure
{
    private:
        Point start, end;
    public:
        cLineFigure() {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const Point& getStart() const  {return start;}
        void setStart(const Point& start)  {this->start = start;}
        const Point& getEnd() const  {return end;}
        void setEnd(const Point& end)  {this->end = end;}
};

class SIM_API cPolylineFigure : public cAbstractLineFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        CapStyle capStyle;
        JoinStyle joinStyle;
    public:
        cPolylineFigure() : smooth(false), capStyle(CAP_BUTT), joinStyle(JOIN_MITER) {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const std::vector<Point>& getPoints() const  {return points;}
        void setPoints(const std::vector<Point>& points) {this->points = points;}
        bool getSmooth() const {return smooth;}
        void setSmooth(bool smooth) {this->smooth = smooth;}
        CapStyle getCapStyle() const {return capStyle;}
        void setCapStyle(CapStyle capStyle) {this->capStyle = capStyle;}
        JoinStyle getJoinStyle() const {return joinStyle;}
        void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle;}
};

class SIM_API cAbstractShapeFigure : public cFigure
{
    private:
        bool filled;
        Color lineColor;
        Color fillColor;
        LineStyle lineStyle;
        int lineWidth;
    public:
        cAbstractShapeFigure() : filled(false), lineColor(BLACK), fillColor(BLUE), lineStyle(LINE_SOLID), lineWidth(1) {}
        virtual void parse(cProperty *property);
        bool isFilled() const  {return filled;}
        void setFilled(bool filled)  {this->filled = filled;}
        const Color& getLineColor() const  {return lineColor;}
        void setLineColor(const Color& lineColor)  {this->lineColor = lineColor;}
        const Color& getFillColor() const  {return fillColor;}
        void setFillColor(const Color& fillColor)  {this->fillColor = fillColor;}
        LineStyle getLineStyle() const  {return lineStyle;}
        void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle;}
        int getLineWidth() const  {return lineWidth;}
        void setLineWidth(int lineWidth)  {this->lineWidth = lineWidth;}
};

class SIM_API cRectangleFigure : public cAbstractShapeFigure
{
    private:
        Point p1, p2;
    public:
        cRectangleFigure() {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const Point& getP1() const  {return p1;}
        void setP1(const Point& p1)  {this->p1 = p1;}
        const Point& getP2() const  {return p2;}
        void setP2(const Point& p2)  {this->p2 = p2;}
};

class SIM_API cOvalFigure : public cAbstractShapeFigure
{
    private:
        Point p1, p2;
    public:
        cOvalFigure() {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const Point& getP1() const  {return p1;}
        void setP1(const Point& p1)  {this->p1 = p1;}
        const Point& getP2() const  {return p2;}
        void setP2(const Point& p2)  {this->p2 = p2;}
};

class SIM_API cPolygonFigure : public cAbstractShapeFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
    public:
        cPolygonFigure() : smooth(false), joinStyle(JOIN_MITER) {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const std::vector<Point>& getPoints() const  {return points;}
        void setPoints(const std::vector<Point>& points) {this->points = points;}
        bool getSmooth() const {return smooth;}
        void setSmooth(bool smooth) {this->smooth = smooth;}
        JoinStyle getJoinStyle() const {return joinStyle;}
        void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle;}
};


class SIM_API cTextFigure : public cFigure
{
    private:
        Point pos;
        Color color;
        Font font;
        std::string text;
        Anchor anchor;
        Alignment alignment;
    public:
        cTextFigure() : color(BLACK), anchor(ANCHOR_NW), alignment(ALIGN_LEFT) {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const Point& getPos() const  {return pos;}
        void setPos(const Point& pos)  {this->pos = pos;}
        const Color& getColor() const  {return color;}
        void setColor(const Color& color)  {this->color = color;}
        Font getFont() const  {return font;}
        void setFont(Font font)  {this->font = font;}
        const char *getText() const  {return text.c_str();}
        void setText(const char *text)  {this->text = text;}
        Anchor getAnchor() const  {return anchor;}
        void setAnchor(Anchor anchor)  {this->anchor = anchor;}
        Alignment getAlignment() const  {return alignment;}
        void setAlignment(Alignment alignment)  {this->alignment = alignment;}
};

class SIM_API cImageFigure : public cFigure
{
    private:
        Point pos;
        std::string imageName;
        Color color;
        int colorization;
    public:
        cImageFigure() : color(BLUE), colorization(0) {}
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        const Point& getPos() const  {return pos;}
        void setPos(const Point& pos)  {this->pos = pos;}
        const char *getImageName() const  {return imageName.c_str();}
        void setImageName(const char *imageName)  {this->imageName = imageName;}
        const Color& getColor() const  {return color;}
        void setColor(const Color& color)  {this->color = color;}
        int getColorization() const  {return colorization;}
        void setColorization(int colorization)  {this->colorization = colorization;}
};


/**
 * TODO
 */
class SIM_API cCanvas : public cOwnedObject
{
    private:
        std::vector<cFigure*> figures;
        std::vector<cLayer*> layers;
    protected:
        void parseLayer(cProperty *property);
        void parseFigure(cProperty *property);
    public:
        cCanvas();
        ~cCanvas();
        static bool containsCanvasItems(cProperties *properties);
        void addItemsFrom(cProperties *properties);
        void addFigure(cFigure *figure);
        void deleteFigure(cFigure *figure);
        cFigure *getFigureByName(const char *name);  // i.e. by property index string
        int getNumFigures() const {return figures.size();}
        cFigure *getFigure(int k) const {return figures[k];} //TODO bounds check

        void addLayer(cLayer *layer);
        void deleteLayer(cLayer *layer);
        cLayer *getLayerByName(const char *name);
        int getNumLayers() const {return layers.size();}
        cLayer *getLayer(int k) const {return layers[k];} //TODO bounds check
};

NAMESPACE_END


#endif


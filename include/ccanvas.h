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
// USE HIERARCHICAL NAMES!
// hierarchical name to appear in the API (would make "parent=foo" superfluous)! getFigurePath(), getFigureByPath(), etc
// zorder= property key allow ordering of figures (NOT PUBLIC API, ONLY USED DURING PARSING!!!)
// recognize color names
// naming consistency: find/get; const!  byName suffix kell?
//
//TODO later:
// use flags field for booleans
// coordinates: pixels vs meters? location, size, linewidth???
//  - 2 options: (1) Point to contain (xUnits,xPixels,yUnits,yPixels), or (2) cFigure to contain flags
//  - also: what should zoom affect? size? (see icons vs rect!) linewidth?
// ---> cLayer: isViewportRelative! (pixels only!)  cFigure: isDistanceInPixels() --> oroklodik, es lejjebb mar nem lehet kikapcsolni!!!
//   kell tudni a viewport minden oldalahoz/sarkahoz anchorolni!!!
//
// add alpha to Color
// add "constraints" (classes that compute and set attrs of figures, to serve e.g. as anchors)
// rotation support? (needed for transformations)
// add cTransformationFigure (wrong name propably)
// layer hierarchy? (~named layer groups)


/**
 * TODO
 */
class SIM_API cFigure : public cOwnedObject
{
    public:
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
            Font(std::string typeface, int pointSize, uint8_t style=FONT_NONE) : typeface(typeface), pointSize(pointSize), style(style) {}
        };

        enum FontStyle { FONT_NONE=0, FONT_BOLD=1, FONT_ITALIC=2, FONT_UNDERLINE=4 };
        enum LineStyle { LINE_SOLID, LINE_DOTTED, LINE_DASHED };
        enum CapStyle { CAP_BUTT, CAP_SQUARE, CAP_ROUND };
        enum JoinStyle { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND };
        enum ArrowHead { ARROW_NONE, ARROW_SIMPLE, ARROW_TRIANGLE, ARROW_BARBED };
        enum Anchor {ANCHOR_CENTER, ANCHOR_N, ANCHOR_E, ANCHOR_S, ANCHOR_W, ANCHOR_NW, ANCHOR_NE, ANCHOR_SE, ANCHOR_SW };
        enum Alignment { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER }; // note: JUSTIFY is not supported by Tk

        // internal:
        enum {CHANGE_VISUAL=1, CHANGE_GEOMETRY=2, CHANGE_STRUCTURAL=4};

    private:
        static int lastId;
        int id;
        bool visible; // treated as structural change, for simpler handing
        std::vector<std::string> tags; //TODO stringpool
        std::vector<cFigure*> children;

        int8_t localChange;
        int8_t treeChange;

    protected:
        virtual void doVisualChange() {doChange(CHANGE_VISUAL);}
        virtual void doGeometryChange() {doChange(CHANGE_GEOMETRY);}
        virtual void doStructuralChange() {doChange(CHANGE_STRUCTURAL);}
        virtual void doChange(int flags);
        static Point parsePoint(cProperty *property, const char *key, int index);
        static std::vector<Point> parsePoints(cProperty *property, const char *key);
        static bool parseBool(const char *s);
        static Color parseColor(const char *s);
        static LineStyle parseLineStyle(const char *s);
        static CapStyle parseCapStyle(const char *s);
        static JoinStyle parseJoinStyle(const char *s);
        static ArrowHead parseArrowHead(const char *s);
        static Anchor parseAnchor(const char *s);
        static Alignment parseAlignment(const char *s);

    public:
        // internal:
        int getLocalChangeFlags() const {return localChange;}
        int getTreeChangeFlags() const {return treeChange;}
        void clearChangeFlags();

    public:
        cFigure(const char *name=NULL) : cOwnedObject(name), localChange(0), treeChange(0), visible(true), id(++lastId) {}
        virtual void parse(cProperty *property);
        virtual const char *getClassNameForRenderer() const {return getClassName();} // which figure class' renderer to use; override if you want to subclass a figure while reusing the base class' renderer
        int getId() const {return id;}
        virtual Point getLocation() const = 0;
        virtual void translate(double x, double y) = 0;
        virtual cCanvas *getCanvas() const;
        virtual bool isVisible() const {return visible;}
        virtual void setVisible(bool visible) {this->visible = visible; doStructuralChange();}
        virtual const std::vector<std::string>& getTags() const {return tags;}
        virtual void setTags(const std::vector<std::string>& tags) {this->tags = tags;}

        virtual void addChild(cFigure *figure);
        virtual void addChild(cFigure *figure, int pos);
        virtual cFigure *removeChild(int pos);
        virtual cFigure *removeChild(cFigure *figure);
        virtual int findChild(const char *name);
        virtual int findChild(cFigure *figure);
        virtual bool hasChildren() const {return !children.empty();}
        virtual int getNumChildren() const {return children.size();}
        virtual cFigure *getChild(int pos);
        virtual cFigure *getChild(const char *name);
        virtual cFigure *getParent()  {return dynamic_cast<cFigure*>(getOwner());}
        virtual cFigure *getFigureByName(const char *name);
};

class SIM_API cLayer : public cFigure
{
    private:
        Point loc;
        std::string description;
    public:
        cLayer(const char *name=NULL) : cFigure(name) {}
        virtual const char *getClassNameForRenderer() const {return "";} // non-visual figure
        virtual void translate(double x, double y);
        virtual Point getLocation() const  {return loc;}
        virtual void setLocation(const Point& loc)  {this->loc = loc; doGeometryChange();}
        virtual const char *getDescription() const {return description.c_str();}
        virtual void setDescription(const char *description) { this->description = description;}
};

class SIM_API cGroupFigure : public cFigure
{
    private:
        Point loc;
    public:
        cGroupFigure(const char *name=NULL) : cFigure(name) {}
        virtual const char *getClassNameForRenderer() const {return "";} // non-visual figure
        virtual void translate(double x, double y);
        virtual Point getLocation() const  {return loc;}
        virtual void setLocation(const Point& loc)  {this->loc = loc; doGeometryChange();}
};

class SIM_API cAbstractLineFigure : public cFigure
{
    private:
        Color lineColor;
        LineStyle lineStyle;
        int lineWidth;
        ArrowHead startArrowHead, endArrowHead;
    public:
        cAbstractLineFigure(const char *name=NULL) : cFigure(name), lineColor(BLACK), lineStyle(LINE_SOLID), lineWidth(1), startArrowHead(ARROW_NONE), endArrowHead(ARROW_NONE) {}
        virtual void parse(cProperty *property);
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor)  {this->lineColor = lineColor; doVisualChange();}
        virtual int getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(int lineWidth)  {this->lineWidth = lineWidth; doVisualChange();}
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle; doVisualChange();}
        virtual ArrowHead getStartArrowHead() const  {return startArrowHead;}
        virtual void setStartArrowHead(ArrowHead startArrowHead)  {this->startArrowHead = startArrowHead; doVisualChange();}
        virtual ArrowHead getEndArrowHead() const  {return endArrowHead;}
        virtual void setEndArrowHead(ArrowHead endArrowHead)  {this->endArrowHead = endArrowHead; doVisualChange();}
};

class SIM_API cLineFigure : public cAbstractLineFigure
{
    private:
        Point start, end;
        CapStyle capStyle;
    public:
        cLineFigure(const char *name=NULL) : cAbstractLineFigure(name), capStyle(CAP_BUTT) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return start;}
        virtual void translate(double x, double y);
        virtual const Point& getStart() const  {return start;}
        virtual void setStart(const Point& start)  {this->start = start; doGeometryChange();}
        virtual const Point& getEnd() const  {return end;}
        virtual void setEnd(const Point& end)  {this->end = end; doGeometryChange();}
        virtual CapStyle getCapStyle() const {return capStyle;}
        virtual void setCapStyle(CapStyle capStyle) {this->capStyle = capStyle; doVisualChange();}
};

class SIM_API cPolylineFigure : public cAbstractLineFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        CapStyle capStyle;
        JoinStyle joinStyle;
    public:
        cPolylineFigure(const char *name=NULL) : cAbstractLineFigure(name), smooth(false), capStyle(CAP_BUTT), joinStyle(JOIN_MITER) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return points.empty() ? Point() : points[0];}
        virtual void translate(double x, double y);
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points) {this->points = points; doGeometryChange();}
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth) {this->smooth = smooth; doVisualChange();}
        virtual CapStyle getCapStyle() const {return capStyle;}
        virtual void setCapStyle(CapStyle capStyle) {this->capStyle = capStyle; doVisualChange();}
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle; doVisualChange();}
};

class SIM_API cAbstractShapeFigure : public cFigure
{
    private:
        bool outline; //TODO
        bool filled;
        Color lineColor;
        Color fillColor;
        LineStyle lineStyle;
        int lineWidth;
    public:
        cAbstractShapeFigure(const char *name=NULL) : cFigure(name), filled(false), outline(true), lineColor(BLACK), fillColor(BLUE), lineStyle(LINE_SOLID), lineWidth(1) {}
        virtual void parse(cProperty *property);
        virtual bool isFilled() const  {return filled;}
        virtual void setFilled(bool filled)  {this->filled = filled; doVisualChange();}
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor)  {this->lineColor = lineColor; doVisualChange();}
        virtual const Color& getFillColor() const  {return fillColor;}
        virtual void setFillColor(const Color& fillColor)  {this->fillColor = fillColor; doVisualChange();}
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle; doVisualChange();}
        virtual int getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(int lineWidth)  {this->lineWidth = lineWidth; doVisualChange();}
};

class SIM_API cRectangleFigure : public cAbstractShapeFigure
{
    private:
        //TODO: location+width/height+anchor! +getTopLeft() +getCenter()?
        Point p1, p2;
    public:
        cRectangleFigure(const char *name=NULL) : cAbstractShapeFigure(name) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return p1;}
        virtual void translate(double x, double y);
        virtual const Point& getP1() const  {return p1;}
        virtual void setP1(const Point& p1)  {this->p1 = p1; doGeometryChange();}
        virtual const Point& getP2() const  {return p2;}
        virtual void setP2(const Point& p2)  {this->p2 = p2; doGeometryChange();}
};

class SIM_API cOvalFigure : public cAbstractShapeFigure
{
    private:
        //TODO: location+width/height+anchor! +getCenter() +getTopLeft()
        Point p1, p2;
    public:
        cOvalFigure(const char *name=NULL) : cAbstractShapeFigure(name) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return p1;}
        virtual void translate(double x, double y);
        virtual const Point& getP1() const  {return p1;}
        virtual void setP1(const Point& p1)  {this->p1 = p1; doGeometryChange();}
        virtual const Point& getP2() const  {return p2;}
        virtual void setP2(const Point& p2)  {this->p2 = p2; doGeometryChange();}
};

class SIM_API cArcFigure : public cAbstractShapeFigure
{
    private:
        //TODO: location+width/height+anchor! +getCenter() +getTopLeft()
        Point p1, p2; // of the oval that arc is part of
        double startAngle, endAngle; // in degrees, CCW, 0=east
        //TODO: outline mode (arc/piechart); note: tk doesn't support LineCap for arc items
    public:
        cArcFigure(const char *name=NULL) : startAngle(0), endAngle(0), cAbstractShapeFigure(name) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return p1;}
        virtual void translate(double x, double y);
        virtual const Point& getP1() const  {return p1;}
        virtual void setP1(const Point& p1)  {this->p1 = p1; doGeometryChange();}
        virtual const Point& getP2() const  {return p2;}
        virtual void setP2(const Point& p2)  {this->p2 = p2; doGeometryChange();}
        virtual double getStartAngle() const {return startAngle;}
        virtual void setStartAngle(double startAngle) {this->startAngle = startAngle; doVisualChange();}
        virtual double getEndAngle() const {return endAngle;}
        virtual void setEndAngle(double endAngle) {this->endAngle = endAngle; doVisualChange();}
};

class SIM_API cPolygonFigure : public cAbstractShapeFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
    public:
        cPolygonFigure(const char *name=NULL) : cAbstractShapeFigure(name), smooth(false), joinStyle(JOIN_MITER) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return points.empty() ? Point() : points[0];}
        virtual void translate(double x, double y);
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points) {this->points = points; doGeometryChange();}
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth) {this->smooth = smooth; doVisualChange();}
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle; doVisualChange();}
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
        cTextFigure(const char *name=NULL) : cFigure(name), color(BLACK), anchor(ANCHOR_NW), alignment(ALIGN_LEFT) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return pos;}
        virtual void translate(double x, double y);
        virtual const Point& getPos() const  {return pos;}
        virtual void setPos(const Point& pos)  {this->pos = pos; doGeometryChange();}
        virtual const Color& getColor() const  {return color;}
        virtual void setColor(const Color& color)  {this->color = color; doVisualChange();}
        virtual Font getFont() const  {return font;}
        virtual void setFont(Font font)  {this->font = font; doVisualChange();}
        virtual const char *getText() const  {return text.c_str();}
        virtual void setText(const char *text)  {this->text = text; doVisualChange();}
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor)  {this->anchor = anchor; doVisualChange();}
        virtual Alignment getAlignment() const  {return alignment;}
        virtual void setAlignment(Alignment alignment)  {this->alignment = alignment; doVisualChange();}
};

class SIM_API cImageFigure : public cFigure
{
    private:
        Point pos;
        std::string imageName;
        Color color;
        int colorization;
        Anchor anchor;  //TODO
    public:
        cImageFigure(const char *name=NULL) : cFigure(name), color(BLUE), colorization(0), anchor(ANCHOR_CENTER) {}
        virtual void parse(cProperty *property);
        virtual Point getLocation() const  {return pos;}
        virtual void translate(double x, double y);
        virtual const Point& getPos() const  {return pos;}
        virtual void setPos(const Point& pos)  {this->pos = pos; doGeometryChange();}
        virtual const char *getImageName() const  {return imageName.c_str();}
        virtual void setImageName(const char *imageName)  {this->imageName = imageName; doVisualChange();}
        virtual const Color& getColor() const  {return color;}
        virtual void setColor(const Color& color)  {this->color = color; doVisualChange();}
        virtual int getColorization() const  {return colorization;}
        virtual void setColorization(int colorization)  {this->colorization = colorization; doVisualChange();}
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor)  {this->anchor = anchor; doVisualChange();}
};


/**
 * TODO
 */
class SIM_API cCanvas : public cOwnedObject
{
    private:
        class SIM_API cLayerContainerFigure : public cFigure
        {
            public:
                cLayerContainerFigure(const char *name=NULL) : cFigure(name) {}
                virtual const char *getClassNameForRenderer() const {return "";} // non-visual figure
                virtual void translate(double x, double y); // disallow
                virtual void setLocation(const Point& loc); // disallow
                virtual Point getLocation() const;  // (0,0)
                virtual void addChild(cFigure *figure); // allow cLayer only
                virtual void addChild(cFigure *figure, int pos); // allow cLayer only
        };

        cLayerContainerFigure *rootFigure;
    protected:
        virtual void parseLayer(cProperty *property);
        virtual void parseFigure(cProperty *property);
        virtual cFigure *createFigure(const char *type);
        virtual cFigure *parseParentFigure(cProperty *property);
    public:
        // internal:
        static bool containsCanvasItems(cProperties *properties);
        virtual void addLayersAndFiguresFrom(cProperties *properties);
    public:
        cCanvas();
        virtual ~cCanvas();
        virtual cFigure *getRootFigure() const {return rootFigure;} // it only accepts cLayer as children!
        virtual cFigure *getFigureByName(const char *name); // recursive search
        virtual void addToplevelLayer(cLayer *layer);
        virtual cLayer *removeToplevelLayer(cLayer *layer);
        virtual cLayer *getToplevelLayerByName(const char *name);
        virtual int getNumToplevelLayers() const {return rootFigure->getNumChildren();}
        virtual cLayer *getToplevelLayer(int k) const {return (cLayer *)rootFigure->getChild(k);}
        virtual cLayer *getDefaultLayer() const;
        //TODO getSubmodulesLayer(), int findLayer(name), moveLayer(cLayer *layer, int pos); also cFigure::moveChild(cFigure *figure, int pos)
};

NAMESPACE_END


#endif


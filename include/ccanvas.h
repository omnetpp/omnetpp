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

#define OMNETPP_CANVAS_VERSION  0x20140709  //XXX identifies canvas code version until API stabilizes

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * A figure in the canvas (see cCanvas).
 *
 * Notes:
 * - figures can be hierarchical (resulting in a figure tree)
 * - coordinates are in canvas units (usually meters), not in pixels! conversion uses the "bgs" (background scale) display string tag
 * - child figures are relative to the point returned by getChildOrigin()
 * - a figure subtree can be hidden by calling setVisible(false)
 * - dup() makes shallow copy (doesn't copy child figures); see dupTree() as well
 * - figures are only data storage classes, rendering is done in the back-end (e.g. Tkenv)
 *   by separate renderer classes.
 *
 * @see cCanvas
 */
class SIM_API cFigure : public cOwnedObject
{
    public:
        struct Point {
            double x, y;
            Point() : x(0), y(0) {}
            Point(double x, double y) : x(x), y(y) {}
        };

        struct Rectangle {
            double x, y, width, height;
            Rectangle() : x(0), y(0), width(0), height(0) {}
            Rectangle(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}
        };

        struct Color {
            uint8_t red, green, blue; // later: alpha
            Color() : red(0), green(0), blue(0) {}
            Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}
            static Color byName(const char *colorName); // throws exception for unknown names
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
            Font() : pointSize(0), style(FONT_NONE) {}
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
        bool visible; // treated as structural change, for simpler handling
        std::vector<cFigure*> children;
        opp_string tags; //TODO stringpool
        uint64 tagBits;  // bit-to-tagname mapping is stored in cCanvas. Note: change to std::bitset if 64 tags are not enough

        int8_t localChange;
        int8_t treeChange;

    protected:
        virtual cFigure *getRootFigure();
        virtual void doVisualChange() {doChange(CHANGE_VISUAL);}
        virtual void doGeometryChange() {doChange(CHANGE_GEOMETRY);}
        virtual void doStructuralChange() {doChange(CHANGE_STRUCTURAL);}
        virtual void doChange(int flags);
        static Point parsePoint(cProperty *property, const char *key, int index);
        static std::vector<Point> parsePoints(cProperty *property, const char *key);
        static void parseBounds(cProperty *property, Point& p1, Point& p2);
        static Rectangle parseBounds(cProperty *property);
        static Font parseFont(cProperty *property, const char *key);
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
        void insertChild(cFigure *figure, std::map<cFigure*,double>& orderMap);
        void refreshTagBits();
        void refreshTagBitsRec();
        int64 getTagBits() const {return tagBits;}
        void setTagBits(uint64 tagBits) {this->tagBits = tagBits;}

    private:
        void copy(const cFigure& other);

    public:
        cFigure(const char *name=NULL) : cOwnedObject(name), id(++lastId), visible(true), tagBits(0), localChange(0), treeChange(0) {}
        cFigure(const cFigure& other) : cOwnedObject(other) {copy(other);}
        virtual ~cFigure();
        cFigure& operator=(const cFigure& other);
        virtual void forEachChild(cVisitor *v);
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual cFigure *dupTree();
        virtual const char *getClassNameForRenderer() const {return getClassName();} // denotes renderer of which figure class to use; override if you want to subclass a figure while reusing renderer of the base class

        int getId() const {return id;}
        virtual Point getChildOrigin() const = 0;
        virtual void translate(double x, double y) = 0;
        virtual cCanvas *getCanvas();
        virtual bool isVisible() const {return visible;} // affects figure subtree, not just this very figure
        virtual void setVisible(bool visible) {this->visible = visible; doStructuralChange();}
        virtual const char *getTags() const {return tags.c_str();} // returns space-separated list of tags
        virtual void setTags(const char *tags) {this->tags = tags; refreshTagBits();} // accepts space-separated list of tags

        virtual void addFigure(cFigure *figure);
        virtual void addFigure(cFigure *figure, int pos);
        virtual void addFigureAbove(cFigure *figure, cFigure *referenceFigure);
        virtual void addFigureBelow(cFigure *figure, cFigure *referenceFigure);
        virtual cFigure *removeFigure(int pos);
        virtual cFigure *removeFigure(cFigure *figure);
        virtual int findFigure(const char *name);
        virtual int findFigure(cFigure *figure);
        virtual bool containsFigures() const {return !children.empty();}
        virtual int getNumFigures() const {return children.size();}
        virtual cFigure *getFigure(int pos);
        virtual cFigure *getFigure(const char *name);
        virtual cFigure *getParentFigure()  {return dynamic_cast<cFigure*>(getOwner());}
        virtual void raiseAbove(cFigure *figure);
        virtual void lowerBelow(cFigure *figure);
        virtual void raiseToTop();
        virtual void lowerToBottom();

        virtual cFigure *findFigureRecursively(const char *name);
        virtual cFigure *getFigureByPath(const char *path);  //NOTE: path has similar syntax to cModule::getModuleByPath()
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * For grouping children. No visual appearance.
 */
class SIM_API cGroupFigure : public cFigure
{
    private:
        Point location;
    private:
        void copy(const cGroupFigure& other);
    public:
        cGroupFigure(const char *name=NULL) : cFigure(name) {}
        cGroupFigure(const cGroupFigure& other) : cFigure(other) {copy(other);}
        cGroupFigure& operator=(const cGroupFigure& other);
        virtual cGroupFigure *dup() const  {return new cGroupFigure(*this);}
        virtual std::string info() const;
        virtual const char *getClassNameForRenderer() const {return "";} // non-visual figure
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return location;}
        virtual const Point& getLocation() const  {return location;}
        virtual void setLocation(const Point& loc)  {this->location = loc; doGeometryChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Notes:
 * - line widths are in non-zooming pixels
 */
class SIM_API cAbstractLineFigure : public cFigure
{
    private:
        Color lineColor;
        LineStyle lineStyle;
        double lineWidth;
        CapStyle capStyle;
        ArrowHead startArrowHead, endArrowHead;
    private:
        void copy(const cAbstractLineFigure& other);
    public:
        cAbstractLineFigure(const char *name=NULL) : cFigure(name), lineColor(BLACK), lineStyle(LINE_SOLID), lineWidth(1), capStyle(CAP_BUTT), startArrowHead(ARROW_NONE), endArrowHead(ARROW_NONE) {}
        cAbstractLineFigure(const cAbstractLineFigure& other) : cFigure(other) {copy(other);}
        cAbstractLineFigure& operator=(const cAbstractLineFigure& other);
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor)  {this->lineColor = lineColor; doVisualChange();}
        virtual double getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(double lineWidth)  {this->lineWidth = lineWidth; doVisualChange();}
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle; doVisualChange();}
        virtual CapStyle getCapStyle() const {return capStyle;}
        virtual void setCapStyle(CapStyle capStyle) {this->capStyle = capStyle; doVisualChange();}
        virtual ArrowHead getStartArrowHead() const  {return startArrowHead;}
        virtual void setStartArrowHead(ArrowHead startArrowHead)  {this->startArrowHead = startArrowHead; doVisualChange();}
        virtual ArrowHead getEndArrowHead() const  {return endArrowHead;}
        virtual void setEndArrowHead(ArrowHead endArrowHead)  {this->endArrowHead = endArrowHead; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Tkenv limitation: there is one arrowhead type for the whole line (cannot be different at the two ends)
 */
class SIM_API cLineFigure : public cAbstractLineFigure
{
    private:
        Point start, end;
    private:
        void copy(const cLineFigure& other);
    public:
        cLineFigure(const char *name=NULL) : cAbstractLineFigure(name) {}
        cLineFigure(const cLineFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cLineFigure& operator=(const cLineFigure& other);
        virtual cLineFigure *dup() const  {return new cLineFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual Point getChildOrigin() const  {return start;}
        virtual void translate(double x, double y);
        virtual const Point& getStart() const  {return start;}
        virtual void setStart(const Point& start)  {this->start = start; doGeometryChange();}
        virtual const Point& getEnd() const  {return end;}
        virtual void setEnd(const Point& end)  {this->end = end; doGeometryChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Notes:
 * - bounds are that of the oval that this arc is part of
 *
 * Tkenv limitation: capStyle and arrowheads not supported
 */
class SIM_API cArcFigure : public cAbstractLineFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that arc is part of
        double startAngle, endAngle; // in degrees, CCW, 0=east
    private:
        void copy(const cArcFigure& other);
    public:
        cArcFigure(const char *name=NULL) : cAbstractLineFigure(name), startAngle(0), endAngle(0) {}
        cArcFigure(const cArcFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cArcFigure& operator=(const cArcFigure& other);
        virtual cArcFigure *dup() const  {return new cArcFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return Point(bounds.x, bounds.y);}
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds)  {this->bounds = bounds; doGeometryChange();}
        virtual double getStartAngle() const {return startAngle;}
        virtual void setStartAngle(double startAngle) {this->startAngle = startAngle; doVisualChange();}
        virtual double getEndAngle() const {return endAngle;}
        virtual void setEndAngle(double endAngle) {this->endAngle = endAngle; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Notes:
 * - smooth=true makes the polyline use Bezier curvers, using midpoints of line segments as control points
 *   (except for the starting and ending Bezier curve, where the first and the last point is used)
 *
 * Tkenv limitation: there is one arrowhead type for the whole line (cannot be different at the two ends)
 */
class SIM_API cPolylineFigure : public cAbstractLineFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
    private:
        void copy(const cPolylineFigure& other);
    public:
        cPolylineFigure(const char *name=NULL) : cAbstractLineFigure(name), smooth(false), joinStyle(JOIN_MITER) {}
        cPolylineFigure(const cPolylineFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cPolylineFigure& operator=(const cPolylineFigure& other);
        virtual cPolylineFigure *dup() const  {return new cPolylineFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {static Point dummy; return points.empty() ? dummy : points[0];}
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points) {this->points = points; doGeometryChange();}
        virtual int getNumPoints() const {return points.size();}
        virtual const Point& getPoint(int i) const {return points[i];}
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth) {this->smooth = smooth; doVisualChange();}
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Notes:
 * - line grows symmetrically (i.e. rectangle outline grows both inside and outside if lineWidth grows)
 */
class SIM_API cAbstractShapeFigure : public cFigure
{
    private:
        bool outlined;
        bool filled;
        Color lineColor;
        Color fillColor;
        LineStyle lineStyle;
        double lineWidth;
    private:
        void copy(const cAbstractShapeFigure& other);
    public:
        cAbstractShapeFigure(const char *name=NULL) : cFigure(name), outlined(true), filled(false), lineColor(BLACK), fillColor(BLUE), lineStyle(LINE_SOLID), lineWidth(1) {}
        cAbstractShapeFigure(const cAbstractShapeFigure& other) : cFigure(other) {copy(other);}
        cAbstractShapeFigure& operator=(const cAbstractShapeFigure& other);
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual bool isFilled() const  {return filled;}
        virtual void setFilled(bool filled)  {this->filled = filled; doVisualChange();}
        virtual bool isOutlined() const  {return outlined;}
        virtual void setOutlined(bool outlined)  {this->outlined = outlined; doVisualChange();}
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor)  {this->lineColor = lineColor; doVisualChange();}
        virtual const Color& getFillColor() const  {return fillColor;}
        virtual void setFillColor(const Color& fillColor)  {this->fillColor = fillColor; doVisualChange();}
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle)  {this->lineStyle = lineStyle; doVisualChange();}
        virtual double getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(double lineWidth)  {this->lineWidth = lineWidth; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 */
class SIM_API cRectangleFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds;
    private:
        void copy(const cRectangleFigure& other);
    public:
        cRectangleFigure(const char *name=NULL) : cAbstractShapeFigure(name) {}
        cRectangleFigure(const cRectangleFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cRectangleFigure& operator=(const cRectangleFigure& other);
        virtual cRectangleFigure *dup() const  {return new cRectangleFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return Point(bounds.x, bounds.y);}
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds)  {this->bounds = bounds; doGeometryChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 */
class SIM_API cOvalFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box
    private:
        void copy(const cOvalFigure& other);
    public:
        cOvalFigure(const char *name=NULL) : cAbstractShapeFigure(name) {}
        cOvalFigure(const cOvalFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cOvalFigure& operator=(const cOvalFigure& other);
        virtual cOvalFigure *dup() const  {return new cOvalFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return Point(bounds.x, bounds.y);}
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds)  {this->bounds = bounds; doGeometryChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Note: bounds are that of the oval that this pie slice is part of.
 */
class SIM_API cPieSliceFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that the pie slice is part of
        double startAngle, endAngle; // in degrees, CCW, 0=east
    private:
        void copy(const cPieSliceFigure& other);
    public:
        cPieSliceFigure(const char *name=NULL) : cAbstractShapeFigure(name), startAngle(0), endAngle(45) {}
        cPieSliceFigure(const cPieSliceFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPieSliceFigure& operator=(const cPieSliceFigure& other);
        virtual cPieSliceFigure *dup() const  {return new cPieSliceFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return Point(bounds.x, bounds.y);}
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds)  {this->bounds = bounds; doGeometryChange();}
        virtual double getStartAngle() const {return startAngle;}
        virtual void setStartAngle(double startAngle) {this->startAngle = startAngle; doVisualChange();}
        virtual double getEndAngle() const {return endAngle;}
        virtual void setEndAngle(double endAngle) {this->endAngle = endAngle; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Note:
 * - smooth makes the polygon use Bezier curves, using midpoints of line segments as control points
 */
class SIM_API cPolygonFigure : public cAbstractShapeFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
    private:
        void copy(const cPolygonFigure& other);
    public:
        cPolygonFigure(const char *name=NULL) : cAbstractShapeFigure(name), smooth(false), joinStyle(JOIN_MITER) {}
        cPolygonFigure(const cPolygonFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPolygonFigure& operator=(const cPolygonFigure& other);
        virtual cPolygonFigure *dup() const  {return new cPolygonFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {static Point dummy; return points.empty() ? dummy : points[0];}
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points) {this->points = points; doGeometryChange();}
        virtual int getNumPoints() const {return points.size();}
        virtual const Point& getPoint(int i) const {return points[i];}
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth) {this->smooth = smooth; doVisualChange();}
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle) {this->joinStyle = joinStyle; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Note: font size is in non-zooming pixels (font doesn't grow with zoom)
 */
class SIM_API cTextFigure : public cFigure
{
    private:
        Point location;
        Color color;
        Font font;
        std::string text;
        Anchor anchor;
        Alignment alignment;
    private:
        void copy(const cTextFigure& other);
    public:
        cTextFigure(const char *name=NULL) : cFigure(name), color(BLACK), anchor(ANCHOR_NW), alignment(ALIGN_LEFT) {}
        cTextFigure(const cTextFigure& other) : cFigure(other) {copy(other);}
        cTextFigure& operator=(const cTextFigure& other);
        virtual cTextFigure *dup() const  {return new cTextFigure(*this);}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return location;}
        virtual const Point& getLocation() const  {return location;}
        virtual void setLocation(const Point& location)  {this->location = location; doGeometryChange();}
        virtual const Color& getColor() const  {return color;}
        virtual void setColor(const Color& color)  {this->color = color; doVisualChange();}
        virtual const Font& getFont() const  {return font;}
        virtual void setFont(Font font)  {this->font = font; doVisualChange();}
        virtual const char *getText() const  {return text.c_str();}
        virtual void setText(const char *text)  {this->text = text; doVisualChange();}
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor)  {this->anchor = anchor; doVisualChange();}
        virtual Alignment getAlignment() const  {return alignment;}
        virtual void setAlignment(Alignment alignment)  {this->alignment = alignment; doVisualChange();}
};

/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Displays an icon already known to Tkenv (i.e. one available in the "i" display string tag).
 *
 * Note: currently not implemented in Tkenv
 */
class SIM_API cImageFigure : public cFigure
{
    private:
        Point location;
        std::string imageName;
        Color tint;  // amount of tinting will come from alpha after it gets added to Color
        bool tinting; // temporary -- we'll use tint color's alpha
        Anchor anchor;
    private:
        void copy(const cImageFigure& other);
    public:
        cImageFigure(const cImageFigure& other) : cFigure(other) {copy(other);}
        cImageFigure& operator=(const cImageFigure& other);
        virtual cImageFigure *dup() const  {return new cImageFigure(*this);}
        cImageFigure(const char *name=NULL) : cFigure(name), tint(BLUE), tinting(false), anchor(ANCHOR_CENTER) {}
        virtual std::string info() const;
        virtual void parse(cProperty *property);
        virtual void translate(double x, double y);
        virtual Point getChildOrigin() const  {return location;}
        virtual const Point& getLocation() const  {return location;}
        virtual void setLocation(const Point& pos)  {this->location = pos; doGeometryChange();}
        virtual const char *getImageName() const  {return imageName.c_str();}
        virtual void setImageName(const char *imageName)  {this->imageName = imageName; doVisualChange();}
        virtual const Color& getTint() const  {return tint;}
        virtual void setTint(const Color& tint)  {this->tint = tint; tinting = true; doVisualChange();}
        virtual void clearTint() { tinting = false; }
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor)  {this->anchor = anchor; doVisualChange();}
};


/**
 * EXPERIMENTAL CLASS, NOT PART OF THE OFFICIAL OMNeT++ API! ALL DETAILS ARE SUBJECT TO CHANGE.
 *
 * Provides a scene graph based 2D drawing API for modules.
 *
 * Notes:
 * - all figures are under a root figure (which we try not to expose too much)
 * - Z-order is defined by child order (bottom-to-top) and preorder traversal
 * - layer-like functionality is provided via figure tags: the Tkenv UI will allow figures
 *   to be shown or hidden according to tags they contain
 * - cModule has one dedicated canvas, additional canvas objects can be created (and Tkenv *will* allow the user to inspect them)
 * - initial module canvas contents comes from @figure properties inside the compound module NED file, see test/anim/canvas for examples!
 * - extensibility: type=foo in a @figure property will cause the canvas to look for a registered FooFigure or cFooFigure class to instantiate
 * - dup() makes deep copy (duplicates the figure tree too)
 * - the submodules layer (see getSubmodulesLayer()) is currently an empty placeholder figure where Tkenv will
 *   draw modules and connections (by means outside cCanvas/cFigure) -- it can be used for Z-order positioning
 *   of other figures relative to the submodules and connections
 *
 */
class SIM_API cCanvas : public cOwnedObject
{
    private:
        cFigure *rootFigure;
        std::map<std::string,int> tagBitIndex;  // tag-to-bitindex
    protected:
        virtual void parseFigure(cProperty *property, std::map<cFigure*,double>& orderMap);
        virtual cFigure *createFigure(const char *type);
    public:
        // internal:
        static bool containsCanvasItems(cProperties *properties);
        virtual void addFiguresFrom(cProperties *properties);
        virtual uint64 parseTags(const char *s);
    private:
        void copy(const cCanvas& other);
    public:
        cCanvas(const char *name = NULL);
        cCanvas(const cCanvas& other) : cOwnedObject(other) {copy(other);}
        virtual ~cCanvas();
        cCanvas& operator=(const cCanvas& other);
        virtual cCanvas *dup() const  {return new cCanvas(*this);}
        virtual void forEachChild(cVisitor *v);
        virtual std::string info() const;

        virtual cFigure *getRootFigure() const {return rootFigure;}
        virtual void addFigure(cFigure *figure) {rootFigure->addFigure(figure);}
        virtual void addFigure(cFigure *figure, int pos) {rootFigure->addFigure(figure, pos);}
        virtual void addFigureAbove(cFigure *figure, cFigure *referenceFigure) {rootFigure->addFigureAbove(figure, referenceFigure);}
        virtual void addFigureBelow(cFigure *figure, cFigure *referenceFigure) {rootFigure->addFigureBelow(figure, referenceFigure);}
        virtual cFigure *removeFigure(int pos) {return rootFigure->removeFigure(pos);}
        virtual cFigure *removeFigure(cFigure *figure) {return rootFigure->removeFigure(figure);}
        virtual int findFigure(const char *name) {return rootFigure->findFigure(name);}
        virtual int findFigure(cFigure *figure) {return rootFigure->findFigure(figure);}
        virtual bool hasFigures() const {return rootFigure->containsFigures();}
        virtual int getNumFigures() const {return rootFigure->getNumFigures();} // note: returns the number of *child* figures, not the total number
        virtual cFigure *getFigure(int pos) {return rootFigure->getFigure(pos);}
        virtual cFigure *getFigure(const char *name) {return rootFigure->getFigure(name);}

        virtual cFigure *getSubmodulesLayer() const; // may return NULL (extra canvases don't have submodules)
        virtual cFigure *findFigureRecursively(const char *name) {return rootFigure->findFigureRecursively(name);}
        virtual cFigure *getFigureByPath(const char *path) {return rootFigure->getFigureByPath(path);}

        virtual std::string getAllTags() const;
        virtual std::vector<std::string> getAllTagsAsVector() const;
};

NAMESPACE_END


#endif



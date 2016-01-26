//==========================================================================
//   CCANVAS.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CCANVAS_H
#define __OMNETPP_CCANVAS_H

#include <string>
#include <map>
#include <vector>
#include "cownedobject.h"

namespace omnetpp {

class cCanvas;
class cProperty;
class cProperties;

/**
 * A lightweight graphical object. Figures are rendered on a canvas (see cCanvas).
 * Figures can be composed to create complex graphics.
 *
 * Notes:
 * - figures can be hierarchical (resulting in a figure tree)
 * - coordinates are in canvas units (usually meters), not in pixels!
 * - coordinates are transformed with the figure's own transform plus the transforms of all ancestors
 * - a figure subtree can be hidden by calling setVisible(false)
 * - dup() makes shallow copy (doesn't copy child figures); see dupTree() as well
 * - figures are only data storage classes, rendering is done in the back-end (e.g. Tkenv)
 *   by separate renderer classes.
 *
 * @see cCanvas
 * @ingroup Canvas
 */
class SIM_API cFigure : public cOwnedObject
{
    public:
        /**
         * Represents a point as (x,y) coordinates.
         * @ingroup Canvas
         */
        struct SIM_API Point {
            double x, y;
            Point() : x(0), y(0) {}
            Point(double x, double y) : x(x), y(y) {}
            Point operator + (const Point& p) const;
            Point operator - (const Point& p) const;
            Point operator * (double s) const;
            Point operator / (double s) const;
            double operator * (const Point& p) const;
            double distanceTo(const Point& p) const;
            double getLength() const;
            Point& translate(double dx, double dy) {x += dx; y += dy; return *this;}
            bool operator==(const Point& other) const {return x == other.x && y == other.y;}
            std::string str() const;
        };

        /**
         * Represents a rectangle as an (x,y,width,height) tuple.
         * @ingroup Canvas
         */
        struct SIM_API Rectangle {
            double x, y, width, height;
            Rectangle() : x(0), y(0), width(0), height(0) {}
            Rectangle(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}
            Point getCenter() const;
            Point getSize() const;
            Rectangle& translate(double dx, double dy) {x += dx; y += dy; return *this;}
            bool operator==(const Rectangle& other) const {return x == other.x && y == other.y && width == other.width && height == other.height;}
            std::string str() const;
        };

        /**
         * Represents an RGB color. Conversion from string exists, and accepts HTML colors (#rrggbb),
         * HSB colors in a similar notation (@hhssbb), and English color names (SVG color names,
         * to be more precise.) Predefined constants for the basic colors (see BLACK, WHITE,
         * GREY, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA), as well as a collection of good dark and
         * light colors, suitable for e.g. chart drawing (see GOOD_DARK_COLORS and GOOD_LIGHT_COLORS)
         * are provided.
         *
         * @ingroup Canvas
         */
        struct SIM_API Color {
            uint8_t red, green, blue;
            Color() : red(0), green(0), blue(0) {}
            Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}
            Color(const char *color) {*this = parseColor(color);}
            bool operator==(const Color& other) const {return red == other.red && green == other.green && blue == other.blue;}
            std::string str() const;
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

        static const int NUM_GOOD_DARK_COLORS;
        static const int NUM_GOOD_LIGHT_COLORS;
        static const Color GOOD_DARK_COLORS[14];
        static const Color GOOD_LIGHT_COLORS[10];

        /**
         * Represents properties of a font.
         * @ingroup Canvas
         */
        struct SIM_API Font {
            std::string typeface; // empty string means default font
            int pointSize;  // zero or negative value means default size
            uint8_t style;  // binary OR of FontStyle constants
            Font() : pointSize(0), style(FONT_NONE) {}
            Font(std::string typeface, int pointSize=-1, uint8_t style=FONT_NONE) : typeface(typeface), pointSize(pointSize), style(style) {}
            bool operator==(const Font& other) const {return typeface == other.typeface && pointSize == other.pointSize && style == other.style;}
            std::string str() const;
        };

        enum FontStyle { FONT_NONE=0, FONT_BOLD=1, FONT_ITALIC=2, FONT_UNDERLINE=4 };
        enum LineStyle { LINE_SOLID, LINE_DOTTED, LINE_DASHED };
        enum CapStyle { CAP_BUTT, CAP_SQUARE, CAP_ROUND };
        enum JoinStyle { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND };
        enum FillRule { FILL_EVENODD, FILL_NONZERO };
        enum Arrowhead { ARROW_NONE, ARROW_SIMPLE, ARROW_TRIANGLE, ARROW_BARBED };
        enum Interpolation { INTERPOLATION_NONE, INTERPOLATION_FAST, INTERPOLATION_BEST };
        enum Anchor {ANCHOR_CENTER, ANCHOR_N, ANCHOR_E, ANCHOR_S, ANCHOR_W, ANCHOR_NW, ANCHOR_NE, ANCHOR_SE, ANCHOR_SW, ANCHOR_BASELINE_START, ANCHOR_BASELINE_MIDDLE, ANCHOR_BASELINE_END };
        //enum Alignment { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER }; // note: multi-line text is always left-aligned in tkpath

        /**
         * Homogeneous 2D transformation matrix (last row is not stored).
         * <pre>
         *  | a  c  t1 |
         *  | b  d  t2 |
         *  | 0  0   1 |
         * </pre>
         *
         * @ingroup Canvas
         */
        struct SIM_API Transform {
                double a, b, c, d, t1, t2;
                Transform() : a(1), b(0), c(0), d(1), t1(0), t2(0) {}
                Transform(double a, double b, double c, double d, double t1, double t2) : a(a), b(b), c(c), d(d), t1(t1), t2(t2) {}
                Transform(const Transform& t) : a(t.a), b(t.b), c(t.c), d(t.d), t1(t.t1), t2(t.t2) {}
                Transform& operator=(const Transform& t) {a=t.a; b=t.b; c=t.c; d=t.d; t1=t.t1; t2=t.t2; return *this;}
                Transform& translate(double dx, double dy);
                Transform& scale(double s) {return scale(s,s);}
                Transform& scale(double sx, double sy);
                Transform& scale(double sx, double sy, double cx, double cy);
                Transform& scale(double sx, double sy, const Point& c) {return scale(sx, sy, c.x, c.y);}
                Transform& rotate(double phi);
                Transform& rotate(double phi, double cx, double cy);
                Transform& rotate(double phi, const Point& c) {return rotate(phi, c.x, c.y);}
                Transform& skewx(double coeff); // note: if you want to skew by an angle, use coeff = tan(phi)
                Transform& skewy(double coeff);
                Transform& skewx(double coeff, double cy);
                Transform& skewy(double coeff, double cx);
                Transform& multiply(const Transform& t); // left-multiply: *this = t * (*this)
                Transform& rightMultiply(const Transform& t); // *this = (*this) * t
                Point applyTo(const Point& p) const;
                bool operator==(const Transform& o) const {return a == o.a && b == o.b && c == o.c && d == o.d && t1 == o.t1 && t2 == o.t2;}
                std::string str() const;
        };

        /**
         * Represents an RGBA pixel, for Pixmap manipulation.
         * @ingroup Canvas
         */
        struct SIM_API RGBA {
            uint8_t red, green, blue, alpha;
            RGBA() : red(0), green(0), blue(0), alpha(0) {}
            RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : red(red), green(green), blue(blue), alpha(alpha) {}
            void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {red=r; green=g; blue=b; alpha=a;}
            void operator=(const Color& color) {red = color.red; green = color.green; blue = color.blue; alpha = 255;}
            operator Color() const {return Color(red, green, blue);}
            bool operator==(const RGBA& o) const {return red == o.red && green == o.green && blue == o.blue && alpha == o.alpha;}
            std::string str() const;
        };

        /**
         * A rectangular RGBA pixel array.
         * @see cPixmapImage
         * @ingroup Canvas
         */
        class SIM_API Pixmap {
            private:
                int width, height; // zero is allowed
                RGBA *data;
            private:
                void allocate(int width, int height);
                static uint8_t alpha(double opacity) {return opacity<=0 ? 0 : opacity>=1.0 ? 255 : (uint8_t)(opacity*255+0.5);}
            public:
                Pixmap();
                Pixmap(int width, int height);
                Pixmap(int width, int height, const RGBA& fill);
                Pixmap(int width, int height, const Color& color, double opacity=1);
                Pixmap(const Pixmap& other);
                ~Pixmap();
                Pixmap& operator=(const Pixmap& other);
                void setSize(int width, int height, const RGBA& fill_);  // nondestructive
                void setSize(int width, int height, const Color& color, double opacity);  // nondestructive
                void fill(const RGBA& fill_);
                void fill(const Color& color, double opacity);
                int getWidth() const {return width;}
                int getHeight() const {return height;}
                RGBA& pixel(int x, int y);
                const RGBA pixel(int x, int y) const {return const_cast<Pixmap*>(this)->pixel(x,y);}
                virtual void setPixel(int x, int y, const Color& color, double opacity=1.0) {RGBA& p = pixel(x,y); p.set(color.red, color.green, color.blue, alpha(opacity));}
                virtual const Color getColor(int x, int y) const {return (Color)pixel(x,y);}
                virtual void setColor(int x, int y, const Color& color) {RGBA& p = pixel(x,y); p.red = color.red; p.green = color.green; p.blue = color.blue;}
                virtual double getOpacity(int x, int y) const {return pixel(x,y).alpha / 255.0;}
                virtual void setOpacity(int x, int y, double opacity) {pixel(x,y).alpha = alpha(opacity);}
                const uint8_t *buffer() const {return (uint8_t*)data;} // direct access for low-level manipulation
                std::string str() const;
        };

        // internal:
        enum {
            CHANGE_STRUCTURAL = 1, // child added, removed, or reordered
            CHANGE_TRANSFORM = 2, // transform change
            CHANGE_GEOMETRY = 4, // geometry information (coords, start/end angle, etc)
            CHANGE_VISUAL = 8,  // styling
            CHANGE_INPUTDATA = 16,  // text, image name or pixmap data, value to be displayed, etc
            CHANGE_TAGS = 32  // figure tags
        };

    private:
        static int lastId;
        static cStringPool stringPool;
        int id;
        bool visible; // treated as structural change, for simpler handling
        Transform transform;
        std::vector<cFigure*> children;
        const char *tags; // stringpool'd
        uint64_t tagBits;  // bit-to-tagname mapping is stored in cCanvas. Note: change to std::bitset if 64 tags are not enough
        uint8_t localChanges;
        uint8_t subtreeChanges;

    protected:
        virtual bool isAllowedPropertyKey(const char *key) const;
        virtual void validatePropertyKeys(cProperty *property) const;
        virtual cFigure *getRootFigure() const;
        void fireStructuralChange() {fire(CHANGE_STRUCTURAL);}
        void fireTransformChange() {fire(CHANGE_TRANSFORM);}
        void fireGeometryChange() {fire(CHANGE_GEOMETRY);}
        void fireVisualChange() {fire(CHANGE_VISUAL);}
        void fireInputDataChange() {fire(CHANGE_INPUTDATA);}
        virtual void fire(uint8_t flags);
        static Point parsePoint(cProperty *property, const char *key, int index);
        static std::vector<Point> parsePoints(cProperty *property, const char *key);
        static Rectangle parseBounds(cProperty *property);
        static Transform parseTransform(cProperty *property, const char *key);
        static Font parseFont(cProperty *property, const char *key);
        static void concatArrays(const char **dest, const char **first, const char **second);

    public:
        // internal::
        static Point parsePoint(const char *s); // parse Point::str() format
        static Rectangle parseRectangle(const char *s); // parse Rectangle::str() format
        static Transform parseTransform(const char *s);  // parse Transform::str() format
        static Font parseFont(const char *s);  // parse Font::str() format
        static Color parseColor(const char *s);
        static bool parseBool(const char *s);
        static LineStyle parseLineStyle(const char *s);
        static CapStyle parseCapStyle(const char *s);
        static JoinStyle parseJoinStyle(const char *s);
        static FillRule parseFillRule(const char *s);
        static Arrowhead parseArrowhead(const char *s);
        static Interpolation parseInterpolation(const char *s);
        static Anchor parseAnchor(const char *s);

    public:
        // internal:
        virtual const char **getAllowedPropertyKeys() const;
        uint8_t getLocalChangeFlags() const {return localChanges;}
        uint8_t getSubtreeChangeFlags() const {return subtreeChanges;}
        void clearChangeFlags();
        void insertChild(cFigure *figure, std::map<cFigure*,double>& orderMap);
        void refreshTagBits();
        void refreshTagBitsRec();
        int64_t getTagBits() const {return tagBits;}
        void setTagBits(uint64_t tagBits) {this->tagBits = tagBits;}

    private:
        void copy(const cFigure& other);

    public:
        /** @name Constructors, destructor, assignment */
        //@{
        explicit cFigure(const char *name=nullptr);
        cFigure(const cFigure& other) : cOwnedObject(other), tags(nullptr) {copy(other);}
        virtual ~cFigure();
        cFigure& operator=(const cFigure& other);
        //@}

        /** @name Redefined cObject member functions. */
        //@{
        virtual cFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}  // see also dupTree()
        virtual void forEachChild(cVisitor *v) override;
        virtual std::string info() const override;
        //@}

        /** @name Miscellaneous. */
        //@{
        virtual void parse(cProperty *property);  // see getAllowedPropertyKeys(); plus, "x-*" keys can be added by the user
        virtual const char *getRendererClassName() const = 0;
        virtual void updateParentTransform(Transform& transform) {transform.rightMultiply(getTransform());}

        virtual void moveLocal(double x, double y) = 0;
        virtual void move(double x, double y);  // recursive
        virtual cCanvas *getCanvas() const;
        //@}

        /** @name Common figure attributes. */
        //@{
        int getId() const {return id;}
        virtual bool isVisible() const {return visible;} // affects figure subtree, not just this very figure
        virtual void setVisible(bool visible) {this->visible = visible; fireStructuralChange();}
        virtual const Transform& getTransform() const {return transform;}
        virtual void setTransform(const Transform& transform) {this->transform = transform; fireTransformChange();}
        virtual const char *getTags() const {return tags;} // returns space-separated list of tags
        virtual void setTags(const char *tags); // accepts space-separated list of tags
        //@}

        /** @name Operations on the transformation matrix. */
        //@{
        virtual void translate(double dx, double dy) {transform.translate(dx,dy); fireTransformChange();}
        virtual void scale(double s) {transform.scale(s); fireTransformChange();}
        virtual void scale(double sx, double sy) {transform.scale(sx,sy); fireTransformChange();}
        virtual void scale(double sx, double sy, double cx, double cy) {transform.scale(sx,sy,cx,cy); fireTransformChange();}
        virtual void scale(double sx, double sy, const Point& c) {scale(sx, sy, c.x, c.y);}
        virtual void rotate(double phi) {transform.rotate(phi); fireTransformChange();}
        virtual void rotate(double phi, double cx, double cy) {transform.rotate(phi,cx,cy); fireTransformChange();}
        virtual void rotate(double phi, const Point& c) {rotate(phi, c.x, c.y);}
        virtual void skewx(double phi) {transform.skewx(phi); fireTransformChange();}
        virtual void skewy(double phi) {transform.skewy(phi); fireTransformChange();}
        virtual void skewx(double phi, double cy) {transform.skewx(phi,cy); fireTransformChange();}
        virtual void skewy(double phi, double cx) {transform.skewy(phi,cx); fireTransformChange();}
        //@}

        /** @name Managing child figures. */
        //@{
        virtual void addFigure(cFigure *figure);
        virtual void addFigure(cFigure *figure, int pos);
        virtual void addFigureAbove(cFigure *figure, cFigure *referenceFigure);
        virtual void addFigureBelow(cFigure *figure, cFigure *referenceFigure);
        virtual cFigure *removeFromParent();
        virtual cFigure *removeFigure(int pos);
        virtual cFigure *removeFigure(cFigure *figure);
        virtual int findFigure(const char *name) const;
        virtual int findFigure(cFigure *figure) const;
        virtual bool containsFigures() const {return !children.empty();}
        virtual int getNumFigures() const {return children.size();}
        virtual cFigure *getFigure(int pos) const;
        virtual cFigure *getFigure(const char *name) const;
        virtual cFigure *getParentFigure() const {return dynamic_cast<cFigure*>(getOwner());}
        virtual void raiseAbove(cFigure *figure);
        virtual void lowerBelow(cFigure *figure);
        virtual void raiseToTop();
        virtual void lowerToBottom();
        //@}

        /** @name Accessing the figure tree. */
        //@{
        virtual cFigure *dupTree() const;
        virtual cFigure *findFigureRecursively(const char *name) const;
        virtual cFigure *getFigureByPath(const char *path) const;  //NOTE: path has similar syntax to cModule::getModuleByPath()
        //@}
};

// import the namespace to be able to use the stream write operators
namespace canvas_stream_ops {
#define STREAMOP(CLASS)  inline std::ostream& operator<<(std::ostream& os, const CLASS& x) { return os << x.str(); }
STREAMOP(cFigure::Point);
STREAMOP(cFigure::Rectangle);
STREAMOP(cFigure::Color);
STREAMOP(cFigure::Font);
STREAMOP(cFigure::Transform);
STREAMOP(cFigure::RGBA);
STREAMOP(cFigure::Pixmap);
#undef STREAMOP
};


/**
 * A figure with the sole purpose of grouping its children. It has no visual
 * appearance. The usefulness of a group figure comes from the fact that
 * transformations are inherited from parent to child, thus, children
 * of a group can be moved, scaled, rotated, etc. together by updating the
 * group's transformation matrix.
 *
 * @ingroup Canvas
 */
class SIM_API cGroupFigure : public cFigure
{
    private:
        void copy(const cGroupFigure& other) {}
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cGroupFigure(const char *name=nullptr) : cFigure(name) {}
        cGroupFigure(const cGroupFigure& other) : cFigure(other) {copy(other);}
        cGroupFigure& operator=(const cGroupFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cGroupFigure *dup() const override  {return new cGroupFigure(*this);}
        virtual std::string info() const override;
        virtual const char *getRendererClassName() const override {return "";} // non-visual figure
        virtual void moveLocal(double x, double y) override {}
        //@}
};

#if 0
/**
 * Sets up an axis-aligned, unscaled coordinate system for children, canceling the
 * effect of any transformation (scaling, rotation, etc.) inherited from ancestor figures.
 * This allows pixel-based positioning of children, and makes them immune to zooming.
 * The origin of the coordinate system is the position of the panel figure.
 * Setting a transformation on the panel figure itself allows rotation, scaling,
 * and adding pixel offset to the coordinate system.
 *
 * The panel figure itself has no visual appearance.
 *
 * @ingroup Canvas
 */
class SIM_API cPanelFigure : public cFigure
{
    private:
        Point position;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void parse(cProperty *property) override;
    private:
        void copy(const cPanelFigure& other);
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPanelFigure(const char *name=nullptr) : cFigure(name) {}
        cPanelFigure(const cPanelFigure& other) : cFigure(other) {copy(other);}
        cPanelFigure& operator=(const cPanelFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPanelFigure *dup() const override  {return new cPanelFigure(*this);}
        virtual std::string info() const override;
        virtual const char *getRendererClassName() const override {return "";} // non-visual figure
        virtual void updateParentTransform(Transform& transform) override;
        virtual void moveLocal(double x, double y) override {position.translate(x,y); fireTransformChange();}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Point& getPosition() const  {return position;}
        virtual void setPosition(const Point& position)  {this->position = position; fireGeometryChange();}
        //@}
};
#endif

/**
 * Common base class for line figures. Provides line color, style, width,
 * opacity, and other properties. Lines may also be augmented with arrowheads
 * at either or both ends.
 *
 * Transformations such as scaling or skew do affect the width of the line as it
 * is rendered on the canvas. Whether zooming (by the user) should also affect
 * it can be controlled by setting a flag (see setZoomLineWidth()).
 *
 * The rendering of zero-width lines is currently undefined. It is attempted
 * to be rendered as a one pixel wide line, regardless of transforms and zoom
 * level, but it is not possible on all platform.
 *
 * @ingroup Canvas
 */
class SIM_API cAbstractLineFigure : public cFigure
{
    private:
        Color lineColor;
        LineStyle lineStyle;
        double lineWidth;
        double lineOpacity;
        CapStyle capStyle;
        Arrowhead startArrowhead, endArrowhead;
        bool zoomLineWidth;
    private:
        void copy(const cAbstractLineFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractLineFigure(const char *name=nullptr) : cFigure(name), lineColor(BLACK), lineStyle(LINE_SOLID), lineWidth(1), lineOpacity(1), capStyle(CAP_BUTT), startArrowhead(ARROW_NONE), endArrowhead(ARROW_NONE), zoomLineWidth(false) {}
        cAbstractLineFigure(const cAbstractLineFigure& other) : cFigure(other) {copy(other);}
        cAbstractLineFigure& operator=(const cAbstractLineFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractLineFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor);
        virtual double getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(double lineWidth);
        virtual double getLineOpacity() const  {return lineOpacity;}
        virtual void setLineOpacity(double lineOpacity);
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle);
        virtual CapStyle getCapStyle() const {return capStyle;}
        virtual void setCapStyle(CapStyle capStyle);
        virtual Arrowhead getStartArrowhead() const  {return startArrowhead;}  //FIXME should be lowercase "h", as "arrowhead" is one word!
        virtual void setStartArrowhead(Arrowhead startArrowhead);
        virtual Arrowhead getEndArrowhead() const  {return endArrowhead;}
        virtual void setEndArrowhead(Arrowhead endArrowhead);
        virtual bool getZoomLineWidth() const {return zoomLineWidth;}
        virtual void setZoomLineWidth(bool zoomLineWidth);
        //@}
};

/**
 * A figure that displays a straight line segment. This class provides the
 * coordinates of the end points. Other properties such as color and line style
 * are inherited from cAbstractLineFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cLineFigure : public cAbstractLineFigure
{
    private:
        Point start, end;
    private:
        void copy(const cLineFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cLineFigure(const char *name=nullptr) : cAbstractLineFigure(name) {}
        cLineFigure(const cLineFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cLineFigure& operator=(const cLineFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cLineFigure *dup() const override  {return new cLineFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "LineFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Point& getStart() const  {return start;}
        virtual void setStart(const Point& start);
        virtual const Point& getEnd() const  {return end;}
        virtual void setEnd(const Point& end);
        //@}
};

/**
 * A figure that displays an arc. An arc's geometry is determined by the
 * bounding box of the circle or ellipse the arc is part of, and the
 * start and end angles. Other properties such as color and line style
 * are inherited from cAbstractLineFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cArcFigure : public cAbstractLineFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that arc is part of
        double startAngle, endAngle; // in radians, CCW, 0=east
    private:
        void copy(const cArcFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cArcFigure(const char *name=nullptr) : cAbstractLineFigure(name), startAngle(0), endAngle(0) {}
        cArcFigure(const cArcFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cArcFigure& operator=(const cArcFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cArcFigure *dup() const override  {return new cArcFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "ArcFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds);
        virtual double getStartAngle() const {return startAngle;}
        virtual void setStartAngle(double startAngle);
        virtual double getEndAngle() const {return endAngle;}
        virtual void setEndAngle(double endAngle);
        //@}
};

/**
 * A figure that displays a line consisting of multiple connecting straight
 * line segments. The class stores geometry information as a sequence of
 * points. The line may be <i>smoothed</i>. A smoothed line is drawn
 * as a series of Bezier curves, which touch the start point of the first
 * line segment, the end point of the last line segment, and the midpoints
 * of intermediate line segments, while intermediate points serve as control
 * points.
 *
 * Additional properties such as color and line style are inherited from
 * cAbstractLineFigure.
 *
 * @see cPathFigure
 * @ingroup Canvas
 */
class SIM_API cPolylineFigure : public cAbstractLineFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
    private:
        void copy(const cPolylineFigure& other);
        void checkIndex(int i) const;
        void checkInsIndex(int i) const;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPolylineFigure(const char *name=nullptr) : cAbstractLineFigure(name), smooth(false), joinStyle(JOIN_MITER) {}
        cPolylineFigure(const cPolylineFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cPolylineFigure& operator=(const cPolylineFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPolylineFigure *dup() const override  {return new cPolylineFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "PolylineFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points);
        virtual int getNumPoints() const {return points.size();}
        virtual const Point& getPoint(int i) const {checkIndex(i); return points[i];}
        virtual void setPoint(int i, const Point& point);
        virtual void addPoint(const Point& point);
        virtual void removePoint(int i);
        virtual void insertPoint(int i, const Point& point);
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth);
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle);
        //@}
};

/**
 * Abstract base class for various shapes, providing line and fill color,
 * line and fill opacity, line style, line width, and other properties
 * for them. Both outline and fill are optional.
 *
 * When the outline is drawn with a width larger than one pixel, it will be
 * drawn symmetrically, i.e. approximately 50-50% of its width will fall
 * inside and outside the shape.
 *
 * Transformations such as scaling or skew do affect the width of the line as it
 * is rendered on the canvas. Whether zooming (by the user) should also affect
 * it can be controlled by setting a flag (see setZoomLineWidth()).
 *
 * The rendering of zero-width lines is currently undefined. It is attempted
 * to be rendered as a one pixel wide line, regardless of transforms and zoom
 * level, but it is not possible on all platform.
 *
 * @ingroup Canvas
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
        double lineOpacity;
        double fillOpacity;
        bool zoomLineWidth;
    private:
        void copy(const cAbstractShapeFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractShapeFigure(const char *name=nullptr) : cFigure(name), outlined(true), filled(false), lineColor(BLACK), fillColor(BLUE), lineStyle(LINE_SOLID), lineWidth(1), lineOpacity(1), fillOpacity(1), zoomLineWidth(false) {}
        cAbstractShapeFigure(const cAbstractShapeFigure& other) : cFigure(other) {copy(other);}
        cAbstractShapeFigure& operator=(const cAbstractShapeFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractShapeFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        //@}

        /** @name Figure attributes */
        //@{
        virtual bool isFilled() const  {return filled;}
        virtual void setFilled(bool filled);
        virtual bool isOutlined() const  {return outlined;}
        virtual void setOutlined(bool outlined);
        virtual const Color& getLineColor() const  {return lineColor;}
        virtual void setLineColor(const Color& lineColor);
        virtual const Color& getFillColor() const  {return fillColor;}
        virtual void setFillColor(const Color& fillColor);
        virtual LineStyle getLineStyle() const  {return lineStyle;}
        virtual void setLineStyle(LineStyle lineStyle);
        virtual double getLineWidth() const  {return lineWidth;}
        virtual void setLineWidth(double lineWidth);   // note: rendering of zero-width lines undefined
        virtual double getLineOpacity() const  {return lineOpacity;}
        virtual void setLineOpacity(double lineOpacity);
        virtual double getFillOpacity() const  {return fillOpacity;}
        virtual void setFillOpacity(double fillOpacity);
        virtual bool getZoomLineWidth() const {return zoomLineWidth;}
        virtual void setZoomLineWidth(bool zoomLineWidth);
        //@}
};

/**
 * A figure that displays a rectangle, with optionally rounded corners.
 * As with all shape figures, drawing of both the outline and the fill
 * are optional. Line and fill color, and several other properties are
 * inherited from cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cRectangleFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds;
        double cornerRx, cornerRy;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    private:
        void copy(const cRectangleFigure& other);
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cRectangleFigure(const char *name=nullptr) : cAbstractShapeFigure(name), cornerRx(0), cornerRy(0) {}
        cRectangleFigure(const cRectangleFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cRectangleFigure& operator=(const cRectangleFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cRectangleFigure *dup() const override  {return new cRectangleFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "RectangleFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds);
        virtual void setCornerRadius(double r)  {setCornerRx(r);setCornerRy(r);}
        virtual double getCornerRx() const  {return cornerRx;}
        virtual void setCornerRx(double rx);
        virtual double getCornerRy() const  {return cornerRy;}
        virtual void setCornerRy(double ry);
        //@}
};

/**
 * A figure that draws a circle or ellipse. As with all shape figures, drawing
 * of both the outline and the fill are optional. Line and fill color, and
 * several other properties are inherited from cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cOvalFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box
    private:
        void copy(const cOvalFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cOvalFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cOvalFigure(const cOvalFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cOvalFigure& operator=(const cOvalFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cOvalFigure *dup() const override  {return new cOvalFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "OvalFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds);
        //@}
};

/**
 * A figure that displays a ring, with explicitly controllable inner/outer radii.
 * The inner/outer circles (or ellipses) form the outline, and the area between
 * them is filled. As with all shape figures, drawing of both the outline and
 * the fill are optional. Line and fill color, and several other properties are
 * inherited from cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cRingFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box
        double innerRx, innerRy;
    private:
        void copy(const cRingFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cRingFigure(const char *name=nullptr) : cAbstractShapeFigure(name), innerRx(0), innerRy(0) {}
        cRingFigure(const cRingFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cRingFigure& operator=(const cRingFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cRingFigure *dup() const override  {return new cRingFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "RingFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds);
        virtual void setInnerRadius(double r)   {setInnerRx(r);setInnerRy(r);}
        virtual double getInnerRx() const  {return innerRx;}
        virtual void setInnerRx(double rx);
        virtual double getInnerRy() const  {return innerRy;}
        virtual void setInnerRy(double ry);
        //@}
};

/**
 * A figure that displays a pie slice, that is, a section of an axis-aligned disc
 * or filled ellipse. A pie slice is determined by the bounding box of the full
 * disc or ellipse, and a start and an end angle. The outline of the pie slice
 * consists of an arc and two radii. As with all shape figures, drawing of
 * both the outline and the fill are optional. Line and fill color, and several
 * other properties are inherited from cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cPieSliceFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that the pie slice is part of
        double startAngle, endAngle; // in radians, CCW, 0=east
    private:
        void copy(const cPieSliceFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPieSliceFigure(const char *name=nullptr) : cAbstractShapeFigure(name), startAngle(0), endAngle(0) {}
        cPieSliceFigure(const cPieSliceFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPieSliceFigure& operator=(const cPieSliceFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPieSliceFigure *dup() const override  {return new cPieSliceFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "PieSliceFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Rectangle& getBounds() const  {return bounds;}
        virtual void setBounds(const Rectangle& bounds);
        virtual double getStartAngle() const {return startAngle;}
        virtual void setStartAngle(double startAngle);
        virtual double getEndAngle() const {return endAngle;}
        virtual void setEndAngle(double endAngle);
        //@}
};

/**
 * A figure that displays a (closed) polygon, determined by a sequence of points.
 * The polygon may be <i>smoothed</i>. A smoothed polygon is drawn as a series
 * of cubic Bezier curves, where the curves touch the midpoints of the sides,
 * and vertices serve as control points. As with all shape figures, drawing of
 * both the outline and the fill are optional. The drawing of filled self-
 * intersecting polygons is controlled by the <i>fill rule</i> property.
 * Line and fill color, and several other properties are inherited from
 * cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cPolygonFigure : public cAbstractShapeFigure
{
    private:
        std::vector<Point> points;
        bool smooth;
        JoinStyle joinStyle;
        FillRule fillRule;
    private:
        void copy(const cPolygonFigure& other);
        void checkIndex(int i) const;
        void checkInsIndex(int i) const;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPolygonFigure(const char *name=nullptr) : cAbstractShapeFigure(name), smooth(false), joinStyle(JOIN_MITER), fillRule(FILL_EVENODD) {}
        cPolygonFigure(const cPolygonFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPolygonFigure& operator=(const cPolygonFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPolygonFigure *dup() const override  {return new cPolygonFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "PolygonFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const std::vector<Point>& getPoints() const  {return points;}
        virtual void setPoints(const std::vector<Point>& points);
        virtual int getNumPoints() const {return points.size();}
        virtual const Point& getPoint(int i) const {checkIndex(i); return points[i];}
        virtual void setPoint(int i, const Point& point);
        virtual void addPoint(const Point& point);
        virtual void removePoint(int i);
        virtual void insertPoint(int i, const Point& point);
        virtual bool getSmooth() const {return smooth;}
        virtual void setSmooth(bool smooth);
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle);
        virtual FillRule getFillRule() const {return fillRule;}
        virtual void setFillRule(FillRule fillRule);
        //@}
};

/**
 * A figure that displays a "path", a complex shape or line modeled after SVG
 * paths. A path is may consist of any number of straight line segments, Bezier
 * curves and arcs. The path can be disjoint as well. Closed paths may be filled.
 * The drawing of filled self-intersecting polygons is controlled by the
 * <i>fill rule</i> property. Line and fill color, and several other properties
 * are inherited from cAbstractShapeFigure.
 *
 * The path may be specified with a string similar to an SVG path, or assembled
 * by calling methods that append new segments (straight lines, arcs or Bezier
 * curves) to the existing path.
 *
 * @ingroup Canvas
 */
class SIM_API cPathFigure : public cAbstractShapeFigure
{
    public:
        struct PathItem { char code; };
        struct MoveTo : PathItem { double x; double y; };  // M
        struct MoveRel : PathItem { double dx; double dy; };  // m
        struct LineTo : PathItem { double x; double y; };  // L
        struct LineRel : PathItem { double dx; double dy; };  // l
        struct HorizLineTo : PathItem { double x; };  // H
        struct HorizLineRel : PathItem { double dx; };  // h
        struct VertLineTo : PathItem { double y; };  // V
        struct VertLineRel : PathItem { double dy; };  // v
        struct ArcTo : PathItem { double rx; double ry; double phi; bool largeArc; bool sweep; double x; double y; }; // A
        struct ArcRel : PathItem { double rx; double ry; double phi; bool largeArc; bool sweep; double dx; double dy; }; // a
        struct CurveTo : PathItem { double x1; double y1; double x; double y; }; // Q
        struct CurveRel : PathItem { double dx1; double dy1; double dx; double dy; }; // q
        struct SmoothCurveTo : PathItem { double x; double y; }; // T
        struct SmoothCurveRel : PathItem { double dx; double dy; }; // t
        struct CubicBezierCurveTo : PathItem { double x1; double y1; double x2; double y2; double x; double y; }; // C
        struct CubicBezierCurveRel : PathItem { double dx1; double dy1; double dx2; double dy2; double dx; double dy; }; // c
        struct SmoothCubicBezierCurveTo : PathItem { double x2; double y2; double x; double y; }; // S
        struct SmoothCubicBezierCurveRel : PathItem { double dx2; double dy2; double dx; double dy; }; // s
        struct ClosePath : PathItem { }; // Z

    private:
        std::vector<PathItem*> path;
        mutable std::string cachedPathString;
        JoinStyle joinStyle;
        CapStyle capStyle;
        Point offset;
        FillRule fillRule;

    private:
        void copy(const cPathFigure& other);
        void addItem(PathItem *item);
        void doClearPath();

    protected:
        virtual const char **getAllowedPropertyKeys() const override;

    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPathFigure(const char *name=nullptr) : cAbstractShapeFigure(name), joinStyle(JOIN_MITER), capStyle(CAP_BUTT), fillRule(FILL_EVENODD) {}
        cPathFigure(const cPathFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPathFigure& operator=(const cPathFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPathFigure *dup() const override  {return new cPathFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        /**
         * The move operation modifies the offset field (see getOffset()).
         */
        virtual void moveLocal(double x, double y) override;
        virtual const char *getRendererClassName() const override {return "PathFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual JoinStyle getJoinStyle() const {return joinStyle;}
        virtual void setJoinStyle(JoinStyle joinStyle);
        virtual CapStyle getCapStyle() const {return capStyle;}
        virtual void setCapStyle(CapStyle capStyle);
        virtual FillRule getFillRule() const {return fillRule;}
        virtual void setFillRule(FillRule fillRule);
        virtual const Point& getOffset() const {return offset;}
        virtual void setOffset(const Point& offset) {this->offset = offset; fireGeometryChange(); fireTransformChange();}
        //@}

        /** @name Path manipulation */
        //@{
        virtual const char *getPath() const;
        virtual void setPath(const char *path);
        virtual int getNumPathItems() const {return path.size();}
        virtual const PathItem *getPathItem(int k) const {return path[k];}
        virtual void clearPath();
        virtual void addMoveTo(double x, double y);  // M x y
        virtual void addMoveRel(double dx, double dy);  // m dx dy
        virtual void addLineTo(double x, double y);  // L x y
        virtual void addLineRel(double dx, double dy);  // l dx dy
        virtual void addHorizontalLineTo(double x);  // H x
        virtual void addHorizontalLineRel(double dx);  // h dx
        virtual void addVerticalLineTo(double y);  // V y
        virtual void addVerticalLineRel(double dy);  // v dy
        virtual void addArcTo(double rx, double ry, double phi, bool largeArc, bool sweep, double x, double y); // A rx ry phi largeArc sweep x y
        virtual void addArcRel(double rx, double ry, double phi, bool largeArc, bool sweep, double dx, double dy); // a rx ry phi largeArc sweep dx dy
        virtual void addCurveTo(double x1, double y1, double x, double y); // Q x1 y1 x y
        virtual void addCurveRel(double dx1, double dy1, double dx, double dy); // q x1 y1 dx dy
        virtual void addSmoothCurveTo(double x, double y); // T x y
        virtual void addSmoothCurveRel(double dx, double dy); // t dx dy
        virtual void addCubicBezierCurveTo(double x1, double y1, double x2, double y2, double x, double y); // C x1 y1 x2 y2 x y
        virtual void addCubicBezierCurveRel(double dx1, double dy1, double dx2, double dy2, double dx, double dy); // c x1 y1 x2 y2 dx dy
        virtual void addSmoothCubicBezierCurveTo(double x2, double y2, double x, double y); // S x2 y2 x y
        virtual void addSmoothCubicBezierCurveRel(double dx2, double dy2, double dx, double dy); // s x2 y2 dx dy
        virtual void addClosePath(); // Z
        //@}
};

/**
 * Abstract base class for figures that display text. Text may be multi-line.
 *
 * The location of the text on the canvas is determined jointly by the
 * <i>position</i> and <i>anchor</i> properties. The anchor tells how to
 * position the text relative to the positioning point. For example,
 * if anchor is ANCHOR_CENTER then the text is centered on the point;
 * if anchor is ANCHOR_N then the text will be drawn so that its top center
 * is at the positioning point. The values ANCHOR_BASELINE_START,
 * ANCHOR_BASELINE_MIDDLE, ANCHOR_BASELINE_END refer to the beginning,
 * middle and end of the baseline of the (first line of the) text as
 * anchor point. Anchor defaults to ANCHOR_CENTER.
 *
 * Other properties in this class define the font, color and opacity of the text.
 *
 * @ingroup Canvas
 */
class SIM_API cAbstractTextFigure : public cFigure
{
    private:
        Point position;
        Color color;  // note: tkpath's text supports separate colors and opacity for fill and outline -- ignore because probably SWT doesn't support it!
        double opacity;
        Font font;
        std::string text;
        Anchor anchor;
    private:
        void copy(const cAbstractTextFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractTextFigure(const char *name=nullptr) : cFigure(name), color(BLACK), opacity(1), anchor(ANCHOR_NW) {}
        cAbstractTextFigure(const cAbstractTextFigure& other) : cFigure(other) {copy(other);}
        cAbstractTextFigure& operator=(const cAbstractTextFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractTextFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        /**
         * Updates the position of the text, see getPosition().
         */
        virtual void moveLocal(double x, double y) override;
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Point& getPosition() const  {return position;}
        virtual void setPosition(const Point& position);
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor);
        virtual const Color& getColor() const  {return color;}
        virtual void setColor(const Color& color);
        virtual double getOpacity() const  {return opacity;}
        virtual void setOpacity(double opacity);
        virtual const Font& getFont() const  {return font;}
        virtual void setFont(Font font);
        virtual const char *getText() const  {return text.c_str();}
        virtual void setText(const char* text);
        //@}
};

/**
 * A figure that displays text which is affected by zooming and transformations.
 * Font, color, position, anchoring and other properties are inherited from
 * cAbstractTextFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cTextFigure : public cAbstractTextFigure
{
    private:
        void copy(const cTextFigure& other) {}
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cTextFigure(const char *name=nullptr) : cAbstractTextFigure(name) {}
        cTextFigure(const cTextFigure& other) : cAbstractTextFigure(other) {copy(other);}
        cTextFigure& operator=(const cTextFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cTextFigure *dup() const override  {return new cTextFigure(*this);}
        virtual const char *getRendererClassName() const override {return "TextFigureRenderer";}
        //@}
};

/**
 * A figure that displays text which is unaffected by zooming or
 * transformations, except its position.
 * Font, color, position, anchoring and other properties are inherited from
 * cAbstractTextFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cLabelFigure : public cAbstractTextFigure
{
    private:
        void copy(const cLabelFigure& other) {}
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cLabelFigure(const char *name=nullptr) : cAbstractTextFigure(name) {}
        cLabelFigure(const cLabelFigure& other) : cAbstractTextFigure(other) {copy(other);}
        cLabelFigure& operator=(const cLabelFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cLabelFigure *dup() const override  {return new cLabelFigure(*this);}
        virtual const char *getRendererClassName() const override {return "LabelFigureRenderer";}
        //@}
};

/**
 * Abstract base class for figures that display an image.
 *
 * The location of the image on the canvas is determined jointly by the
 * <i>position</i> and <i>anchor</i> properties. The anchor tells how to
 * position the image relative to the positioning point. For example,
 * if anchor is ANCHOR_CENTER then the image is centered on the point;
 * if anchor is ANCHOR_N then the image will be drawn so that its top center
 * point is at the positioning point. Anchor defaults to ANCHOR_CENTER.
 *
 * Images may be drawn at their "natural" size, or may be scaled to a
 * specified size by setting the width and/or height properties. One can
 * choose from several interpolation modes that control how the image is
 * rendered. Interpolation defaults to INTERPOLATION_FAST.
 *
 * Images can be tinted; this feature is controlled by a tint color and
 * a tint amount, a [0,1] real number.
 *
 * Images may be rendered as partially transparent, which is controlled
 * by the opacity property, a [0,1] real number. (The rendering process
 * will combine this property with the transparency information contained
 * within the image, i.e. the alpha channel.)
 *
 * @ingroup Canvas
 */
class SIM_API cAbstractImageFigure : public cFigure
{
    private:
        Point position;
        Anchor anchor;  // note: do not use the ANCHOR_BASELINE_START/MIDDLE/END constants, as they are for text items
        double width, height; // zero or negative values mean using the image's own size
        Interpolation interpolation;
        double opacity;
        Color tintColor;
        double tintAmount; // in the range 0..1
    private:
        void copy(const cAbstractImageFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractImageFigure(const char *name=nullptr) : cFigure(name), anchor(ANCHOR_CENTER), width(0), height(0), interpolation(INTERPOLATION_FAST), opacity(1), tintColor(BLUE), tintAmount(0) { }
        cAbstractImageFigure(const cAbstractImageFigure& other) : cFigure(other) {copy(other);}
        cAbstractImageFigure& operator=(const cAbstractImageFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractImageFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual void parse(cProperty *property) override;
        /**
         * Updates the position of the image, see getPosition().
         */
        virtual void moveLocal(double x, double y) override;
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Point& getPosition() const  {return position;}
        virtual void setPosition(const Point& position);
        virtual Anchor getAnchor() const  {return anchor;}
        virtual void setAnchor(Anchor anchor);
        virtual double getWidth() const  {return width;}
        virtual void setWidth(double width);  // zero means "unset"
        virtual double getHeight() const  {return height;}
        virtual void setHeight(double height); // zero means "unset"
        virtual void setSize(double width, double height) {setWidth(width); setHeight(height);}
        virtual Interpolation getInterpolation() const {return interpolation;}
        virtual void setInterpolation(Interpolation interpolation);
        virtual double getOpacity() const  {return opacity;}
        virtual void setOpacity(double opacity);
        virtual const Color& getTintColor() const  {return tintColor;}
        virtual void setTintColor(const Color& tintColor);
        virtual double getTintAmount() const  {return tintAmount;}
        virtual void setTintAmount(double tintAmount);
        //@}
};

/**
 * A figure that displays an image, typically an icon or a background image,
 * loaded from the OMNeT++ image path. Positioning and other properties
 * are inherited from cAbstractImageFigure.
 *
 * @see cIconFigure, cPixmapFigure
 * @ingroup Canvas
 */
class SIM_API cImageFigure : public cAbstractImageFigure
{
    private:
        std::string imageName;
    private:
        void copy(const cImageFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cImageFigure(const char *name=nullptr) : cAbstractImageFigure(name) {}
        cImageFigure(const cImageFigure& other) : cAbstractImageFigure(other) {copy(other);}
        cImageFigure& operator=(const cImageFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cImageFigure *dup() const override  {return new cImageFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual const char *getRendererClassName() const override {return "ImageFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const char *getImageName() const  {return imageName.c_str();}
        virtual void setImageName(const char* imageName);
        //@}
};

/**
 * A figure that displays an image, typically an icon or a background image,
 * loaded from the OMNeT++ image path, in a non-zooming way.
 *
 * Positioning and other properties are inherited from cAbstractImageFigure.
 *
 * Note: not affected by transforms or zoom, except its position
 *
 * @ingroup Canvas
 */
class SIM_API cIconFigure : public cImageFigure
{
    private:
        void copy(const cIconFigure& other) {}
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cIconFigure(const char *name=nullptr) : cImageFigure(name) {}
        cIconFigure(const cIconFigure& other) : cImageFigure(other) {copy(other);}
        cIconFigure& operator=(const cIconFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cIconFigure *dup() const override  {return new cIconFigure(*this);}
        virtual const char *getRendererClassName() const override {return "IconFigureRenderer";}
        //@}
};

/**
 * A figure that displays an image that can be manipulated programmatically.
 * A pixmap figure may be used to display e.g. a heat map. Support for scaling
 * and various interpolation modes are useful here.
 *
 * @see cFigure::Pixmap
 * @ingroup Canvas
 */
class SIM_API cPixmapFigure : public cAbstractImageFigure
{
    private:
        Pixmap pixmap;
    private:
        void copy(const cPixmapFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPixmapFigure(const char *name=nullptr) : cAbstractImageFigure(name) {}
        cPixmapFigure(const cPixmapFigure& other) : cAbstractImageFigure(other) {copy(other);}
        virtual ~cPixmapFigure() {}
        cPixmapFigure& operator=(const cPixmapFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPixmapFigure *dup() const override  {return new cPixmapFigure(*this);}
        virtual std::string info() const override;
        virtual void parse(cProperty *property) override;
        virtual const char *getRendererClassName() const override {return "PixmapFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Pixmap& getPixmap() const {return pixmap;}
        virtual void setPixmap(const Pixmap& pixmap);
        virtual int getPixmapHeight() const {return pixmap.getHeight();}
        virtual int getPixmapWidth() const {return pixmap.getWidth();}
        virtual void setPixmapSize(int width, int height, const RGBA& fill); // nondestructive, set *newly added* pixels with this color and opacity
        virtual void setPixmapSize(int width, int height, const Color& color, double opacity);  // nondestructive, fills *newly added* pixels with this color and opacity
        virtual void fillPixmap(const RGBA& fill);
        virtual void fillPixmap(const Color& color, double opacity);
        virtual const RGBA getPixel(int x, int y) const {return pixmap.pixel(x, y);}
        virtual void setPixel(int x, int y, const RGBA& argb);
        virtual void setPixel(int x, int y, const Color& color, double opacity = 1.0);
        virtual const Color getPixelColor(int x, int y) const {return pixmap.getColor(x,y);}
        virtual void setPixelColor(int x, int y, const Color& color);
        virtual double getPixelOpacity(int x, int y) const {return pixmap.getOpacity(x,y);}
        virtual void setPixelOpacity(int x, int y, double opacity);
        //@}
};

/**
 * Provides a scene graph based 2D drawing API for modules.
 *
 * Notes:
 * - all figures are under a root figure (which we try not to expose too much)
 * - Z-order is defined by child order (bottom-to-top) and preorder traversal
 * - layer-like functionality is provided via figure tags: the Tkenv UI will allow figures
 *   to be shown or hidden according to tags they contain
 * - cModule has one dedicated canvas, additional canvas objects can be created
 * - initial module canvas contents comes from @figure properties inside the compound module NED file, see samples/canvas for examples!
 * - extensibility: type=foo in a @figure property will cause the canvas to look for a registered FooFigure or cFooFigure class to instantiate
 * - dup() makes deep copy (duplicates the figure tree too)
 * - the submodules layer (see getSubmodulesLayer()) is currently an empty placeholder figure where Tkenv will
 *   draw modules and connections (by means outside cCanvas/cFigure) -- it can be used for Z-order positioning
 *   of other figures relative to the submodules and connections
 *
 * @ingroup Canvas
 */
class SIM_API cCanvas : public cOwnedObject
{
    private:
        cFigure::Color backgroundColor;
        cFigure *rootFigure;
        std::map<std::string,int> tagBitIndex;  // tag-to-bitindex
    protected:
        virtual void parseFigure(cProperty *property, std::map<cFigure*,double>& orderMap) const;
        virtual cFigure *createFigure(const char *type) const ;
    public:
        // internal:
        static bool containsCanvasItems(cProperties *properties);
        virtual void addFiguresFrom(cProperties *properties);
        virtual uint64_t parseTags(const char *s);
        virtual std::string getTags(uint64_t tagBits);
        void dumpSupportedPropertyKeys(std::ostream& out) const;
    private:
        void copy(const cCanvas& other);
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cCanvas(const char *name = nullptr);
        cCanvas(const cCanvas& other) : cOwnedObject(other), rootFigure(nullptr) {copy(other);}
        virtual ~cCanvas();
        cCanvas& operator=(const cCanvas& other);
        //@}

        /** @name Redefined cObject member functions. */
        //@{
        virtual cCanvas *dup() const override  {return new cCanvas(*this);}
        virtual void forEachChild(cVisitor *v) override;
        virtual std::string info() const override;
        //@}

        /** @name Canvas attributes. */
        //@{
        virtual const cFigure::Color& getBackgroundColor() const {return backgroundColor;}
        virtual void setBackgroundColor(const cFigure::Color& color) {this->backgroundColor = color;}
        //@}

        /** @name Managing child figures. */
        //@{
        virtual cFigure *getRootFigure() const {return rootFigure;}
        virtual void addFigure(cFigure *figure) {rootFigure->addFigure(figure);}
        virtual void addFigure(cFigure *figure, int pos) {rootFigure->addFigure(figure, pos);}
        virtual void addFigureAbove(cFigure *figure, cFigure *referenceFigure) {rootFigure->addFigureAbove(figure, referenceFigure);}
        virtual void addFigureBelow(cFigure *figure, cFigure *referenceFigure) {rootFigure->addFigureBelow(figure, referenceFigure);}
        virtual cFigure *removeFigure(int pos) {return rootFigure->removeFigure(pos);}
        virtual cFigure *removeFigure(cFigure *figure) {return rootFigure->removeFigure(figure);}
        virtual int findFigure(const char *name) const  {return rootFigure->findFigure(name);}
        virtual int findFigure(cFigure *figure) const  {return rootFigure->findFigure(figure);}
        virtual bool hasFigures() const {return rootFigure->containsFigures();}
        virtual int getNumFigures() const {return rootFigure->getNumFigures();} // note: returns the number of *child* figures, not the total number
        virtual cFigure *getFigure(int pos) const {return rootFigure->getFigure(pos);}
        virtual cFigure *getFigure(const char *name) const  {return rootFigure->getFigure(name);}
        //@}

        /** @name Accessing the figure tree. */
        //@{
        virtual cFigure *getSubmodulesLayer() const; // may return nullptr (extra canvases don't have submodules)
        virtual cFigure *findFigureRecursively(const char *name) const {return rootFigure->findFigureRecursively(name);}
        virtual cFigure *getFigureByPath(const char *path) const {return rootFigure->getFigureByPath(path);}
        //@}

        /** @name Figure tags. */
        //@{
        virtual std::string getAllTags() const;
        virtual std::vector<std::string> getAllTagsAsVector() const;
        //@}
};

}  // namespace omnetpp


#endif



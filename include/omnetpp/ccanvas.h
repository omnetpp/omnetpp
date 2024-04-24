//==========================================================================
//   CCANVAS.H  -  header for
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

#ifndef __OMNETPP_CCANVAS_H
#define __OMNETPP_CCANVAS_H

#include <string>
#include <map>
#include <vector>
#include <cmath>
#include "cownedobject.h"
#include "opp_pooledstring.h"

namespace omnetpp {

class cCanvas;
class cProperty;
class cProperties;
class cObjectFactory;
class cHasher;

//TODO: doc: default values as precise enum names
//TODO: doc: @figure attributes for each figure type
//TODO: doc: revise class descriptions

/**
 * @brief A lightweight graphical object for cCanvas.
 *
 * This is the central class of \opp's 2D drawing API. Graphical elements
 * are called figures, and are represented by subclasses of cFigure.
 * To display a figure-based drawing, one needs to add figures to a canvas
 * (cCanvas). Every module has its own canvas, accessible via
 * cModule::getCanvas(), and one may also create additional ones.
 *
 * Notes:
 * - figures can be hierarchical (resulting in a figure tree)
 * - coordinates are in canvas units, not in pixels (interactive zooming!)
 * - coordinates are transformed with the figure's own transform plus the transforms of all ancestors
 * - a figure subtree can be hidden by calling setVisible(false)
 * - the order of child figures also denotes drawing order (of overlapping figures, the last one will appear on top)
 * - dup() makes shallow copy (doesn't copy child figures); see dupTree() as well
 * - figures are only data storage classes, rendering is done in the back-end (e.g. Qtenv)
 *   by separate renderer classes.
 *
 * @see cCanvas
 * @ingroup Canvas
 */
class SIM_API cFigure : public cOwnedObject
{
    public:
        /**
         * @brief Represents a point as (x,y) coordinates.
         * @ingroup Canvas
         */
        struct SIM_API Point {
            /** @name Coordinates. */
            //@{
            double x = 0, y = 0;
            //@}
            /** @name Methods. */
            //@{
            Point() {}
            Point(double x, double y) : x(x), y(y) {}
            Point operator + (const Point& p) const;
            Point operator - (const Point& p) const;
            Point operator * (double s) const;
            Point operator / (double s) const;
            double operator * (const Point& p) const;
            double distanceTo(const Point& p) const;
            double getLength() const;
            double getAngle() const {return std::atan2(y,x);}
            Point& translate(double dx, double dy) {x += dx; y += dy; return *this;}
            bool operator==(const Point& other) const {return x == other.x && y == other.y;}
            std::string str() const;
            //@}
        };

        /**
         * @brief Represents a rectangle as an (x,y,width,height) tuple.
         * @ingroup Canvas
         */
        struct SIM_API Rectangle {
            /** @name Rectangle geometry as top-left corner and size. */
            //@{
            double x = 0, y = 0, width = 0, height = 0;
            //@}
            /** @name Methods. */
            //@{
            Rectangle() {}
            Rectangle(double x, double y, double width, double height) : x(x), y(y), width(width), height(height) {}
            Point getCenter() const;
            Point getSize() const;
            Rectangle& translate(double dx, double dy) {x += dx; y += dy; return *this;}
            bool operator==(const Rectangle& other) const {return x == other.x && y == other.y && width == other.width && height == other.height;}
            std::string str() const;
            //@}
        };

        /**
         * @brief Represents an RGB color.
         *
         * Conversion from string exists, and accepts HTML colors (\#rrggbb),
         * HSB colors in a similar notation (@hhssbb), and English color names
         * (SVG color names, to be more precise.) Predefined constants for the
         * basic colors (see BLACK, WHITE, GREY, RED, GREEN, BLUE, YELLOW, CYAN,
         * MAGENTA), as well as a collection of good dark and light colors, suitable
         * for e.g. chart drawing (see GOOD_DARK_COLORS and GOOD_LIGHT_COLORS)
         * are provided.
         *
         * @ingroup Canvas
         */
        struct SIM_API Color {
            /** @name RGB color components. */
            //@{
            uint8_t red = 0, green = 0, blue = 0;
            //@}
            /** @name Methods. */
            //@{
            Color() {}
            Color(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}
            Color(const char *color) {*this = parseColor(color);}
            Color(const Color& other) = default;
            bool operator==(const Color& other) const {return red == other.red && green == other.green && blue == other.blue;}
            std::string str() const;
            //@}
        };

        /** @name Predefined colors. */
        //@{
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
        //@}

        /**
         * @brief Represents properties of a font.
         * @ingroup Canvas
         */
        struct SIM_API Font {
            /** @name Font attributes. */
            //@{
            std::string typeface; ///< Typeface of the font. An empty string means the default font.
            int pointSize = 0;  ///< Font size in points. A zero or negative value means the default size.
            uint8_t style = FONT_NONE;  ///< Font style. Binary OR of FontStyle constants such as FONT_BOLD.
            //@}
            /** @name Methods. */
            //@{
            Font() {}
            Font(std::string typeface, int pointSize=-1, uint8_t style=FONT_NONE) : typeface(typeface), pointSize(pointSize), style(style) {}
            Font(const Font& other) = default;
            bool operator==(const Font& other) const {return typeface == other.typeface && pointSize == other.pointSize && style == other.style;}
            std::string str() const;
            //@}
        };

        /** @brief Font style constants: FONT_NONE, FONT_BOLD, etc. @ingroup Canvas */
        enum FontStyle { FONT_NONE=0, FONT_BOLD=1, FONT_ITALIC=2, FONT_UNDERLINE=4 };

        /** @brief Line style constants: LINE_SOLID, LINE_DOTTED, etc. @ingroup Canvas */
        enum LineStyle { LINE_SOLID, LINE_DOTTED, LINE_DASHED };

        /** @brief Line cap style constants: CAP_BUTT, CAP_SQUARE, etc. @ingroup Canvas */
        enum CapStyle { CAP_BUTT, CAP_SQUARE, CAP_ROUND };

        /** @brief Line join style constants: JOIN_BEVEL, JOIN_MITER, etc. @ingroup Canvas */
        enum JoinStyle { JOIN_BEVEL, JOIN_MITER, JOIN_ROUND };

        /** @brief Fill rule constants: FILL_EVENODD, FILL_NONZERO. @ingroup Canvas */
        enum FillRule { FILL_EVENODD, FILL_NONZERO };

        /** @brief Arrowhead style constants: ARROW_NONE, ARROW_SIMPLE, etc. @ingroup Canvas */
        enum Arrowhead { ARROW_NONE, ARROW_SIMPLE, ARROW_TRIANGLE, ARROW_BARBED };

        /** @brief Image interpolation mode constants: INTERPOLATION_NONE, INTERPOLATION_FAST, etc. @ingroup Canvas */
        enum Interpolation { INTERPOLATION_NONE, INTERPOLATION_FAST, INTERPOLATION_BEST };

        /** @brief Anchoring mode constants: ANCHOR_CENTER, ANCHOR_N, etc. @ingroup Canvas */
        enum Anchor {ANCHOR_CENTER, ANCHOR_N, ANCHOR_E, ANCHOR_S, ANCHOR_W, ANCHOR_NW, ANCHOR_NE, ANCHOR_SE, ANCHOR_SW, ANCHOR_BASELINE_START, ANCHOR_BASELINE_MIDDLE, ANCHOR_BASELINE_END };

        /** @brief Text alignment mode constants: ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER @ingroup Canvas */
        enum Alignment { ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER };

        /**
         * @brief Homogeneous 2D transformation matrix.
         *
         * Note that the last row is not stored.
         * ```
         *  | a  c  t1 |
         *  | b  d  t2 |
         *  | 0  0   1 |
         * ```
         *
         * @ingroup Canvas
         */
        struct SIM_API Transform {
            /** @name Elements of the transformation matrix. */
            //@{
            double a = 1, b = 0, c = 0, d = 1, t1 = 0, t2 = 0;
            //@}

            /** @name Methods. */
            //@{
            Transform() {}
            Transform(double a, double b, double c, double d, double t1, double t2) : a(a), b(b), c(c), d(d), t1(t1), t2(t2) {}
            Transform(const Transform& t) = default;
            Transform& operator=(const Transform& t) = default;
            Transform& translate(double dx, double dy);
            Transform& translate(const Point& p) {return translate(p.x, p.y);}
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
            //@}
        };

        /**
         * @brief Represents an RGBA pixel, for Pixmap manipulation.
         * @ingroup Canvas
         */
        struct SIM_API RGBA {
            /** @name Components of the pixel. */
            //@{
            uint8_t red = 0, green = 0, blue = 0, alpha = 0;
            //@}
            /** @name Methods. */
            //@{
            RGBA() {}
            RGBA(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) : red(red), green(green), blue(blue), alpha(alpha) {}
            explicit RGBA(const Color& color, double opacity=1) : red(color.red), green(color.green), blue(color.blue), alpha(toAlpha(opacity)) {}
            RGBA(const RGBA& other) = default;
            void set(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {red=r; green=g; blue=b; alpha=a;}
            void operator=(const Color& color) {red = color.red; green = color.green; blue = color.blue; alpha = 255;}
            operator Color() const {return Color(red, green, blue);}
            bool operator==(const RGBA& o) const {return red == o.red && green == o.green && blue == o.blue && alpha == o.alpha;}
            static uint8_t toAlpha(double opacity) {return opacity<=0 ? 0 : opacity>=1.0 ? 255 : (uint8_t)(opacity*255+0.5);}
            std::string str() const;
            //@}
        };

        /**
         * @brief A rectangular RGBA pixel array.
         * @see cPixmapImage
         * @ingroup Canvas
         */
        class SIM_API Pixmap {
            private:
                int width = 0, height = 0; // zero is allowed
                RGBA *data = nullptr;
            private:
                void allocate(int width, int height);
            public:
                /** @name Methods. */
                //@{
                Pixmap() {}
                Pixmap(int width, int height, const RGBA& fill=RGBA(BLACK,0));
                Pixmap(int width, int height, const Color& color, double opacity=1) : Pixmap(width, height, RGBA(color, opacity)) {}
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
                void setPixel(int x, int y, const Color& color, double opacity=1.0) {RGBA& p = pixel(x,y); p.set(color.red, color.green, color.blue, RGBA::toAlpha(opacity));}
                const Color getColor(int x, int y) const {return (Color)pixel(x,y);}
                void setColor(int x, int y, const Color& color) {RGBA& p = pixel(x,y); p.red = color.red; p.green = color.green; p.blue = color.blue;}
                double getOpacity(int x, int y) const {return pixel(x,y).alpha / 255.0;}
                void setOpacity(int x, int y, double opacity) {pixel(x,y).alpha = RGBA::toAlpha(opacity);}
                const uint8_t *buffer() const {return (uint8_t*)data;} // direct access for low-level manipulation
                std::string str() const;
                //@}
        };

        // internal:
        enum {
            CHANGE_STRUCTURAL = 1, // child added, removed, or reordered, shown/hidden
            CHANGE_TRANSFORM = 2, // transform change
            CHANGE_GEOMETRY = 4, // geometry information (bounds, position, start/end angle, etc)
            CHANGE_VISUAL = 8,  // styling
            CHANGE_INPUTDATA = 16,  // text, image name or pixmap data, value to be displayed, etc
            CHANGE_TAGS = 32,  // figure tags
            CHANGE_ZINDEX = 64,  // zIndex
            CHANGE_OTHER = 128  // tooltip, associated object
        };

    private:
        static int lastId;
        int id;
        bool visible = true; // treated as structural change, for simpler handling
        bool hasTooltip = false;
        double zIndex = 0;
        opp_pooledstring tooltip;
        cObject *associatedObject = nullptr;
        Transform transform;
        std::vector<cFigure*> children;
        opp_pooledstring tags;
        uint64_t tagBits = 0;  // bit-to-tagname mapping is stored in cCanvas. Note: change to std::bitset if 64 tags are not enough
        uint8_t localChanges = 0;
        uint8_t subtreeChanges = 0;
        mutable bool cachedHashValid = false; // separate flag needed because zero hash is also a valid value
        mutable uint32_t cachedHash = 0;

    protected:
        // internal:
        virtual void validatePropertyKeys(cProperty *property) const; // relies on isAllowedPropertyKey()
        virtual bool isAllowedPropertyKey(const char *key) const; // allows "x-*" keys, plus the ones returned by getAllowedPropertyKeys()
        virtual cFigure *getRootFigure() const;
        void fireStructuralChange() {fire(CHANGE_STRUCTURAL);}
        void fireTransformChange() {fire(CHANGE_TRANSFORM);}
        void fireGeometryChange() {fire(CHANGE_GEOMETRY);}
        void fireVisualChange() {fire(CHANGE_VISUAL);}
        void fireInputDataChange() {fire(CHANGE_INPUTDATA);}
        virtual void fire(uint8_t flags);
        virtual void hashTo(cHasher& hasher) const;

    protected:
        // helpers for parse(cProperty*)
        static Point parsePoint(cProperty *property, const char *key, int index);
        static std::vector<Point> parsePoints(cProperty *property, const char *key);
        static Rectangle parseBounds(cProperty *property, const Rectangle& defaults);
        static Transform parseTransform(cProperty *property, const char *key);
        static Font parseFont(cProperty *property, const char *key);
        static Rectangle computeBoundingBox(const Point& position, const Point& size, double ascent, Anchor anchor);
        static void concatArrays(const char **dest, const char **first, const char **second); // concatenates nullptr-terminated arrays

    public:
        // helpers for class descriptors and parse(cProperty*)
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
        static Alignment parseAlignment(const char *s);

    public:
        // internal, mostly used by runtime GUIs:
        virtual void updateParentTransform(Transform& transform) {transform.rightMultiply(getTransform());}
        virtual void callRefreshDisplay(); // call refreshDisplay(), and recurse to its children
        uint8_t getLocalChangeFlags() const {return localChanges;}
        uint8_t getSubtreeChangeFlags() const {return subtreeChanges;}
        void clearChangeFlags();
        void refreshTagBitsRec(cCanvas *ownerCanvas);
        int64_t getTagBits() const {return tagBits;}
        void setTagBits(uint64_t tagBits) {this->tagBits = tagBits;}
        uint32_t getHash() const;
        void clearCachedHash(); // for debugging

    private:
        void copy(const cFigure& other);

    public:
        /** @name Constructors, destructor, assignment */
        //@{
        /**
         * Constructor.
         */
        explicit cFigure(const char *name=nullptr);

        /**
         * Copy constructor. Child figures and the figure ID are not copied.
         */
        cFigure(const cFigure& other) : cOwnedObject(other) {copy(other);}

        /**
         * Destructor
         */
        virtual ~cFigure();

        /**
         * Assignment operator. It copies all data members except the object
         * name, figure ID and child figures. (See cNamedObject::operator=()
         * for details.)
         */
        cFigure& operator=(const cFigure& other);
        //@}

        /** @name Redefined cObject member functions. */
        //@{
        /**
         * Clones the figure, ignoring its child figures. (The copy will have
         * no children.) To clone a figure together with its figure subtree,
         * use dupTree().
         */
        virtual cFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}

        /**
         * Calls v->visit(this) on each child figure.
         * See cObject for more details.
         */
        virtual void forEachChild(cVisitor *v) override;

        /**
         * Returns a one-line string with the most characteristic data of the figure.
         */
        virtual std::string str() const override;
        //@}

        /** @name Common figure attributes. */
        //@{
        /**
         * Returns the unique ID of this figure object. This ID is mostly
         * intended for internal use.
         */
        int getId() const {return id;}

        /**
         * Returns the visibility flag of the figure.
         */
        virtual bool isVisible() const {return visible;}

        /**
         * Sets the visibility flag of the figure. Setting the visibility flag
         * to false will cause the whole figure subtree to be omitted from
         * rendering.
         */
        virtual void setVisible(bool visible);

        /**
         * Returns the transform associated with this figure.
         */
        virtual const Transform& getTransform() const {return transform;}

        /**
         * Sets the transform associated with this figure. The transform will
         * affect the rendering of this figure and its whole figure subtree.
         * The transform may also be modified with the various
         * translate(), scale(), rotate(), skewx()/skewy() methods.
         */
        virtual void setTransform(const Transform& transform);

        /**
         * Convenience method: sets the transformation to identity
         * transform.
         */
        virtual void resetTransform() {setTransform(Transform());}

        /**
         * Returns the Z-index of the figure.
         *
         * @see getEffectiveZIndex()
         */
        virtual double getZIndex() const {return zIndex;}

        /**
         * Sets the Z-index of the figure. The Z-index affects the stacking
         * order of figures via the effective Z-index. The effective Z-index
         * is computed as the sum of the Z-indices of this figure and its
         * ancestors'. A figure with a greater effective Z-index is always in
         * front of an element with a lower effective Z-index. For figures
         * with the same effective Z-index, their relative order in the
         * preorder traversal of the figure hierarchy defines the stacking
         * order. (A later figure is in front of the earlier ones.)
         */
        virtual void setZIndex(double zIndex);

        /**
         * Returns the effective Z-index of the figure. The effective Z-index
         * is computed as the sum of the Z-indices of this figure and its
         * ancestors'. A figure with a greater effective Z-index is always in
         * front of an element with a lower effective Z-index.
         */
        virtual double getEffectiveZIndex() const;

        /**
         * Returns the tooltip of the figure, or nullptr if it does not have one.
         */
        virtual const char *getTooltip() const {return hasTooltip ? tooltip.c_str() : nullptr;}

        /**
         * Sets the tooltip of the figure. Pass nullptr to clear the tooltip.
         * If nullptr is set, the effective tooltip will be inherited
         * from the associated object if there is one (see getAssociatedObject()),
         * or from the parent figure. Empty tooltips will not be shown.
         */
        virtual void setTooltip(const char *tooltip);

        /**
         * Returns the simulation object associated with this figure, or nullptr
         * if it does not have one. The GUI may use this function to provide a
         * shortcut access to the specified object, e.g. select the object in an
         * inspector when the user clicks the figure.
         */
        virtual cObject *getAssociatedObject() const {return associatedObject;}

        /**
         * Sets the simulation object associated with this figure. The user is
         * responsible for ensuring that the pointer is valid (the corresponding
         * object exists), and remains valid while it is associated with the figure.
         */
        virtual void setAssociatedObject(cObject *obj);

        /**
         * Returns the space-separated list of the tags associated with the figure.
         * Tags may be used in the GUI displaying a canvas for implementing
         * layers or other kind of filtering.
         */
        virtual const char *getTags() const {return tags.c_str();}

        /**
         * Sets the list of tags associated with the figure. This method accepts
         * a space-separated list of tags. (Tags themselves may not contain spaces;
         * underscores are recommended instead where needed.)
         */
        virtual void setTags(const char *tags);
        //@}

        /** @name Accessing the parent figure and child figures. */
        //@{
        /**
         * Returns the parent figure of this figure, or nullptr if it has none.
         */
        virtual cFigure *getParentFigure() const {return dynamic_cast<cFigure*>(getOwner());}

        /**
         * Returns the canvas object to which this figure was added, or nullptr
         * if there is no such canvas. Note that the runtime cost of this method
         * is proportional to the depth of this figure in the figure tree.
         */
        virtual cCanvas *getCanvas() const;

        /**
         * Returns the number of child figures.
         */
        virtual int getNumFigures() const {return children.size();}

        /**
         * Returns the kth figure in the child list. The index must be in the
         * range 0..getNumFigures()-1. An out-of-bounds index will cause a runtime
         * error.
         */
        virtual cFigure *getFigure(int pos) const;

        /**
         * Returns the first child figure with the given name, or nullptr if
         * there is no such figure.
         */
        virtual cFigure *getFigure(const char *name) const;

        /**
         * Finds the first figure with the given name among the children of this
         * figure, and returns its index. If there is no such figure, -1 is returned.
         */
        virtual int findFigure(const char *name) const;

        /**
         * Finds the given figure among the children of this figure, and returns
         * its index. If it is not found, -1 is returned.
         */
        virtual int findFigure(const cFigure *figure) const;

        /**
         * Returns true if this figure has child figures, and false otherwise.
         */
        virtual bool containsFigures() const {return !children.empty();}

        /**
         * Find the first figure with the given name in this figure's subtree,
         * including the figure itself. Returns nullptr if no such figure is found.
         */
        virtual cFigure *findFigureRecursively(const char *name) const;

        /**
         * Finds a figure in the subtree, given by its absolute or relative path.
         * The path is a string of figure names separated by dots; the special
         * module name ^ (caret) stands for the parent figure. If the path starts
         * with a dot or caret, it is understood as relative to this figure,
         * otherwise it is taken to mean an absolute path (i.e. relative to the
         * root figure, see getRootFigure()). Returns nullptr if the figure
         * was not found.
         *
         * Examples:
         *   - "." means this figure;
         *   - ".icon" means the child figure name "icon";
         *   - ".group.label" means the "label" child of the "group" child figure;
         *   - "^.icon" or ".^.icon" means the "icon" sibling of this figure;
         *   - "icon" means the "icon" child of the root figure
         *
         * The above syntax is similar to the one used by cModule::getModuleByPath()
         */
        virtual cFigure *getFigureByPath(const char *path) const;
        //@}

        /** @name Managing child figures. */
        //@{
        /**
         * Appends the given figure to the child list of this figure.
         */
        virtual void addFigure(cFigure *figure);

        /**
         * Inserts the given figure into the child list of this figure at
         * the given position. Note that relative order in the child list
         * only affects stacking order if the respective figures have the
         * same Z-index.
         */
        virtual void addFigure(cFigure *figure, int pos);

        /**
         * Removes the given figure from the child list of this figure.
         * An error is raised if the figure is not a child of this figure.
         *
         * Prefer removeFromParent() to this method.
         */
        virtual cFigure *removeFigure(cFigure *figure);

        /**
         * Removes the kth figure from the child list of this figure. An error
         * is raised if pos is not in the range 0..getNumFigures()-1.
         *
         * Prefer removeFromParent() to this method.
         */
        virtual cFigure *removeFigure(int pos);

        /**
         * Removes this figure from the child list of its parent figure.
         * It has no effect if the figure has no parent figure.
         */
        virtual cFigure *removeFromParent();
        //@}

        /** @name Managing stacking order */
        //@{
        /**
         * Returns true if this figure is above the given reference figure
         * in stacking order, based on their Z-index values and their relative
         * order in the parent's child list, and false if not. The method
         * throws an error if the two figures are not under the same parent.
         */
        virtual bool isAbove(const cFigure *figure) const;

        /**
         * Returns true if this figure is below the given reference figure
         * in stacking order, based on their Z-index values and their relative
         * order in the parent's child list, and false if not. The method
         * throws an error if the two figures are not under the same parent.
         */
        virtual bool isBelow(const cFigure *figure) const;

        /**
         * Inserts this figure under the same parent as the reference figure,
         * guaranteeing that it will appear above the reference figure
         * in stacking order. In order to achieve the latter, the Z-index
         * of the figure is set to the Z-index of the reference figure if
         * it was smaller. In case of equal Z-indices, the figure is inserted
         * right after the reference figure. If the Z-index of the figure is
         * already greater than that of the reference figure, nothing should be
         * assumed about the insertion position.
         *
         * @see insertAfter()
         */
        virtual void insertAbove(cFigure *referenceFigure);

        /**
         * Inserts this figure under the same parent as the reference figure,
         * guaranteeing that it will appear below the reference figure
         * in stacking order. In order to achieve the latter, the Z-index
         * of the figure is set to the Z-index of the reference figure if
         * it was greater. In case of equal Z-indices, the figure is inserted
         * right before the reference figure. If the Z-index of the figure is
         * already smaller than that of the reference figure, nothing should be
         * assumed about the insertion position.
         *
         * @see insertBefore()
         */
        virtual void insertBelow(cFigure *referenceFigure);

        /**
         * Inserts this figure after the given reference figure in its parent's
         * child list. BEWARE: Relative order in the child list only affects
         * stacking order if the respective figures have the same Z-index.
         */
        virtual void insertAfter(const cFigure *referenceFigure);

        /**
         * Inserts this figure in front of the given reference figure in its
         * parent's child list. BEWARE: Relative order in the child list only
         * affects stacking order if the respective figures have the same Z-index.
         */
        virtual void insertBefore(const cFigure *referenceFigure);

        /**
         * If this figure is below the given reference figure in stacking order,
         * move it directly above. This is achieved by updating its Z-index
         * if needed, and moving it in the parent's child list. An error is
         * raised if the two figures do not share the same parent.
         */
        virtual void raiseAbove(cFigure *figure);

        /**
         * If this figure is above the given reference figure in stacking order,
         * move it directly below. This is achieved by updating its Z-index
         * if needed, and moving it in the parent's child list. An error is
         * raised if the two figures do not share the same parent.
         */
        virtual void lowerBelow(cFigure *figure);

        /**
         * Raise this figure above all other figures under the same parent. This is
         * achieved by updating its Z-index if needed (setting it to be at least
         * the maximum of the Z-indices of the other figures), and moving it in the
         * parent's child list. It is an error if this figure has no parent.
         */
        virtual void raiseToTop();

        /**
         * Lower this figure below all other figures under the same parent. This is
         * achieved by updating its Z-index if needed (setting it to be at most
         * the minimum of the Z-indices of the other figures), and moving it in the
         * parent's child list. It is an error if this figure has no parent.
         */
        virtual void lowerToBottom();

        /**
         * Duplicate the figure subtree.
         */
        virtual cFigure *dupTree() const;
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
        virtual void skewx(double coeff) {transform.skewx(coeff); fireTransformChange();}
        virtual void skewy(double coeff) {transform.skewy(coeff); fireTransformChange();}
        virtual void skewx(double coeff, double cy) {transform.skewx(coeff,cy); fireTransformChange();}
        virtual void skewy(double coeff, double cx) {transform.skewy(coeff,cx); fireTransformChange();}
        //@}

        /** @name Miscellaneous. */
        //@{
        /**
         * This method is invoked by the simulation library to initialize
         * the figure from a \@figure NED property. Custom figure classes
         * usually need to override this method (and call the super class'
         * similar method in it).
         *
         * Note: When overriding parse(), it is usually also necessary to override
         * getAllowedPropertyKeys(), in order to allow new keys in the \@figure
         * property.
         */
        virtual void parse(cProperty *property);

        /**
         * Returns the list of allowed keys in \@figure properties with this
         * figure type, in a nullptr-terminated array of const char* elements.
         * This method is invoked e.g. from parse(), in order to be able to
         * issue error messages for invalid keys in the property. Note that
         * property keys starting with "x-" are already allowed by default.
         * Custom figure classes usually need to override this method together
         * with parse(); see the source code of the library classes for example
         * implementations.
         */
        virtual const char **getAllowedPropertyKeys() const;

        /**
         * Change the figure's position by the given x and y deltas.
         * Child figures will not be affected.
         */
        virtual void moveLocal(double dx, double dy) = 0;

        /**
         * Change the position of this figure and the figures in its subtree
         * by the given x and y deltas. The implementation of this method
         * calls moveLocal() on this figure, then move() recursively on child
         * figures.
         */
        virtual void move(double dx, double dy);

        /**
         * This method is invoked by runtime GUIs on every display refresh,
         * and can be overridden if the figure needs to be able to update
         * its contents dynamically (self-refreshing figures.) The changes
         * done inside this method should be restricted to this figure
         * and its subfigure tree.
         */
        virtual void refreshDisplay() {}

        /**
         * Returns the name of the class responsible for rendering this figure.
         * Renderer classes are specific to, and are usually implemented as
         * part of, the graphical user interface library e.g. Qtenv.
         */
        virtual const char *getRendererClassName() const = 0;
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
 * @brief A figure with the sole purpose of grouping its children, and no visual
 * representation.
 *
 * The usefulness of a group figure comes from the fact that transformations
 * are inherited from parent to child, thus, children of a group can be moved,
 * scaled, rotated, etc. together by updating the group's transformation matrix.
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
        virtual std::string str() const override;
        virtual const char *getRendererClassName() const override {return "GroupFigureRenderer";}
        virtual void moveLocal(double dx, double dy) override {}
        //@}
};

/**
 * @brief Sets up an axis-aligned, unscaled coordinate system for children, canceling the
 * effect of any transformation (scaling, rotation, etc.) inherited from ancestor figures.
 *
 * This allows pixel-based positioning of children, and makes them immune to zooming.
 * The anchorPoint in the coordinate system is mapped to the position of the panel figure.
 * Setting a transformation on the panel figure itself allows rotation, scaling,
 * and skewing of the coordinate system. The anchorPoint is affected by this transformation.
 *
 * The panel figure itself has no visual appearance.
 *
 * @ingroup Canvas
 */
class SIM_API cPanelFigure : public cFigure
{
    private:
        Point position;
        Point anchorPoint;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void parse(cProperty *property) override;
        virtual void hashTo(cHasher& hasher) const override;

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
        virtual std::string str() const override;
        virtual const char *getRendererClassName() const override {return "";} // non-visual figure
        virtual void updateParentTransform(Transform& transform) override;
        virtual void move(double dx, double dy) override { moveLocal(dx, dy); }
        virtual void moveLocal(double dx, double dy) override {position.x += dx; position.y += dy; fireTransformChange();}
        //@}

        /** @name Figure attributes */
        //@{
        virtual const Point& getPosition() const  {return position;}
        virtual void setPosition(const Point& position)  {this->position = position; fireTransformChange();}

        /**
         * By default, the (0,0) point in cPanelFigure's coordinate system will be mapped
         * to the given position (i.e. getPosition()) in the parent figure's coordinate system.
         * By setting an anchorPoint, one can change (0,0) to an arbitrary point. E.g. by setting
         * anchorPoint=(100,0), the (100,0) point will be mapped to the given position, i.e.
         * panel contents will appear 100 pixels to the left (given there are no transforms set).
         * The translation part of the local transform is cancelled out because the anchorPoint
         * is subject to the transformation of the panel figure the same way as the child figures.
         */
        virtual const Point& getAnchorPoint() const  {return anchorPoint;}
        virtual void setAnchorPoint(const Point& anchorPoint)  {this->anchorPoint = anchorPoint; fireTransformChange();}

        //@}
};

/**
 * @brief Common base class for line figures.
 *
 * This class provides line color, style, width, opacity, and other properties.
 * Lines may also be augmented with arrowheads at either or both ends.
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
        LineStyle lineStyle = LINE_SOLID;
        double lineWidth = 1;
        double lineOpacity = 1;
        CapStyle capStyle = CAP_BUTT;
        Arrowhead startArrowhead = ARROW_NONE, endArrowhead = ARROW_NONE;
        bool zoomLineWidth = false;
    private:
        void copy(const cAbstractLineFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractLineFigure(const char *name=nullptr) : cFigure(name), lineColor(BLACK) {}
        cAbstractLineFigure(const cAbstractLineFigure& other) : cFigure(other) {copy(other);}
        cAbstractLineFigure& operator=(const cAbstractLineFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractLineFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        //@}

        /** @name Figure attributes */
        //@{
        /**
         * Returns the line color of the figure.
         */
        virtual const Color& getLineColor() const  {return lineColor;}

        /**
         * Sets the line color of the figure. The default color is black.
         */
        virtual void setLineColor(const Color& lineColor);

        /**
         * Returns the line width of the figure. Note that the actual line
         * width will also be affected by the figure transformations and,
         * if zoomLineWidth is set, by the zoom level as well.
         */
        virtual double getLineWidth() const  {return lineWidth;}

        /**
         * Sets the line width of the figure. The number must be
         * positive (zero is not allowed). The default line width is 1.
         * See also setZoomLineWidth().
         */
        virtual void setLineWidth(double lineWidth);

        /**
         * Returns the line opacity of the figure, a number in the [0,1] interval.
         */
        virtual double getLineOpacity() const  {return lineOpacity;}

        /**
         * Sets the line opacity of the figure. The argument must be a number
         * in the [0,1] interval. The default opacity is 1.0.
         */
        virtual void setLineOpacity(double lineOpacity);

        /**
         * Returns the line style of the figure.
         */
        virtual LineStyle getLineStyle() const  {return lineStyle;}

        /**
         * Sets the line style of the figure (dotted, dashed, etc.)
         * The default line style is solid.
         */
        virtual void setLineStyle(LineStyle lineStyle);

        /**
         * Returns the cap style for the figure's line.
         */
        virtual CapStyle getCapStyle() const {return capStyle;}

        /**
         * Sets the cap style for the figure's line. The default cap style
         * is butt.
         */
        virtual void setCapStyle(CapStyle capStyle);

        /**
         * Returns the type of the arrowhead at the start of the line.
         */
        virtual Arrowhead getStartArrowhead() const  {return startArrowhead;}

        /**
         * Sets the type of the arrowhead at the start of the line.
         * The default is no arrowhead.
         */
        virtual void setStartArrowhead(Arrowhead startArrowhead);

        /**
         * Returns the type of the arrowhead at the end of the line.
         */
        virtual Arrowhead getEndArrowhead() const  {return endArrowhead;}

        /**
         * Sets the type of the arrowhead at the end of the line.
         * The default is no arrowhead.
         */
        virtual void setEndArrowhead(Arrowhead endArrowhead);

        /**
         * Returns true if zoom level affects the line width the figure is
         * drawn with, and false if it does not.
         */
        virtual bool getZoomLineWidth() const {return zoomLineWidth;}

        /**
         * Sets the flag that controls whether zoom level should affect
         * the line width the figure is drawn with. The default setting
         * is false, i.e. zoom does not affect line width.
         */
        virtual void setZoomLineWidth(bool zoomLineWidth);
        //@}
};

/**
 * @brief A figure that displays a straight line segment.
 *
 * This class provides the coordinates of the end points. Other properties
 * such as color and line style are inherited from cAbstractLineFigure.
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
        virtual void hashTo(cHasher& hasher) const override;
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
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "LineFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the start point of the line segment.
         */
        virtual const Point& getStart() const  {return start;}

        /**
         * Sets the start point of the line segment.
         */
        virtual void setStart(const Point& start);

        /**
         * Returns the end point of the line segment.
         */
        virtual const Point& getEnd() const  {return end;}

        /**
         * Sets the end point of the line segment.
         */
        virtual void setEnd(const Point& end);
        //@}
};

/**
 * @brief A figure that displays an arc.
 *
 * An arc's geometry is determined by the bounding box of the circle or ellipse
 * the arc is part of, and the start and end angles. Other properties such as
 * color and line style are inherited from cAbstractLineFigure.
 *
 * For the start and end angles, zero points east, and angles are measured in
 * the counter-clockwise direction. Values outside the (0,2*PI) interval are
 * accepted for both angles.
 *
 * The arc is drawn from the start angle to the end angle in *counter-clockwise*
 * direction. In non-edge cases, the end angle should be in the (startAngle,
 * startAngle + 2*PI) interval. If the end angle equals or is less than the
 * start angle, nothing is drawn. If the end angle equals startAngle+2*PI
 * or is greater than that, a full circle is drawn.
 *
 * @ingroup Canvas
 */
class SIM_API cArcFigure : public cAbstractLineFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that arc is part of
        double startAngle = 0, endAngle = 0; // in radians, CCW, 0=east
    private:
        void copy(const cArcFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cArcFigure(const char *name=nullptr) : cAbstractLineFigure(name) {}
        cArcFigure(const cArcFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cArcFigure& operator=(const cArcFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cArcFigure *dup() const override  {return new cArcFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "ArcFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the bounding box of the axis-aligned circle or ellipse
         * the arc is part of.
         */
        virtual const Rectangle& getBounds() const  {return bounds;}

        /**
         * Sets the bounding box of the axis-aligned circle or ellipse
         * the arc is part of.
         */
        virtual void setBounds(const Rectangle& bounds);

        /**
         * Utility function. Moves the bounding box so that its point
         * denoted by anchor will be at the given position.
         */
        virtual void setPosition(const Point& position, Anchor anchor);

        /**
         * Returns the start angle of the arc in radians. See the class
         * documentation for details on how the arc is drawn.
         */
        virtual double getStartAngle() const {return startAngle;}

        /**
         * Sets the start angle of the arc in radians. See the class
         * documentation for details on how the arc is drawn.
         */
        virtual void setStartAngle(double startAngle);

        /**
         * Returns the end angle of the arc in radians. See the class
         * documentation for details on how the arc is drawn.
         */
        virtual double getEndAngle() const {return endAngle;}

        /**
         * Sets the end angle of the arc in radians. See the class
         * documentation for details on how the arc is drawn.
         */
        virtual void setEndAngle(double endAngle);
        //@}
};

/**
 * @brief A figure that displays a line that consists of multiple connecting
 * straight line segments or of a single smooth curve.
 *
 * The class stores geometry information as a sequence of points. The line
 * may be *smoothed*. A smoothed line is drawn as a series of Bezier
 * curves, which touch the start point of the first line segment, the
 * end point of the last line segment, and the midpoints of intermediate
 * line segments, while intermediate points serve as control points.
 * To add corners to a smoothed curve, duplicate points.
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
        bool smooth = false;
        JoinStyle joinStyle = JOIN_MITER;
    private:
        void copy(const cPolylineFigure& other);
        void checkIndex(int i) const;
        void checkInsIndex(int i) const;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPolylineFigure(const char *name=nullptr) : cAbstractLineFigure(name) {}
        cPolylineFigure(const cPolylineFigure& other) : cAbstractLineFigure(other) {copy(other);}
        cPolylineFigure& operator=(const cPolylineFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPolylineFigure *dup() const override  {return new cPolylineFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "PolylineFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the points that define the polyline.
         */
        virtual const std::vector<Point>& getPoints() const  {return points;}

        /**
         * Sets the points that define the polyline.
         */
        virtual void setPoints(const std::vector<Point>& points);

        /**
         * Returns the number of points. Equivalent to getPoints().size().
         */
        virtual int getNumPoints() const {return points.size();}

        /**
         * Returns the ith point. The index must be in the 0..getNumPoints()-1
         * interval.
         */
        virtual const Point& getPoint(int i) const {checkIndex(i); return points[i];}

        /**
         * Sets the number of points that define the polyline. (Appends new
         * points or drops trailing points as needed.)
         */
        virtual void setNumPoints(int size); // primarily for sim_std.msg

        /**
         * Sets the ith point. The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void setPoint(int i, const Point& point);

        /**
         * Appends a point.
         */
        virtual void addPoint(const Point& point);

        /**
         * Removes the ith point. The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void removePoint(int i);

        /**
         * Inserts a point at position i, shifting up the point at and above index i.
         * The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void insertPoint(int i, const Point& point);

        /**
         * Returns true if the figure is rendered as a smoothed curve, and
         * false if as a sequence of connecting of straight line segments.
         * See the class description for details on how the points define
         * the curve in the smoothed case.
         */
        virtual bool getSmooth() const {return smooth;}

        /**
         * Sets the flag that determines whether connecting straight line segments
         * or a single smooth curve should be drawn. See the class description for
         * details on how the points define the curve in the smoothed case.
         * The default value is false (no smoothing).
         */
        virtual void setSmooth(bool smooth);
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns the join style line segments will be connected with.
         */
        virtual JoinStyle getJoinStyle() const {return joinStyle;}

        /**
         * Sets the join style line segments will be connected with. The
         * default join style is miter.
         */
        virtual void setJoinStyle(JoinStyle joinStyle);
        //@}
};

/**
 * @brief Abstract base class for various shapes.
 *
 * This class provides line and fill color, line and fill opacity, line style,
 * line width, and other properties for them. Both outline and fill are optional.
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
        bool outlined = true;
        bool filled = false;
        Color lineColor;
        Color fillColor;
        LineStyle lineStyle = LINE_SOLID;
        double lineWidth = 1;
        double lineOpacity = 1;
        double fillOpacity = 1;
        bool zoomLineWidth = false;
    private:
        void copy(const cAbstractShapeFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractShapeFigure(const char *name=nullptr) : cFigure(name),  lineColor(BLACK), fillColor(BLUE) {}
        cAbstractShapeFigure(const cAbstractShapeFigure& other) : cFigure(other) {copy(other);}
        cAbstractShapeFigure& operator=(const cAbstractShapeFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractShapeFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns true if the figure rendered as filled, and false if not.
         */
        virtual bool isFilled() const  {return filled;}

        /**
         * Sets the flag that controls whether the figure should be filled.
         * The default setting is false (unfilled).
         */
        virtual void setFilled(bool filled);

        /**
         * Returns true if the figure's outline is to be drawn, and false if not.
         */
        virtual bool isOutlined() const  {return outlined;}

        /**
         * Sets the flag that controls whether the figure's outline should be
         * drawn. The default setting is true.
         */
        virtual void setOutlined(bool outlined);

        /**
         * Returns the color of the figure's outline.
         */
        virtual const Color& getLineColor() const  {return lineColor;}

        /**
         * Sets the color of the figure's outline. The default outline color
         * is black.
         */
        virtual void setLineColor(const Color& lineColor);

        /**
         * Returns the fill color of the figure.
         */
        virtual const Color& getFillColor() const  {return fillColor;}

        /**
         * Sets the fill color of the figure. Note that setting the fill color
         * in itself does not make the figure filled, calling setFilled(true)
         * is also needed. The default fill color is blue.
         */
        virtual void setFillColor(const Color& fillColor);

        /**
         * Returns the line style of the figure's outline.
         */
        virtual LineStyle getLineStyle() const  {return lineStyle;}

        /**
         * Sets the line style of the figure's outline (dotted, dashed, etc.)
         * The default line style is solid.
         */
        virtual void setLineStyle(LineStyle lineStyle);

        /**
         * Returns the line width of the figure's outline. Note that the actual line
         * width will also be affected by the figure transformations and,
         * if zoomLineWidth is set, by the zoom level as well.
         */
        virtual double getLineWidth() const  {return lineWidth;}

        /**
         * Sets the line width of the figure's outline. The number must be
         * positive (zero is not allowed). The default line width is 1.
         * See also setZoomLineWidth().
         */
        virtual void setLineWidth(double lineWidth);

        /**
         * Returns the opacity of the figure's outline, a number in the
         * [0,1] interval.
         */
        virtual double getLineOpacity() const  {return lineOpacity;}

        /**
         * Sets the opacity of the figure's outline. The argument must be a
         * number in the [0,1] interval. The default opacity is 1.0.
         */
        virtual void setLineOpacity(double lineOpacity);

        /**
         * Returns the opacity of the figure's fill, a number in the
         * [0,1] interval.
         */
        virtual double getFillOpacity() const  {return fillOpacity;}

        /**
         * Sets the opacity of the figure's fill. The argument must be a
         * number in the [0,1] interval. The default opacity is 1.0.
         */
        virtual void setFillOpacity(double fillOpacity);

        /**
         * Returns true if zoom level affects the line width the figure's
         * outline is drawn with, and false if it does not.
         */
        virtual bool getZoomLineWidth() const {return zoomLineWidth;}

        /**
         * Sets the flag that controls whether zoom level should affect
         * the line width the figure's outline is drawn with. The default
         * setting is false, i.e. zoom does not affect line width.
         */
        virtual void setZoomLineWidth(bool zoomLineWidth);
        //@}
};

/**
 * @brief A figure that displays a rectangle, with optionally rounded corners.
 *
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
        double cornerRx = 0, cornerRy = 0;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    private:
        void copy(const cRectangleFigure& other);
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cRectangleFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cRectangleFigure(const cRectangleFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cRectangleFigure& operator=(const cRectangleFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cRectangleFigure *dup() const override  {return new cRectangleFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "RectangleFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the rectangle that defines the figure's geometry.
         */
        virtual const Rectangle& getBounds() const  {return bounds;}

        /**
         * Sets the rectangle that defines the figure's geometry.
         */
        virtual void setBounds(const Rectangle& bounds);

        /**
         * Utility function. Moves the bounding box so that its point
         * denoted by anchor will be at the given position.
         */
        virtual void setPosition(const Point& position, Anchor anchor);

        /**
         * Sets both the horizontal and vertical (x and y) corner radii to
         * the same value.
         */
        virtual void setCornerRadius(double r)  {setCornerRx(r);setCornerRy(r);}

        /**
         * Returns the horizontal radius of the rectangle's rounded corners.
         */
        virtual double getCornerRx() const  {return cornerRx;}

        /**
         * Sets the horizontal radius of the rectangle's rounded corners.
         * Specify 0 to turn off rounded corners. The default is 0.
         */
        virtual void setCornerRx(double rx);

        /**
         * Returns the vertical radius of the rectangle's rounded corners.
         */
        virtual double getCornerRy() const  {return cornerRy;}

        /**
         * Sets the vertical radius of the rectangle's rounded corners.
         * Specify 0 to turn off rounded corners. The default is 0.
         */
        virtual void setCornerRy(double ry);
        //@}
};

/**
 * @brief A figure that draws a circle or ellipse.
 *
 * As with all shape figures, drawing of both the outline and the fill are
 * optional. Line and fill color, and several other properties are inherited
 * from cAbstractShapeFigure.
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
        virtual void hashTo(cHasher& hasher) const override;
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
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "OvalFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the rectangle that defines the figure's geometry.
         */
        virtual const Rectangle& getBounds() const  {return bounds;}

        /**
         * Sets the rectangle that defines the figure's geometry.
         */
        virtual void setBounds(const Rectangle& bounds);

        /**
         * Utility function. Moves the bounding box so that its point
         * denoted by anchor will be at the given position.
         */
        virtual void setPosition(const Point& position, Anchor anchor);
        //@}
};

/**
 * @brief A figure that displays a ring, with explicitly controllable inner/outer radii.
 *
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
        double innerRx = 0, innerRy = 0;
    private:
        void copy(const cRingFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cRingFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cRingFigure(const cRingFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cRingFigure& operator=(const cRingFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cRingFigure *dup() const override  {return new cRingFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "RingFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the rectangle that defines the ring's outer circle or ellipse.
         */
        virtual const Rectangle& getBounds() const  {return bounds;}

        /**
         * Sets the rectangle that defines the ring's outer circle or ellipse.
         */
        virtual void setBounds(const Rectangle& bounds);

        /**
         * Utility function. Moves the bounding box so that its point
         * denoted by anchor will be at the given position.
         */
        virtual void setPosition(const Point& position, Anchor anchor);

        /**
         * Sets the ring's inner curve to a circle with the given radius.
         * This method sets the horizontal and vertical inner radii to the
         * same value.
         */
        virtual void setInnerRadius(double r)   {setInnerRx(r);setInnerRy(r);}

        /**
         * Returns the horizontal radius of the ring's inner circle or ellipse.
         */
        virtual double getInnerRx() const  {return innerRx;}

        /**
         * Sets the horizontal radius of the ring's inner circle or ellipse.
         */
        virtual void setInnerRx(double rx);

        /**
         * Returns the vertical radius of the ring's inner circle or ellipse.
         */
        virtual double getInnerRy() const  {return innerRy;}

        /**
         * Sets the vertical radius of the ring's inner circle or ellipse.
         */
        virtual void setInnerRy(double ry);
        //@}
};

/**
 * @brief A figure that displays a pie slice, that is, a section of an
 * axis-aligned disc or filled ellipse.
 *
 * A pie slice is determined by the bounding box of the full disc or ellipse,
 * and a start and an end angle. The outline of the pie slice in non-degenerate
 * cases consists of an arc, and two straight lines from the center of the pie
 * to the end points of the arc.
 *
 * For the start and end angles, zero points east, and angles are measured in
 * the counter-clockwise direction. Values outside the (0,2*PI) interval are
 * accepted for both angles.
 *
 * As with all shape figures, drawing of both the outline and the fill are
 * optional. Line and fill color, and several other properties are inherited
 * from cAbstractShapeFigure.
 *
 * The pie slice is drawn from the start angle to the end angle in *counter-clockwise*
 * direction. In non-degenerate cases, the end angle should be
 * in the (startAngle, startAngle + 2*PI) interval. If the end angle equals to
 * or is less than the start angle, only a straight line is drawn from the
 * center to the circumference. If the end angle equals startAngle+2*PI
 * or is greater than that, a full circle or ellipse, and a straight line from
 * the center to the circumference at angle endAngle is drawn.
 *
 * @ingroup Canvas
 */
class SIM_API cPieSliceFigure : public cAbstractShapeFigure
{
    private:
        Rectangle bounds; // bounding box of the oval that the pie slice is part of
        double startAngle = 0, endAngle = 0; // in radians, CCW, 0=east
    private:
        void copy(const cPieSliceFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPieSliceFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cPieSliceFigure(const cPieSliceFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPieSliceFigure& operator=(const cPieSliceFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPieSliceFigure *dup() const override  {return new cPieSliceFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "PieSliceFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the bounding box of the axis-aligned circle or ellipse
         * the pie slice is part of.
         */
        virtual const Rectangle& getBounds() const  {return bounds;}

        /**
         * Sets the bounding box of the axis-aligned circle or ellipse
         * the pie slice is part of.
         */
        virtual void setBounds(const Rectangle& bounds);

        /**
         * Utility function. Moves the bounding box so that its point
         * denoted by anchor will be at the given position.
         */
        virtual void setPosition(const Point& position, Anchor anchor);

        /**
         * Returns the start angle of the pie slice in radians. See the class
         * documentation for details on how the pie slice is drawn.
         */
        virtual double getStartAngle() const {return startAngle;}

        /**
         * Sets the start angle of the pie slice in radians. See the class
         * documentation for details on how the pie slice is drawn.
         */
        virtual void setStartAngle(double startAngle);

        /**
         * Returns the end angle of the pie slice in radians. See the class
         * documentation for details on how the pie slice is drawn.
         */
        virtual double getEndAngle() const {return endAngle;}

        /**
         * Sets the end angle of the pie slice in radians. See the class
         * documentation for details on how the pie slice is drawn.
         */
        virtual void setEndAngle(double endAngle);
        //@}
};

/**
 * @brief A figure that displays a (closed) polygon, determined by a sequence of points.
 *
 * The polygon may be *smoothed*. A smoothed polygon is drawn as a series
 * of cubic Bezier curves, where the curves touch the midpoints of the sides,
 * and vertices serve as control points. As with all shape figures, drawing of
 * both the outline and the fill are optional. The drawing of filled self-
 * intersecting polygons is controlled by the *fill rule* property.
 * Line and fill color, and several other properties are inherited from
 * cAbstractShapeFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cPolygonFigure : public cAbstractShapeFigure
{
    private:
        std::vector<Point> points;
        bool smooth = false;
        JoinStyle joinStyle = JOIN_MITER;
        FillRule fillRule = FILL_EVENODD;
    private:
        void copy(const cPolygonFigure& other);
        void checkIndex(int i) const;
        void checkInsIndex(int i) const;
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPolygonFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cPolygonFigure(const cPolygonFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        cPolygonFigure& operator=(const cPolygonFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPolygonFigure *dup() const override  {return new cPolygonFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "PolygonFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the points that define the polyline.
         */
        virtual const std::vector<Point>& getPoints() const  {return points;}

        /**
         * Sets the points that define the polyline.
         */
        virtual void setPoints(const std::vector<Point>& points);

        /**
         * Returns the number of points. Equivalent to getPoints().size().
         */
        virtual int getNumPoints() const {return points.size();}

        /**
         * Returns the ith point. The index must be in the 0..getNumPoints()-1
         * interval.
         */
        virtual const Point& getPoint(int i) const {checkIndex(i); return points[i];}

        /**
         * Sets the number of points that define the polygon. (Appends new
         * points or drops trailing points as needed.)
         */
        virtual void setNumPoints(int size); // primarily for sim_std.msg

        /**
         * Sets the ith point. The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void setPoint(int i, const Point& point);

        /**
         * Appends a point.
         */
        virtual void addPoint(const Point& point);

        /**
         * Removes the ith point. The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void removePoint(int i);

        /**
         * Inserts a point at position i, shifting up the points at and above index i.
         * The index must be in the 0..getNumPoints()-1 interval.
         */
        virtual void insertPoint(int i, const Point& point);

        /**
         * Returns true if the figure is rendered as a closed smooth curve, and
         * false if as a polygon. See the class description for details.
         */
        virtual bool getSmooth() const {return smooth;}

        /**
         * Sets the flag that determines whether a polygon or a smooth closed
         * curve should be drawn. See the class description for details on how
         * the points define the curve in the smoothed case.
         */
        virtual void setSmooth(bool smooth);
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns the join style line segments will be connected with.
         */
        virtual JoinStyle getJoinStyle() const {return joinStyle;}

        /**
         * Sets the join style line segments will be connected with.
         * The default join style is miter.
         */
        virtual void setJoinStyle(JoinStyle joinStyle);

        /**
         * Returns the fill rule of the polygon. This attribute is only
         * important with filled, self-intersecting polygons. The
         * interpretation of the fill rule is consistent with its SVG
         * definition.
         */
        virtual FillRule getFillRule() const {return fillRule;}

        /**
         * Sets the fill rule of the polygon. This attribute is only important
         * with filled, self-intersecting polygons.  The interpretation of
         * the fill rule is consistent with its SVG definition.
         *
         * The default fill rule is evenodd.
         */
        virtual void setFillRule(FillRule fillRule);
        //@}
};

/**
 * @brief A figure that displays a "path", a complex shape or line modeled
 * after SVG paths.
 *
 * A path is may consist of any number of straight line segments, Bezier curves
 * and arcs. The path can be disjoint as well. Closed paths may be filled.
 * The drawing of filled self-intersecting polygons is controlled by the
 * *fill rule* property. Line and fill color, and several other properties
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
        struct PathItem { char code; }; ///< Represents an item in a cPathFigure path
        struct MoveTo : PathItem { double x; double y; };  ///< Represents the "M" path command with parameters
        struct MoveRel : PathItem { double dx; double dy; };  ///< Represents the "m" path command with parameters
        struct LineTo : PathItem { double x; double y; };  ///< Represents the "L" path command with parameters
        struct LineRel : PathItem { double dx; double dy; };  ///< Represents the "l" path command with parameters
        struct HorizontalLineTo : PathItem { double x; };  ///< Represents the "H" path command with parameters
        struct HorizontalLineRel : PathItem { double dx; };  ///< Represents the "h" path command with parameters
        struct VerticalLineTo : PathItem { double y; };  ///< Represents the "V" path command with parameters
        struct VerticalLineRel : PathItem { double dy; };  ///< Represents the "v" path command with parameters
        struct ArcTo : PathItem { double rx; double ry; double phi; bool largeArc; bool sweep; double x; double y; }; ///< Represents the "A" path command with parameters
        struct ArcRel : PathItem { double rx; double ry; double phi; bool largeArc; bool sweep; double dx; double dy; }; ///< Represents the "a" path command with parameters
        struct CurveTo : PathItem { double x1; double y1; double x; double y; }; ///< Represents the "Q" path command with parameters
        struct CurveRel : PathItem { double dx1; double dy1; double dx; double dy; }; ///< Represents the "q" path command with parameters
        struct SmoothCurveTo : PathItem { double x; double y; }; ///< Represents the "T" path command with parameters
        struct SmoothCurveRel : PathItem { double dx; double dy; }; ///< Represents the "t" path command with parameters
        struct CubicBezierCurveTo : PathItem { double x1; double y1; double x2; double y2; double x; double y; }; ///< Represents the "C" path command with parameters
        struct CubicBezierCurveRel : PathItem { double dx1; double dy1; double dx2; double dy2; double dx; double dy; }; ///< Represents the "c" path command with parameters
        struct SmoothCubicBezierCurveTo : PathItem { double x2; double y2; double x; double y; }; ///< Represents the "S" path command with parameters
        struct SmoothCubicBezierCurveRel : PathItem { double dx2; double dy2; double dx; double dy; }; ///< Represents the "s" path command with parameters
        struct ClosePath : PathItem { }; ///< Represents the "Z" path command

    private:
        std::vector<PathItem*> path;
        mutable std::string cachedPathString;
        JoinStyle joinStyle = JOIN_MITER;
        CapStyle capStyle = CAP_BUTT;
        Point offset;
        FillRule fillRule = FILL_EVENODD;

    private:
        void copy(const cPathFigure& other);
        void addItem(PathItem *item);
        void doClearPath();

    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;

    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cPathFigure(const char *name=nullptr) : cAbstractShapeFigure(name) {}
        cPathFigure(const cPathFigure& other) : cAbstractShapeFigure(other) {copy(other);}
        virtual ~cPathFigure() {doClearPath();}
        cPathFigure& operator=(const cPathFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cPathFigure *dup() const override  {return new cPathFigure(*this);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        /**
         * The move operation modifies the offset field (see getOffset()).
         */
        virtual void moveLocal(double dx, double dy) override;
        virtual const char *getRendererClassName() const override {return "PathFigureRenderer";}
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns the join style line segments will be connected with.
         */
        virtual JoinStyle getJoinStyle() const {return joinStyle;}

        /**
         * Sets the join style line segments will be connected with.
         * The default join style is miter.
         */
        virtual void setJoinStyle(JoinStyle joinStyle);

        /**
         * Returns the cap style for the figure's line.
         */
        virtual CapStyle getCapStyle() const {return capStyle;}

        /**
         * Sets the cap style for the figure's line. The default cap style
         * is butt.
         */
        virtual void setCapStyle(CapStyle capStyle);

        /**
         * Returns the fill rule of the path. This attribute is only important
         * with closed, filled, self-intersecting paths (and those that contain
         * such parts). The interpretation of the fill rule is consistent
         * with its SVG definition.
         */
        virtual FillRule getFillRule() const {return fillRule;}

        /**
         * Sets the fill rule of the path. This attribute is only important
         * with closed, filled, self-intersecting paths (and those that contain
         * such parts). The interpretation of the fill rule is consistent
         * with its SVG definition.
         *
         * The default fill rule is evenodd.
         */
        virtual void setFillRule(FillRule fillRule);
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the coordinate offset of the path. A nonzero offset
         * causes the path to appear translated by the given amount in the
         * local coordinate system.
         */
        virtual const Point& getOffset() const {return offset;}

        /**
         * Sets the coordinate offset of the path. Setting a nonzero offset
         * causes the path to appear translated by the given amount in the
         * local coordinate system. The elements of the path remain unchanged.
         * This method primarily exists so that moveLocal() can be a
         * constant-time operation.
         */
        virtual void setOffset(const Point& offset);
        //@}

        /** @name Path manipulation */
        //@{
        /**
         * Returns the path as a string.
         */
        virtual const char *getPath() const;

        /**
         * Sets the path from a string. It will be parsed, made available
         * via the getNumPathItems(), getPathItem() methods, and may be
         * further modified with the "add" methods.
         */
        virtual void setPath(const char *path);

        /**
         * Returns the number of path items.
         */
        virtual int getNumPathItems() const {return path.size();}

        /**
         * Returns the kth path item. The returned item may be cast to the
         * appropriate subtype (MoveTo, MoveRel, etc.) after examining
         * its `code` field.
         */
        virtual const PathItem *getPathItem(int k) const {return path[k];}

        /**
         * Clears the path by discarding its path items.
         */
        virtual void clearPath();

        /**
         * Appends a MoveTo item to the path. It will move the "pen" to (x,y),
         * and begin a new subpath. Paths should begin with a MoveTo.
         *
         * SVG equivalent: "M <x> <y>"
         */
        virtual void addMoveTo(double x, double y);  // M x y

        /**
         * Appends a MoveRel item to the path. It will move the "pen" from
         * the current point (lastx,lasty) to (lastx+dx,lasty+dy), and begin
         * a new subpath.
         *
         * SVG equivalent: "m <dx> <dy>"
         */
        virtual void addMoveRel(double dx, double dy);  // m dx dy

        /**
         * Appends a LineTo item to the path. It will draw a line from the last
         * point to (x,y).
         *
         * SVG equivalent: "L <x> <y>"
         */
        virtual void addLineTo(double x, double y);  // L x y

        /**
         * Appends a LineRel item to the path. It will draw a line
         * from the current point (lastx, lasty) to (lastx+dx, lasty+dy).
         *
         * SVG equivalent: "l <dx> <dy>"
         */
        virtual void addLineRel(double dx, double dy);  // l dx dy

        /**
         * Appends a HorizontalLineTo item to the path. It will draw a line
         * from the current point (lastx, lasty) to (x, lasty).
         *
         * SVG equivalent: "H <x>"
         */
        virtual void addHorizontalLineTo(double x);  // H x

        /**
         * Appends a HorizontalLineRel item to the path. It will draw a line
         * from the current point (lastx, lasty) to (lastx+dx, lasty).
         *
         * SVG equivalent: "h <dx>"
         */
        virtual void addHorizontalLineRel(double dx);  // h dx

        /**
         * Appends a VerticalLineTo item to the path. It will draw a line
         * from the current point (lastx, lasty) to the (lastx, y) point.
         *
         * SVG equivalent: "V <y>"
         */
        virtual void addVerticalLineTo(double y);  // V y

        /**
         * Appends a VerticalLineRel item to the path. It will draw a line
         * from the current point (lastx, lasty) to (lastx, lasty+dy).
         *
         * SVG equivalent: "v <dy>"
         */
        virtual void addVerticalLineRel(double dy);  // v dy

        /**
         * Appends an ArcTo item to the path. It will draw an arc from the last
         * point to the (x,y) point. The size and orientation of the ellipse
         * are defined by two radii (rx, ry) and a phi rotation angle. The
         * center of the ellipse is calculated automatically to satisfy the
         * constraints imposed by the other parameters. The largeArc and sweep
         * flags determine which of the four possible arcs are chosen. If
         * largeArc=true, then one of the larger (greater than 180 degrees)
         * arcs are chosen, otherwise one of the the smaller ones. If
         * sweep=true, the arc will be drawn in the "positive-angle" direction,
         * otherwise in the negative-angle direction.
         *
         * SVG equivalent: "A <rx> <ry> <phi> <largeArc> <sweep> <x> <y>"
         */
        virtual void addArcTo(double rx, double ry, double phi, bool largeArc, bool sweep, double x, double y); // A rx ry phi largeArc sweep x y

        /**
         * Appends an ArcRel item to the path. It will draw an arc from the last
         * point (lastx, lasty) to (lastx+dx, lasty+dy). Just as with
         * addArcTo(), the size and orientation of the ellipse are defined
         * by two radii (rx, ry) and a phi rotation angle. The center of the
         * ellipse is calculated automatically to satisfy the constraints
         * imposed by the other parameters. The largeArc and sweep flags
         * determine which of the four possible arcs are chosen. If
         * largeArc=true, then one of the larger (greater than 180 degrees)
         * arcs are chosen, otherwise one of the the smaller ones. If
         * sweep=true, the arc will be drawn in the "positive-angle" direction,
         * otherwise in the negative-angle direction.
         *
         * SVG equivalent: "a <rx> <ry> <phi> <largeArc> <sweep> <dx> <dy>"
         */
        virtual void addArcRel(double rx, double ry, double phi, bool largeArc, bool sweep, double dx, double dy); // a rx ry phi largeArc sweep dx dy

        /**
         * Appends a CurveTo item to the path. It will draw a quadratic Bezier curve
         * from the current point to (x,y) using (x1,y1) as the control point.
         *
         * SVG equivalent: "Q <x1> <y1> <x> <y>"
         */
        virtual void addCurveTo(double x1, double y1, double x, double y); // Q x1 y1 x y

        /**
         * Appends a CurveRel item to the path. It will draw a quadratic Bezier curve
         * from the current point (lastx, lasty) to (lastx+dx, lasty+dy) using
         * (lastx+dx1, lasty+dy1) as the control point.
         *
         * SVG equivalent: "q <dx1> <dy1> <dx> <dy>"
         */
        virtual void addCurveRel(double dx1, double dy1, double dx, double dy); // q dx1 dy1 dx dy

        /**
         * Appends a SmoothCurveTo item to the path. It will draw a quadratic
         * Bezier curve from the current point to (x,y). The control point is
         * assumed to be the reflection of the control point on the previous
         * command relative to the current point. (If there is no previous
         * command or if the previous command was not a quadratic Bezier curve
         * command such as Q, q, T or t, assume the control point is coincident
         * with the current point.)
         *
         * SVG equivalent: "T <x> <y>"
         */
        virtual void addSmoothCurveTo(double x, double y); // T x y

        /**
         * Appends a SmoothCurveRel item to the path. It will draw a quadratic
         * Bezier curve from the current point (lastx, lasty) to (last+dx, lasty+dy).
         * The control point is assumed to be the reflection of the control
         * point on the previous command relative to the current point.
         * (If there is no previous command or if the previous command was
         * not a quadratic Bezier curve command such as Q, q, T or t, assume
         * the control point is coincident with the current point.)
         *
         * SVG equivalent: "t <dx> <dy>"
         */
        virtual void addSmoothCurveRel(double dx, double dy); // t dx dy

        /**
         * Appends a CubicBezierCurveTo item to the path. It will draw a cubic
         * Bezier curve from the current point to (x,y) using (x1,y1) as the
         * control point at the beginning of the curve and (x2,y2) as the
         * control point at the end of the curve.
         *
         * SVG equivalent: "C <x1> <y1> <x2> <y2> <x> <y>"
         */
        virtual void addCubicBezierCurveTo(double x1, double y1, double x2, double y2, double x, double y); // C x1 y1 x2 y2 x y

        /**
         * Appends a CubicBezierCurveRel item to the path. It will draw a cubic
         * Bezier curve from the current point (lastx, lasty) to (lastx+dx, lasty+dy)
         * using (lastx+dx1, lasty+dy1) as the control point at the beginning
         * of the curve and (lastx+dx2, lasty+dy2) as the control point at the
         * end of the curve.
         *
         * SVG equivalent: "c <dx1> <dy1> <dx2> <dy2> <dx> <dy>"
         */
        virtual void addCubicBezierCurveRel(double dx1, double dy1, double dx2, double dy2, double dx, double dy); // c dx1 dy1 dx2 dy2 dx dy

        /**
         * Appends a SmoothCubicBezierCurveTo item to the path. It will draw
         * a cubic Bezier curve from the current point to (x,y).
         * The first control point is assumed to be the reflection of the
         * second control point on the previous command relative to the last
         * point. (If there is no previous command or if the previous command was
         * not a cubic Bezier command such as C, c, S or s, assume the first
         * control point is coincident with the current point.) (x2,y2) is the
         * second control point (i.e., the control point at the end of the curve).
         *
         * SVG equivalent: "S <x2> <y2> <x> <y>"
         */
        virtual void addSmoothCubicBezierCurveTo(double x2, double y2, double x, double y); // S x2 y2 x y

        /**
         * Appends a SmoothCubicBezierCurveRel item to the path. It will draw
         * a cubic Bezier curve from the current point (lastx, lasty) to (lastx+dx,
         * lasty+dy). The first control point is assumed to be the reflection
         * of the second control point on the previous command relative to the last
         * point. (If there is no previous command or if the previous command was
         * not a cubic Bezier command such as C, c, S or s, assume the first
         * control point is coincident with the current point.) (lastx+dx2, lasty+dy2)
         * is the second control point (i.e., the control point at the end of the curve).
         *
         * SVG equivalent: "s <dx2> <dy2> <dx> <dy>"
         */
        virtual void addSmoothCubicBezierCurveRel(double dx2, double dy2, double dx, double dy); // s dx2 dy2 dx dy

        /**
         * Appends a ClosePath item to the path. It will close the current subpath
         * by drawing a straight line from the current point to current subpath's
         * initial point.
         *
         * SVG equivalent: "Z" or "z"
         */
        virtual void addClosePath(); // Z
        //@}
};

/**
 * @brief Abstract base class for figures that display text. Text may be multi-line.
 *
 * The location of the text on the canvas is determined jointly by the
 * *position* and *anchor* properties. The anchor tells how to
 * position the text relative to the positioning point. For example,
 * if anchor is ANCHOR_CENTER then the text is centered on the point;
 * if anchor is ANCHOR_N then the text will be drawn so that its top center
 * is at the positioning point. The values ANCHOR_BASELINE_START,
 * ANCHOR_BASELINE_MIDDLE, ANCHOR_BASELINE_END refer to the beginning,
 * middle and end of the baseline of the (first line of the) text as
 * anchor point. The chosen alignment mode does not affect this,
 * the baseline anchor points always behave as if the first line spanned
 * the entire width of the bounding box. Anchor defaults to ANCHOR_CENTER.
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
        double opacity = 1;
        bool halo = false;
        Font font;
        std::string text;
        Anchor anchor = ANCHOR_NW;
        Alignment alignment = ALIGN_LEFT;
    private:
        void copy(const cAbstractTextFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractTextFigure(const char *name=nullptr) : cFigure(name), color(BLACK) {}
        cAbstractTextFigure(const cAbstractTextFigure& other) : cFigure(other) {copy(other);}
        cAbstractTextFigure& operator=(const cAbstractTextFigure& other);
        //@}

        /** @name Redefined cObject and cFigure member functions. */
        //@{
        virtual cAbstractTextFigure *dup() const override {throw cRuntimeError(this, E_CANTDUP);}
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        /**
         * Updates the position of the text, see getPosition().
         */
        virtual void moveLocal(double dx, double dy) override;
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the position of the text. The position and the anchoring
         * mode jointly determine the placement of the text item; see the
         * class description for details.
         */
        virtual const Point& getPosition() const  {return position;}

        /**
         * Sets the position of the text. The position and the anchoring
         * mode jointly determine the placement of the text item; see the
         * class description for details.
         */
        virtual void setPosition(const Point& position);

        /**
         * Returns the anchoring mode of the text. The position and the
         * anchoring mode jointly determine the placement of the text item;
         * see the class description for details.
         */
        virtual Anchor getAnchor() const  {return anchor;}

        /**
         * Sets the anchoring mode of the text. The position and the anchoring
         * mode jointly determine the placement of the text item; see the
         * class description for details.
         */
        virtual void setAnchor(Anchor anchor);

        /**
         * Returns the alignment mode of the text. This only has an effect
         * on figures with multi-line text. Each line will be aligned
         * within the figure bounding box according to this option.
         * This does not affect the anchor points on the baseline; see the
         * class description for details.
         */
        virtual Alignment getAlignment() const  {return alignment;}

        /**
         * Sets the alignment mode of the text. This only has an effect
         * on figures with multi-line text. Each line will be aligned
         * within the figure bounding box according to this option.
         * This does not affect the anchor points on the baseline; see the
         * class description for details.
         */
        virtual void setAlignment(Alignment alignment);

        /**
         * Returns the bounding box of the text figure.
         *
         * Caveat: Note that the figure itself only has a position and an anchor
         * available for computing the bounding box, size (and for some cases
         * ascent height) need to be supplied by the user interface of the
         * simulation (cEnvir). The result depends on the details of font
         * rendering and other factors, so the returned numbers and their
         * accuracy may vary across user interfaces, operating systems or
         * even installations. Cmdenv and other non-GUI environments
         * may return completely made-up (but not entirely unrealistic) numbers.
         *
         * @see cEnvir::getTextExtent()
         * @see getTextExtent()
         * @see getFontAscent()
         */
        virtual Rectangle getBounds() const;

        /**
         * Returns the size of the text figure, not taking into account the
         * anchor point, the position, or (in case of cLabelFigure) the angle.
         *
         * Caveat: The result depends on the details of font rendering and other
         * factors, so the returned numbers and their accuracy may vary across
         * user interfaces, operating systems or even installations. Cmdenv and
         * other non-GUI environments may return completely made-up (but not
         * entirely unrealistic) numbers.
         *
         * @see cEnvir::getTextExtent()
         * @see getBounds()
         * @see getFontAscent()
         */
        virtual Point getTextExtent() const;

        /**
         * Returns the ascent of the font set on the text figure.
         *
         * Caveat: The result depends on the details of font rendering and other
         * factors, so the returned number and its accuracy may vary across user
         * interfaces, operating systems or even installations. Cmdenv and other
         * non-GUI environments may return a completely made-up (but not entirely
         * unrealistic) number.
         *
         * @see cEnvir::getTextExtent()
         * @see getBounds()
         * @see getTextExtent()
         */
        virtual double getFontAscent() const;
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns the text color.
         */
        virtual const Color& getColor() const  {return color;}

        /**
         * Sets the text color. The default color is black.
         */
        virtual void setColor(const Color& color);

        /**
         * Returns the opacity of the figure, a number in the [0,1] interval.
         */
        virtual double getOpacity() const  {return opacity;}

        /**
         * Sets the line opacity of the figure. The argument must be a number
         * in the [0,1] interval. The default opacity is 1.0.
         */
        virtual void setOpacity(double opacity);

        /**
         * Returns true if a "halo" is displayed around the text.
         */
        virtual bool getHalo() const  {return halo;}

        /**
         * Sets the flag that controls whether a partially transparent "halo"
         * should be displayed around the text. The halo improves the
         * readability of the text when it is displayed over a background
         * that has similar color as the text, or when it overlaps with other
         * text items. The default setting is false (no halo).
         *
         * Note: In Qtenv, halo may be slow to render for text figures
         * (cTextFigure) under some circumstances. There is no such penalty
         * for label figures (cLabelFigure).
         */
        virtual void setHalo(bool enabled);

        /**
         * Returns the font used for the figure.
         */
        virtual const Font& getFont() const  {return font;}

        /**
         * Sets the font to be used for the figure. The default font has no
         * specific typeface or size set, allowing the UI to use its preferred
         * font.
         */
        virtual void setFont(Font font);
        //@}

        /** @name Input */
        //@{
        /**
         * Returns the text to be displayed.
         */
        virtual const char *getText() const  {return text.c_str();}

        /**
         * Sets the text to be displayed. The text may contain newline and tab
         * characters ("\n", "\t") for formatting.
         */
        virtual void setText(const char* text);
        //@}
};

/**
 * @brief A figure that displays text which is affected by zooming and transformations.
 *
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
 * @brief A figure that displays text which is unaffected by zooming or
 * transformations, except for its position.
 *
 * Font, color, position, anchoring and other properties are inherited from
 * cAbstractTextFigure.
 *
 * @ingroup Canvas
 */
class SIM_API cLabelFigure : public cAbstractTextFigure
{
    private:
        double angle = 0.0; // in radians, positive is CCW, 0 is "horizontal" or "unrotated"
    private:
        void copy(const cLabelFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
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
        virtual void parse(cProperty *property) override;
        virtual const char *getRendererClassName() const override {return "LabelFigureRenderer";}
        //@}

        /** @name Geometry */
        //@{
        virtual double getAngle() const  {return angle;}
        virtual void setAngle(double angle);
        virtual Rectangle getBounds() const override;
        //@}
};

/**
 * @brief Abstract base class for figures that display an image.
 *
 * The location of the image on the canvas is determined jointly by the
 * *position* and *anchor* properties. The anchor tells how to
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
 * in the image, i.e. the alpha channel.)
 *
 * @ingroup Canvas
 */
class SIM_API cAbstractImageFigure : public cFigure
{
    private:
        Point position;
        Anchor anchor = ANCHOR_CENTER;  // note: do not use the ANCHOR_BASELINE_START/MIDDLE/END constants, as they are for text items
        double width = 0, height = 0; // zero or negative values mean using the image's own size
        Interpolation interpolation = INTERPOLATION_FAST;
        double opacity = 1;
        Color tintColor;
        double tintAmount = 0; // in the range 0..1
    private:
        void copy(const cAbstractImageFigure& other);
    protected:
        virtual const char **getAllowedPropertyKeys() const override;
        virtual void hashTo(cHasher& hasher) const override;
        virtual Point getDefaultSize() const = 0;
    public:
        /** @name Constructors, destructor, assignment. */
        //@{
        explicit cAbstractImageFigure(const char *name=nullptr) : cFigure(name),  tintColor(BLUE) { }
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
        virtual void moveLocal(double dx, double dy) override;
        //@}

        /** @name Geometry */
        //@{
        /**
         * Returns the position of the image. The position and the anchoring
         * mode jointly determine the placement of the image; see the class
         * description for details.
         */
        virtual const Point& getPosition() const  {return position;}

        /**
         * Sets the position of the image. The position and the anchoring
         * mode jointly determine the placement of the image; see the
         * class description for details.
         */
        virtual void setPosition(const Point& position);

        /**
         * Returns the anchoring mode of the image. The position and the
         * anchoring mode jointly determine the placement of the image;
         * see the class description for details.
         */
        virtual Anchor getAnchor() const  {return anchor;}

        /**
         * Sets the anchoring mode of the image. The position and the
         * anchoring mode jointly determine the placement of the image;
         * see the class description for details.
         */
        virtual void setAnchor(Anchor anchor);

        /**
         * Returns zero if the image is displayed with its "natural" width,
         * or the width (a positive number) if it is scaled.
         */
        virtual double getWidth() const  {return width;}

        /**
         * Sets the width of the image figure. Use this method if you want
         * to display the image at a size different from its "natural" size.
         * To clear this setting, specify 0 as parameter. The default value
         * is 0 (natural width).
         */
        virtual void setWidth(double width);

        /**
         * Returns zero if the image is displayed with its "natural" height,
         * or the height (a positive number) if it is scaled.
         */
        virtual double getHeight() const  {return height;}

        /**
         * Sets the height of the image figure. Use this method if you want
         * to display the image at a size different from its "natural" size.
         * To clear this setting, specify 0 as parameter. The default value
         * is 0 (natural height).
         */
        virtual void setHeight(double height); // zero means "unset"

        /**
         * Sets the image figure's width and height together. Delegates to
         * setWidth() and setHeight().
         */
        virtual void setSize(double width, double height) {setWidth(width); setHeight(height);}

        /**
         * Returns the image's bounds in the current coordinate system.
         *
         * Caveat: for cImageFigure/cIconFigure, when no explicit size is
         * specified for the image using setWidth()/setHeight() and the
         * simulation is running under Cmdenv or another non-GUI environment,
         * the image size used for the bounds computation may be a guess or
         * a made-up size, as Cmdenv does not actually load the images files.
         *
         * @see cEnvir::getImageSize().
         */
        virtual Rectangle getBounds() const;
        //@}

        /** @name Styling */
        //@{
        /**
         * Returns the interpolation mode for the image. The interpolation mode
         * determines how the image is scaled to the size it is rendered with
         * on the screen.
         */
        virtual Interpolation getInterpolation() const {return interpolation;}

        /**
         * Sets the interpolation mode for the image. The interpolation mode
         * determines how the image is scaled to the size it is rendered with
         * on the screen. The default interpolation mode is "fast".
         */
        virtual void setInterpolation(Interpolation interpolation);

        /**
         * Returns the opacity of the figure.
         */
        virtual double getOpacity() const  {return opacity;}

        /**
         * Sets the opacity of the figure. The argument must be a number
         * in the [0,1] interval. The default opacity is 1.0.
         */
        virtual void setOpacity(double opacity);

        /**
         * Returns the tint color of the figure.
         */
        virtual const Color& getTintColor() const  {return tintColor;}

        /**
         * Sets the tint color of the figure. The tint color and the tint
         * amount jointly determine the colorization of the figure.
         * The default tint color is blue.
         *
         * @see setTintAmount()
         */
        virtual void setTintColor(const Color& tintColor);

        /**
         * Returns the tint amount of the figure, a number in the [0,1]
         * interval.
         */
        virtual double getTintAmount() const  {return tintAmount;}

        /**
         * Sets the tint amount of the figure. The argument must be a number
         * in the [0,1] interval. The tint color and the tint amount jointly
         * determine the colorization of the figure. The default tint amount
         * is 0.
         *
         * @see setTintColor()
         */
        virtual void setTintAmount(double tintAmount);
        //@}
};

/**
 * @brief A figure that displays an image, typically an icon or a background image,
 * loaded from the \opp image path.
 *
 * Positioning and other properties are inherited from cAbstractImageFigure.
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
        virtual void hashTo(cHasher& hasher) const override;
        virtual Point getDefaultSize() const override;
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
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual const char *getRendererClassName() const override {return "ImageFigureRenderer";}
        //@}

        /** @name Figure attributes */
        //@{
        /**
         * Returns the string that identifies the image to display.
         */
        virtual const char *getImageName() const  {return imageName.c_str();}

        /**
         * Sets the image to display. The imageName parameter must identify an
         * image loaded from the \opp image path, in the form also used by the
         * display string "i" tag: with subdirectory names but without filename
         * extension. Examples: "block/queue" or "device/switch_vl".
         */
        virtual void setImageName(const char* imageName);

        /**
         * Returns the "natural" width of the image. For raster images, this is
         * the width of the image in pixels.
         *
         * Caveat: when the simulation is running under Cmdenv or another
         * non-GUI environment, the returned width may be a guess or a made-up
         * number, as Cmdenv does not actually load the images files.
         *
         * @see cAbstractImageFigure::getBounds(), cEnvir::getImageSize()
         */
        virtual int getImageNaturalWidth() const {return getDefaultSize().x;}

        /**
         * Returns the "natural" height of the image. For raster images, this is
         * the height of the image in pixels.
         *
         * Caveat: when the simulation is running under Cmdenv or another
         * non-GUI environment, the returned height may be a guess or a made-up
         * number, as Cmdenv does not actually load the images files.
         *
         * @see cAbstractImageFigure::getBounds(), cEnvir::getImageSize()
         */
        virtual int getImageNaturalHeight() const  {return getDefaultSize().y;}
        //@}
};

/**
 * @brief A figure that displays an image, typically an icon or a background image,
 * loaded from the \opp image path, in a non-zooming way.
 *
 * Positioning and other properties are inherited from cAbstractImageFigure.
 *
 * Note: The image is not affected by transforms or zoom, except for its position.
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
 * @brief A figure that displays an image that can be manipulated programmatically.
 *
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
        virtual void hashTo(cHasher& hasher) const override;
        virtual Point getDefaultSize() const override;
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
        virtual std::string str() const override;
        virtual void parse(cProperty *property) override;
        virtual const char *getRendererClassName() const override {return "PixmapFigureRenderer";}
        //@}

        /** @name Pixmap manipulation */
        //@{
        /**
         * Returns the displayed pixmap. It must not be modified directly (e.g.
         * after casting away the const modifier), because the GUI would not be
         * notified about the change. Use the pixel manipulation methods of the
         * figure class, or make a copy of the pixmap, update it, and then set it
         * back on the figure using setPixmap().
         */
        virtual const Pixmap& getPixmap() const {return pixmap;}

        /**
         * Sets the pixmap to be displayed. Note that this involves copying
         * the pixmap data.
         */
        virtual void setPixmap(const Pixmap& pixmap);

        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual int getPixmapHeight() const {return pixmap.getHeight();}
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual int getPixmapWidth() const {return pixmap.getWidth();}
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixmapSize(int width, int height, const RGBA& fill); // nondestructive, set *newly added* pixels with this color and opacity
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixmapSize(int width, int height, const Color& color, double opacity);  // nondestructive, fills *newly added* pixels with this color and opacity
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void fillPixmap(const RGBA& fill);
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void fillPixmap(const Color& color, double opacity);
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual const RGBA getPixel(int x, int y) const {return pixmap.pixel(x, y);}
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixel(int x, int y, const RGBA& argb);
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixel(int x, int y, const Color& color, double opacity = 1.0);
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual const Color getPixelColor(int x, int y) const {return pixmap.getColor(x,y);}
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixelColor(int x, int y, const Color& color);
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual double getPixelOpacity(int x, int y) const {return pixmap.getOpacity(x,y);}
        /** Delegates to the similar method of the contained Pixmap instance. */
        virtual void setPixelOpacity(int x, int y, double opacity);
        //@}
};

/**
 * @brief Provides a scene graph based 2D drawing API for modules.
 *
 * Notes:
 * - all figures are under a root figure (which we try not to expose too much)
 * - Z-order is defined by child order (bottom-to-top) and preorder traversal
 * - layer-like functionality is provided via figure tags: the Qtenv UI allows figures
 *   to be shown or hidden according to tags they contain
 * - cModule has one dedicated canvas, additional canvas objects can be created
 * - initial module canvas contents comes from @figure properties inside the compound module NED file, see samples/canvas for examples!
 * - extensibility: type=foo in a @figure property will cause the canvas to look for a registered FooFigure or cFooFigure class to instantiate
 * - dup() makes deep copy (duplicates the figure tree too)
 * - the submodules layer (see getSubmodulesLayer()) is currently an empty placeholder figure where Qtenv will
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
        static std::map<std::string,cObjectFactory*> figureFactories;
        std::map<const cObject*,double> animationSpeedMap;  // maps source to animationSpeed
        double minAnimationSpeed = DBL_MAX; // minimum of the values in animationSpeedMap cached, or DBL_MAX for none
        double animationHoldEndTime = 0; // the effective one will be the maximum endTime of all visible canvases
    public:
        // internal:
        virtual cFigure *parseFigure(cProperty *property) const;
        virtual cFigure *createFigure(const char *type) const;
        static bool containsCanvasItems(cProperties *properties);
        virtual void addFiguresFrom(cProperties *properties);
        virtual uint64_t parseTags(const char *s);
        virtual std::string getTags(uint64_t tagBits);
        const std::map<const cObject*,double>& getAnimationSpeedMap() const {return animationSpeedMap;}  // for e.g. Qtenv
        double getMinAnimationSpeed() const {return minAnimationSpeed;} // for e.g. Qtenv; DBL_MAX if none
        double getAnimationHoldEndTime() const {return animationHoldEndTime;}  // for e.g. Qtenv
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
        virtual std::string str() const override;
        //@}

        /** @name Canvas attributes. */
        //@{
        /**
         * Returns the background color of the canvas.
         */
        virtual const cFigure::Color& getBackgroundColor() const {return backgroundColor;}

        /**
         * Sets the background color of the canvas.
         */
        virtual void setBackgroundColor(const cFigure::Color& color) {this->backgroundColor = color;}

        /**
         * Returns a hash computed from all figures in the canvas, for
         * fingerprint computation.
         */
        virtual uint32_t getHash() const;
        //@}

        /** @name Managing child figures. */
        //@{
        /**
         * Returns the root figure of the canvas. The root figure has no visual
         * appearance, it solely serves as a container for other figures.
         */
        virtual cFigure *getRootFigure() const {return rootFigure;}

        /**
         * Appends the given figure to the child list of the root figure.
         */
        virtual void addFigure(cFigure *figure) {rootFigure->addFigure(figure);}

        /**
         * Inserts the given figure into the child list of the root figure at
         * the given position. Note that relative order in the child list
         * only affects stacking order if the respective figures have the
         * same Z-index.
         */
        virtual void addFigure(cFigure *figure, int pos) {rootFigure->addFigure(figure, pos);}

        /**
         * Removes the given figure from the child list of the root figure.
         * An error is raised if the figure is not a child of the root figure.
         */
        virtual cFigure *removeFigure(cFigure *figure) {return rootFigure->removeFigure(figure);}

        /**
         * Removes the kth figure from the child list of the root figure. An error
         * is raised if pos is not in the range 0..getNumFigures()-1.
         */
        virtual cFigure *removeFigure(int pos) {return rootFigure->removeFigure(pos);}

        /**
         * Finds the first figure with the given name among the children of
         * the root figure, and returns its index. If there is no such figure,
         * -1 is returned.
         */
        virtual int findFigure(const char *name) const  {return rootFigure->findFigure(name);}

        /**
         * Finds the given figure among the children of the root figure, and
         * returns its index. If it is not found, -1 is returned.
         */
        virtual int findFigure(cFigure *figure) const  {return rootFigure->findFigure(figure);}

        /**
         * Returns true if this canvas contains any figures (apart from the
         * root figure), and false otherwise.
         */
        virtual bool hasFigures() const {return rootFigure->containsFigures();}

        /**
         * Returns the number of the root figure's child figures.
         */
        virtual int getNumFigures() const {return rootFigure->getNumFigures();}

        /**
         * Returns the kth figure in the child list of the root figure. The
         * index must be in the range 0..getNumFigures()-1. An out-of-bounds
         * index will cause a runtime error.
         */
        virtual cFigure *getFigure(int pos) const {return rootFigure->getFigure(pos);}

        /**
         * Returns the first child figure of the root figure with the given name,
         * or nullptr if there is no such figure.
         */
        virtual cFigure *getFigure(const char *name) const  {return rootFigure->getFigure(name);}
        //@}

        /** @name Accessing the figure tree. */
        //@{
        /**
         * Returns the figure that (conceptionally) contains the figures
         * displaying the submodules and connection arrows in the compound
         * module. It may be used for positioning figures in Z order relative
         * to submodules.  This method returns nullptr for extra canvases that
         * don't display submodules.
         */
        virtual cFigure *getSubmodulesLayer() const;

        /**
         * Returns the first figure with the given name in the figure tree of
         * the canvas, or nullptr if there is no such figure.
         */
        virtual cFigure *findFigureRecursively(const char *name) const {return rootFigure->findFigureRecursively(name);}

        /**
         * Returns the figure identified by the given path, a string of figure
         * names separated by dots. See cFigure::getFigureByPath().
         */
        virtual cFigure *getFigureByPath(const char *path) const {return rootFigure->getFigureByPath(path);}
        //@}

        /** @name Figure tags. */
        //@{
        /**
         * Returns the union of the tags used by figures in this canvas as a
         * single space-separated string.
         */
        virtual std::string getAllTags() const;

        /**
         * Returns the union of the tags used by figures in this canvas as a
         * vector of strings.
         */
        virtual std::vector<std::string> getAllTagsAsVector() const;
        //@}

        /** @name Animation. */
        //@{
        /**
         * Sets the preferred animation speed associated with the given source
         * object. Specify zero or a negative speed value to clear (withdraw)
         * a previously set animation speed request. The source object only
         * serves as a key to allow clients register several animation speed
         * requests with the option to later update and clear any of them.
         *
         * The UI of the simulation will take these requests into account when
         * determining the speed used for actual animation. (For example, the
         * UI may take the minimum of the requested animation speeds across all
         * currently viewed canvases, or it may ignore the requests and use an
         * interactively given animation speed.)
         *
         * @see cEnvir::getAnimationSpeed(), cEnvir::getAnimationTime()
         */
        virtual void setAnimationSpeed(double animationSpeed, const cObject *source);

        /**
         * Returns the animation speed request associated with the given
         * source object, or 0 if none exists.
         */
        virtual double getAnimationSpeed(const cObject *source);

        /**
         * Requests a "hold" interval of the given length, starting at the
         * current animation time. During a hold interval, only animation
         * takes place, but simulation time does not progress and no events
         * are processed. Hold intervals are intended for animating actions
         * that take zero simulation time.
         *
         * If a hold request is issued when there is one already in progress,
         * the current hold will be extended as needed to incorporate the
         * request. A hold request cannot be cancelled or shrunk.
         *
         * When rendering (i.e. inside refreshDisplay()), use
         * `getEnvir()->getRemainingAnimationHoldTime()` or
         * `getEnvir()->getAnimationTime()` to query the
         * progress of the animation.
         *
         * @see cEnvir::getRemainingAnimationHoldTime(), cEnvir::getAnimationTime()
         */
        virtual void holdSimulationFor(double animationTimeDelta);
        //@}
};

}  // namespace omnetpp


#endif


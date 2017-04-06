//==========================================================================
//  FIGURERENDERERS.CC - part of
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

#include "common/stringutil.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/platdep/platmisc.h"
#include "tkutil.h"
#include "figurerenderers.h"
#include "tkenv.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace tkenv {

std::map<std::string, FigureRenderer *> FigureRenderer::rendererCache;

Register_Class(NullRenderer);
Register_Class(GroupFigureRenderer);
Register_Class(LineFigureRenderer);
Register_Class(ArcFigureRenderer);
Register_Class(PolylineFigureRenderer);
Register_Class(RectangleFigureRenderer);
Register_Class(OvalFigureRenderer);
Register_Class(RingFigureRenderer);
Register_Class(PieSliceFigureRenderer);
Register_Class(PolygonFigureRenderer);
Register_Class(PathFigureRenderer);
Register_Class(TextFigureRenderer);
Register_Class(LabelFigureRenderer);
Register_Class(ImageFigureRenderer);
Register_Class(IconFigureRenderer);
Register_Class(PixmapFigureRenderer);

#define MAXARGC  40

char *FigureRenderer::point(const cFigure::Point& p)
{
    char *buf = getBuf(40);
    sprintf(buf, "%g %g", p.x, p.y);
    return buf;
}

char *FigureRenderer::point2(const cFigure::Point& p1, const cFigure::Point& p2)
{
    char *buf = getBuf(80);
    sprintf(buf, "%g %g %g %g", p1.x, p1.y, p2.x, p2.y);
    return buf;
}

char *FigureRenderer::bounds(const cFigure::Rectangle& bounds)
{
    char *buf = getBuf(64);
    sprintf(buf, " %g %g %g %g", bounds.x, bounds.y, bounds.x+bounds.width, bounds.y+bounds.height);
    return buf;
}

char *FigureRenderer::matrix(const cFigure::Transform& transform)
{
    char *buf = getBuf(128);
    sprintf(buf, "{%g %g} {%g %g} {%g %g}", transform.a, transform.b, transform.c, transform.d, transform.t1, transform.t2);
    return buf;
}

char *FigureRenderer::color(const cFigure::Color& color)
{
    char *buf = getBuf(16);
    sprintf(buf, "#%2.2x%2.2x%2.2x", color.red, color.green, color.blue);
    return buf;
}

char *FigureRenderer::itoa(int i)
{
    char *buf = getBuf(16);
    sprintf(buf, "%d", i);
    return buf;
}

char *FigureRenderer::dtoa(double d)
{
    char *buf = getBuf(20);
    sprintf(buf, "%f", d);
    return buf;
}

void FigureRenderer::lineStyle(cFigure::LineStyle style, int& argc, const char *argv[])
{
    argv[argc++] = "-strokedasharray";

    switch (style) {
        case cFigure::LINE_SOLID: argv[argc++] = ""; break;
        case cFigure::LINE_DOTTED: argv[argc++] = "1 2"; break;
        case cFigure::LINE_DASHED: argv[argc++] = "3 3"; break;
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

void FigureRenderer::capStyle(cFigure::CapStyle style, int& argc, const char *argv[])
{
    argv[argc++] = "-strokelinecap";

    switch (style) {
        case cFigure::CAP_BUTT: argv[argc++] = "butt"; break;
        case cFigure::CAP_SQUARE: argv[argc++] = "projecting"; break;
        case cFigure::CAP_ROUND: argv[argc++] = "round"; break;
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

void FigureRenderer::joinStyle(cFigure::JoinStyle style, int& argc, const char *argv[])
{
    argv[argc++] = "-strokelinejoin";

    switch (style) {
        case cFigure::JOIN_MITER: argv[argc++] = "miter"; break;
        case cFigure::JOIN_BEVEL: argv[argc++] = "bevel"; break;
        case cFigure::JOIN_ROUND: argv[argc++] = "round"; break;
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

void FigureRenderer::arrowHead(bool isStart, cFigure::Arrowhead arrowHead, int lineWidth, double zoom, int& argc, const char *argv[])
{
    bool hasArrow = arrowHead != cFigure::ARROW_NONE;
    argv[argc++] = isStart ? "-startarrow" : "-endarrow";
    argv[argc++] = hasArrow ? "1" : "0";

    if (hasArrow) {
        const char *fill;
        switch (arrowHead) {
            case cFigure::ARROW_BARBED: fill = "0.7"; break;
            case cFigure::ARROW_SIMPLE: fill = "0"; break;
            case cFigure::ARROW_TRIANGLE: fill = "1"; break;
            default: throw cRuntimeError("Unexpected arrow type %d", arrowHead);
        }
        argv[argc++] = isStart ? "-startarrowfill" : "-endarrowfill";
        argv[argc++] = fill;
        argv[argc++] = isStart ? "-startarrowlength" : "-endarrowlength";
        argv[argc++] = dtoa(10.0 / zoom);
        argv[argc++] = isStart ? "-startarrowwidth" : "-endarrowwidth";
        argv[argc++] = dtoa(5.0 / zoom);
    }
}

void FigureRenderer::fillRule(cFigure::FillRule fill, int& argc, const char *argv[])
{
    argv[argc++] = "-fillrule";

    switch (fill) {
        case cFigure::FILL_EVENODD: argv[argc++] = "evenodd"; break;
        case cFigure::FILL_NONZERO: argv[argc++] = "nonzero"; break;
        default: throw cRuntimeError("Unexpected fill rule %d", fill);
    }
}

void FigureRenderer::interpolation(cFigure::Interpolation interpolation, int& argc, const char *argv[])
{
    argv[argc++] = "-interpolation";

    switch (interpolation) {
        case cFigure::INTERPOLATION_NONE: argv[argc++] = "none"; break;
        case cFigure::INTERPOLATION_FAST: argv[argc++] = "fast"; break;
        case cFigure::INTERPOLATION_BEST: argv[argc++] = "best"; break;
        default: throw cRuntimeError("Unexpected interpolation mode %d", interpolation);
    }
}

static void doAnchor(cFigure::Anchor anchor, bool text, int& argc, const char *argv[])
{
    argv[argc++] = text ? "-textanchor" : "-anchor";

    switch (anchor) {
        case cFigure::ANCHOR_CENTER: argv[argc++] = "c"; break;
        case cFigure::ANCHOR_N: argv[argc++] = "n"; break;
        case cFigure::ANCHOR_E: argv[argc++] = "e"; break;
        case cFigure::ANCHOR_S: argv[argc++] = "s"; break;
        case cFigure::ANCHOR_W: argv[argc++] = "w"; break;
        case cFigure::ANCHOR_NW: argv[argc++] = "nw"; break;
        case cFigure::ANCHOR_NE: argv[argc++] = "ne"; break;
        case cFigure::ANCHOR_SE: argv[argc++] = "se"; break;
        case cFigure::ANCHOR_SW: argv[argc++] = "sw"; break;
        case cFigure::ANCHOR_BASELINE_START: argv[argc++] = text ? "start" : "sw"; break;
        case cFigure::ANCHOR_BASELINE_MIDDLE: argv[argc++] = text ? "middle" : "s"; break;
        case cFigure::ANCHOR_BASELINE_END: argv[argc++] = text ? "end" : "se"; break;
        default: throw cRuntimeError("Unexpected anchor %d", anchor);
    }
}

void FigureRenderer::anchor(cFigure::Anchor anchor, int& argc, const char *argv[])
{
    doAnchor(anchor, false, argc, argv);
}

void FigureRenderer::textanchor(cFigure::Anchor anchor, int& argc, const char *argv[])
{
    doAnchor(anchor, true, argc, argv);
}

void FigureRenderer::font(const cFigure::Font& font, FigureRenderingHints *hints, int& argc, const char *argv[])
{
    argv[argc++] = "-fontfamily";
    argv[argc++] = !font.typeface.empty() ? font.typeface.c_str() : hints->defaultFont.c_str();

    int points = (font.pointSize > 0 ? font.pointSize : hints->defaultFontSize);
    argv[argc++] = "-fontsize";
    char *buf = getBuf(16);
    sprintf(buf, "%d", (int)std::round((double)points * (16.0/12))); // scaling correction for 96 DPI (1 point i 1/72 inch)
    argv[argc++] = buf;

    argv[argc++] = "-fontweight";
    argv[argc++] = (font.style & cFigure::FONT_BOLD) != 0 ? "bold" : "normal";

    argv[argc++] = "-fontslant";
    argv[argc++] = (font.style & cFigure::FONT_ITALIC) != 0 ? "italic" : "normal";

    // note: no tkpath support for underline
}

#define PT(p)    ((p).x) << " " << ((p).y)

std::string FigureRenderer::polylinePath(const std::vector<cFigure::Point>& points, bool smooth)
{
    if (points.size() < 2)
        return std::string("M 0 0");  // empty path causes error (item will not be be created)

    std::stringstream ss;

    if (points.size() == 2) {
        const cFigure::Point& from = points[0];
        const cFigure::Point& to = points[1];
        ss << "M " << PT(from) << " L " << PT(to);
    }
    else {
        const cFigure::Point& start = points[0];
        ss << "M " << PT(start);

        if (smooth) {
            for (int i = 2; i < (int)points.size(); i++) {
                const cFigure::Point& control = points[i-1];
                bool isLast = (i == (int)points.size()-1);
                cFigure::Point to = isLast ? points[i] : (points[i-1] + points[i]) * 0.5;
                ss << " Q " << PT(control) << " " << PT(to);
            }
        }
        else {
            for (int i = 1; i < (int)points.size(); i++) {
                const cFigure::Point& p = points[i];
                ss << " L " << PT(p);
            }
        }
    }
    return ss.str();
}

std::string FigureRenderer::polygonPath(const std::vector<cFigure::Point>& points, bool smooth)
{
    if (points.size() < 2)
        return std::string("M 0 0");  // empty path causes error (item will not be be created)

    std::stringstream ss;

    if (points.size() == 2) {
        const cFigure::Point& from = points[0];
        const cFigure::Point& to = points[1];
        ss << "M " << PT(from) << " L " << PT(to);
    }
    else {
        if (smooth) {
            cFigure::Point start = (points[0] + points[1]) * 0.5;
            ss << "M " << PT(start);

            for (int i = 0; i < (int)points.size(); i++) {
                int i1 = (i + 1) % points.size();
                int i2 = (i + 2) % points.size();

                cFigure::Point control = points[i1];
                cFigure::Point to = (points[i1] + points[i2]) * 0.5;

                ss << " Q " << PT(control) << " " << PT(to);
            }
        }
        else {
            const cFigure::Point& start = points[0];
            ss << "M " << PT(start);

            for (auto p : points) {
                ss << " L " << PT(p);
            }
        }
    }

    ss << " Z";
    return ss.str();
}

std::string FigureRenderer::arcPath(const cFigure::Rectangle& rect, double start_rad, double end_rad)
{
    cFigure::Point center = rect.getCenter();
    cFigure::Point radii = rect.getSize() * 0.5;

    cFigure::Point from = center;
    from.x += cos(start_rad) * radii.x;
    from.y -= sin(start_rad) * radii.y;

    cFigure::Point to = center;
    to.x += cos(end_rad) * radii.x;
    to.y -= sin(end_rad) * radii.y;

    double d_rad = end_rad - start_rad;
    while (d_rad < 0)
        d_rad += 2 * M_PI;
    while (d_rad >= 2*M_PI)
        d_rad -= 2 * M_PI;
    bool largeArc = d_rad > M_PI;

    std::stringstream ss;

    ss << "M " << PT(from)
       << " A " << PT(radii) // <- rx ry
       << " 0 " // <- phi
       << (largeArc ? "1" : "0")
       << " 0 " // <- sweep (CCW)
       << PT(to);

    return ss.str();
}

std::string FigureRenderer::pieSlicePath(const cFigure::Rectangle& rect, double start_rad, double end_rad)
{
    cFigure::Point center = rect.getCenter();
    cFigure::Point radii = rect.getSize() * 0.5;

    cFigure::Point from = center;
    from.x += cos(start_rad) * radii.x;
    from.y -= sin(start_rad) * radii.y;

    cFigure::Point to = center;
    to.x += cos(end_rad) * radii.x;
    to.y -= sin(end_rad) * radii.y;

    double d_rad = end_rad - start_rad;
    while (d_rad < 0)
        d_rad += 2 * M_PI;
    while (d_rad >= 2*M_PI)
        d_rad -= 2 * M_PI;
    bool largeArc = d_rad > M_PI;

    std::stringstream ss;

    ss << "M " << PT(center)
       << " L " << PT(from)
       << " A " << PT(radii) // <- rx ry
       << " 0 " // <- phi
       << (largeArc ? "1" : "0")
       << " 0 " // <- sweep (CCW)
       << PT(to) << " Z";

    return ss.str();
}

std::string FigureRenderer::ringPath(const cFigure::Rectangle& rect, double innerRx, double innerRy)
{
    cFigure::Point center = rect.getCenter();
    double outerRx = rect.getSize().x * 0.5;
    double outerRy = rect.getSize().y * 0.5;

    std::stringstream ss;

    // drawing the outer ellipse with a 270° and a 90° arc
    // (to avoid undefined behavior, also 2*180° didn't work on windows)
    ss << "M " << center.x + outerRx << " " << center.y
       << " A " << outerRx << " " << outerRy // <- rx ry
       << " 0 1 0 " // <- phi, largeArc, sweep (CCW)
       << center.x << " " <<  center.y + outerRy
       << " A " << outerRx << " " << outerRy // <- rx ry
       << " 0 0 0 " // <- phi, largeArc, sweep (CCW)
       << center.x + outerRx << " " <<  center.y << " Z ";

    // then drawing the inner ellipse with another two, if applicable
    if ((innerRx) > 0 && (innerRy > 0)) {  // avoiding undefined behaviour again
        ss << "M " << center.x + innerRx << " " << center.y
           << " A " << innerRx << " " << innerRy // <- rx ry
           << " 0 1 0 " // <- phi, largeArc, sweep (CCW)
           << center.x << " " <<  center.y + innerRy
           << " A " << innerRx << " " << innerRy // <- rx ry
           << " 0 0 0 " // <- phi, largeArc, sweep (CCW)
           << center.x + innerRx << " " <<  center.y << " Z";
    }

    return ss.str();
}

#undef PT

const char *FigureRenderer::resolveIcon(Tcl_Interp *interp, const char *iconName, int& outWidth, int& outHeight)
{
    // TODO result should be precomputed or cached
    const char *imageName = Tcl_GetVar2(interp, "bitmaps", TCLCONST(iconName), TCL_GLOBAL_ONLY);
    if (!imageName)
        imageName = Tcl_GetVar2(interp, "icons", "unknown", TCL_GLOBAL_ONLY);
    ASSERT(imageName);

    CHK(Tcl_VarEval(interp, "image width ", imageName, TCL_NULL));
    outWidth = opp_atol(Tcl_GetStringResult(interp));

    CHK(Tcl_VarEval(interp, "image height ", imageName, TCL_NULL));
    outHeight = opp_atol(Tcl_GetStringResult(interp));

    return imageName;
}

void FigureRenderer::moveItemsAbove(Tcl_Interp *interp, Tcl_CmdInfo *cmd, const char *tag, const char *referenceTag)
{
    int argc = 0;
    const char *argv[6];
    argv[argc++] = "dummy";
    argv[argc++] = "raise";
    argv[argc++] = tag;
    argv[argc++] = referenceTag;
    invokeTclCommand(interp, cmd, argc, argv);
}

void FigureRenderer::removeItems(Tcl_Interp *interp, Tcl_CmdInfo *cmd, const char *tag)
{
    int argc = 0;
    const char *argv[6];
    argv[argc++] = "dummy";
    argv[argc++] = "delete";
    argv[argc++] = "withtag";
    argv[argc++] = tag;
    invokeTclCommand(interp, cmd, argc, argv);
}

FigureRenderer *FigureRenderer::getRendererFor(cFigure *figure)
{
    FigureRenderer *renderer;
    std::string rendererClassName = figure->getRendererClassName();
    std::map<std::string, FigureRenderer *>::iterator it = rendererCache.find(rendererClassName);
    if (it != rendererCache.end())
        renderer = it->second;
    else {
        // create renderer and add to the cache
        if (rendererClassName == "")
            renderer = new NullRenderer();
        else {
            if (rendererClassName.find(':') == std::string::npos)
                rendererClassName = "omnetpp::tkenv::" + rendererClassName;
            cObjectFactory *factory = cObjectFactory::find(rendererClassName.c_str());
            if (!factory)
                throw cRuntimeError("No renderer class '%s' for figure class '%s'", rendererClassName.c_str(), figure->getClassName());
            cObject *obj = factory->createOne();
            renderer = dynamic_cast<FigureRenderer *>(obj);
            if (!renderer)
                throw cRuntimeError("Wrong figure renderer class: Cannot cast %s to FigureRenderer", obj->getClassName());
        }
        rendererCache[rendererClassName] = renderer;
    }
    return renderer;
}

//----

void AbstractCanvasItemFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    initBufs();
    char tags[40];
    sprintf(tags, "fig f%d ", figure->getId());
    ptrToStr(figure, tags + strlen(tags));

    int argc = 0;
    const char *argv[MAXARGC];
    argv[argc++] = "dummy";
    argv[argc++] = "create";
    argv[argc++] = getItemType();
    std::string coords = getCoords(figure, interp, transform, hints);
    argv[argc++] = coords.c_str();
    addMatrix(figure, transform, argc, argv);
    addOptions(figure, ~0, interp, argc, argv, transform, hints);
    argv[argc++] = "-tags";
    argv[argc++] = tags;
    Assert(argc <= MAXARGC);
    invokeTclCommand(interp, cmd, argc, argv);
}

void AbstractCanvasItemFigureRenderer::refresh(cFigure *figure, int8_t what, Tcl_Interp *interp, Tcl_CmdInfo *cmd, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());

    if (what & (cFigure::CHANGE_GEOMETRY | cFigure::CHANGE_TRANSFORM)) {  // some getCoords() implementations use the transform as input
        initBufs();
        int argc = 0;
        const char *argv[MAXARGC];
        argv[argc++] = "dummy";
        argv[argc++] = "coords";
        argv[argc++] = tag;
        std::string coords = getCoords(figure, interp, transform, hints);
        argv[argc++] = coords.c_str();
        Assert(argc <= MAXARGC);
        invokeTclCommand(interp, cmd, argc, argv);
    }

    initBufs();
    int argc = 0;
    const char *argv[MAXARGC];
    argv[argc++] = "dummy";
    argv[argc++] = "itemconfig";
    argv[argc++] = tag;
    int savedArgc = argc;
    if (what & (cFigure::CHANGE_TRANSFORM | cFigure::CHANGE_GEOMETRY)) // HACK: CHANGE_GEOMETRY is in there because path figure offset is a geometry change, but is implemented with transform
        addMatrix(figure, transform, argc, argv);
    addOptions(figure, what, interp, argc, argv, transform, hints);
    Assert(argc <= MAXARGC);
    if (argc > savedArgc)
        invokeTclCommand(interp, cmd, argc, argv);
}

void AbstractCanvasItemFigureRenderer::remove(cFigure *figure, Tcl_Interp *interp, Tcl_CmdInfo *cmd)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());
    removeItems(interp, cmd, tag);
}

void AbstractCanvasItemFigureRenderer::addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[])
{
    argv[argc++] = "-matrix";
    argv[argc++] = matrix(transform);
}

//----

void AbstractLineFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    if (what & cFigure::CHANGE_VISUAL) {
        cAbstractLineFigure *lineFigure = check_and_cast<cAbstractLineFigure *>(figure);
        argv[argc++] = "-stroke";
        argv[argc++] = color(lineFigure->getLineColor());
        argv[argc++] = "-strokewidth";
        argv[argc++] = lineFigure->getZoomLineWidth() ? dtoa(lineFigure->getLineWidth()) : dtoa(lineFigure->getLineWidth() / hints->zoom);
        argv[argc++] = "-strokeopacity";
        argv[argc++] = dtoa(lineFigure->getLineOpacity());
        lineStyle(lineFigure->getLineStyle(), argc, argv);
        capStyle(lineFigure->getCapStyle(), argc, argv);
        arrowHead(true, lineFigure->getStartArrowhead(), lineFigure->getLineWidth(), hints->zoom, argc, argv);
        arrowHead(false, lineFigure->getEndArrowhead(), lineFigure->getLineWidth(), hints->zoom, argc, argv);
    }
}

//----

std::string LineFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cLineFigure *lineFigure = check_and_cast<cLineFigure *>(figure);
    return point2(lineFigure->getStart(), lineFigure->getEnd());
}

//----

std::string ArcFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cArcFigure *arcFigure = check_and_cast<cArcFigure *>(figure);
    return arcPath(arcFigure->getBounds(), arcFigure->getStartAngle(), arcFigure->getEndAngle());
}

//----

std::string PolylineFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cPolylineFigure *polylineFigure = check_and_cast<cPolylineFigure *>(figure);
    return polylinePath(polylineFigure->getPoints(), polylineFigure->getSmooth());
}

void PolylineFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractLineFigureRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_VISUAL) {
        cPolylineFigure *lineFigure = check_and_cast<cPolylineFigure *>(figure);
        joinStyle(lineFigure->getJoinStyle(), argc, argv);
    }
}

//----

void AbstractShapeRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cAbstractShapeFigure *shapeFigure = check_and_cast<cAbstractShapeFigure *>(figure);
    if (what & cFigure::CHANGE_VISUAL) {
        if (shapeFigure->isOutlined()) {
            argv[argc++] = "-stroke";
            argv[argc++] = color(shapeFigure->getLineColor());
            argv[argc++] = "-strokewidth";
            argv[argc++] = shapeFigure->getZoomLineWidth() ? dtoa(shapeFigure->getLineWidth()) : dtoa(shapeFigure->getLineWidth() / hints->zoom);
            argv[argc++] = "-strokeopacity";
            argv[argc++] = dtoa(shapeFigure->getLineOpacity());
            lineStyle(shapeFigure->getLineStyle(), argc, argv);
        }
        else {
            argv[argc++] = "-stroke";
            argv[argc++] = "";
        }
        if (shapeFigure->isFilled()) {
            argv[argc++] = "-fill";
            argv[argc++] = color(shapeFigure->getFillColor());
            argv[argc++] = "-fillopacity";
            argv[argc++] = dtoa(shapeFigure->getFillOpacity());
        }
        else {
            argv[argc++] = "-fill";
            argv[argc++] = "";
        }
    }
}

//----

std::string RectangleFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cRectangleFigure *rectangleFigure = check_and_cast<cRectangleFigure *>(figure);
    return bounds(rectangleFigure->getBounds());
}

void RectangleFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_GEOMETRY) {
        cRectangleFigure *rectangleFigure = check_and_cast<cRectangleFigure *>(figure);
        argv[argc++] = "-rx";
        argv[argc++] = dtoa(rectangleFigure->getCornerRx());
        argv[argc++] = "-ry";
        argv[argc++] = dtoa(rectangleFigure->getCornerRy());
    }
}

//----

std::string OvalFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cOvalFigure *ovalFigure = check_and_cast<cOvalFigure *>(figure);
    return point(ovalFigure->getBounds().getCenter());
}

void OvalFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_GEOMETRY) {
        cOvalFigure *ovalFigure = check_and_cast<cOvalFigure *>(figure);
        cFigure::Point size = ovalFigure->getBounds().getSize();
        argv[argc++] = "-rx";
        argv[argc++] = dtoa(size.x * 0.5);
        argv[argc++] = "-ry";
        argv[argc++] = dtoa(size.y * 0.5);
    }
}

//----

std::string RingFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cRingFigure *ringFigure = check_and_cast<cRingFigure *>(figure);
    return ringPath(ringFigure->getBounds(), ringFigure->getInnerRx(), ringFigure->getInnerRy());
}

void RingFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_VISUAL) {
        // cRingFigure *ringFigure = check_and_cast<cRingFigure*>(figure);
        argv[argc++] = "-fillrule";
        argv[argc++] = "evenodd";
    }
}

//----

std::string PieSliceFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cPieSliceFigure *pieSliceFigure = check_and_cast<cPieSliceFigure *>(figure);
    return pieSlicePath(pieSliceFigure->getBounds(), pieSliceFigure->getStartAngle(), pieSliceFigure->getEndAngle());
}

void PieSliceFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);
}

//----

std::string PolygonFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cPolygonFigure *polygonFigure = check_and_cast<cPolygonFigure *>(figure);
    return polygonPath(polygonFigure->getPoints(), polygonFigure->getSmooth());
}

void PolygonFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_VISUAL) {
        cPolygonFigure *polygonFigure = check_and_cast<cPolygonFigure *>(figure);
        joinStyle(polygonFigure->getJoinStyle(), argc, argv);
        fillRule(polygonFigure->getFillRule(), argc, argv);
    }
}

//----

std::string PathFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cPathFigure *pathFigure = check_and_cast<cPathFigure *>(figure);
    std::string path = pathFigure->getPath();
    return path.empty() ? "M 0 0" : path;  // empty path causes error (item will not be be created)
}

void PathFigureRenderer::addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[])
{
    // modify transform with offset
    cPathFigure *pathFigure = check_and_cast<cPathFigure *>(figure);
    cFigure::Point offset = pathFigure->getOffset();

    cFigure::Transform tmp = cFigure::Transform().translate(offset.x, offset.y).multiply(transform);

    AbstractShapeRenderer::addMatrix(figure, tmp, argc, argv);
}

void PathFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractShapeRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    if (what & cFigure::CHANGE_VISUAL) {
        cPathFigure *pathFigure = check_and_cast<cPathFigure *>(figure);
        joinStyle(pathFigure->getJoinStyle(), argc, argv);
        capStyle(pathFigure->getCapStyle(), argc, argv);
        fillRule(pathFigure->getFillRule(), argc, argv);
    }
}

//----

std::string AbstractTextFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = check_and_cast<cAbstractTextFigure *>(figure);
    return point(textFigure->getPosition());
}

void AbstractTextFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cAbstractTextFigure *textFigure = check_and_cast<cAbstractTextFigure *>(figure);
    if (what & cFigure::CHANGE_INPUTDATA) {
        argv[argc++] = "-text";
        argv[argc++] = textFigure->getText();
    }
    if (what & cFigure::CHANGE_GEOMETRY) {
        textanchor(textFigure->getAnchor(), argc, argv);
    }
    if (what & cFigure::CHANGE_VISUAL) {
        font(textFigure->getFont(), hints, argc, argv);
        argv[argc++] = "-fill";
        argv[argc++] = color(textFigure->getColor());
        argv[argc++] = "-fillopacity";
        argv[argc++] = dtoa(textFigure->getOpacity());
    }
}

//----

void LabelFigureRenderer::addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[])
{
    // no matrix -- label should not be affected by transforms
}

std::string LabelFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cLabelFigure *labelFigure = check_and_cast<cLabelFigure *>(figure);
    return point(transform.applyTo(labelFigure->getPosition()));
}

//----

std::string AbstractImageFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cAbstractImageFigure *imageFigure = check_and_cast<cAbstractImageFigure *>(figure);
    return point(imageFigure->getPosition());
}

void AbstractImageFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cAbstractImageFigure *imageFigure = check_and_cast<cAbstractImageFigure *>(figure);
    if (what & cFigure::CHANGE_GEOMETRY) {
        anchor(imageFigure->getAnchor(), argc, argv);
        argv[argc++] = "-width";
        argv[argc++] = dtoa(imageFigure->getWidth());
        argv[argc++] = "-height";
        argv[argc++] = dtoa(imageFigure->getHeight());
    }
    if (what & cFigure::CHANGE_VISUAL) {
        interpolation(imageFigure->getInterpolation(), argc, argv);
        argv[argc++] = "-fillopacity";
        argv[argc++] = dtoa(imageFigure->getOpacity());
        argv[argc++] = "-tintcolor";
        argv[argc++] = color(imageFigure->getTintColor());
        argv[argc++] = "-tintamount";
        argv[argc++] = dtoa(imageFigure->getTintAmount());
    }
}

//----

void ImageFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractImageFigureRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    cImageFigure *imageFigure = check_and_cast<cImageFigure *>(figure);
    if (what & cFigure::CHANGE_INPUTDATA) {
        const char *imageName = Tcl_GetVar2(interp, "bitmaps", TCLCONST(imageFigure->getImageName()), TCL_GLOBAL_ONLY);
        if (!imageName)
            imageName = Tcl_GetVar2(interp, "icons", "unknown", TCL_GLOBAL_ONLY);
        ASSERT(imageName);
        argv[argc++] = "-image";
        argv[argc++] = imageName;
    }
}

//----

void IconFigureRenderer::addMatrix(cFigure *figure, const cFigure::Transform& transform, int& argc, const char *argv[])
{
    // no matrix -- icon should not be affected by transforms
}

std::string IconFigureRenderer::getCoords(cFigure *figure, Tcl_Interp *interp, const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    cIconFigure *iconFigure = check_and_cast<cIconFigure *>(figure);
    return point(transform.applyTo(iconFigure->getPosition()));
}

//----

void PixmapFigureRenderer::addOptions(cFigure *figure, int8_t what, Tcl_Interp *interp, int& argc, const char *argv[], const cFigure::Transform& transform, FigureRenderingHints *hints)
{
    AbstractImageFigureRenderer::addOptions(figure, what, interp, argc, argv, transform, hints);

    cPixmapFigure *pixmapFigure = check_and_cast<cPixmapFigure *>(figure);
    if (what & cFigure::CHANGE_INPUTDATA) {
        char *imageName = getBuf(32);
        sprintf(imageName, "pixmap%d", figure->getId());
        const cFigure::Pixmap& pixmap = pixmapFigure->getPixmap();
        ensureImage(interp, imageName, pixmap);
        argv[argc++] = "-image";
        argv[argc++] = imageName;
    }
}

void PixmapFigureRenderer::ensureImage(Tcl_Interp *interp, const char *imageName, const cFigure::Pixmap& pixmap)
{
    Tk_PhotoHandle handle = Tk_FindPhoto(interp, TCLCONST(imageName));
    if (!handle) {
        char cmd[80];
        sprintf(cmd, "image create photo %s -width %d -height %d", imageName, pixmap.getWidth(), pixmap.getHeight());
        CHK(Tcl_Eval(interp, cmd));
        handle = Tk_FindPhoto(interp, TCLCONST(imageName));
        if (!handle)
            return;
    }

    Tk_PhotoImageBlock imageBlock;
    Tk_PhotoGetImage(handle, &imageBlock);

    if (imageBlock.pixelSize != 4) {
        getTkenv()->logTclError(__FILE__, __LINE__, "PixmapFigureRenderer: unsupported color depth (pixelSize!=4)");
        return;
    }

    if (imageBlock.width != pixmap.getWidth() || imageBlock.height != pixmap.getHeight())
        Tk_PhotoSetSize(interp, handle, pixmap.getWidth(), pixmap.getHeight());

    int width = imageBlock.width;
    int height = imageBlock.height;

    // copy pixels
    int redOffset = imageBlock.offset[0];
    int greenOffset = imageBlock.offset[1];
    int blueOffset = imageBlock.offset[2];
    int alphaOffset = imageBlock.offset[3];
    bool compatible = redOffset == 0 && greenOffset == 1 && blueOffset == 2 && alphaOffset == 3;  // pixel layout is same as cFigure::RGBA
    if (compatible && imageBlock.pitch == 4 * width)
        memcpy(imageBlock.pixelPtr, pixmap.buffer(), width * height * 4);
    else {
        for (int y = 0; y < height; y++) {
            unsigned char *pixel = imageBlock.pixelPtr + y*imageBlock.pitch;
            if (compatible)
                memcpy(pixel, pixmap.buffer() + y * width * 4, width * 4);
            else {
                for (int x = 0; x < width; x++, pixel += 4) {
                    cFigure::RGBA px = pixmap.pixel(x, y);
                    pixel[redOffset] = px.red;
                    pixel[greenOffset] = px.green;
                    pixel[blueOffset] = px.blue;
                    pixel[alphaOffset] = px.alpha;
                }
            }
        }
    }
}

}  // namespace tkenv
}  // namespace omnetpp


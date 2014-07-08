//==========================================================================
//  FIGURERENDERERS.CC - part of
//
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

#include "tkutil.h"
#include "figurerenderers.h"
#include "cobjectfactory.h"

NAMESPACE_BEGIN

std::map<std::string, FigureRenderer*> FigureRenderer::rendererCache;

Register_Class(NullRenderer);
Register_Class(LineFigureRenderer);
Register_Class(ArcFigureRenderer);
Register_Class(PolylineFigureRenderer);
Register_Class(RectangleFigureRenderer);
Register_Class(OvalFigureRenderer);
Register_Class(PieSliceFigureRenderer);
Register_Class(PolygonFigureRenderer);
Register_Class(TextFigureRenderer);
Register_Class(ImageFigureRenderer);


char *FigureRenderer::point(const cFigure::Point& point, ICoordMapping *mapping, char *buf)
{
    cFigure::Point p = mapping->apply(point);
    sprintf(buf, " %g %g ", p.x, p.y);
    return buf;
}

std::string FigureRenderer::points(const std::vector<cFigure::Point>& points, ICoordMapping *mapping)
{
    std::stringstream os;
    for (int i=0; i<(int)points.size(); i++) {
        cFigure::Point p = mapping->apply(points[i]);
        os << p.x << " " << p.y << " ";
    }
    return os.str();
}

char *FigureRenderer::bounds(const cFigure::Rectangle& bounds, ICoordMapping *mapping, char *buf)
{
    cFigure::Point p1 = mapping->apply(cFigure::Point(bounds.x, bounds.y));
    cFigure::Point p2 = mapping->apply(cFigure::Point(bounds.x+bounds.width, bounds.y+bounds.height));
    sprintf(buf, " %g %g %g %g", p1.x, p1.y, p2.x, p2.y);
    return buf;
}

char *FigureRenderer::color(const cFigure::Color& color, char *buf)
{
    sprintf(buf, "#%2.2x%2.2x%2.2x ", color.red, color.green, color.blue);
    return buf;
}

char *FigureRenderer::itoa(int i, char *buf)
{
    sprintf(buf, "%d ", i);
    return buf;
}

char *FigureRenderer::arrows(cFigure::ArrowHead start, cFigure::ArrowHead end, char *buf)
{
    //Note: Tk cannot draw different arrows at the two ends of the line!
    cFigure::ArrowHead type;
    if (start == cFigure::ARROW_NONE && end == cFigure::ARROW_NONE) {
        buf[0] = 0;
        return buf;
    }

    if (start == cFigure::ARROW_NONE) {
        sprintf(buf, " -arrow last ");
        type = end;
    }
    else if (end == cFigure::ARROW_NONE) {
        sprintf(buf, " -arrow first ");
        type = start;
    }
    else {
        sprintf(buf, " -arrow both ");
        type = end;
    }
    switch (type) {
        case cFigure::ARROW_SIMPLE:   strcat(buf, " -arrowshape {6 14 5} "); break;
        case cFigure::ARROW_BARBED:   strcat(buf, " -arrowshape {10 14 5} "); break;
        case cFigure::ARROW_TRIANGLE: strcat(buf, " -arrowshape {11 11 4} "); break;
        default: throw cRuntimeError("Unexpected arrowhead type %d", type);
    }
    return buf;
}

const char *FigureRenderer::smooth(bool smooth)
{
    return smooth ? " -smooth true " : "";
}

const char *FigureRenderer::lineStyle(cFigure::LineStyle style)
{
    switch (style) {
        case cFigure::LINE_SOLID: return "";
        case cFigure::LINE_DOTTED: return " -dash . ";
        case cFigure::LINE_DASHED: return " -dash - ";
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

const char *FigureRenderer::capStyle(cFigure::CapStyle style)
{
    switch (style) {
        case cFigure::CAP_BUTT: return "";
        case cFigure::CAP_SQUARE: return " -capstyle projecting ";
        case cFigure::CAP_ROUND: return " -capstyle round ";
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

const char *FigureRenderer::joinStyle(cFigure::JoinStyle style)
{
    switch (style) {
        case cFigure::JOIN_MITER: return "";
        case cFigure::JOIN_BEVEL: return " -joinstyle bevel ";
        case cFigure::JOIN_ROUND: return " -joinstyle round ";
        default: throw cRuntimeError("Unexpected line style %d", style);
    }
}

const char *FigureRenderer::anchor(cFigure::Anchor anchor)
{
    switch (anchor) {
        case cFigure::ANCHOR_CENTER: return " -anchor c ";
        case cFigure::ANCHOR_N: return " -anchor n ";
        case cFigure::ANCHOR_E: return " -anchor e ";
        case cFigure::ANCHOR_S: return " -anchor s ";
        case cFigure::ANCHOR_W: return " -anchor w ";
        case cFigure::ANCHOR_NW: return " -anchor nw ";
        case cFigure::ANCHOR_NE: return " -anchor ne ";
        case cFigure::ANCHOR_SE: return " -anchor se ";
        case cFigure::ANCHOR_SW: return " -anchor sw ";
        default: throw cRuntimeError("Unexpected anchor %d", anchor);
    }
}

const char *FigureRenderer::alignment(cFigure::Alignment alignment)
{
    switch (alignment) {
        case cFigure::ALIGN_LEFT: return "";
        case cFigure::ALIGN_RIGHT: return " -justify right ";
        case cFigure::ALIGN_CENTER: return " -justify center ";
        default: throw cRuntimeError("Unexpected alignment %d", alignment);
    }
}

std::string FigureRenderer::font(const cFigure::Font& font)
{
    if (font.typeface == "" && font.pointSize <= 0 && font.style == 0)
        return "";
    std::stringstream os;
    os << " -font {{" << font.typeface << "} " << (font.pointSize <= 0 ? 8 : font.pointSize);
    if (font.style & cFigure::FONT_BOLD) os << " bold";
    if (font.style & cFigure::FONT_ITALIC) os << " italic";
    if (font.style & cFigure::FONT_UNDERLINE) os << " underline";
    os << "} ";
    return os.str();
}

void FigureRenderer::remove(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());
    CHK(Tcl_VarEval(interp, canvas, " delete ", tag, NULL));
}

FigureRenderer *FigureRenderer::getRendererFor(cFigure *figure)
{
    FigureRenderer *renderer;
    std::string className = figure->getClassNameForRenderer();
    std::map<std::string, FigureRenderer*>::iterator it = rendererCache.find(className);
    if (it != rendererCache.end())
        renderer = it->second;
    else {
        // create renderer and add to the cache
        if (className == "")
            renderer = new NullRenderer();
        else {
            // find registered class named "<type>Renderer"
            std::string rendererClassName = className + "Renderer";
            if (rendererClassName[0] == 'c')
                rendererClassName = rendererClassName.substr(1);
            cObjectFactory *factory = cObjectFactory::find(rendererClassName.c_str());
            if (!factory)
                throw cRuntimeError("No renderer class '%s' for figure class '%s'", rendererClassName.c_str(), figure->getClassName());
            cObject *obj = factory->createOne();
            renderer = dynamic_cast<FigureRenderer*>(obj);
            if (!renderer)
                throw cRuntimeError("Wrong figure renderer class: cannot cast %s to FigureRenderer", obj->getClassName());
        }
        rendererCache[className] = renderer;
    }
    return renderer;
}

void NullRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());
    CHK(Tcl_VarEval(interp, canvas, " create line 0 0 0 0 -fill \"\" -tags {fig ", tag, "}", NULL)); // layer marker for raise/lower commands
}

void NullRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
}

void NullRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
}

void LineFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32], buf2[32], buf3[32], buf4[32], buf5[64];
    sprintf(tag, "f%d", figure->getId());
    cLineFigure *lineFigure = check_and_cast<cLineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create line ",
            point(lineFigure->getStart(), mapping, buf1), point(lineFigure->getEnd(), mapping, buf2),
            " -fill ", color(lineFigure->getLineColor(), buf3),
            " -width ", itoa(round(lineFigure->getLineWidth()), buf4),
            lineStyle(lineFigure->getLineStyle()),
            capStyle(lineFigure->getCapStyle()),
            arrows(lineFigure->getStartArrowHead(), lineFigure->getEndArrowHead(), buf5),
            " -tags {fig ", tag, "}", NULL));
}

void LineFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32], buf2[32];
    sprintf(tag, "f%d", figure->getId());
    cLineFigure *lineFigure = check_and_cast<cLineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ",
            point(lineFigure->getStart(), mapping, buf1), point(lineFigure->getEnd(), mapping, buf2), NULL));
}

void LineFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf3[32], buf4[32], buf5[64];
    sprintf(tag, "f%d", figure->getId());
    cLineFigure *lineFigure = check_and_cast<cLineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -fill ", color(lineFigure->getLineColor(), buf3),
            " -width ", itoa(round(lineFigure->getLineWidth()), buf4),
            lineStyle(lineFigure->getLineStyle()),
            capStyle(lineFigure->getCapStyle()),
            arrows(lineFigure->getStartArrowHead(), lineFigure->getEndArrowHead(), buf5),
            NULL));
}

void ArcFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    //TODO arrowheads, capstyle
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80], buf3[32], buf4[32], buf5[32], buf6[32];
    sprintf(tag, "f%d", figure->getId());
    cArcFigure *arcFigure = check_and_cast<cArcFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create arc ",
            bounds(arcFigure->getBounds(), mapping, buf1),
            " -start ", itoa(arcFigure->getStartAngle(), buf3),
            " -extent ", itoa(arcFigure->getEndAngle() - arcFigure->getStartAngle(), buf4),
            " -style arc",
            " -fill ", color(arcFigure->getLineColor(), buf5),
            " -width ", itoa(round(arcFigure->getLineWidth()), buf6),
            lineStyle(arcFigure->getLineStyle()),
            " -tags {fig ", tag, "}", NULL));
}

void ArcFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80];
    sprintf(tag, "f%d", figure->getId());
    cArcFigure *arcFigure = check_and_cast<cArcFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            bounds(arcFigure->getBounds(), mapping, buf1), NULL));
}

void ArcFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    //TODO arrowheads, capstyle
    char tag[32], buf3[32], buf4[32], buf5[32], buf6[32];
    sprintf(tag, "f%d", figure->getId());
    cArcFigure *arcFigure = check_and_cast<cArcFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -start ", itoa(arcFigure->getStartAngle(), buf3),
            " -extent ", itoa(arcFigure->getEndAngle() - arcFigure->getStartAngle(), buf4),
            " -style arc",
            " -fill ", color(arcFigure->getLineColor(), buf5),
            " -width ", itoa(round(arcFigure->getLineWidth()), buf6),
            lineStyle(arcFigure->getLineStyle()),
            NULL));
}

void PolylineFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32], buf2[32], buf3[64];
    sprintf(tag, "f%d", figure->getId());
    cPolylineFigure *polylineFigure = check_and_cast<cPolylineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create line ",
            points(polylineFigure->getPoints(), mapping).c_str(),
            " -fill ", color(polylineFigure->getLineColor(), buf1),
            " -width ", itoa(round(polylineFigure->getLineWidth()), buf2),
            lineStyle(polylineFigure->getLineStyle()),
            capStyle(polylineFigure->getCapStyle()),
            joinStyle(polylineFigure->getJoinStyle()),
            smooth(polylineFigure->getSmooth()),
            arrows(polylineFigure->getStartArrowHead(), polylineFigure->getEndArrowHead(), buf3),
            " -tags {fig ", tag, "}", NULL));
}

void PolylineFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());
    cPolylineFigure *polylineFigure = check_and_cast<cPolylineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            points(polylineFigure->getPoints(), mapping).c_str(), NULL));
}

void PolylineFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf2[32], buf3[32], buf4[64];
    sprintf(tag, "f%d", figure->getId());
    cPolylineFigure *polylineFigure = check_and_cast<cPolylineFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -fill ", color(polylineFigure->getLineColor(), buf2),
            " -width ", itoa(round(polylineFigure->getLineWidth()), buf3),
            lineStyle(polylineFigure->getLineStyle()),
            capStyle(polylineFigure->getCapStyle()),
            joinStyle(polylineFigure->getJoinStyle()),
            smooth(polylineFigure->getSmooth()),
            arrows(polylineFigure->getStartArrowHead(), polylineFigure->getEndArrowHead(), buf4),
            NULL));
}

void RectangleFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80], buf3[32], buf4[32], buf5[32];
    sprintf(tag, "f%d", figure->getId());
    cRectangleFigure *rectangleFigure = check_and_cast<cRectangleFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create rect ",
            bounds(rectangleFigure->getBounds(), mapping, buf1),
            " -fill ", (rectangleFigure->isFilled() ? color(rectangleFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", (rectangleFigure->isOutlined() ? color(rectangleFigure->getLineColor(), buf4) : "\"\""),
            " -width ", itoa(round(rectangleFigure->getLineWidth()), buf5),
            lineStyle(rectangleFigure->getLineStyle()),
            " -tags {fig ", tag, "}", NULL));
}

void RectangleFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80];
    sprintf(tag, "f%d", figure->getId());
    cRectangleFigure *rectangleFigure = check_and_cast<cRectangleFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            bounds(rectangleFigure->getBounds(), mapping, buf1), NULL));
}

void RectangleFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf1[32], buf2[32], buf3[32];
    sprintf(tag, "f%d", figure->getId());
    cRectangleFigure *rectangleFigure = check_and_cast<cRectangleFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -fill ", (rectangleFigure->isFilled() ? color(rectangleFigure->getFillColor(), buf1) : "\"\""),
            " -outline ", (rectangleFigure->isOutlined() ? color(rectangleFigure->getLineColor(), buf2) : "\"\""),
            " -width ", itoa(round(rectangleFigure->getLineWidth()), buf3),
            lineStyle(rectangleFigure->getLineStyle()),
            NULL));
}

void OvalFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80], buf3[32], buf4[32], buf5[32];
    sprintf(tag, "f%d", figure->getId());
    cOvalFigure *ovalFigure = check_and_cast<cOvalFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create oval ",
            bounds(ovalFigure->getBounds(), mapping, buf1),
            " -fill ", (ovalFigure->isFilled() ? color(ovalFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", (ovalFigure->isOutlined() ? color(ovalFigure->getLineColor(), buf4) : "\"\""),
            " -width ", itoa(round(ovalFigure->getLineWidth()), buf5),
            lineStyle(ovalFigure->getLineStyle()),
            " -tags {fig ", tag, "}", NULL));
}

void OvalFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80];
    sprintf(tag, "f%d", figure->getId());
    cOvalFigure *ovalFigure = check_and_cast<cOvalFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            bounds(ovalFigure->getBounds(), mapping, buf1), NULL));
}

void OvalFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf3[32], buf4[32], buf5[32];
    sprintf(tag, "f%d", figure->getId());
    cOvalFigure *ovalFigure = check_and_cast<cOvalFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -fill ", (ovalFigure->isFilled() ? color(ovalFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", (ovalFigure->isOutlined() ? color(ovalFigure->getLineColor(), buf4) : "\"\""),
            " -width ", itoa(round(ovalFigure->getLineWidth()), buf5),
            lineStyle(ovalFigure->getLineStyle()),
            NULL));
}

void PieSliceFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80], buf3[32], buf4[32], buf5[32], buf6[32], buf7[32];
    sprintf(tag, "f%d", figure->getId());
    cPieSliceFigure *piesliceFigure = check_and_cast<cPieSliceFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create arc ",
            bounds(piesliceFigure->getBounds(), mapping, buf1),
            " -start ", itoa(piesliceFigure->getStartAngle(), buf3),
            " -extent ", itoa(piesliceFigure->getEndAngle() - piesliceFigure->getStartAngle(), buf4),
            " -style pieslice ",
            " -fill ", (piesliceFigure->isFilled() ? color(piesliceFigure->getFillColor(), buf5) : "\"\""),
            " -outline ", (piesliceFigure->isOutlined() ? color(piesliceFigure->getLineColor(), buf6) : "\"\""),
            " -width ", itoa(round(piesliceFigure->getLineWidth()), buf7),
            lineStyle(piesliceFigure->getLineStyle()),
            " -tags {fig ", tag, "}", NULL));
}

void PieSliceFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    ASSERT2(mapping->isLinear(), "nonlinear mapping not supported");
    char tag[32], buf1[80];
    sprintf(tag, "f%d", figure->getId());
    cPieSliceFigure *piesliceFigure = check_and_cast<cPieSliceFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            bounds(piesliceFigure->getBounds(), mapping, buf1), NULL));
}

void PieSliceFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
    sprintf(tag, "f%d", figure->getId());
    cPieSliceFigure *piesliceFigure = check_and_cast<cPieSliceFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -start ", itoa(piesliceFigure->getStartAngle(), buf1),
            " -extent ", itoa(piesliceFigure->getEndAngle() - piesliceFigure->getStartAngle(), buf2),
            " -fill ", (piesliceFigure->isFilled() ? color(piesliceFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", (piesliceFigure->isOutlined() ? color(piesliceFigure->getLineColor(), buf4) : "\"\""),
            " -width ", itoa(round(piesliceFigure->getLineWidth()), buf5),
            lineStyle(piesliceFigure->getLineStyle()),
            NULL));
}

void PolygonFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32], buf2[32], buf3[32];
    sprintf(tag, "f%d", figure->getId());
    cPolygonFigure *polygonFigure = check_and_cast<cPolygonFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create polygon ",
            points(polygonFigure->getPoints(), mapping).c_str(),
            " -fill ", (polygonFigure->isFilled() ? color(polygonFigure->getFillColor(), buf1) : "\"\""),
            " -outline ", (polygonFigure->isOutlined() ? color(polygonFigure->getLineColor(), buf2) : "\"\""),
            " -width ", itoa(round(polygonFigure->getLineWidth()), buf3),
            lineStyle(polygonFigure->getLineStyle()),
            joinStyle(polygonFigure->getJoinStyle()),
            smooth(polygonFigure->getSmooth()),
            " -tags {fig ", tag, "}", NULL));
}

void PolygonFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32];
    sprintf(tag, "f%d", figure->getId());
    cPolygonFigure *polygonFigure = check_and_cast<cPolygonFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            points(polygonFigure->getPoints(), mapping).c_str(), NULL));
}

void PolygonFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf1[32], buf2[32], buf3[32];
    sprintf(tag, "f%d", figure->getId());
    cPolygonFigure *polygonFigure = check_and_cast<cPolygonFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -fill ", (polygonFigure->isFilled() ? color(polygonFigure->getFillColor(), buf1) : "\"\""),
            " -outline ", (polygonFigure->isOutlined() ? color(polygonFigure->getLineColor(), buf2) : "\"\""),
            " -width ", itoa(round(polygonFigure->getLineWidth()), buf3),
            lineStyle(polygonFigure->getLineStyle()),
            joinStyle(polygonFigure->getJoinStyle()),
            smooth(polygonFigure->getSmooth()),
            NULL));
}

void TextFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32], buf2[32];
    sprintf(tag, "f%d", figure->getId());
    cTextFigure *textFigure = check_and_cast<cTextFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " create text ",
            point(textFigure->getLocation(), mapping, buf1),
            " -text ", TclQuotedString(textFigure->getText()).get(),
            " -fill ", color(textFigure->getColor(), buf2),
            anchor(textFigure->getAnchor()),
            alignment(textFigure->getAlignment()),
            font(textFigure->getFont()).c_str(),
            " -tags {fig ", tag, "}", NULL));
}

void TextFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    char tag[32], buf1[32];
    sprintf(tag, "f%d", figure->getId());
    cTextFigure *textFigure = check_and_cast<cTextFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " coords ", tag,
            point(textFigure->getLocation(), mapping, buf1), NULL));
}

void TextFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char tag[32], buf1[32];
    sprintf(tag, "f%d", figure->getId());
    cTextFigure *textFigure = check_and_cast<cTextFigure*>(figure);
    CHK(Tcl_VarEval(interp, canvas, " itemconfig ", tag,
            " -text ", TclQuotedString(textFigure->getText()).get(),
            " -fill ", color(textFigure->getColor(), buf1),
            anchor(textFigure->getAnchor()),
            alignment(textFigure->getAlignment()),
            font(textFigure->getFont()).c_str(),
            NULL));
}

void ImageFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    //TODO
}

void ImageFigureRenderer::refreshGeometry(cFigure *figure, Tcl_Interp *interp, const char *canvas, ICoordMapping *mapping)
{
    //TODO
}

void ImageFigureRenderer::refreshVisuals(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    //TODO
}

NAMESPACE_END


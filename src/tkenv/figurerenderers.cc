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

NAMESPACE_BEGIN

//TODO obey: zoom, enabled/disabled, zOrder
//TODO figure-ok legyenek a layer child-jai

char *FigureRenderer::point(const cFigure::Point& point, char *buf)
{
    sprintf(buf, " %g %g ", point.x, point.y);
    return buf;
}

std::string FigureRenderer::points(const std::vector<cFigure::Point>& points)
{
    std::stringstream os;
    for (int i=0; i<points.size(); i++)
        os << points[i].x << " " << points[i].y << " ";
    return os.str();
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
    //TODO arrow shape!
    //XXX Tk cannot draw different arrows at the two ends of the line!
    if (start == cFigure::ARROW_NONE && end == cFigure::ARROW_NONE)
        buf[0] = 0;
    else if (start == cFigure::ARROW_NONE)
        sprintf(buf, " -arrow last ");
    else if (end == cFigure::ARROW_NONE)
        sprintf(buf, " -arrow first ");
    else
        sprintf(buf, " -arrow both ");
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

void LineFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32], buf4[32], buf5[64];
    cLineFigure *lineFigure = (cLineFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create line ",
            point(lineFigure->getStart(), buf1), point(lineFigure->getEnd(), buf2),
            " -fill ", color(lineFigure->getLineColor(), buf3),
            " -width ", itoa(lineFigure->getLineWidth(), buf4),
            lineStyle(lineFigure->getLineStyle()),
            capStyle(lineFigure->getCapStyle()),
            arrows(lineFigure->getStartArrowHead(), lineFigure->getEndArrowHead(), buf5),
            " -tags fig ", NULL));
}

void PolylineFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32], buf4[64];
    cPolylineFigure *polylineFigure = (cPolylineFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create line ",
            points(polylineFigure->getPoints()).c_str(),
            " -fill ", color(polylineFigure->getLineColor(), buf2),
            " -width ", itoa(polylineFigure->getLineWidth(), buf3),
            lineStyle(polylineFigure->getLineStyle()),
            capStyle(polylineFigure->getCapStyle()),
            joinStyle(polylineFigure->getJoinStyle()),
            smooth(polylineFigure->getSmooth()),
            arrows(polylineFigure->getStartArrowHead(), polylineFigure->getEndArrowHead(), buf4),
            " -tags fig ", NULL));
}

void RectangleFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
    cRectangleFigure *rectangleFigure = (cRectangleFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create rect ",
            point(rectangleFigure->getP1(), buf1), point(rectangleFigure->getP2(), buf2),
            " -fill ", (rectangleFigure->isFilled() ? color(rectangleFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", color(rectangleFigure->getLineColor(), buf4),
            " -width ", itoa(rectangleFigure->getLineWidth(), buf5),
            lineStyle(rectangleFigure->getLineStyle()),
            " -tags fig ", NULL));
}

void OvalFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
    cOvalFigure *ovalFigure = (cOvalFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create oval ",
            point(ovalFigure->getP1(), buf1), point(ovalFigure->getP2(), buf2),
            " -fill ", (ovalFigure->isFilled() ? color(ovalFigure->getFillColor(), buf3) : "\"\""),
            " -outline ", color(ovalFigure->getLineColor(), buf4),
            " -width ", itoa(ovalFigure->getLineWidth(), buf5),
            lineStyle(ovalFigure->getLineStyle()),
            " -tags fig ", NULL));
}

void PolygonFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32];
    cPolygonFigure *polygonFigure = (cPolygonFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create polygon ",
            points(polygonFigure->getPoints()).c_str(),
            " -fill ", (polygonFigure->isFilled() ? color(polygonFigure->getFillColor(), buf1) : "\"\""),
            " -outline ", color(polygonFigure->getLineColor(), buf2),
            " -width ", itoa(polygonFigure->getLineWidth(), buf3),
            lineStyle(polygonFigure->getLineStyle()),
            joinStyle(polygonFigure->getJoinStyle()),
            smooth(polygonFigure->getSmooth()),
            " -tags fig ", NULL));
}

void TextFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    char buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
    cTextFigure *textFigure = (cTextFigure*)figure;
    CHK(Tcl_VarEval(interp, canvas, " create text ",
            point(textFigure->getPos(), buf1),
            " -text ", TclQuotedString(textFigure->getText()).get(),
            " -fill ", color(textFigure->getColor(), buf3),
            anchor(textFigure->getAnchor()),
            alignment(textFigure->getAlignment()),
            //TODO font
            " -tags fig ", NULL));
}

void ImageFigureRenderer::render(cFigure *figure, Tcl_Interp *interp, const char *canvas)
{
    //TODO
}

NAMESPACE_END


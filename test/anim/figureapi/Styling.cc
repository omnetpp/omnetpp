//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "Styling.h"
#include "Figures.h"

Define_Module(LineStyle);

std::string LineStyle::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure();
    bg->setBounds({10, 80, 270, 120});
    bg->setFilled(true);
    bg->setFillColor("white");
    c->addFigure(bg);

    for (int i = 0; i < numProps; ++i) {
        auto line = new cLineFigure(("line_" + std::to_string(i)).c_str());

        line->setStart({30.0 + i * 30, 100});
        line->setEnd({40.0 + i * 30, 120});

        auto polyline = new cPolylineFigure(("polyline_" + std::to_string(i)).c_str());
        polyline->setPoints({{30.0 + i * 30, 130}, {35.0 + i * 30, 150}, {50.0 + i * 30, 140}});

        auto arc = new cArcFigure(("arc_" + std::to_string(i)).c_str());
        arc->setBounds({30.0 + i * 30, 160, 20, 30});
        arc->setStartAngle(0.75);
        arc->setEndAngle(4);

        bg->addFigure(line);
        bg->addFigure(polyline);
        bg->addFigure(arc);

        lines[3*i] = line;
        lines[3*i+1] = polyline;
        lines[3*i+2] = arc;
    }

    for (auto l : lines) {
        l->setZoomLineWidth(true);
        l->setEndArrowhead(cFigure::ARROW_SIMPLE);
    }

    return "Different types of figures are laid out in rows.\n"
           "In each column, a different property of the figures is cycled between\n"
           "a few appropriate values. The properties changed in the columns are:\n"
           "lineColor, lineWidth, lineOpacity, lineStyle, capStyle, startArrowhead, endArrowhead,\n"
           "and zoomLineWidth, respectively. All changes should be visible, and no visual glitches should occur.";
}

std::string LineStyle::onTick(int tick)
{
    int step = tick % 4; // iterating over 4 values on each prop

    const char *colors[] = {"red", "purple", "orange", "darkgreen"};
    double widths[] = {0.1, 1.0, 3.0, 7.0};
    double alphas[] = {0.0, 0.2, 0.7, 1.0};
    cFigure::LineStyle lineStyles[] = {cFigure::LINE_SOLID, cFigure::LINE_DASHED, cFigure::LINE_SOLID, cFigure::LINE_DOTTED};
    cFigure::CapStyle capStyles[] = {cFigure::CAP_BUTT, cFigure::CAP_ROUND, cFigure::CAP_BUTT, cFigure::CAP_SQUARE};
    cFigure::Arrowhead arrows[] = {cFigure::ARROW_NONE, cFigure::ARROW_SIMPLE, cFigure::ARROW_BARBED, cFigure::ARROW_TRIANGLE};
    bool zooms[] = {false, true, false, true};

    for (int i = 0; i < 3; ++i) {
        lines[i]->setLineColor(colors[step]);
        lines[i+3]->setLineWidth(widths[step]);
        lines[i+6]->setLineOpacity(alphas[step]);
        lines[i+9]->setLineStyle(lineStyles[step]);
        lines[i+12]->setCapStyle(capStyles[step]);
        lines[i+15]->setStartArrowhead(arrows[step]);
        lines[i+18]->setEndArrowhead(arrows[step]);
        lines[i+21]->setZoomLineWidth(zooms[step]);
    }

    return std::string("Current values:")
            + "\t lineColor: " + colors[step]
            + "\t lineWidth: " + std::to_string(widths[step])
            + "\t lineOpacity: "  + std::to_string(alphas[step])
            + "\t lineStyle: " + getEnumString(lineStyles[step])

            + "\ncapStyle: " + getEnumString(capStyles[step])
            + "\t startArrowHead: " + getEnumString(arrows[step])
            + "\t endArrowHead: " + getEnumString(arrows[step])
            + "\t zoomLineWidth: " + (zooms[step] ? "true" : "false");
}


Define_Module(ShapeStyle);

std::string ShapeStyle::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure("bg");
    bg->setBounds({10, 80, 290, 220});
    bg->setFilled(true);
    bg->setFillColor("white");
    c->addFigure(bg);

    for (int i = 0; i < numProps; ++i) {
        auto oval = new cOvalFigure(("oval_" + std::to_string(i)).c_str());
        oval->setBounds({30.0 + i * 30, 100, 20, 10});

        auto path = new cPathFigure(("path_" + std::to_string(i)).c_str());
        path->setPath("M 0 0 L 0 10 Q 5 5 20 10 L 20 0 Z");
        path->setOffset({30.0 + i * 30, 130});

        auto pieslice = new cPieSliceFigure(("pieslice_" + std::to_string(i)).c_str());
        pieslice->setBounds({30.0 + i * 30, 160, 20, 30});
        pieslice->setStartAngle(0.75);
        pieslice->setEndAngle(4.5);

        auto polygon = new cPolygonFigure(("polygon_" + std::to_string(i)).c_str());
        polygon->setPoints({{40.0 + i * 30, 220}, {35.0 + i * 30, 210}, {50.0 + i * 30, 200}});

        auto rectangle = new cRectangleFigure(("rectangle_" + std::to_string(i)).c_str());
        rectangle->setBounds({30.0 + i * 30, 240, 20, 10});

        auto ring = new cRingFigure(("ring_" + std::to_string(i)).c_str());
        ring->setBounds({30.0 + i * 30, 270, 20, 10});


        bg->addFigure(oval);
        bg->addFigure(path);
        bg->addFigure(pieslice);
        bg->addFigure(polygon);
        bg->addFigure(rectangle);
        bg->addFigure(ring);

        shapes[6*i] = oval;
        shapes[6*i+1] = path;
        shapes[6*i+2] = pieslice;
        shapes[6*i+3] = polygon;
        shapes[6*i+4] = rectangle;
        shapes[6*i+5] = ring;
    }

    for (auto s : shapes) {
        s->setZoomLineWidth(true);
        s->setFilled(true);
        s->setFillColor("lightgreen");
    }

    return "Different types of figures are laid out in rows.\n"
           "In each column, a different property of the figures is cycled between\n"
           "a few appropriate values. The properties changed in the columns are:\n"
           "filled, outlined, lineColor, fillColor, lineStyle, lineWidth, lineOpacity, fillOpacity,\n"
           "and zoomLineWidth, respectively. All changes should be visible, and no visual glitches should occur.";
}

std::string ShapeStyle::onTick(int tick)
{
    int step = tick % 4; // iterating over 4 values on each prop

    const char *colors[] = {"red", "purple", "orange", "darkgreen"};
    double widths[] = {0.1, 1.0, 3.0, 7.0};
    double alphas[] = {0.0, 0.2, 0.7, 1.0};
    cFigure::LineStyle lineStyles[] = {cFigure::LINE_SOLID, cFigure::LINE_DASHED, cFigure::LINE_SOLID, cFigure::LINE_DOTTED};
    bool bools[] = {false, true, false, true};

    for (int i = 0; i < 6; ++i) {
        shapes[i]->setFilled(bools[step]);
        shapes[i+6]->setOutlined(bools[step]);
        shapes[i+12]->setLineColor(colors[step]);
        shapes[i+18]->setFillColor(colors[step]);
        shapes[i+24]->setLineStyle(lineStyles[step]);
        shapes[i+30]->setLineWidth(widths[step]);
        shapes[i+36]->setLineOpacity(alphas[step]);
        shapes[i+42]->setFillOpacity(alphas[step]);
        shapes[i+48]->setZoomLineWidth(bools[step]);
    }

    return std::string("Current values:")
            + "\t filled: " + (bools[step] ? "true" : "false")
            + "\t outlined: " + (bools[step] ? "true" : "false")
            + "\t lineColor: " + colors[step]
            + "\t fillColor: " + colors[step]

            + "\nlineStyle: " + getEnumString(lineStyles[step])
            + "\t lineWidth: " + std::to_string(widths[step])
            + "\t lineOpacity: " + std::to_string(alphas[step])
            + "\t fillOpacity: " + std::to_string(alphas[step])
            + "\t zoomLineWidth: " + (bools[step] ? "true" : "false");
}

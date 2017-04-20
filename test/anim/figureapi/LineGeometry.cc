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

#include "LineGeometry.h"
#include "Figures.h"

Define_Module(LineGeometry);

std::string LineGeometry::init()
{
    auto c = getCanvas();

    lines[0] = new cLineFigure("line_0");
    lines[1] = new cLineFigure("line_1");

    lines[0]->setStart({40, 100});
    lines[0]->setEnd({40, 150});
    lines[1]->setStart({80, 100});
    lines[1]->setEnd({80, 150});

    arcs[0] = new cArcFigure("arc_0");
    arcs[1] = new cArcFigure("arc_1");
    arcs[2] = new cArcFigure("arc_2");
    arcs[3] = new cArcFigure("arc_3");

    arcs[0]->setBounds({150,100,20,30});
    arcs[1]->setBounds({180,100,20,30});
    arcs[2]->setBounds({210,100,20,30});
    arcs[3]->setBounds({240,100,20,30});

    arcs[0]->setStartAngle(1);
    arcs[1]->setStartAngle(1);
    arcs[2]->setStartAngle(1);
    arcs[3]->setStartAngle(1);

    arcs[0]->setEndAngle(5);
    arcs[1]->setEndAngle(5);
    arcs[2]->setEndAngle(5);
    arcs[3]->setEndAngle(5);

    polylines[0] = new cPolylineFigure("polyline_0");
    polylines[1] = new cPolylineFigure("polyline_1");
    polylines[2] = new cPolylineFigure("polyline_2");
    polylines[3] = new cPolylineFigure("polyline_3");

    polylines[0]->setPoints({{40, 200}, {50, 230}, {70, 200}});
    polylines[1]->setPoints({{80, 200}, {90, 230}, {110, 200}});
    polylines[2]->setPoints({{120, 200}, {130, 230}, {150, 200}});
    polylines[3]->setPoints({{160, 200}, {170, 230}, {190, 200}});

    auto grid = new GridFigure("grid", "grey");
    grid->translate(0, 50);
    c->addFigure(grid);


    c->addFigure(lines[0]);
    c->addFigure(lines[1]);

    c->addFigure(arcs[0]);
    c->addFigure(arcs[1]);
    c->addFigure(arcs[2]);
    c->addFigure(arcs[3]);

    c->addFigure(polylines[0]);
    c->addFigure(polylines[1]);
    c->addFigure(polylines[2]);
    c->addFigure(polylines[3]);


    auto root = c->getRootFigure();
    for (int i = 0; i < root->getNumFigures(); ++i)
        if (auto l = dynamic_cast<cAbstractLineFigure *>(root->getFigure(i))) {
            l->setLineWidth(2);
            l->setZoomLineWidth(true);
        }


    state->setPosition({20, 50});

    return "This test exercises the non-inherited functions of cLineFigure, cArcFigure, and cPolylineFigure.\n"
           "Namely: setStart and setEnd in cLineFigure; setBounds, setPosition, setStartAndle and setEndAngle in cArcFigure;\n"
           "setPoints, setPoint, setSmooth and setJoinStyle in cPolylineFigure. Each on a different figure, in this order.";
}

std::string LineGeometry::onTick(int tick)
{
    int step = tick % 4;

    cFigure::Point starts[] = {{40,100}, {30,100}, {30,80}, {50, 100}};
    cFigure::Point ends[] = {{80,150}, {70,150}, {70,130}, {90,150}};

    lines[0]->setStart(starts[step]);
    lines[1]->setEnd(ends[step]);


    cFigure::Rectangle bounds[] = {{150,100,20,30}, {160,100,10,30}, {160,80,20,10}, {150,120,20,20}};
    cFigure::Point positions[] = {{180,100}, {190,115}, {200,125}, {190,140}};
    cFigure::Anchor anchors[] = {cFigure::ANCHOR_NW, cFigure::ANCHOR_W, cFigure::ANCHOR_CENTER, cFigure::ANCHOR_S};

    arcs[0]->setBounds(bounds[step]);
    arcs[1]->setPosition(positions[step], anchors[step]);

    double startAngles[] = {-1, 0, 2, 8};
    double endAngles[] = {-0.5, 0, 1.5, 7};

    arcs[2]->setStartAngle(startAngles[step]);
    arcs[3]->setEndAngle(endAngles[step]);

    std::vector<cFigure::Point> pointLists[] = {
        {{40, 200}, {50, 230}, {70, 200}},
        {{70, 200}, {40, 200}, {50, 210}, {50, 230}},
        {{50, 230}, {40, 200}},
        {{40, 200}, {50, 210}, {70, 200}}
    };

    int indices[] = {0, 1, 0, 1};
    cFigure::Point points[] = {{80, 200}, {90, 210}, {100, 200}, {90, 230}};
    bool smooths[] = {false, true, false, true};
    cFigure::JoinStyle joinStyles[] = {cFigure::JOIN_MITER, cFigure::JOIN_BEVEL, cFigure::JOIN_MITER, cFigure::JOIN_ROUND};


    polylines[0]->setPoints(pointLists[step]);
    polylines[1]->setPoint(indices[step], points[step]);
    polylines[2]->setSmooth(smooths[step]);
    polylines[3]->setJoinStyle(joinStyles[step]);

    std::stringstream pointListString;
    for (auto p : pointLists[step])
        pointListString << p.str() << ", ";

    return "Line start: " + starts[step].str() + "\t Line end: " + ends[step].str()
            + "\nArc bounds: " + bounds[step].str() + "\t Arc position: " + positions[step].str() + ", " + getEnumString(anchors[step])
            + "\nArc start angle: " + std::to_string(startAngles[step]) + "\t Arc end angle: " + std::to_string(endAngles[step])
            + "\nPolyline points: " + pointListString.str()
            + "\nPolyline point " + std::to_string(indices[step]) + " set to " + points[step].str()
            + "\t Polyline smooth: " + std::to_string(smooths[step]) + "\t Polyline join style: " + getEnumString(joinStyles[step]);
}

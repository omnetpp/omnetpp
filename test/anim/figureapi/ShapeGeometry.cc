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

#include "ShapeGeometry.h"
#include "Figures.h"

Define_Module(ShapeGeometry);

std::string ShapeGeometry::init()
{
    auto c = getCanvas();


    rectangles[0] = new cRectangleFigure("rectangle_0");
    rectangles[1] = new cRectangleFigure("rectangle_1");
    rectangles[2] = new cRectangleFigure("rectangle_2");
    rectangles[3] = new cRectangleFigure("rectangle_3");

    ovals[0] = new cOvalFigure("oval_0");
    ovals[1] = new cOvalFigure("oval_1");

    rings[0] = new cRingFigure("ring_0");
    rings[1] = new cRingFigure("ring_1");
    rings[2] = new cRingFigure("ring_2");
    rings[3] = new cRingFigure("ring_3");

    pieSlices[0] = new cPieSliceFigure("pieslice_0");
    pieSlices[1] = new cPieSliceFigure("pieslice_1");
    pieSlices[2] = new cPieSliceFigure("pieslice_2");
    pieSlices[3] = new cPieSliceFigure("pieslice_3");

    polygons[0] = new cPolygonFigure("polygon_0");
    polygons[1] = new cPolygonFigure("polygon_1");
    polygons[2] = new cPolygonFigure("polygon_2");
    polygons[3] = new cPolygonFigure("polygon_3");
    polygons[4] = new cPolygonFigure("polygon_4");


    rectangles[0]->setBounds({50,100,20,30});
    rectangles[1]->setBounds({90,100,20,30});
    rectangles[2]->setBounds({130,100,20,30});
    rectangles[3]->setBounds({170,100,20,30});

    rectangles[2]->setCornerRadius(5);
    rectangles[3]->setCornerRadius(5);

    ovals[0]->setBounds({50,150,20,30});
    ovals[1]->setBounds({80,150,20,30});

    rings[0]->setBounds({50,200,20,30});
    rings[1]->setBounds({90,200,20,30});
    rings[2]->setBounds({130,200,20,30});
    rings[3]->setBounds({170,200,20,30});

    rings[0]->setInnerRadius(4);
    rings[1]->setInnerRadius(4);
    rings[2]->setInnerRadius(4);
    rings[3]->setInnerRadius(4);

    pieSlices[0]->setBounds({50,250,20,30});
    pieSlices[1]->setBounds({90,250,20,30});
    pieSlices[2]->setBounds({130,250,20,30});
    pieSlices[3]->setBounds({170,250,20,30});

    pieSlices[0]->setStartAngle(1);
    pieSlices[1]->setStartAngle(1);
    pieSlices[2]->setStartAngle(1);
    pieSlices[3]->setStartAngle(1);

    pieSlices[0]->setEndAngle(5);
    pieSlices[1]->setEndAngle(5);
    pieSlices[2]->setEndAngle(5);
    pieSlices[3]->setEndAngle(5);

    polygons[0]->setPoints({{140, 150}, {150, 180}, {170, 150}});
    polygons[1]->setPoints({{180, 150}, {190, 180}, {210, 150}});
    polygons[2]->setPoints({{220, 150}, {230, 180}, {250, 150}});
    polygons[3]->setPoints({{260, 150}, {270, 180}, {290, 150}});

    polygons[4]->setPoints({{250, 200}, {290, 200}, {290, 250}, {250, 250}, {280, 200}, {280, 250}});
    polygons[4]->setSmooth(true);


    auto grid = new GridFigure("grid", "grey");
    grid->translate(0, 90);
    c->addFigure(grid);


    c->addFigure(rectangles[0]);
    c->addFigure(rectangles[1]);
    c->addFigure(rectangles[2]);
    c->addFigure(rectangles[3]);

    c->addFigure(ovals[0]);
    c->addFigure(ovals[1]);

    c->addFigure(rings[0]);
    c->addFigure(rings[1]);
    c->addFigure(rings[2]);
    c->addFigure(rings[3]);

    c->addFigure(pieSlices[0]);
    c->addFigure(pieSlices[1]);
    c->addFigure(pieSlices[2]);
    c->addFigure(pieSlices[3]);

    c->addFigure(polygons[0]);
    c->addFigure(polygons[1]);
    c->addFigure(polygons[2]);
    c->addFigure(polygons[3]);
    c->addFigure(polygons[4]);

    auto root = c->getRootFigure();
    for (int i = 0; i < root->getNumFigures(); ++i)
        if (auto s = dynamic_cast<cAbstractShapeFigure *>(root->getFigure(i))) {
            s->setLineWidth(2);
            s->setZoomLineWidth(true);
            s->setFilled(true);
            s->setFillColor("yellow");
        }


    state->setPosition({20, 40});

    return "This test exercises the non-inherited functions of the cAbstractShapeItem subclasses,\n"
           "with the exception of cPathFigure. All changes should be visible, and no graphical glitches should occur.";
}

std::string ShapeGeometry::onTick(int tick)
{
    int step = tick % 4;

    cFigure::Anchor anchors[] = {cFigure::ANCHOR_NW, cFigure::ANCHOR_W, cFigure::ANCHOR_CENTER, cFigure::ANCHOR_S};


    cFigure::Rectangle rectangleBounds[] = {{50,100,20,30}, {60,100,10,30}, {60,110,20,10}, {50,120,20,20}};
    cFigure::Point rectanglePositions[] = {{90,100}, {100,115}, {110,125}, {100,140}};

    double cornerRxs[] = {0, 1, 5, 20};
    double cornerRys[] = {0, 1, 5, 20};

    rectangles[0]->setBounds(rectangleBounds[step]);
    rectangles[1]->setPosition(rectanglePositions[step], anchors[step]);
    rectangles[2]->setCornerRx(cornerRxs[step]);
    rectangles[3]->setCornerRy(cornerRys[step]);


    cFigure::Rectangle ovalBounds[] = {{50,150,20,30}, {60,150,10,30}, {60,160,20,10}, {50,170,20,20}};
    cFigure::Point ovalPositions[] = {{90,150}, {100,165}, {110,175}, {100,190}};

    ovals[0]->setBounds(ovalBounds[step]);
    ovals[1]->setPosition(ovalPositions[step], anchors[step]);


    cFigure::Rectangle ringBounds[] = {{50,200,20,30}, {60,200,10,30}, {60,210,20,10}, {50,220,20,20}};
    cFigure::Point ringPositions[] = {{90,200}, {100,215}, {110,225}, {100,240}};

    double innerRxs[] = {0, 2, 8, 24};
    double innerRys[] = {0, 2, 8, 24};

    rings[0]->setBounds(ringBounds[step]);
    rings[1]->setPosition(ringPositions[step], anchors[step]);
    rings[2]->setInnerRx(innerRxs[step]);
    rings[3]->setInnerRy(innerRys[step]);


    cFigure::Rectangle pieSliceBounds[] = {{50,250,20,30}, {60,250,10,30}, {60,260,20,10}, {50,270,20,20}};
    cFigure::Point pieSlicePositions[] = {{90,250}, {100,265}, {110,275}, {100,290}};

    double startAngles[] = {-1, 0, 2, 8};
    double endAngles[] = {-0.5, 0, 1.5, 7};

    pieSlices[0]->setBounds(pieSliceBounds[step]);
    pieSlices[1]->setPosition(pieSlicePositions[step], anchors[step]);
    pieSlices[2]->setStartAngle(startAngles[step]);
    pieSlices[3]->setEndAngle(endAngles[step]);

    std::vector<cFigure::Point> pointLists[] = {
        {{140, 150}, {150, 180}, {170, 150}},
        {{170, 150}, {140, 150}, {150, 160}, {150, 180}},
        {{150, 180}, {140, 150}},
        {{150, 160}, {140, 150}, {170, 150}}
    };


    int indices[] = {0, 1, 0, 1};
    cFigure::Point points[] = {{180, 150}, {190, 160}, {200, 150}, {190, 180}};
    bool smooths[] = {false, true, false, true};
    cFigure::JoinStyle joinStyles[] = {cFigure::JOIN_MITER, cFigure::JOIN_BEVEL, cFigure::JOIN_MITER, cFigure::JOIN_ROUND};
    cFigure::FillRule fillRules[] = {cFigure::FILL_EVENODD, cFigure::FILL_NONZERO, cFigure::FILL_EVENODD, cFigure::FILL_NONZERO};

    polygons[0]->setPoints(pointLists[step]);
    polygons[1]->setPoint(indices[step], points[step]);
    polygons[2]->setSmooth(smooths[step]);
    polygons[3]->setJoinStyle(joinStyles[step]);
    polygons[4]->setFillRule(fillRules[step]);


    std::stringstream pointListString;
    for (auto p : pointLists[step])
        pointListString << p.str() << ", ";

    return "Rectangle bounds: " + rectangleBounds[step].str() + "\t Rectangle position: " + rectanglePositions[step].str() + ", " + getEnumString(anchors[step])
            + "\nRectangle corner radius:\t X: " + std::to_string(cornerRxs[step]) + "\t Y: " + std::to_string(cornerRys[step])
            + "\nOval bounds: " + ovalBounds[step].str() + "\t Oval position: " + ovalPositions[step].str() + ", " + getEnumString(anchors[step])
            + "\nRing bounds: " + ringBounds[step].str() + "\t Ring position: " + ringPositions[step].str() + ", " + getEnumString(anchors[step])
            + "\nRing inner radius:\t X: " + std::to_string(innerRxs[step]) + "\t Y: " + std::to_string(innerRys[step])
            + "\nPieSlice bounds: " + pieSliceBounds[step].str() + "\t PieSlice position: " + pieSlicePositions[step].str() + ", " + getEnumString(anchors[step])
            + "\nPieslice start angle: " + std::to_string(startAngles[step]) + "\t PieSlice end angle: " + std::to_string(endAngles[step])
            + "\nPolygon points: " + pointListString.str()
            + "\nPolygon point " + std::to_string(indices[step]) + " set to " + points[step].str()
            + "\t Polygon smooth: " + std::to_string(smooths[step]) + "\t Polygon join style: " + getEnumString(joinStyles[step])
            + "\nPolygon fill rule: " + getEnumString(fillRules[step]);

}

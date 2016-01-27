#include <omnetpp.h>

using namespace omnetpp;

class Animator : public cSimpleModule
{
    virtual void initialize() override;
};

Define_Module(Animator);

void Animator::initialize()
{
    //--------------------------
    cLineFigure *line = new cLineFigure("line");
    line->setStart(cFigure::Point(0,0));
    line->setEnd(cFigure::Point(100,50));
    line->setLineWidth(2);
    line->setEndArrowhead(cFigure::ARROW_BARBED);

    //--------------------------
    cArcFigure *arc = new cArcFigure("arc");
    arc->setBounds(cFigure::Rectangle(100,100,100,100));
    arc->setStartAngle(0);
    arc->setEndAngle(M_PI/2);
    arc->setLineColor(cFigure::BLUE);
    arc->setEndArrowhead(cFigure::ARROW_BARBED);

    //--------------------------
    cPolylineFigure *polyline = new cPolylineFigure("polyline");
    const double C=1.1;
    for (int i = 0; i < 10; i++)
        polyline->addPoint(cFigure::Point(5*i*cos(C*i), 5*i*sin(C*i)));
    polyline->move(100, 100);

    cPolylineFigure *polyline2 = polyline->dup();
    polyline2->setSmooth(true);

    //--------------------------
    cRectangleFigure *rect = new cRectangleFigure("rect");
    rect->setBounds(cFigure::Rectangle(100,100,80,50));
    rect->setCornerRadius(5);
    rect->setFilled(true);
    rect->setFillColor(cFigure::GOOD_LIGHT_COLORS[0]);

    //--------------------------
    cOvalFigure *circle = new cOvalFigure("circle");
    circle->setBounds(cFigure::Rectangle(100,100,50,50));
    circle->setLineStyle(cFigure::LINE_DASHED);

    //--------------------------
    cRingFigure *ring = new cRingFigure("ring");
    ring->setBounds(cFigure::Rectangle(100,100,50,50));
    ring->setInnerRadius(10);
    ring->setFilled(true);
    ring->setFillColor(cFigure::YELLOW);

    //--------------------------
    cPieSliceFigure *pieslice = new cPieSliceFigure("pieslice");
    pieslice->setBounds(cFigure::Rectangle(100,100,100,100));
    pieslice->setStartAngle(0);
    pieslice->setEndAngle(2*M_PI/3);
    pieslice->setFilled(true);
    pieslice->setLineColor(cFigure::BLUE);
    pieslice->setFillColor(cFigure::YELLOW);

    //--------------------------
    cPolygonFigure *polygon = new cPolygonFigure("polygon");
    std::vector<cFigure::Point> points;
    points.push_back(cFigure::Point(0, 100));
    points.push_back(cFigure::Point(50, 100));
    points.push_back(cFigure::Point(100, 100));
    points.push_back(cFigure::Point(50, 50));
    polygon->setPoints(points);
    polygon->setLineColor(cFigure::BLUE);
    polygon->setLineWidth(3);

    cPolygonFigure *polygon2 = polygon->dup();
    polygon2->setSmooth(true);

    //--------------------------
    // In the first example, the path is given as a string:
    cPathFigure *path = new cPathFigure("path");
    path->setPath("M 0 150 L 50 50 L 100 150 Z");
    path->setFilled(true);
    path->setLineColor(cFigure::BLUE);
    path->setFillColor(cFigure::YELLOW);

    //--------------------------
    // The second example creates the equivalent path programmatically.
    cPathFigure *path2 = new cPathFigure("path2");
    path2->addMoveTo(0,150);
    path2->addLineTo(50,50);
    path2->addLineTo(100,150);
    path2->addClosePath();
    path2->setFilled(true);
    path2->setLineColor(cFigure::BLUE);
    path2->setFillColor(cFigure::YELLOW);

    // TODO example that draws several disjoint items in one path: a rect, zigzag curve, etc.

    //--------------------------
    cTextFigure *text = new cTextFigure("text");
    text->setText("This is some text.");
    text->setPosition(cFigure::Point(100,100));
    text->setAnchor(cFigure::ANCHOR_BASELINE_MIDDLE);
    text->setColor(cFigure::Color("#000040"));
    text->setFont(cFigure::Font("Arial", 12, cFigure::FONT_BOLD));

    //--------------------------
    cLabelFigure *label = new cLabelFigure("label");
    label->setText("This is a label.");
    label->setPosition(cFigure::Point(100,100));
    label->setAnchor(cFigure::ANCHOR_NW);
    label->setFont(cFigure::Font("Courier New"));
    label->setOpacity(0.9);

    //--------------------------
    cImageFigure *image = new cImageFigure("map");
    image->setPosition(cFigure::Point(0,0));
    image->setAnchor(cFigure::ANCHOR_NW);
    image->setImageName("maps/europe");
    image->setWidth(600);
    image->setHeight(500);

    //--------------------------
    cIconFigure *icon = new cIconFigure("icon");
    icon->setPosition(cFigure::Point(100,100));
    icon->setImageName("block/sink");
    icon->setTintColor(cFigure::Color("gold"));
    icon->setTintAmount(0.6);
    icon->setOpacity(0.8);

    //--------------------------
    cPixmapFigure *pixmapFigure = new cPixmapFigure("pixmap");
    pixmapFigure->setPosition(cFigure::Point(100,100));
    pixmapFigure->setSize(100, 100);
    pixmapFigure->setPixmapSize(20, 20, cFigure::GREEN, 1);
    for (int y = 0; y < pixmapFigure->getPixmapHeight(); y++) {
        for (int x = 0; x < pixmapFigure->getPixmapWidth(); x++) {
            double opacity = 1 - sqrt((x-10)*(x-10)/100.0 + (y-10)*(y-10)/100.0);
            if (opacity < 0) opacity = 0;
            pixmapFigure->setPixelOpacity(x, y, opacity);
        }
    }
    pixmapFigure->setInterpolation(cFigure::INTERPOLATION_NONE);

    cPixmapFigure *pixmapFigure2 = pixmapFigure->dup();
    pixmapFigure2->setInterpolation(cFigure::INTERPOLATION_FAST);

    //--------------------------
    cGroupFigure *group = new cGroupFigure("group");

    cImageFigure *image2 = new cImageFigure("img");
    image2->setPosition(cFigure::Point(0,0));
    image2->setAnchor(cFigure::ANCHOR_S);
    image2->setImageName("block/sink");

    cLineFigure *line2 = new cLineFigure("line");
    line2->setStart(cFigure::Point(-50,10));
    line2->setEnd(cFigure::Point(50,10));

    group->addFigure(image2);
    group->addFigure(line2);
    group->translate(100, 100);
    group->rotate(M_PI/6, 100, 100);

    //--------------------------

    cFigure *figures[] = {
        line,
        arc,
        polyline,
        polyline2,
        rect,
        circle,
        ring,
        pieslice,
        polygon,
        polygon2,
        path,
        text,
        label,
        image,
        icon,
        pixmapFigure,
        pixmapFigure2,
        group,
        nullptr
    };

    int dx = 0;
    cCanvas *canvas = getParentModule()->getCanvas();
    for (cFigure **p = figures; *p; p++) {
        cFigure *figure = *p;
        figure->move(dx,0);
        canvas->addFigure(figure);
        dx += 100;
    }
}

//TODO move() should be recursive

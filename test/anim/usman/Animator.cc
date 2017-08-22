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
    arc->setBounds(cFigure::Rectangle(10,10,100,100));
    arc->setStartAngle(0);
    arc->setEndAngle(M_PI/2);
    arc->setLineColor(cFigure::BLUE);
    arc->setEndArrowhead(cFigure::ARROW_BARBED);

    //--------------------------
    cPolylineFigure *polyline = new cPolylineFigure("polyline");
    const double C = 1.1;
    for (int i = 0; i < 10; i++)
        polyline->addPoint(cFigure::Point(5*i*cos(C*i), 5*i*sin(C*i)));
    polyline->move(100, 100);

    cPolylineFigure *polyline2 = polyline->dup();
    polyline2->setSmooth(true);

    //--------------------------
    cRectangleFigure *rect = new cRectangleFigure("rect");
    rect->setBounds(cFigure::Rectangle(100,100,160,100));
    rect->setCornerRadius(5);
    rect->setFilled(true);
    rect->setFillColor(cFigure::GOOD_LIGHT_COLORS[0]);

    //--------------------------
    cOvalFigure *circle = new cOvalFigure("circle");
    circle->setBounds(cFigure::Rectangle(100,100,120,120));
    circle->setLineWidth(2);
    circle->setLineStyle(cFigure::LINE_DOTTED);

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
    path->setPath("M 0 150 L 50 50 Q 20 120 100 150 Z");
    path->setFilled(true);
    path->setLineColor(cFigure::BLUE);
    path->setFillColor(cFigure::YELLOW);

    //--------------------------
    // The second example creates the equivalent path programmatically.
    cPathFigure *path2 = new cPathFigure("path2");
    path2->addMoveTo(0,150);
    path2->addLineTo(50,50);
    path2->addCurveTo(20,120,100,150);
    path2->addClosePath();
    path2->setFilled(true);
    path2->setLineColor(cFigure::BLUE);
    path2->setFillColor(cFigure::YELLOW);

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
    pixmapFigure->setPixmapSize(9, 9, cFigure::BLUE, 1);
    for (int y = 0; y < pixmapFigure->getPixmapHeight(); y++) {
        for (int x = 0; x < pixmapFigure->getPixmapWidth(); x++) {
            double opacity = 1 - sqrt((x-4)*(x-4) + (y-4)*(y-4))/4;
            if (opacity < 0) opacity = 0;
            pixmapFigure->setPixelOpacity(x, y, opacity);
        }
    }
    pixmapFigure->setInterpolation(cFigure::INTERPOLATION_NONE);

    cPixmapFigure *pixmapFigure2 = pixmapFigure->dup();
    pixmapFigure2->setInterpolation(cFigure::INTERPOLATION_FAST);

    //--------------------------
    cGroupFigure *group = new cGroupFigure("group");

    cRectangleFigure *rect2 = new cRectangleFigure("rect");
    rect2->setBounds(cFigure::Rectangle(-50,0,100,40));
    rect2->setCornerRadius(5);
    rect2->setFilled(true);
    rect2->setFillColor(cFigure::YELLOW);

    cLineFigure *line2 = new cLineFigure("line2");
    line2->setStart(cFigure::Point(-80,50));
    line2->setEnd(cFigure::Point(80,50));
    line2->setLineWidth(3);

    group->addFigure(rect2);
    group->addFigure(line2);
    group->translate(100, 100);
    group->rotate(M_PI/6, 100, 100);

    //--------------------------
    cGroupFigure *layer = new cGroupFigure("layer");
    layer->skewx(-0.3);

    cImageFigure *referenceImg = new cImageFigure("ref");
    referenceImg->setImageName("block/broadcast");
    referenceImg->setPosition(cFigure::Point(200,200));
    referenceImg->setOpacity(0.3);
    layer->addFigure(referenceImg);

    cPanelFigure *panel = new cPanelFigure("panel");

    cImageFigure *img = new cImageFigure("img");
    img->setImageName("block/broadcast");
    img->setPosition(cFigure::Point(0,0));
    panel->addFigure(img);

    cRectangleFigure *border = new cRectangleFigure("border");
    border->setBounds(cFigure::Rectangle(-25,-25,50,50));
    border->setLineWidth(3);
    panel->addFigure(border);

    layer->addFigure(panel);
    panel->setAnchorPoint(cFigure::Point(0,0));
    panel->setPosition(cFigure::Point(210,200));

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
        path2,
        text,
        label,
        image,
        icon,
        pixmapFigure,
        pixmapFigure2,
        group,
        layer,
        nullptr
    };

    int x = 100;
    cCanvas *canvas = getParentModule()->getCanvas();
    for (cFigure **p = figures; *p; p++) {
        cFigure *figure = *p;
        if (figure!=layer)
            figure->move(x, 50);
        canvas->addFigure(figure);
        x += 200;
    }
}


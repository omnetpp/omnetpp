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

#include "Basics.h"
#include "Figures.h"

Define_Module(Dummy);

Define_Module(Visible);

std::string Visible::init()
{
    image = new cImageFigure("image");
    image->setPosition(cFigure::Point(100, 100));
    image->setImageName("block/broadcast");
    getCanvas()->addFigure(image);

    auto axes = new AxesFigure("axes");
    axes->translate(100, 100);
    image->addFigure(axes);

    return "The visibility of the image is toggled on each tick.\n"
            "The red and green arrows should disappear with the image as well.";
}

std::string Visible::onTick(int tick)
{
    bool visible = tick % 2;

    image->setVisible(visible);
    return visible ? "it is visible" : "it is NOT visible";
}


Define_Module(AddRemove);

std::string AddRemove::init()
{
    image = new cImageFigure("image");
    image->setPosition(cFigure::Point(100, 100));
    image->setImageName("block/broadcast");
    getCanvas()->addFigure(image);

    auto axes = new AxesFigure("axes");
    axes->translate(100, 100);
    image->addFigure(axes);

    return "The image is alternately added and removed to/from the canvas on each tick.\n"
            "The red and green arrows should disappear with the image as well.";
}

std::string AddRemove::onTick(int tick)
{
    bool on = tick % 2;

    auto canvas = getCanvas();
    if (on)
        canvas->addFigure(image);
    else
        canvas->removeFigure(image);

    return on ? "it is on the canvas" : "it is NOT on the canvas";
}


Define_Module(ZIndex);

std::string ZIndex::init()
{
    auto c = getCanvas();

    auto rb = new cRectangleFigure("bottom_rect");
    rb->setFilled(true);
    rb->setFillColor("#AAAAAA");
    rb->setBounds(cFigure::Rectangle(175, 100, 50, 150));
    rb->setZIndex(-0.5);

    auto rt = new cRectangleFigure("top_rect");
    rt->setFilled(true);
    rt->setFillColor("#EEEEEE");
    rt->setBounds(cFigure::Rectangle(200, 110, 50, 150));
    rt->setZIndex(0.5);

    c->addFigure(rb);
    c->addFigure(rt);

    rectangle = new cRectangleFigure("text_rect");

    rectangle->setFilled(true);
    rectangle->setFillColor("blue");
    rectangle->setFillOpacity(0.25);
    rectangle->setBounds(cFigure::Rectangle(140, 120, 300, 120));

    for (int i = 0; i < 5; ++i) {
        double z = i - 2.4;
        texts[i] = new cTextFigure(("text_" + std::to_string(i)).c_str());
        texts[i]->setText(("################### Z: " + std::to_string(z)).c_str());
        texts[i]->setPosition(cFigure::Point(150, 130 + i * 20));
        texts[i]->setColor("red");
        texts[i]->setZIndex(z);
        rectangle->addFigure(texts[i]);
    }

    c->addFigure(rectangle);

    return "The grey rectangles on the left have a ZIndex of -0.5 and 0.5, respectively.\n"
           "The texts are children of the blue rectangle that cycles its ZIndex in on each tick: -1 then 0 then 1.\n"
           "The blue rectangle should be below, between, and above the grey ones, respectively.\n"
           "The bottommost text is always above the grey rectangles, the topmost is always below them.\n"
           "And the three middle ones cycle back and forth.";
}

std::string ZIndex::onTick(int tick)
{
    int z = (tick % 3) - 1; // -1, 0, 1

    rectangle->setZIndex(z);

    return "The blue rectangle is now at Z " + std::to_string(z);
}


Define_Module(Tags);

std::string Tags::init()
{
    auto c = getCanvas();

    A = new cRectangleFigure("A");
    A->setFilled(true);
    A->setFillColor("lightblue");
    A->setBounds(cFigure::Rectangle(100, 150, 50, 50));
    A->setZIndex(-0.5);

    B = new cRectangleFigure("B");
    B->setFilled(true);
    B->setFillColor("lightgreen");
    B->setBounds(cFigure::Rectangle(200, 150, 50, 50));
    B->setZIndex(0.5);

    A->setTags("A");
    B->setTags("B");

    c->addFigure(A);
    c->addFigure(B);

    auto axesA = new AxesFigure("axes");
    axesA->translate(125, 175);
    A->addFigure(axesA);

    auto axesB = new AxesFigure("axes");
    axesB->translate(225, 175);
    B->addFigure(axesB);


    state->setPosition(cFigure::Point(25, 120));

    return "The 'A' tag is always on the left rectangle, and the 'B' tag is always on the right rectangle.\n"
            "The 'C' tag acts as a two bit counter. So first it is on neither of them, then it is on the\n"
            "right one, then on the left one, and then on both of them. And so on and so forth.\n"
            "Play around with the figure tag filter dialog of the GUI and check if you see what you expect.\n"
            "For example, try to make it show only the figures with the tag 'C' by unticking 'A' and 'B'.\n"
            "The red and green arrows should disappear with the image as well.";
}

std::string Tags::onTick(int tick)
{
    bool onA = (tick % 4) % 2;
    bool onB = (tick % 4) / 2;

    A->setTags(onA ? "A C" : "A");
    B->setTags(onB ? "B C" : "B");

    return std::string("tags of A: '") + A->getTags() + "'\ntags of B: '" + B->getTags() + "'";
}


Define_Module(Transform);

void Transform::addFigures(bool bottom) {
    cRectangleFigure *rect = new cRectangleFigure(bottom ? "rect_b" : "rect_t");
    if (bottom) {
        rect->setBounds({0, 0, 30, 20});
        rect->translate(15, 115);
    } else
        rect->setBounds({15, 15, 30, 20});
    rect->setFilled(true);
    rect->setFillColor("yellow");
    grid->addFigure(rect);
    rect->addFigure(new AxesFigure());


    cArcFigure *arc = new cArcFigure(bottom ? "arc_b" : "arc_t");
    if (bottom) {
        arc->setBounds({0, 0, 30, 20});
        arc->translate(75, 115);
    } else
        arc->setBounds({75, 15, 30, 20});
    arc->setStartAngle(0.5);
    arc->setEndAngle(5);
    arc->setLineWidth(2);
    arc->setStartArrowhead(cFigure::ARROW_SIMPLE);
    arc->setEndArrowhead(cFigure::ARROW_TRIANGLE);
    grid->addFigure(arc);
    arc->addFigure(new AxesFigure());


    cPolygonFigure *polygon = new cPolygonFigure(bottom ? "polygon_b" : "polygon_t");
    if (bottom) {
        polygon->setPoints({{0, 0}, {0, 20}, {30, 20}, {30, 0}, {20, 10}});
        polygon->translate(150, 110);
    } else
        polygon->setPoints({{150, 10}, {150, 30}, {180, 30}, {180, 10}, {170, 20}});
    polygon->setFilled(true);
    polygon->setFillColor("lightgreen");
    grid->addFigure(polygon);
    polygon->addFigure(new AxesFigure());


    for (int anchor = 4; anchor >= 0; --anchor) {
        cImageFigure *image = new cImageFigure(("image_" + std::to_string(anchor) + (bottom ? "_b" : "_t")).c_str());
        image->setImageName("block/routing");
        if (bottom)
            image->translate(30, 170);
        else
            image->setPosition({30, 70});
        image->setAnchor((cFigure::Anchor)anchor);
        image->setSize(25, 15);
        grid->addFigure(image);
        image->addFigure(new AxesFigure());


        cIconFigure *icon = new cIconFigure(("icon_" + std::to_string(anchor) + (bottom ? "_b" : "_t")).c_str());
        icon->setImageName("block/routing");
        if (bottom)
            icon->translate(100, 170);
        else
            icon->setPosition({100, 70});
        icon->setAnchor((cFigure::Anchor)anchor);
        icon->setSize(35, 25);
        grid->addFigure(icon);
        icon->addFigure(new AxesFigure());


        cTextFigure *text = new cTextFigure(("text_" + std::to_string(anchor) + (bottom ? "_b" : "_t")).c_str());
        text->setText("text");
        text->setColor("purple");
        text->setHalo(true);
        if (bottom)
            text->translate(175, 170);
        else
            text->setPosition({175, 70});
        text->setAnchor((cFigure::Anchor)anchor);
        grid->addFigure(text);
        text->addFigure(new AxesFigure());


        cLabelFigure *label = new cLabelFigure(("label_" + std::to_string(anchor) + (bottom ? "_b" : "_t")).c_str());
        label->setText("label");
        label->setColor("darkred");
        label->setHalo(true);
        if (bottom)
            label->translate(250, 170);
        else
            label->setPosition({250, 70});
        label->setAnchor((cFigure::Anchor)anchor);
        grid->addFigure(label);
        label->addFigure(new AxesFigure());
    }


    std::string octocat =
            "M23.132,12.012c0,6.169-4.997,11.219-11.164,11.219c-6.178,0-11.175-5.05-11.175-11.219"
            "c0-6.178,4.997-11.173,11.175-11.173C18.136,0.839,23.132,5.835,23.132,12.012 M14.756,21.132c3.827-1.368,6.626-4.899,6.626-9.12"
            "c0-5.202-4.22-9.425-9.414-9.425c-5.204,0-9.426,4.223-9.426,9.425c0,4.264,2.844,7.818,6.736,9.12v-1.793"
            "c0-1.072,0.448-1.64,0.896-1.946c-2.427-0.253-5.03-1.128-5.03-5.327c0-1.177,0.416-2.141,1.105-2.885"
            "C6.141,8.897,5.78,7.805,6.359,6.328c0,0,0.908-0.295,2.963,1.105c0.853-0.241,1.783-0.361,2.689-0.361"
            "c0.918,0,1.838,0.119,2.701,0.361c2.056-1.4,2.952-1.105,2.952-1.105c0.59,1.477,0.219,2.569,0.11,2.853"
            "c0.689,0.744,1.105,1.708,1.105,2.885c0,4.199-2.603,5.074-5.03,5.327c0.447,0.306,0.907,0.874,0.907,1.946V21.132z";

    for (char &c : octocat)
        if (c == ',')
            c = ' ';

    auto path = new cPathFigure(bottom ? "octocat_b" : "octocat_t");
    path->setPath(octocat.c_str());
    path->setFilled(true);
    path->setFillColor("#444444");
    if (bottom)
        path->translate(230, 110);
    else
        path->setOffset({230, 10});
    grid->addFigure(path);
    path->addFigure(new AxesFigure());
}

std::string Transform::init()
{
    // just for positioning
    cGroupFigure *parent = new cGroupFigure("parent");

    parent->translate(100, 100);
    getCanvas()->addFigure(parent);

    grid = new GridFigure("grid", "blue");

    addFigures(false);
    addFigures(true);

    // a horizontal separator
    auto sep = new cLineFigure();
    sep->setStart({0, 100});
    sep->setEnd({300, 100});
    sep->setLineWidth(2);
    sep->setLineColor("red");
    grid->addFigure(sep);

    // adding a transparent white rectangle, and behind that, an original that will stay untransformed.
    auto veil = new cRectangleFigure("veil");
    veil->setBounds({-10, -10, 320, 220});
    veil->setOutlined(false);
    veil->setFilled(true);
    veil->setFillColor("white");
    veil->setFillOpacity(0.75);

    auto reference = grid->dupTree();
    reference->setTags("reference");
    veil->setTags("veil");
    parent->addFigure(reference);
    parent->addFigure(veil);
    parent->addFigure(grid);

    state->setPosition({20, 75});
    return "Testing the dynamic transformation of various figures in a hierarchy.\n"
            "A copy is kept untransformed behind a white veil for reference, can be hidden using the tag filter.\n"
            "You should see the same thing on the top and bottom half of the grid.\n"
            "The ones on the top are positioned by their inherent abilities (setPosition, setBounds, setOffset),\n"
            "while the ones on the bottom are put in place using a translating transformation.\n"
            "The little red and green arrows show the child coordinate systems of all figures.\n"
            "The top ones' should stay on the top left corner of the blue grid, and the bottom ones' should\n"
            "be at their local origins. All of them should follow the transformations of the grid.";
}

std::string Transform::onFrame(int tick, double t) const
{
    int step = tick % 5;
    double tween = 0.5 - std::cos(t*2*M_PI) * 0.5;

    cFigure::Transform tf;

    switch (step) {
    case 0: tf.translate(70*tween, 60*tween); break;
    case 1: tf.rotate(tween, 120, 160); break;
    case 2: tf.scale(1 + 0.4*tween, 1 - 0.5*tween, 40, 70); break;
    case 3: tf.skewx(0.6*tween, 50); break;
    case 4: tf.skewy(0.6*tween, 80); break;
    }

    grid->setTransform(tf);

    const char *transformStrings[] = {"translate", "rotate", "scale", "skewx", "skewy"};

    return std::string("now testing ") + transformStrings[step] + ", transform is " + tf.str();
}

class TooltipFigure : public cRectangleFigure {
    cRectangleFigure *N, *A, *B;
public:
    explicit TooltipFigure(const char *name, const char *color, cModule *a, cModule *b) : cRectangleFigure(name) {
        setBounds(cFigure::Rectangle(0, 0, 130, 40));
        setFilled(true);
        setFillColor(color);

        N = new cRectangleFigure("N");
        N->setFilled(true);
        N->setFillColor("grey");
        N->setBounds(cFigure::Rectangle(10, 20, 30, 30));

        A = new cRectangleFigure("A");
        A->setFilled(true);
        A->setFillColor("purple");
        A->setBounds(cFigure::Rectangle(50, 20, 30, 30));
        A->setAssociatedObject(a);

        B = new cRectangleFigure("B");
        B->setFilled(true);
        B->setFillColor("orange");
        B->setBounds(cFigure::Rectangle(90, 20, 30, 30));
        B->setAssociatedObject(b);

        addFigure(N);
        addFigure(A);
        addFigure(B);
    }

    void setInnerTooltip(const char *tt) {
        N->setTooltip(tt);
        A->setTooltip(tt);
        B->setTooltip(tt);
    }
};


Define_Module(Misc);

std::string Misc::init()
{
    cModuleType *moduleType = cModuleType::get("Dummy");

    a = moduleType->create("a", this);
    b = moduleType->create("b", this);

    a->setDisplayString("p=200,210;tt=tooltip of submodule a;i=,purple,100");
    b->setDisplayString("p=200,280;tt=tooltip of submodule b;i=,orange,100");

    auto c = getCanvas();

    N = new TooltipFigure("N", "grey", a, b);
    A = new TooltipFigure("A", "purple", a, b);
    B = new TooltipFigure("B", "orange", a, b);

    A->setAssociatedObject(a);
    B->setAssociatedObject(b);

    N->translate(20, 120);
    A->translate(20, 190);
    B->translate(20, 260);

    c->addFigure(N);
    c->addFigure(A);
    c->addFigure(B);

    state->setPosition({200, 120});

    return "Testing the tooltip and associated object properties of figures.\n"
           "The grey rectangles have no associated objects, the purple ones are\n"
           "associated with the 'a' submodule, and the orange ones with 'b'.\n"
           "The small squares are children of the rectangles they are on top of.\n"
           "The tooltip of the figure itself should always take precedence.\n"
           "If that is nullptr, then the tooltip of the associated object is shown,\n"
           "either set locally, or inherited. If there is no associatedObject, the\n"
           "tooltip of the parent figure is inherited. Empty tooltips are not shown.\n"
           "The tooltips of all parent rectangles and child squares are cycled\n"
           "between nullptr, '', '    ', 'singleline' and 'multi\\nline'.";

}

std::string Misc::onTick(int tick)
{
    const char *tooltips[] = {nullptr, "", "   ", "singleline", "multi\nline"};

    int innerIndex = tick % 5;
    int outerIndex = (tick / 5) % 5;

    N->setTooltip(tooltips[outerIndex]);
    A->setTooltip(tooltips[outerIndex]);
    B->setTooltip(tooltips[outerIndex]);

    N->setInnerTooltip(tooltips[innerIndex]);
    A->setInnerTooltip(tooltips[innerIndex]);
    B->setInnerTooltip(tooltips[innerIndex]);

    const char *ot = tooltips[outerIndex];
    if (!ot)
        ot = "<nullptr>";
    const char *it = tooltips[innerIndex];
    if (!it)
        it = "<nullptr>";

    return std::string("Parent tooltips: \n'") + ot + "'\nChild tooltips:\n'" + it + "'";
}

#include <omnetpp.h>

using namespace omnetpp;

class ClippingAnimator : public cModule
{
  protected:
    // for case 17
    struct Ball {
        cOvalFigure *figure;
        cFigure::Point startPos;
        cFigure::Point velocity;
    };
    std::vector<Ball> balls;
    double areaWidth = 0, areaHeight = 0;

  protected:
    virtual void initialize() override;
    virtual void refreshDisplay() const override;
};

Define_Module(ClippingAnimator);

static const double BALL_SIZE = 10;

// Folds x into [0,max], as if it was bouncing back and forth between 0 and max
static double bounce(double x, double max)
{
    double r = std::fmod(x, 2 * max);
    if (r < 0)
        r += 2 * max;
    return r <= max ? r : 2 * max - r;
}

class ClippingTicker : public cSimpleModule
{
  protected:
    virtual void initialize() override {scheduleAt(simTime(), new cMessage("tick"));}
    virtual void handleMessage(cMessage *msg) override {scheduleAt(simTime() + 0.05, msg);}
};

Define_Module(ClippingTicker);

void ClippingAnimator::initialize()
{
    // 17: lots of balls under the window, moving all over the module area
    areaWidth = std::atof(getDisplayString().getTagArg("bgb", 0)) - BALL_SIZE;
    areaHeight = std::atof(getDisplayString().getTagArg("bgb", 1)) - BALL_SIZE;
    cFigure *window = getCanvas()->getFigure("c17");
    int numBalls = par("numBalls");
    for (int i = 0; i < numBalls; i++) {
        cOvalFigure *figure = new cOvalFigure("ball");
        figure->setFilled(true);
        figure->setOutlined(false);
        figure->setFillColor(cFigure::GOOD_LIGHT_COLORS[i % cFigure::NUM_GOOD_LIGHT_COLORS]);
        window->addFigure(figure);
        double speed = uniform(30, 150);
        double direction = uniform(0, 2 * M_PI);
        cFigure::Point startPos(uniform(0, areaWidth), uniform(0, areaHeight));
        balls.push_back(Ball{figure, startPos, cFigure::Point(speed * std::cos(direction), speed * std::sin(direction))});
    }
}

void ClippingAnimator::refreshDisplay() const
{
    double t = simTime().dbl();
    cCanvas *canvas = getCanvas();

    // 13: rotating clipper, moving child
    cFigure *c13 = canvas->getFigure("c13");
    c13->setTransform(cFigure::Transform().rotate(t * 0.5, 160, 800));
    auto oval13 = check_and_cast<cOvalFigure *>(c13->getFigure("oval"));
    oval13->setBounds(cFigure::Rectangle(60 + 100 * std::sin(t), 760, 80, 80));

    // 14: clipper geometry changes
    auto c14 = check_and_cast<cRectangleFigure *>(canvas->getFigure("c14"));
    double w = 120 + 80 * std::sin(t);
    c14->setBounds(cFigure::Rectangle(430 - w / 2, 740, w, 120));

    // 15: clipping toggled
    auto c15 = check_and_cast<cAbstractShapeFigure *>(canvas->getFigure("c15"));
    c15->setClipChildren(((int)(t / 2)) % 2 == 0);

    // 16: zIndex of the clipper toggled; the effective Z-indices of its subtree follow it,
    // so the whole subtree must move above/below the green bar together
    cAbstractShapeFigure *c16 = check_and_cast<cAbstractShapeFigure *>(canvas->getFigure("c16"));
    c16->setClipChildren(((int)(t / 2)) % 2 == 0);

    // 17: balls moving all over the module area, clipping of the window toggled
    auto c17 = check_and_cast<cAbstractShapeFigure *>(canvas->getFigure("c17"));
    c17->setClipChildren(((int)(t / 5)) % 2 == 0);
    for (const Ball& ball : balls) {
        double x = bounce(ball.startPos.x + ball.velocity.x * t, areaWidth);
        double y = bounce(ball.startPos.y + ball.velocity.y * t, areaHeight);
        ball.figure->setBounds(cFigure::Rectangle(x, y, BALL_SIZE, BALL_SIZE));
    }
}

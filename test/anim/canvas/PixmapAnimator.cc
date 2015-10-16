#include <omnetpp.h>

using namespace omnetpp;

class PixmapAnimator : public cSimpleModule
{
    private:
        cPixmapFigure *pixmapFigure;

    protected:
        virtual void initialize() override;
        virtual void handleMessage(cMessage *msg) override;
        void fillPixmap(cFigure::Pixmap& pixmap);
        void changePixmap(cFigure::Pixmap& pixmap);
};

Define_Module(PixmapAnimator);

void PixmapAnimator::initialize()
{
    // sine/cosine pattern
    cCanvas *canvas = getParentModule()->getCanvas();
    pixmapFigure = check_and_cast<cPixmapFigure *>(canvas->getFigure("pixmap"));

    cFigure::Pixmap pixmap = pixmapFigure->getPixmap();
    fillPixmap(pixmap);
    pixmapFigure->setPixmap(pixmap);

    scheduleAt(1, new cMessage);
}

void PixmapAnimator::fillPixmap(cFigure::Pixmap& pixmap)
{
    int width = pixmap.getWidth();
    int height = pixmap.getHeight();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cFigure::RGBA& px = pixmap.pixel(x, y);
            px.red = 127 + 127 * sin(20 * (double)x / width);
            px.green = 127 + 127 * cos(20 * (double)y / height);
            px.blue = 0;
            px.alpha = (px.red + px.green) / 2;
        }
    }
}

void PixmapAnimator::changePixmap(cFigure::Pixmap& pixmap)
{
    int width = pixmap.getWidth();
    int height = pixmap.getHeight();

    double xoff = simTime().dbl() / 50;
    double yoff = simTime().dbl() / 10;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            cFigure::RGBA& px = pixmap.pixel(x, y);
            int red = 127 + 127 * sin(20 * ((double)x + xoff) / width);
            int green = 127 + 127 * cos(20 * ((double)y + yoff)/ height);
            px.alpha = (red + green) / 2;
        }
    }
}

void PixmapAnimator::handleMessage(cMessage *msg)
{
    cFigure::Pixmap pixmap = pixmapFigure->getPixmap();
    changePixmap(pixmap);
    pixmapFigure->setPixmap(pixmap);

    scheduleAt(simTime()+1, msg);
}


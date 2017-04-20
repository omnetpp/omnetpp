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

#include "Image.h"
#include "Figures.h"

#include <omnetpp/ccanvas.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/regmacros.h>
#include <string>

Define_Module(Image);

std::string Image::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure("bg");
    bg->setBounds({10, 80, 430, 180});
    bg->setFilled(true);
    bg->setFillColor("white");
    c->addFigure(bg);

    for (int i = 0; i < numProps; ++i) {
        auto image = new cImageFigure(("image_" + std::to_string(i)).c_str());
        image->setPosition({50.0 + i * 50, 120});
        image->setImageName("block/app2");

        auto icon = new cIconFigure(("icon_" + std::to_string(i)).c_str());
        icon->setPosition({50.0 + i * 50, 170});
        icon->setImageName("block/app2");

        auto pixmap = new cPixmapFigure(("pixmap_" + std::to_string(i)).c_str());
        pixmap->setPosition({50.0 + i * 50, 220});
        pixmap->setPixmap(cFigure::Pixmap(20, 30, "lightblue"));

        bg->addFigure(image);
        bg->addFigure(icon);
        bg->addFigure(pixmap);

        images[3*i] = image;
        images[3*i+1] = icon;
        images[3*i+2] = pixmap;
    }

    for (auto i : images) {
        cFigure *axes = new AxesFigure();
        axes->translate(i->getPosition().x, i->getPosition().y);
        axes->setZIndex(2);
        i->setZIndex(1);
        i->addFigure(axes);
        i->setTintColor("red");
        i->setTintAmount(0.5);
        i->setWidth(15);
        i->setHeight(15);
    }

    return "Different types of figures are laid out in rows.\n"
           "In each column, a different property of the figures is cycled between\n"
           "a few appropriate values. The properties changed in the columns are:\n"
           "position, anchor, width, height, interpolation, opacity, tintColor and tintAmount,\n"
           "respectively. All changes should be visible, and no visual glitches should occur.";
}

std::string Image::onTick(int tick)
{
    int step = tick % 4; // iterating over 4 values on each prop

    const char *colors[] = {"red", "purple", "orange", "darkgreen"};
    double alphas[] = {0.0, 0.2, 0.7, 1.0};
    double sizes[] = {0, 5, 15, 60};
    cFigure::Interpolation interps[] = {cFigure::INTERPOLATION_NONE, cFigure::INTERPOLATION_FAST, cFigure::INTERPOLATION_NONE, cFigure::INTERPOLATION_BEST};
    cFigure::Anchor anchors[] = {cFigure::ANCHOR_NW, cFigure::ANCHOR_CENTER, cFigure::ANCHOR_SW, cFigure::ANCHOR_SE};

    for (int i = 0; i < 3; ++i) {
        images[i]->setPosition({50.0, 120.0 + i*50 + step*5});
        images[i+3]->setAnchor(anchors[step]);
        images[i+6]->setWidth(sizes[step]);
        images[i+9]->setHeight(sizes[step]);
        images[i+12]->setInterpolation(interps[step]);
        images[i+15]->setOpacity(alphas[step]);
        images[i+18]->setTintColor(colors[step]);
        images[i+21]->setTintAmount(alphas[step]);
    }

    return std::string("Current values:")
            + "\t position: " + images[0]->getPosition().str() + " + (0, i*50)"
            + "\t anchor: " + getEnumString(anchors[step])
            + "\t width: " + std::to_string(sizes[step])
            + "\t height: " + std::to_string(sizes[step])

            + "\ninterpolation: " + getEnumString(interps[step])
            + "\t opacity: " + std::to_string(alphas[step])
            + "\t tintColor: " + colors[step]
            + "\t tintAmount: " + std::to_string(alphas[step]);
}

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

#include "ImageContents.h"

Define_Module(ImageContents);

std::string ImageContents::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure("bg");
    bg->setBounds({10, 147.5, 280, 4});
    bg->setFilled(true);
    bg->setFillColor("white");
    bg->setLineWidth(2);
    bg->setZoomLineWidth(true);
    c->addFigure(bg);

    image = new cImageFigure("image");
    image->setPosition(cFigure::Point(100, 100));
    image->setImageName("block/broadcast");
    c->addFigure(image);

    icon = new cIconFigure("icon");
    icon->setPosition(cFigure::Point(150, 100));
    icon->setImageName("block/broadcast");
    c->addFigure(icon);

    for (int i = 0; i < 7; ++i) {
        auto pixmap = new cPixmapFigure(("pixmap_" + std::to_string(i)).c_str());
        pixmap->setPosition(cFigure::Point(30 + i*40, 150));
        pixmap->setPixmapSize(30, 20, "red", 0.6);
        if (i > 2)
            pixmap->setInterpolation(cFigure::INTERPOLATION_NONE);
        c->addFigure(pixmap);

        pixmaps[i] = pixmap;
    }

    state->setPosition({20, 40});

    return "Tests setImageName for image and icon figures, as well as\n"
           "most content manipulation methods of pixmap figures (setPixmap,\n"
           "setPixmapSize, setPixel, setPixelColor, setPixelOpacity).";
}

std::string ImageContents::onTick(int tick)
{
    int step = tick % 4;

    const char *imageNames[] = {
        "abstract/penguin",
        "status/green",
        "misc/globe",
        "block/broadcast"
    };

    image->setImageName(imageNames[step]);
    icon->setImageName(imageNames[step]);

    static cFigure::Pixmap pix[] {
        cFigure::Pixmap(0, 0),
        cFigure::Pixmap(15, 15),
        cFigure::Pixmap(10, 30, {100, 200, 150, 210}),
        cFigure::Pixmap(25, 20, "blue", 0.3)
    };

    int widths[] = {10, 20, 15, 30};
    int heights[] = {25, 10, 15, 30};

    cFigure::RGBA rgbas[] = {
        {180, 120, 150, 210},
        {  0, 200,   0, 120},
        {100,  20,  90, 250},
        {100, 200, 150, 150}
    };

    const char *colors[] = {"gold", "crimson", "tan", "lime"};
    double alphas[] = {0.8, 0.4, 1.0, 0.6};

    int xs[] = { 5, 20, 5, 20 };
    int ys[] = { 6, 12, 6, 12 };

    pixmaps[0]->setPixmap(pix[step]);
    pixmaps[1]->setPixmapSize(widths[step], heights[step], rgbas[step]);
    pixmaps[2]->setPixmapSize(widths[step], heights[step], colors[step], alphas[step]);
    pixmaps[3]->setPixel(xs[step], ys[step], rgbas[step]);
    pixmaps[4]->setPixel(xs[step], ys[step], colors[step], alphas[step]);
    pixmaps[5]->setPixelColor(xs[step], ys[step], colors[step]);
    pixmaps[6]->setPixelOpacity(xs[step], ys[step], alphas[step]);

    return std::string("Image name: ") + imageNames[step]
           + "\nsetPixmap with a pixmap of size " + pix[step].str()
           + "\nsetPixmapSize with width: " + std::to_string(widths[step]) + ", height: " + std::to_string(heights[step])
           + "\nsetPixel with x: " + std::to_string(xs[step]) + ", y: " + std::to_string(ys[step])
           + "\nRGBA: " + rgbas[step].str() + ", color: " + colors[step] + ", alpha: " + std::to_string(alphas[step]);
}


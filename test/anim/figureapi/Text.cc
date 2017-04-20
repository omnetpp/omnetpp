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

#include "Text.h"

#include <omnetpp/ccanvas.h>
#include <omnetpp/cobjectfactory.h>
#include <omnetpp/regmacros.h>
#include <string>
#include "Figures.h"

Define_Module(Text);

std::string Text::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure("bg");
    bg->setBounds({10, 80, 380, 120});
    bg->setFilled(true);
    bg->setFillColor("white");
    c->addFigure(bg);

    for (int i = 0; i < numProps; ++i) {
        auto text = new cTextFigure(("text_" + std::to_string(i)).c_str());
        text->setPosition({30.0 + i * 50, 100});
        text->setText("text");

        auto label = new cLabelFigure(("label_" + std::to_string(i)).c_str());
        label->setPosition({30.0 + i * 50, 130});
        label->setText("label");

        bg->addFigure(text);
        bg->addFigure(label);

        texts[2*i] = text;
        texts[2*i+1] = label;

        text->setAnchor(cFigure::ANCHOR_BASELINE_MIDDLE);
        label->setAnchor(cFigure::ANCHOR_BASELINE_MIDDLE);
    }

    for (auto t : texts) {
        cRectangleFigure *anch = new cRectangleFigure("anchor");
        anch->setBounds({0.0, 0.0, 1.0, 1.0});
        anch->setFilled(true);
        anch->setFillColor("darkgrey");
        anch->setPosition(t->getPosition(), cFigure::ANCHOR_CENTER);
        anch->setZIndex(-1);
        t->setZIndex(2);
        t->addFigure(anch);
    }

    return "Different types of figures are laid out in rows.\n"
           "In each column, a different property of the figures is cycled between\n"
           "a few appropriate values. The properties changed in the columns are:\n"
           "position, anchor, color, opacity, halo, font, and text, respectively.\n"
           "All changes should be visible, and no visual glitches should occur.";
}

std::string Text::onTick(int tick)
{
    int step = tick % 4; // iterating over 4 values on each prop

    const char *colors[] = {"red", "purple", "orange", "darkgreen"};
    double alphas[] = {0.0, 0.2, 0.7, 1.0};
    bool bools[] = {false, true, false, true};
    cFigure::Anchor anchors[] = {cFigure::ANCHOR_NW, cFigure::ANCHOR_CENTER, cFigure::ANCHOR_BASELINE_END, cFigure::ANCHOR_SE};
    cFigure::Font fonts[] = {cFigure::Font("Arial", 10, cFigure::FONT_NONE),
            cFigure::Font("Tahoma", 17, cFigure::FONT_BOLD),
            cFigure::Font("Courier", 0, cFigure::FONT_ITALIC),
            cFigure::Font("Impact", 7, cFigure::FONT_BOLD | cFigure::FONT_ITALIC | cFigure::FONT_UNDERLINE)};
    const char *values[] = {"foxtrot", "0", "charlie\nkilo", "celery meme"};

    for (int i = 0; i < 2; ++i) {
        texts[i]->setPosition({30.0, 100.0 + i*30 + step*5});
        texts[i+2]->setAnchor(anchors[step]);
        texts[i+4]->setColor(colors[step]);
        texts[i+6]->setOpacity(alphas[step]);
        texts[i+8]->setHalo(bools[step]);
        texts[i+10]->setFont(fonts[step]);
        texts[i+12]->setText(values[step]);
    }

    return std::string("Current values:")
            + "\t position: " + texts[0]->getPosition().str() + " + (0, i*30)"
            + "\t anchor: " + getEnumString(anchors[step])
            + "\t color: " + colors[step]

            + "\nopacity: " + std::to_string(alphas[step])
            + "\t halo: " + (bools[step] ? "true" : "false")
            + "\t font: " + fonts[step].str()
            + "\t text: '" + values[step] + "'";
}

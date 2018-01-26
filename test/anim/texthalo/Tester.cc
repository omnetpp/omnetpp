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

#include "Tester.h"

Define_Module(Tester);

std::string Tester::makeString(int length)
{
    const static std::string words[] = {
            "lorem", "ipsum", "dolor", "sit", "amet", "consectetur", "adipisicing", "elit",
            "event", "module", "message", "channel", "simulation", "network",
            "internet", "IP", "DHCP", "protocol", "header", "Ethernet", "frame",
            "1", "128", "100", "%", "-", "[i]", "_", "3", "69", "NaN"
    };

    const static int numWords = std::end(words) - std::begin(words);

    std::stringstream ss;
    while ((int)ss.str().length() < length)
        ss << words[intuniform(0, numWords-1)] << ((intuniform(0, 10) > 0) ? " " : "");

    return ss.str().substr(0, length);
}

void Tester::initialize()
{
    cCanvas *canvas = getParentModule()->getCanvas();
    int n = par("numFigures");

    light = new cRectangleFigure("light");
    light->setZIndex(-2);
    light->setBounds(cFigure::Rectangle(50, 100, 700, 100));
    canvas->addFigure(light);

    dark = new cRectangleFigure("dark");
    dark->setZIndex(-2);
    dark->setBounds(cFigure::Rectangle(50, 300, 700, 100));
    canvas->addFigure(dark);

    for (int i = 0; i < n; ++i) {
        cAbstractTextFigure *figure;

        if (par("useLabels").boolValue())
            figure = new cLabelFigure(("figure-" + std::to_string(i)).c_str());
        else
            figure = new cTextFigure(("figure-" + std::to_string(i)).c_str());

        figure->setAnchor(cFigure::ANCHOR_W);
        figure->setText(makeString(par("textLength")).c_str());
        figure->setFont(cFigure::Font("Arial", par("fontSize")));
        figure->setPosition(cFigure::Point(100, 50 + (73*i) % 400));
        figure->setHalo(par("haloEnabled").boolValue());
        canvas->addFigure(figure);
        figures.push_back(figure);
    }

    scheduleAt(0, new cMessage("tick"));

    wave = par("waveEnabled").boolValue();
//    if (wave)
  //      canvas->setAnimationSpeed(1, this);
}

void Tester::handleMessage(cMessage *msg)
{
    static int i = 0;
    static int darkIndex = 0;
    static int lightIndex = 0;

    figures[i % figures.size()]->setText(makeString(par("textLength")).c_str());
    //figures[i % figures.size()]->setFont(cFigure::Font("Arial", par("fontSize")));

    if ((i % 100) == 0) {
        dark->setFilled(true);
        light->setFilled(true);

        dark->setFillColor(cFigure::GOOD_DARK_COLORS[darkIndex]);
        light->setFillColor(cFigure::GOOD_LIGHT_COLORS[lightIndex]);

        darkIndex = (darkIndex+1) % cFigure::NUM_GOOD_DARK_COLORS;
        lightIndex = (lightIndex+1) % cFigure::NUM_GOOD_LIGHT_COLORS;
    }

    i++;

    scheduleAt(simTime() + 1, msg);
}


void Tester::refreshDisplay() const
{
    if (wave) {
        for (auto f : figures) {
            auto pos = f->getPosition();
            f->setPosition({100 + 20 * std::sin(simTime().dbl() - pos.y / 10.0), pos.y});
            f->setZIndex(std::cos(simTime().dbl() - pos.y / 10.0));
        }
    }
}

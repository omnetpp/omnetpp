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

#include "Path.h"
#include "Figures.h"

Define_Module(Path);

std::string Path::init()
{
    auto c = getCanvas();

    auto bg = new cRectangleFigure();
    bg->setBounds({10, 100, 270, 80});
    bg->setFilled(true);
    bg->setFillColor("white");
    c->addFigure(bg);

    for (int i = 0; i < 5; ++i) {
        auto path = new cPathFigure(("path_" + std::to_string(i)).c_str());

        path->setPath("M 20 20 L 20 30 L 30 30 Z M 25 10 L 40 10 L 40 25 M 50 15 L 35 15 L 35 0");
        path->setOffset({i*50.0 + 10, 120});

        paths[i] = path;

        bg->addFigure(path);
    }

    for (auto p : paths) {
        p->setZoomLineWidth(true);
        p->setFilled(true);
        p->setFillColor("lightblue");
    }

    return "Tests the path, offset, joinStyle, capStyle and fillRule properties of cPathFigure.\n"
           "All changes should be visible and no graphical glitches should occur.";
}

std::string Path::onTick(int tick)
{
    int step = tick % 4; // iterating over 4 values on each prop

    const char *pathDefs[] = {
            "M 35 20 L 20 20 A 15 10 0 1 0 35 10 z",
            "M 35 20 L 20 20 A 15 10 0 0 1 35 10 z",
            "M 35 20 L 20 20 L 35 35 z",
            "M 30 20 Q 40 20 40 30 T 30 40 T 30 20"
    };

    cFigure::Point offsets[] = {{50,120}, {60,120}, {60,130}, {50,130}};
    cFigure::JoinStyle joinStyles[] = {cFigure::JOIN_MITER, cFigure::JOIN_BEVEL, cFigure::JOIN_MITER, cFigure::JOIN_ROUND};
    cFigure::CapStyle capStyles[] = {cFigure::CAP_BUTT, cFigure::CAP_ROUND, cFigure::CAP_BUTT, cFigure::CAP_SQUARE};
    cFigure::FillRule fillRules[] = {cFigure::FILL_EVENODD, cFigure::FILL_NONZERO, cFigure::FILL_EVENODD, cFigure::FILL_NONZERO};


    paths[0]->setPath(pathDefs[step]);
    paths[1]->setOffset(offsets[step]);
    paths[2]->setJoinStyle(joinStyles[step]);
    paths[3]->setCapStyle(capStyles[step]);
    paths[4]->setFillRule(fillRules[step]);


    return std::string("Current values:")
            + "\npath: " + pathDefs[step]
            + "\noffset: " + offsets[step].str()
            + "\njoinStyle: " + getEnumString(joinStyles[step])
            + "\ncapStyle: " + getEnumString(capStyles[step])
            + "\nfillRule: " + getEnumString(fillRules[step]);
}


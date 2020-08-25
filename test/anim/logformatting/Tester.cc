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

#include <iomanip>

Define_Module(Tester);

// TODO: "reset" sequences, format combinations, interaction with tabs, perhaps more invalid control sequence variants
void Tester::initialize()
{
    msg = new cMessage("selfMsg");
    scheduleAt(0, msg);

    // TODO: should not assume that the default BG is white(ish), and the default FG is black(ish) - consider dark UI themes
    EV_INFO << "Testing the normal and bright foreground palette:" << std::endl;

    for (int i = 30; i <= 37; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << std::endl;
    for (int i = 90; i <= 97; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[39m";

    EV_INFO << std::endl << "With black background:" << std::endl;

    EV_INFO << "\x1b[40m";
    for (int i = 30; i <= 37; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << "\x1b[0m" << std::endl << "\x1b[40m";
    for (int i = 90; i <= 97; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << "\x1b[0m" << std::endl;


    EV_INFO << "Testing the normal and bright foreground palette, with faint option:" << std::endl;

    for (int i = 30; i <= 37; ++i)
        EV_INFO << "\x1b[" << i << "m\x1b[2m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << std::endl;
    for (int i = 90; i <= 97; ++i)
        EV_INFO << "\x1b[" << i << "m\x1b[2m " << std::setw(3) << i << " \x1b[39m";

    EV_INFO << std::endl << "With black background:" << std::endl;

    EV_INFO << "\x1b[40m";
    for (int i = 30; i <= 37; ++i)
        EV_INFO << "\x1b[" << i << "m\x1b[2m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << "\x1b[0m" << std::endl << "\x1b[40m";
    for (int i = 90; i <= 97; ++i)
        EV_INFO << "\x1b[" << i << "m\x1b[2m " << std::setw(3) << i << " \x1b[39m";
    EV_INFO << "\x1b[0m" << std::endl;


    EV_INFO << "Testing the normal and bright background palette:" << std::endl;

    for (int i = 40; i <= 47; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[49m";
    EV_INFO << std::endl;
    for (int i = 100; i <= 107; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[49m";

    EV_INFO << std::endl << "With white foreground:" << std::endl;

    EV_INFO << "\x1b[97m";
    for (int i = 40; i <= 47; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[49m";
    EV_INFO << "\x1b[0m" << std::endl << "\x1b[97m";
    for (int i = 100; i <= 107; ++i)
        EV_INFO << "\x1b[" << i << "m " << std::setw(3) << i << " \x1b[49m";
    EV_INFO << "\x1b[0m" << std::endl;


    EV_INFO << "Testing font styles:" << std::endl;

    EV_INFO << "bold: \x1b[1mAaBbCcEeIiMmWwXxYyZz\x1b[0m" << std::endl;
    EV_INFO << "faint: \x1b[2mAaBbCcEeIiMmWwXxYyZz\x1b[0m" << std::endl;
    EV_INFO << "italic: \x1b[3mAaBbCcEeIiMmWwXxYyZz\x1b[0m" << std::endl;
    EV_INFO << "underlined: \x1b[4mAaBbCcEeIiMmWwXxYyZz\x1b[0m" << std::endl;

    EV_INFO << "Testing the 8 bit color palette:" << std::endl;

    EV_INFO << "4-bit equivalents:" << std::endl;
    EV_INFO << "\x1b[97m";
    for (int i = 0; i < 16; ++i) {
        if (i == 8)
            EV_INFO << "\x1b[39m";
        EV_INFO << "\x1b[48;5;" << i << "m " << std::setw(2) << i << " ";
    }

    EV_INFO << "\x1b[0m" << std::endl;

    EV_INFO << "6x6x6 RGB cube:" << std::endl;

    for (int i = 16; i < 232; ++i) {
        if (i < 16 + (232-16)/2)
            EV_INFO << "\x1b[97m";
        EV_INFO << "\x1b[48;5;" << i << "m " << std::setw(3) << i << " ";
        if ((i-16+1) % 6 == 0)
            EV_INFO << "\x1b[0m" << "\x1b[0m" << std::endl;
    }

    EV_INFO << "24 gray levels:" << std::endl;
    EV_INFO << "\x1b[97m";
    for (int i = 232; i < 256; ++i) {
        if (i == 244)
            EV_INFO << "\x1b[39m";
        EV_INFO << "\x1b[48;5;" << i << "m " << i << " ";
    }

    EV_INFO << "\x1b[0m" << std::endl;

    EV_INFO << "Invalid sequences:" << std::endl;

    EV_INFO << "  Missing terminator: \x1b[32 Rest of the line." << std::endl;
    EV_INFO << "  Not square bracket: \x1b{34 Rest of the line." << std::endl;
    EV_INFO << "  Wrong format: \x1b\x1b[32mm Rest of the line." << std::endl;
    EV_INFO << "  Invalid character: \x1b[34;-5;1m Rest of the line." << std::endl;
    EV_INFO << "  Unsupported argument: \x1b[20m Rest of the line." << std::endl;

    EV_INFO << "  Unsupported color format: \x1b[38;3;213;221m Rest of the line." << std::endl;
    EV_INFO << "  Missing 8-bit color argument: \x1b[38;5m Rest of the line." << std::endl;
    EV_INFO << "  No RGB color arguments: \x1b[38;2m Rest of the line." << std::endl;
    EV_INFO << "  Not enough RGB color arguments: \x1b[38;2;34;56m Rest of the line." << std::endl;

    EV_INFO << "Rainbow scroller: (start the simulation and turn off event banner printing for the best effect)" << std::endl;
}

static void printLetterWithColor(char letter, int r, int g, int b)
{
    EV_INFO << "\x1b[48;2;" << r << ";" << g << ";" << b << "m" << letter << letter << letter << letter << letter;
}

void Tester::handleMessage(cMessage *msg)
{
    scheduleAt(simTime() + SimTime(33, SIMTIME_MS), msg);

    const char *text = "        ALWAYS I WANNA BE WITH YOU    AND MAKE BELIEVE WITH YOU    AND LIVE IN HARMONY  HARMONY    OH LOVE        ";
    int len = strlen(text);

    for (int k = 0; k < 15; ++k) {
        std::stringstream color;

        color << "@" << std::hex << std::setfill('0') << std::setw(2) << ((i*3 + k*2) % 255) << 255 << 255;
        auto c = cFigure::Color(color.str().c_str());

        EV_INFO << "\x1b[1;97m";

        printLetterWithColor(((k % 2) == 1) ? text[i % len] : ' ', c.red, c.green, c.blue);
    }
    EV_INFO << "\x1b[0m" << std::endl;

    i++;
}

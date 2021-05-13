//=========================================================================
//  COLORUTIL.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "colorutil.h"

#include "exception.h"
#include "math.h"
#include <algorithm>
#include <map>
#include <cstring>

namespace omnetpp {
namespace common {

struct rgb_t { // only used internally for easier map storage
    uint8_t r = 0, g = 0, b = 0;
    rgb_t() {}
    rgb_t(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

// Preventing the "static initialization order fiasco".
// https://isocpp.org/wiki/faq/ctors#construct-on-first-use-v2
static std::map<std::string, rgb_t> &colors() {
    // This will only get constructed once, and it is guaranteed to be before the first use.
    // This could only crash if the map is used in a destructor during exit and the map already got destructed.
    // Luckily, this could only happen if parseColor is called from a destructor, but it's very unlikely.
    static std::map<std::string, rgb_t> colorMap;
    return colorMap;
}

static std::string lc(const char *s)
{
    std::string tmp = s;
    std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);
    return tmp;
}

//
// HSB-to-RGB conversion
// source: http://nuttybar.drama.uga.edu/pipermail/dirgames-l/2001-March/006061.html
// Input:   hue, saturation, and brightness as floats scaled from 0.0 to 1.0
// Output:  red, green, and blue as floats scaled from 0.0 to 1.0
//
static void hsbToRgb(double hue, double saturation, double brightness,
        double& red, double& green, double& blue)
{
    if (brightness == 0.0) {  // safety short circuit again
        red = 0.0;
        green = 0.0;
        blue = 0.0;
        return;
    }

    if (saturation == 0.0) {  // grey
        red = brightness;
        green = brightness;
        blue = brightness;
        return;
    }

    float domainOffset;  // hue mod 1/6
    if (hue < 1.0 / 6) {  // red domain; green ascends
        domainOffset = hue;
        red = brightness;
        blue = brightness * (1.0 - saturation);
        green = blue + (brightness - blue) * domainOffset * 6;
    }
    else if (hue < 2.0 / 6) {  // yellow domain; red descends
        domainOffset = hue - 1.0 / 6;
        green = brightness;
        blue = brightness * (1.0 - saturation);
        red = green - (brightness - blue) * domainOffset * 6;
    }
    else if (hue < 3.0 / 6) {  // green domain; blue ascends
        domainOffset = hue - 2.0 / 6;
        green = brightness;
        red = brightness * (1.0 - saturation);
        blue = red + (brightness - red) * domainOffset * 6;
    }
    else if (hue < 4.0 / 6) {  // cyan domain; green descends
        domainOffset = hue - 3.0 / 6;
        blue = brightness;
        red = brightness * (1.0 - saturation);
        green = blue - (brightness - red) * domainOffset * 6;
    }
    else if (hue < 5.0 / 6) {  // blue domain; red ascends
        domainOffset = hue - 4.0 / 6;
        blue = brightness;
        green = brightness * (1.0 - saturation);
        red = green + (brightness - green) * domainOffset * 6;
    }
    else {  // magenta domain; blue descends
        domainOffset = hue - 5.0 / 6;
        red = brightness;
        green = brightness * (1.0 - saturation);
        blue = red - (brightness - green) * domainOffset * 6;
    }
}

inline int h2d(const char *s, int index)
{
    char c = s[index];
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    throw opp_runtime_error("Illegal hex digit '%c' in string '%s'", c, s);
}

static void fillColorsMap();

void parseColor(const char *s, uint8_t &r, uint8_t &g, uint8_t &b)
{
    if (s[0] == '#') {
        if (strlen(s) != 7)
            throw opp_runtime_error("Wrong color syntax '%s': 6 hex digits expected after '#'", s);
        r = h2d(s, 1) * 16 + h2d(s, 2);
        g = h2d(s, 3) * 16 + h2d(s, 4);
        b = h2d(s, 5) * 16 + h2d(s, 6);
    }
    else if (s[0] == '@') {
        if (strlen(s) != 7)
            throw opp_runtime_error("Wrong color syntax '%s': 6 hex digits expected after '@'", s);
        double hue = (h2d(s, 1) * 16 + h2d(s, 2)) / 255.0;
        double saturation = (h2d(s, 3) * 16 + h2d(s, 4)) / 255.0;
        double brightness = (h2d(s, 5) * 16 + h2d(s, 6)) / 255.0;
        double red, green, blue;
        hsbToRgb(hue, saturation, brightness, red, green, blue);
        r = std::min(255, (int)floor(red * 255 + 0.5));
        g = std::min(255, (int)floor(green * 255 + 0.5));
        b = std::min(255, (int)floor(blue * 255 + 0.5));
    }
    else {
        if (colors().empty())
           fillColorsMap();
        std::map<std::string, rgb_t>::const_iterator it = colors().find(lc(s));
        if (it == colors().end())
            throw opp_runtime_error("No such color: %s", s);
        r = (*it).second.r;
        g = (*it).second.g;
        b = (*it).second.b;
    }
}

//
// Fill the table of recognized color names. This is a union of two sets:
// X11 colors and SVG colors. Unfortunately there are four conflicting color
// names: green, grey, maroon and purple. We prefer SVG, so these colors are
// defined with their SVG meanings. This is achieved with the order of code
// blocks inside the function.
//
static void fillColorsMap()
{
    // X11 colors from http://cgit.freedesktop.org/xorg/app/rgb/plain/rgb.txt, as of 10/08/2015.

    colors()["aliceblue"] = rgb_t(240, 248, 255);
    colors()["antiquewhite"] = rgb_t(250, 235, 215);
    colors()["antiquewhite1"] = rgb_t(255, 239, 219);
    colors()["antiquewhite2"] = rgb_t(238, 223, 204);
    colors()["antiquewhite3"] = rgb_t(205, 192, 176);
    colors()["antiquewhite4"] = rgb_t(139, 131, 120);
    colors()["aqua"] = rgb_t(0, 255, 255);
    colors()["aquamarine"] = rgb_t(127, 255, 212);
    colors()["aquamarine1"] = rgb_t(127, 255, 212);
    colors()["aquamarine2"] = rgb_t(118, 238, 198);
    colors()["aquamarine3"] = rgb_t(102, 205, 170);
    colors()["aquamarine4"] = rgb_t(69, 139, 116);
    colors()["azure"] = rgb_t(240, 255, 255);
    colors()["azure1"] = rgb_t(240, 255, 255);
    colors()["azure2"] = rgb_t(224, 238, 238);
    colors()["azure3"] = rgb_t(193, 205, 205);
    colors()["azure4"] = rgb_t(131, 139, 139);
    colors()["beige"] = rgb_t(245, 245, 220);
    colors()["bisque"] = rgb_t(255, 228, 196);
    colors()["bisque1"] = rgb_t(255, 228, 196);
    colors()["bisque2"] = rgb_t(238, 213, 183);
    colors()["bisque3"] = rgb_t(205, 183, 158);
    colors()["bisque4"] = rgb_t(139, 125, 107);
    colors()["black"] = rgb_t(0, 0, 0);
    colors()["blanchedalmond"] = rgb_t(255, 235, 205);
    colors()["blue"] = rgb_t(0, 0, 255);
    colors()["blue1"] = rgb_t(0, 0, 255);
    colors()["blue2"] = rgb_t(0, 0, 238);
    colors()["blue3"] = rgb_t(0, 0, 205);
    colors()["blue4"] = rgb_t(0, 0, 139);
    colors()["blueviolet"] = rgb_t(138, 43, 226);
    colors()["brown"] = rgb_t(165, 42, 42);
    colors()["brown1"] = rgb_t(255, 64, 64);
    colors()["brown2"] = rgb_t(238, 59, 59);
    colors()["brown3"] = rgb_t(205, 51, 51);
    colors()["brown4"] = rgb_t(139, 35, 35);
    colors()["burlywood"] = rgb_t(222, 184, 135);
    colors()["burlywood1"] = rgb_t(255, 211, 155);
    colors()["burlywood2"] = rgb_t(238, 197, 145);
    colors()["burlywood3"] = rgb_t(205, 170, 125);
    colors()["burlywood4"] = rgb_t(139, 115, 85);
    colors()["cadetblue"] = rgb_t(95, 158, 160);
    colors()["cadetblue1"] = rgb_t(152, 245, 255);
    colors()["cadetblue2"] = rgb_t(142, 229, 238);
    colors()["cadetblue3"] = rgb_t(122, 197, 205);
    colors()["cadetblue4"] = rgb_t(83, 134, 139);
    colors()["chartreuse"] = rgb_t(127, 255, 0);
    colors()["chartreuse1"] = rgb_t(127, 255, 0);
    colors()["chartreuse2"] = rgb_t(118, 238, 0);
    colors()["chartreuse3"] = rgb_t(102, 205, 0);
    colors()["chartreuse4"] = rgb_t(69, 139, 0);
    colors()["chocolate"] = rgb_t(210, 105, 30);
    colors()["chocolate1"] = rgb_t(255, 127, 36);
    colors()["chocolate2"] = rgb_t(238, 118, 33);
    colors()["chocolate3"] = rgb_t(205, 102, 29);
    colors()["chocolate4"] = rgb_t(139, 69, 19);
    colors()["coral"] = rgb_t(255, 127, 80);
    colors()["coral1"] = rgb_t(255, 114, 86);
    colors()["coral2"] = rgb_t(238, 106, 80);
    colors()["coral3"] = rgb_t(205, 91, 69);
    colors()["coral4"] = rgb_t(139, 62, 47);
    colors()["cornflowerblue"] = rgb_t(100, 149, 237);
    colors()["cornsilk"] = rgb_t(255, 248, 220);
    colors()["cornsilk1"] = rgb_t(255, 248, 220);
    colors()["cornsilk2"] = rgb_t(238, 232, 205);
    colors()["cornsilk3"] = rgb_t(205, 200, 177);
    colors()["cornsilk4"] = rgb_t(139, 136, 120);
    colors()["crimson"] = rgb_t(220, 20, 60);
    colors()["cyan"] = rgb_t(0, 255, 255);
    colors()["cyan1"] = rgb_t(0, 255, 255);
    colors()["cyan2"] = rgb_t(0, 238, 238);
    colors()["cyan3"] = rgb_t(0, 205, 205);
    colors()["cyan4"] = rgb_t(0, 139, 139);
    colors()["darkblue"] = rgb_t(0, 0, 139);
    colors()["darkcyan"] = rgb_t(0, 139, 139);
    colors()["darkgoldenrod"] = rgb_t(184, 134, 11);
    colors()["darkgoldenrod1"] = rgb_t(255, 185, 15);
    colors()["darkgoldenrod2"] = rgb_t(238, 173, 14);
    colors()["darkgoldenrod3"] = rgb_t(205, 149, 12);
    colors()["darkgoldenrod4"] = rgb_t(139, 101, 8);
    colors()["darkgray"] = rgb_t(169, 169, 169);
    colors()["darkgreen"] = rgb_t(0, 100, 0);
    colors()["darkgrey"] = rgb_t(169, 169, 169);
    colors()["darkkhaki"] = rgb_t(189, 183, 107);
    colors()["darkmagenta"] = rgb_t(139, 0, 139);
    colors()["darkolivegreen"] = rgb_t(85, 107, 47);
    colors()["darkolivegreen1"] = rgb_t(202, 255, 112);
    colors()["darkolivegreen2"] = rgb_t(188, 238, 104);
    colors()["darkolivegreen3"] = rgb_t(162, 205, 90);
    colors()["darkolivegreen4"] = rgb_t(110, 139, 61);
    colors()["darkorange"] = rgb_t(255, 140, 0);
    colors()["darkorange1"] = rgb_t(255, 127, 0);
    colors()["darkorange2"] = rgb_t(238, 118, 0);
    colors()["darkorange3"] = rgb_t(205, 102, 0);
    colors()["darkorange4"] = rgb_t(139, 69, 0);
    colors()["darkorchid"] = rgb_t(153, 50, 204);
    colors()["darkorchid1"] = rgb_t(191, 62, 255);
    colors()["darkorchid2"] = rgb_t(178, 58, 238);
    colors()["darkorchid3"] = rgb_t(154, 50, 205);
    colors()["darkorchid4"] = rgb_t(104, 34, 139);
    colors()["darkred"] = rgb_t(139, 0, 0);
    colors()["darksalmon"] = rgb_t(233, 150, 122);
    colors()["darkseagreen"] = rgb_t(143, 188, 143);
    colors()["darkseagreen1"] = rgb_t(193, 255, 193);
    colors()["darkseagreen2"] = rgb_t(180, 238, 180);
    colors()["darkseagreen3"] = rgb_t(155, 205, 155);
    colors()["darkseagreen4"] = rgb_t(105, 139, 105);
    colors()["darkslateblue"] = rgb_t(72, 61, 139);
    colors()["darkslategray"] = rgb_t(47, 79, 79);
    colors()["darkslategray1"] = rgb_t(151, 255, 255);
    colors()["darkslategray2"] = rgb_t(141, 238, 238);
    colors()["darkslategray3"] = rgb_t(121, 205, 205);
    colors()["darkslategray4"] = rgb_t(82, 139, 139);
    colors()["darkslategrey"] = rgb_t(47, 79, 79);
    colors()["darkturquoise"] = rgb_t(0, 206, 209);
    colors()["darkviolet"] = rgb_t(148, 0, 211);
    colors()["deeppink"] = rgb_t(255, 20, 147);
    colors()["deeppink1"] = rgb_t(255, 20, 147);
    colors()["deeppink2"] = rgb_t(238, 18, 137);
    colors()["deeppink3"] = rgb_t(205, 16, 118);
    colors()["deeppink4"] = rgb_t(139, 10, 80);
    colors()["deepskyblue"] = rgb_t(0, 191, 255);
    colors()["deepskyblue1"] = rgb_t(0, 191, 255);
    colors()["deepskyblue2"] = rgb_t(0, 178, 238);
    colors()["deepskyblue3"] = rgb_t(0, 154, 205);
    colors()["deepskyblue4"] = rgb_t(0, 104, 139);
    colors()["dimgray"] = rgb_t(105, 105, 105);
    colors()["dimgrey"] = rgb_t(105, 105, 105);
    colors()["dodgerblue"] = rgb_t(30, 144, 255);
    colors()["dodgerblue1"] = rgb_t(30, 144, 255);
    colors()["dodgerblue2"] = rgb_t(28, 134, 238);
    colors()["dodgerblue3"] = rgb_t(24, 116, 205);
    colors()["dodgerblue4"] = rgb_t(16, 78, 139);
    colors()["firebrick"] = rgb_t(178, 34, 34);
    colors()["firebrick1"] = rgb_t(255, 48, 48);
    colors()["firebrick2"] = rgb_t(238, 44, 44);
    colors()["firebrick3"] = rgb_t(205, 38, 38);
    colors()["firebrick4"] = rgb_t(139, 26, 26);
    colors()["floralwhite"] = rgb_t(255, 250, 240);
    colors()["forestgreen"] = rgb_t(34, 139, 34);
    colors()["fuchsia"] = rgb_t(255, 0, 255);
    colors()["gainsboro"] = rgb_t(220, 220, 220);
    colors()["ghostwhite"] = rgb_t(248, 248, 255);
    colors()["gold"] = rgb_t(255, 215, 0);
    colors()["gold1"] = rgb_t(255, 215, 0);
    colors()["gold2"] = rgb_t(238, 201, 0);
    colors()["gold3"] = rgb_t(205, 173, 0);
    colors()["gold4"] = rgb_t(139, 117, 0);
    colors()["goldenrod"] = rgb_t(218, 165, 32);
    colors()["goldenrod1"] = rgb_t(255, 193, 37);
    colors()["goldenrod2"] = rgb_t(238, 180, 34);
    colors()["goldenrod3"] = rgb_t(205, 155, 29);
    colors()["goldenrod4"] = rgb_t(139, 105, 20);
    colors()["gray"] = rgb_t(190, 190, 190);
    colors()["gray0"] = rgb_t(0, 0, 0);
    colors()["gray1"] = rgb_t(3, 3, 3);
    colors()["gray10"] = rgb_t(26, 26, 26);
    colors()["gray100"] = rgb_t(255, 255, 255);
    colors()["gray11"] = rgb_t(28, 28, 28);
    colors()["gray12"] = rgb_t(31, 31, 31);
    colors()["gray13"] = rgb_t(33, 33, 33);
    colors()["gray14"] = rgb_t(36, 36, 36);
    colors()["gray15"] = rgb_t(38, 38, 38);
    colors()["gray16"] = rgb_t(41, 41, 41);
    colors()["gray17"] = rgb_t(43, 43, 43);
    colors()["gray18"] = rgb_t(46, 46, 46);
    colors()["gray19"] = rgb_t(48, 48, 48);
    colors()["gray2"] = rgb_t(5, 5, 5);
    colors()["gray20"] = rgb_t(51, 51, 51);
    colors()["gray21"] = rgb_t(54, 54, 54);
    colors()["gray22"] = rgb_t(56, 56, 56);
    colors()["gray23"] = rgb_t(59, 59, 59);
    colors()["gray24"] = rgb_t(61, 61, 61);
    colors()["gray25"] = rgb_t(64, 64, 64);
    colors()["gray26"] = rgb_t(66, 66, 66);
    colors()["gray27"] = rgb_t(69, 69, 69);
    colors()["gray28"] = rgb_t(71, 71, 71);
    colors()["gray29"] = rgb_t(74, 74, 74);
    colors()["gray3"] = rgb_t(8, 8, 8);
    colors()["gray30"] = rgb_t(77, 77, 77);
    colors()["gray31"] = rgb_t(79, 79, 79);
    colors()["gray32"] = rgb_t(82, 82, 82);
    colors()["gray33"] = rgb_t(84, 84, 84);
    colors()["gray34"] = rgb_t(87, 87, 87);
    colors()["gray35"] = rgb_t(89, 89, 89);
    colors()["gray36"] = rgb_t(92, 92, 92);
    colors()["gray37"] = rgb_t(94, 94, 94);
    colors()["gray38"] = rgb_t(97, 97, 97);
    colors()["gray39"] = rgb_t(99, 99, 99);
    colors()["gray4"] = rgb_t(10, 10, 10);
    colors()["gray40"] = rgb_t(102, 102, 102);
    colors()["gray41"] = rgb_t(105, 105, 105);
    colors()["gray42"] = rgb_t(107, 107, 107);
    colors()["gray43"] = rgb_t(110, 110, 110);
    colors()["gray44"] = rgb_t(112, 112, 112);
    colors()["gray45"] = rgb_t(115, 115, 115);
    colors()["gray46"] = rgb_t(117, 117, 117);
    colors()["gray47"] = rgb_t(120, 120, 120);
    colors()["gray48"] = rgb_t(122, 122, 122);
    colors()["gray49"] = rgb_t(125, 125, 125);
    colors()["gray5"] = rgb_t(13, 13, 13);
    colors()["gray50"] = rgb_t(127, 127, 127);
    colors()["gray51"] = rgb_t(130, 130, 130);
    colors()["gray52"] = rgb_t(133, 133, 133);
    colors()["gray53"] = rgb_t(135, 135, 135);
    colors()["gray54"] = rgb_t(138, 138, 138);
    colors()["gray55"] = rgb_t(140, 140, 140);
    colors()["gray56"] = rgb_t(143, 143, 143);
    colors()["gray57"] = rgb_t(145, 145, 145);
    colors()["gray58"] = rgb_t(148, 148, 148);
    colors()["gray59"] = rgb_t(150, 150, 150);
    colors()["gray6"] = rgb_t(15, 15, 15);
    colors()["gray60"] = rgb_t(153, 153, 153);
    colors()["gray61"] = rgb_t(156, 156, 156);
    colors()["gray62"] = rgb_t(158, 158, 158);
    colors()["gray63"] = rgb_t(161, 161, 161);
    colors()["gray64"] = rgb_t(163, 163, 163);
    colors()["gray65"] = rgb_t(166, 166, 166);
    colors()["gray66"] = rgb_t(168, 168, 168);
    colors()["gray67"] = rgb_t(171, 171, 171);
    colors()["gray68"] = rgb_t(173, 173, 173);
    colors()["gray69"] = rgb_t(176, 176, 176);
    colors()["gray7"] = rgb_t(18, 18, 18);
    colors()["gray70"] = rgb_t(179, 179, 179);
    colors()["gray71"] = rgb_t(181, 181, 181);
    colors()["gray72"] = rgb_t(184, 184, 184);
    colors()["gray73"] = rgb_t(186, 186, 186);
    colors()["gray74"] = rgb_t(189, 189, 189);
    colors()["gray75"] = rgb_t(191, 191, 191);
    colors()["gray76"] = rgb_t(194, 194, 194);
    colors()["gray77"] = rgb_t(196, 196, 196);
    colors()["gray78"] = rgb_t(199, 199, 199);
    colors()["gray79"] = rgb_t(201, 201, 201);
    colors()["gray8"] = rgb_t(20, 20, 20);
    colors()["gray80"] = rgb_t(204, 204, 204);
    colors()["gray81"] = rgb_t(207, 207, 207);
    colors()["gray82"] = rgb_t(209, 209, 209);
    colors()["gray83"] = rgb_t(212, 212, 212);
    colors()["gray84"] = rgb_t(214, 214, 214);
    colors()["gray85"] = rgb_t(217, 217, 217);
    colors()["gray86"] = rgb_t(219, 219, 219);
    colors()["gray87"] = rgb_t(222, 222, 222);
    colors()["gray88"] = rgb_t(224, 224, 224);
    colors()["gray89"] = rgb_t(227, 227, 227);
    colors()["gray9"] = rgb_t(23, 23, 23);
    colors()["gray90"] = rgb_t(229, 229, 229);
    colors()["gray91"] = rgb_t(232, 232, 232);
    colors()["gray92"] = rgb_t(235, 235, 235);
    colors()["gray93"] = rgb_t(237, 237, 237);
    colors()["gray94"] = rgb_t(240, 240, 240);
    colors()["gray95"] = rgb_t(242, 242, 242);
    colors()["gray96"] = rgb_t(245, 245, 245);
    colors()["gray97"] = rgb_t(247, 247, 247);
    colors()["gray98"] = rgb_t(250, 250, 250);
    colors()["gray99"] = rgb_t(252, 252, 252);
    colors()["green"] = rgb_t(0, 255, 0);
    colors()["green1"] = rgb_t(0, 255, 0);
    colors()["green2"] = rgb_t(0, 238, 0);
    colors()["green3"] = rgb_t(0, 205, 0);
    colors()["green4"] = rgb_t(0, 139, 0);
    colors()["greenyellow"] = rgb_t(173, 255, 47);
    colors()["grey"] = rgb_t(190, 190, 190);
    colors()["grey0"] = rgb_t(0, 0, 0);
    colors()["grey1"] = rgb_t(3, 3, 3);
    colors()["grey10"] = rgb_t(26, 26, 26);
    colors()["grey100"] = rgb_t(255, 255, 255);
    colors()["grey11"] = rgb_t(28, 28, 28);
    colors()["grey12"] = rgb_t(31, 31, 31);
    colors()["grey13"] = rgb_t(33, 33, 33);
    colors()["grey14"] = rgb_t(36, 36, 36);
    colors()["grey15"] = rgb_t(38, 38, 38);
    colors()["grey16"] = rgb_t(41, 41, 41);
    colors()["grey17"] = rgb_t(43, 43, 43);
    colors()["grey18"] = rgb_t(46, 46, 46);
    colors()["grey19"] = rgb_t(48, 48, 48);
    colors()["grey2"] = rgb_t(5, 5, 5);
    colors()["grey20"] = rgb_t(51, 51, 51);
    colors()["grey21"] = rgb_t(54, 54, 54);
    colors()["grey22"] = rgb_t(56, 56, 56);
    colors()["grey23"] = rgb_t(59, 59, 59);
    colors()["grey24"] = rgb_t(61, 61, 61);
    colors()["grey25"] = rgb_t(64, 64, 64);
    colors()["grey26"] = rgb_t(66, 66, 66);
    colors()["grey27"] = rgb_t(69, 69, 69);
    colors()["grey28"] = rgb_t(71, 71, 71);
    colors()["grey29"] = rgb_t(74, 74, 74);
    colors()["grey3"] = rgb_t(8, 8, 8);
    colors()["grey30"] = rgb_t(77, 77, 77);
    colors()["grey31"] = rgb_t(79, 79, 79);
    colors()["grey32"] = rgb_t(82, 82, 82);
    colors()["grey33"] = rgb_t(84, 84, 84);
    colors()["grey34"] = rgb_t(87, 87, 87);
    colors()["grey35"] = rgb_t(89, 89, 89);
    colors()["grey36"] = rgb_t(92, 92, 92);
    colors()["grey37"] = rgb_t(94, 94, 94);
    colors()["grey38"] = rgb_t(97, 97, 97);
    colors()["grey39"] = rgb_t(99, 99, 99);
    colors()["grey4"] = rgb_t(10, 10, 10);
    colors()["grey40"] = rgb_t(102, 102, 102);
    colors()["grey41"] = rgb_t(105, 105, 105);
    colors()["grey42"] = rgb_t(107, 107, 107);
    colors()["grey43"] = rgb_t(110, 110, 110);
    colors()["grey44"] = rgb_t(112, 112, 112);
    colors()["grey45"] = rgb_t(115, 115, 115);
    colors()["grey46"] = rgb_t(117, 117, 117);
    colors()["grey47"] = rgb_t(120, 120, 120);
    colors()["grey48"] = rgb_t(122, 122, 122);
    colors()["grey49"] = rgb_t(125, 125, 125);
    colors()["grey5"] = rgb_t(13, 13, 13);
    colors()["grey50"] = rgb_t(127, 127, 127);
    colors()["grey51"] = rgb_t(130, 130, 130);
    colors()["grey52"] = rgb_t(133, 133, 133);
    colors()["grey53"] = rgb_t(135, 135, 135);
    colors()["grey54"] = rgb_t(138, 138, 138);
    colors()["grey55"] = rgb_t(140, 140, 140);
    colors()["grey56"] = rgb_t(143, 143, 143);
    colors()["grey57"] = rgb_t(145, 145, 145);
    colors()["grey58"] = rgb_t(148, 148, 148);
    colors()["grey59"] = rgb_t(150, 150, 150);
    colors()["grey6"] = rgb_t(15, 15, 15);
    colors()["grey60"] = rgb_t(153, 153, 153);
    colors()["grey61"] = rgb_t(156, 156, 156);
    colors()["grey62"] = rgb_t(158, 158, 158);
    colors()["grey63"] = rgb_t(161, 161, 161);
    colors()["grey64"] = rgb_t(163, 163, 163);
    colors()["grey65"] = rgb_t(166, 166, 166);
    colors()["grey66"] = rgb_t(168, 168, 168);
    colors()["grey67"] = rgb_t(171, 171, 171);
    colors()["grey68"] = rgb_t(173, 173, 173);
    colors()["grey69"] = rgb_t(176, 176, 176);
    colors()["grey7"] = rgb_t(18, 18, 18);
    colors()["grey70"] = rgb_t(179, 179, 179);
    colors()["grey71"] = rgb_t(181, 181, 181);
    colors()["grey72"] = rgb_t(184, 184, 184);
    colors()["grey73"] = rgb_t(186, 186, 186);
    colors()["grey74"] = rgb_t(189, 189, 189);
    colors()["grey75"] = rgb_t(191, 191, 191);
    colors()["grey76"] = rgb_t(194, 194, 194);
    colors()["grey77"] = rgb_t(196, 196, 196);
    colors()["grey78"] = rgb_t(199, 199, 199);
    colors()["grey79"] = rgb_t(201, 201, 201);
    colors()["grey8"] = rgb_t(20, 20, 20);
    colors()["grey80"] = rgb_t(204, 204, 204);
    colors()["grey81"] = rgb_t(207, 207, 207);
    colors()["grey82"] = rgb_t(209, 209, 209);
    colors()["grey83"] = rgb_t(212, 212, 212);
    colors()["grey84"] = rgb_t(214, 214, 214);
    colors()["grey85"] = rgb_t(217, 217, 217);
    colors()["grey86"] = rgb_t(219, 219, 219);
    colors()["grey87"] = rgb_t(222, 222, 222);
    colors()["grey88"] = rgb_t(224, 224, 224);
    colors()["grey89"] = rgb_t(227, 227, 227);
    colors()["grey9"] = rgb_t(23, 23, 23);
    colors()["grey90"] = rgb_t(229, 229, 229);
    colors()["grey91"] = rgb_t(232, 232, 232);
    colors()["grey92"] = rgb_t(235, 235, 235);
    colors()["grey93"] = rgb_t(237, 237, 237);
    colors()["grey94"] = rgb_t(240, 240, 240);
    colors()["grey95"] = rgb_t(242, 242, 242);
    colors()["grey96"] = rgb_t(245, 245, 245);
    colors()["grey97"] = rgb_t(247, 247, 247);
    colors()["grey98"] = rgb_t(250, 250, 250);
    colors()["grey99"] = rgb_t(252, 252, 252);
    colors()["honeydew"] = rgb_t(240, 255, 240);
    colors()["honeydew1"] = rgb_t(240, 255, 240);
    colors()["honeydew2"] = rgb_t(224, 238, 224);
    colors()["honeydew3"] = rgb_t(193, 205, 193);
    colors()["honeydew4"] = rgb_t(131, 139, 131);
    colors()["hotpink"] = rgb_t(255, 105, 180);
    colors()["hotpink1"] = rgb_t(255, 110, 180);
    colors()["hotpink2"] = rgb_t(238, 106, 167);
    colors()["hotpink3"] = rgb_t(205, 96, 144);
    colors()["hotpink4"] = rgb_t(139, 58, 98);
    colors()["indianred"] = rgb_t(205, 92, 92);
    colors()["indianred1"] = rgb_t(255, 106, 106);
    colors()["indianred2"] = rgb_t(238, 99, 99);
    colors()["indianred3"] = rgb_t(205, 85, 85);
    colors()["indianred4"] = rgb_t(139, 58, 58);
    colors()["indigo"] = rgb_t(75, 0, 130);
    colors()["ivory"] = rgb_t(255, 255, 240);
    colors()["ivory1"] = rgb_t(255, 255, 240);
    colors()["ivory2"] = rgb_t(238, 238, 224);
    colors()["ivory3"] = rgb_t(205, 205, 193);
    colors()["ivory4"] = rgb_t(139, 139, 131);
    colors()["khaki"] = rgb_t(240, 230, 140);
    colors()["khaki1"] = rgb_t(255, 246, 143);
    colors()["khaki2"] = rgb_t(238, 230, 133);
    colors()["khaki3"] = rgb_t(205, 198, 115);
    colors()["khaki4"] = rgb_t(139, 134, 78);
    colors()["lavender"] = rgb_t(230, 230, 250);
    colors()["lavenderblush"] = rgb_t(255, 240, 245);
    colors()["lavenderblush1"] = rgb_t(255, 240, 245);
    colors()["lavenderblush2"] = rgb_t(238, 224, 229);
    colors()["lavenderblush3"] = rgb_t(205, 193, 197);
    colors()["lavenderblush4"] = rgb_t(139, 131, 134);
    colors()["lawngreen"] = rgb_t(124, 252, 0);
    colors()["lemonchiffon"] = rgb_t(255, 250, 205);
    colors()["lemonchiffon1"] = rgb_t(255, 250, 205);
    colors()["lemonchiffon2"] = rgb_t(238, 233, 191);
    colors()["lemonchiffon3"] = rgb_t(205, 201, 165);
    colors()["lemonchiffon4"] = rgb_t(139, 137, 112);
    colors()["lightblue"] = rgb_t(173, 216, 230);
    colors()["lightblue1"] = rgb_t(191, 239, 255);
    colors()["lightblue2"] = rgb_t(178, 223, 238);
    colors()["lightblue3"] = rgb_t(154, 192, 205);
    colors()["lightblue4"] = rgb_t(104, 131, 139);
    colors()["lightcoral"] = rgb_t(240, 128, 128);
    colors()["lightcyan"] = rgb_t(224, 255, 255);
    colors()["lightcyan1"] = rgb_t(224, 255, 255);
    colors()["lightcyan2"] = rgb_t(209, 238, 238);
    colors()["lightcyan3"] = rgb_t(180, 205, 205);
    colors()["lightcyan4"] = rgb_t(122, 139, 139);
    colors()["lightgoldenrod"] = rgb_t(238, 221, 130);
    colors()["lightgoldenrod1"] = rgb_t(255, 236, 139);
    colors()["lightgoldenrod2"] = rgb_t(238, 220, 130);
    colors()["lightgoldenrod3"] = rgb_t(205, 190, 112);
    colors()["lightgoldenrod4"] = rgb_t(139, 129, 76);
    colors()["lightgoldenrodyellow"] = rgb_t(250, 250, 210);
    colors()["lightgray"] = rgb_t(211, 211, 211);
    colors()["lightgreen"] = rgb_t(144, 238, 144);
    colors()["lightgrey"] = rgb_t(211, 211, 211);
    colors()["lightpink"] = rgb_t(255, 182, 193);
    colors()["lightpink1"] = rgb_t(255, 174, 185);
    colors()["lightpink2"] = rgb_t(238, 162, 173);
    colors()["lightpink3"] = rgb_t(205, 140, 149);
    colors()["lightpink4"] = rgb_t(139, 95, 101);
    colors()["lightsalmon"] = rgb_t(255, 160, 122);
    colors()["lightsalmon1"] = rgb_t(255, 160, 122);
    colors()["lightsalmon2"] = rgb_t(238, 149, 114);
    colors()["lightsalmon3"] = rgb_t(205, 129, 98);
    colors()["lightsalmon4"] = rgb_t(139, 87, 66);
    colors()["lightseagreen"] = rgb_t(32, 178, 170);
    colors()["lightskyblue"] = rgb_t(135, 206, 250);
    colors()["lightskyblue1"] = rgb_t(176, 226, 255);
    colors()["lightskyblue2"] = rgb_t(164, 211, 238);
    colors()["lightskyblue3"] = rgb_t(141, 182, 205);
    colors()["lightskyblue4"] = rgb_t(96, 123, 139);
    colors()["lightslateblue"] = rgb_t(132, 112, 255);
    colors()["lightslategray"] = rgb_t(119, 136, 153);
    colors()["lightslategrey"] = rgb_t(119, 136, 153);
    colors()["lightsteelblue"] = rgb_t(176, 196, 222);
    colors()["lightsteelblue1"] = rgb_t(202, 225, 255);
    colors()["lightsteelblue2"] = rgb_t(188, 210, 238);
    colors()["lightsteelblue3"] = rgb_t(162, 181, 205);
    colors()["lightsteelblue4"] = rgb_t(110, 123, 139);
    colors()["lightyellow"] = rgb_t(255, 255, 224);
    colors()["lightyellow1"] = rgb_t(255, 255, 224);
    colors()["lightyellow2"] = rgb_t(238, 238, 209);
    colors()["lightyellow3"] = rgb_t(205, 205, 180);
    colors()["lightyellow4"] = rgb_t(139, 139, 122);
    colors()["lime"] = rgb_t(0, 255, 0);
    colors()["limegreen"] = rgb_t(50, 205, 50);
    colors()["linen"] = rgb_t(250, 240, 230);
    colors()["magenta"] = rgb_t(255, 0, 255);
    colors()["magenta1"] = rgb_t(255, 0, 255);
    colors()["magenta2"] = rgb_t(238, 0, 238);
    colors()["magenta3"] = rgb_t(205, 0, 205);
    colors()["magenta4"] = rgb_t(139, 0, 139);
    colors()["maroon"] = rgb_t(176, 48, 96);
    colors()["maroon1"] = rgb_t(255, 52, 179);
    colors()["maroon2"] = rgb_t(238, 48, 167);
    colors()["maroon3"] = rgb_t(205, 41, 144);
    colors()["maroon4"] = rgb_t(139, 28, 98);
    colors()["mediumaquamarine"] = rgb_t(102, 205, 170);
    colors()["mediumblue"] = rgb_t(0, 0, 205);
    colors()["mediumorchid"] = rgb_t(186, 85, 211);
    colors()["mediumorchid1"] = rgb_t(224, 102, 255);
    colors()["mediumorchid2"] = rgb_t(209, 95, 238);
    colors()["mediumorchid3"] = rgb_t(180, 82, 205);
    colors()["mediumorchid4"] = rgb_t(122, 55, 139);
    colors()["mediumpurple"] = rgb_t(147, 112, 219);
    colors()["mediumpurple1"] = rgb_t(171, 130, 255);
    colors()["mediumpurple2"] = rgb_t(159, 121, 238);
    colors()["mediumpurple3"] = rgb_t(137, 104, 205);
    colors()["mediumpurple4"] = rgb_t(93, 71, 139);
    colors()["mediumseagreen"] = rgb_t(60, 179, 113);
    colors()["mediumslateblue"] = rgb_t(123, 104, 238);
    colors()["mediumspringgreen"] = rgb_t(0, 250, 154);
    colors()["mediumturquoise"] = rgb_t(72, 209, 204);
    colors()["mediumvioletred"] = rgb_t(199, 21, 133);
    colors()["midnightblue"] = rgb_t(25, 25, 112);
    colors()["mintcream"] = rgb_t(245, 255, 250);
    colors()["mistyrose"] = rgb_t(255, 228, 225);
    colors()["mistyrose1"] = rgb_t(255, 228, 225);
    colors()["mistyrose2"] = rgb_t(238, 213, 210);
    colors()["mistyrose3"] = rgb_t(205, 183, 181);
    colors()["mistyrose4"] = rgb_t(139, 125, 123);
    colors()["moccasin"] = rgb_t(255, 228, 181);
    colors()["navajowhite"] = rgb_t(255, 222, 173);
    colors()["navajowhite1"] = rgb_t(255, 222, 173);
    colors()["navajowhite2"] = rgb_t(238, 207, 161);
    colors()["navajowhite3"] = rgb_t(205, 179, 139);
    colors()["navajowhite4"] = rgb_t(139, 121, 94);
    colors()["navy"] = rgb_t(0, 0, 128);
    colors()["navyblue"] = rgb_t(0, 0, 128);
    colors()["oldlace"] = rgb_t(253, 245, 230);
    colors()["olive"] = rgb_t(128, 128, 0);
    colors()["olivedrab"] = rgb_t(107, 142, 35);
    colors()["olivedrab1"] = rgb_t(192, 255, 62);
    colors()["olivedrab2"] = rgb_t(179, 238, 58);
    colors()["olivedrab3"] = rgb_t(154, 205, 50);
    colors()["olivedrab4"] = rgb_t(105, 139, 34);
    colors()["orange"] = rgb_t(255, 165, 0);
    colors()["orange1"] = rgb_t(255, 165, 0);
    colors()["orange2"] = rgb_t(238, 154, 0);
    colors()["orange3"] = rgb_t(205, 133, 0);
    colors()["orange4"] = rgb_t(139, 90, 0);
    colors()["orangered"] = rgb_t(255, 69, 0);
    colors()["orangered1"] = rgb_t(255, 69, 0);
    colors()["orangered2"] = rgb_t(238, 64, 0);
    colors()["orangered3"] = rgb_t(205, 55, 0);
    colors()["orangered4"] = rgb_t(139, 37, 0);
    colors()["orchid"] = rgb_t(218, 112, 214);
    colors()["orchid1"] = rgb_t(255, 131, 250);
    colors()["orchid2"] = rgb_t(238, 122, 233);
    colors()["orchid3"] = rgb_t(205, 105, 201);
    colors()["orchid4"] = rgb_t(139, 71, 137);
    colors()["palegoldenrod"] = rgb_t(238, 232, 170);
    colors()["palegreen"] = rgb_t(152, 251, 152);
    colors()["palegreen1"] = rgb_t(154, 255, 154);
    colors()["palegreen2"] = rgb_t(144, 238, 144);
    colors()["palegreen3"] = rgb_t(124, 205, 124);
    colors()["palegreen4"] = rgb_t(84, 139, 84);
    colors()["paleturquoise"] = rgb_t(175, 238, 238);
    colors()["paleturquoise1"] = rgb_t(187, 255, 255);
    colors()["paleturquoise2"] = rgb_t(174, 238, 238);
    colors()["paleturquoise3"] = rgb_t(150, 205, 205);
    colors()["paleturquoise4"] = rgb_t(102, 139, 139);
    colors()["palevioletred"] = rgb_t(219, 112, 147);
    colors()["palevioletred1"] = rgb_t(255, 130, 171);
    colors()["palevioletred2"] = rgb_t(238, 121, 159);
    colors()["palevioletred3"] = rgb_t(205, 104, 137);
    colors()["palevioletred4"] = rgb_t(139, 71, 93);
    colors()["papayawhip"] = rgb_t(255, 239, 213);
    colors()["peachpuff"] = rgb_t(255, 218, 185);
    colors()["peachpuff1"] = rgb_t(255, 218, 185);
    colors()["peachpuff2"] = rgb_t(238, 203, 173);
    colors()["peachpuff3"] = rgb_t(205, 175, 149);
    colors()["peachpuff4"] = rgb_t(139, 119, 101);
    colors()["peru"] = rgb_t(205, 133, 63);
    colors()["pink"] = rgb_t(255, 192, 203);
    colors()["pink1"] = rgb_t(255, 181, 197);
    colors()["pink2"] = rgb_t(238, 169, 184);
    colors()["pink3"] = rgb_t(205, 145, 158);
    colors()["pink4"] = rgb_t(139, 99, 108);
    colors()["plum"] = rgb_t(221, 160, 221);
    colors()["plum1"] = rgb_t(255, 187, 255);
    colors()["plum2"] = rgb_t(238, 174, 238);
    colors()["plum3"] = rgb_t(205, 150, 205);
    colors()["plum4"] = rgb_t(139, 102, 139);
    colors()["powderblue"] = rgb_t(176, 224, 230);
    colors()["purple"] = rgb_t(160, 32, 240);
    colors()["purple1"] = rgb_t(155, 48, 255);
    colors()["purple2"] = rgb_t(145, 44, 238);
    colors()["purple3"] = rgb_t(125, 38, 205);
    colors()["purple4"] = rgb_t(85, 26, 139);
    colors()["rebeccapurple"] = rgb_t(102, 51, 153);
    colors()["red"] = rgb_t(255, 0, 0);
    colors()["red1"] = rgb_t(255, 0, 0);
    colors()["red2"] = rgb_t(238, 0, 0);
    colors()["red3"] = rgb_t(205, 0, 0);
    colors()["red4"] = rgb_t(139, 0, 0);
    colors()["rosybrown"] = rgb_t(188, 143, 143);
    colors()["rosybrown1"] = rgb_t(255, 193, 193);
    colors()["rosybrown2"] = rgb_t(238, 180, 180);
    colors()["rosybrown3"] = rgb_t(205, 155, 155);
    colors()["rosybrown4"] = rgb_t(139, 105, 105);
    colors()["royalblue"] = rgb_t(65, 105, 225);
    colors()["royalblue1"] = rgb_t(72, 118, 255);
    colors()["royalblue2"] = rgb_t(67, 110, 238);
    colors()["royalblue3"] = rgb_t(58, 95, 205);
    colors()["royalblue4"] = rgb_t(39, 64, 139);
    colors()["saddlebrown"] = rgb_t(139, 69, 19);
    colors()["salmon"] = rgb_t(250, 128, 114);
    colors()["salmon1"] = rgb_t(255, 140, 105);
    colors()["salmon2"] = rgb_t(238, 130, 98);
    colors()["salmon3"] = rgb_t(205, 112, 84);
    colors()["salmon4"] = rgb_t(139, 76, 57);
    colors()["sandybrown"] = rgb_t(244, 164, 96);
    colors()["seagreen"] = rgb_t(46, 139, 87);
    colors()["seagreen1"] = rgb_t(84, 255, 159);
    colors()["seagreen2"] = rgb_t(78, 238, 148);
    colors()["seagreen3"] = rgb_t(67, 205, 128);
    colors()["seagreen4"] = rgb_t(46, 139, 87);
    colors()["seashell"] = rgb_t(255, 245, 238);
    colors()["seashell1"] = rgb_t(255, 245, 238);
    colors()["seashell2"] = rgb_t(238, 229, 222);
    colors()["seashell3"] = rgb_t(205, 197, 191);
    colors()["seashell4"] = rgb_t(139, 134, 130);
    colors()["sienna"] = rgb_t(160, 82, 45);
    colors()["sienna1"] = rgb_t(255, 130, 71);
    colors()["sienna2"] = rgb_t(238, 121, 66);
    colors()["sienna3"] = rgb_t(205, 104, 57);
    colors()["sienna4"] = rgb_t(139, 71, 38);
    colors()["silver"] = rgb_t(192, 192, 192);
    colors()["skyblue"] = rgb_t(135, 206, 235);
    colors()["skyblue1"] = rgb_t(135, 206, 255);
    colors()["skyblue2"] = rgb_t(126, 192, 238);
    colors()["skyblue3"] = rgb_t(108, 166, 205);
    colors()["skyblue4"] = rgb_t(74, 112, 139);
    colors()["slateblue"] = rgb_t(106, 90, 205);
    colors()["slateblue1"] = rgb_t(131, 111, 255);
    colors()["slateblue2"] = rgb_t(122, 103, 238);
    colors()["slateblue3"] = rgb_t(105, 89, 205);
    colors()["slateblue4"] = rgb_t(71, 60, 139);
    colors()["slategray"] = rgb_t(112, 128, 144);
    colors()["slategray1"] = rgb_t(198, 226, 255);
    colors()["slategray2"] = rgb_t(185, 211, 238);
    colors()["slategray3"] = rgb_t(159, 182, 205);
    colors()["slategray4"] = rgb_t(108, 123, 139);
    colors()["slategrey"] = rgb_t(112, 128, 144);
    colors()["snow"] = rgb_t(255, 250, 250);
    colors()["snow1"] = rgb_t(255, 250, 250);
    colors()["snow2"] = rgb_t(238, 233, 233);
    colors()["snow3"] = rgb_t(205, 201, 201);
    colors()["snow4"] = rgb_t(139, 137, 137);
    colors()["springgreen"] = rgb_t(0, 255, 127);
    colors()["springgreen1"] = rgb_t(0, 255, 127);
    colors()["springgreen2"] = rgb_t(0, 238, 118);
    colors()["springgreen3"] = rgb_t(0, 205, 102);
    colors()["springgreen4"] = rgb_t(0, 139, 69);
    colors()["steelblue"] = rgb_t(70, 130, 180);
    colors()["steelblue1"] = rgb_t(99, 184, 255);
    colors()["steelblue2"] = rgb_t(92, 172, 238);
    colors()["steelblue3"] = rgb_t(79, 148, 205);
    colors()["steelblue4"] = rgb_t(54, 100, 139);
    colors()["tan"] = rgb_t(210, 180, 140);
    colors()["tan1"] = rgb_t(255, 165, 79);
    colors()["tan2"] = rgb_t(238, 154, 73);
    colors()["tan3"] = rgb_t(205, 133, 63);
    colors()["tan4"] = rgb_t(139, 90, 43);
    colors()["teal"] = rgb_t(0, 128, 128);
    colors()["thistle"] = rgb_t(216, 191, 216);
    colors()["thistle1"] = rgb_t(255, 225, 255);
    colors()["thistle2"] = rgb_t(238, 210, 238);
    colors()["thistle3"] = rgb_t(205, 181, 205);
    colors()["thistle4"] = rgb_t(139, 123, 139);
    colors()["tomato"] = rgb_t(255, 99, 71);
    colors()["tomato1"] = rgb_t(255, 99, 71);
    colors()["tomato2"] = rgb_t(238, 92, 66);
    colors()["tomato3"] = rgb_t(205, 79, 57);
    colors()["tomato4"] = rgb_t(139, 54, 38);
    colors()["turquoise"] = rgb_t(64, 224, 208);
    colors()["turquoise1"] = rgb_t(0, 245, 255);
    colors()["turquoise2"] = rgb_t(0, 229, 238);
    colors()["turquoise3"] = rgb_t(0, 197, 205);
    colors()["turquoise4"] = rgb_t(0, 134, 139);
    colors()["violet"] = rgb_t(238, 130, 238);
    colors()["violetred"] = rgb_t(208, 32, 144);
    colors()["violetred1"] = rgb_t(255, 62, 150);
    colors()["violetred2"] = rgb_t(238, 58, 140);
    colors()["violetred3"] = rgb_t(205, 50, 120);
    colors()["violetred4"] = rgb_t(139, 34, 82);
    colors()["webgray"] = rgb_t(128, 128, 128);
    colors()["webgreen"] = rgb_t(0, 128, 0);
    colors()["webgrey"] = rgb_t(128, 128, 128);
    colors()["webmaroon"] = rgb_t(128, 0, 0);
    colors()["webpurple"] = rgb_t(128, 0, 128);
    colors()["wheat"] = rgb_t(245, 222, 179);
    colors()["wheat1"] = rgb_t(255, 231, 186);
    colors()["wheat2"] = rgb_t(238, 216, 174);
    colors()["wheat3"] = rgb_t(205, 186, 150);
    colors()["wheat4"] = rgb_t(139, 126, 102);
    colors()["white"] = rgb_t(255, 255, 255);
    colors()["whitesmoke"] = rgb_t(245, 245, 245);
    colors()["x11gray"] = rgb_t(190, 190, 190);
    colors()["x11green"] = rgb_t(0, 255, 0);
    colors()["x11grey"] = rgb_t(190, 190, 190);
    colors()["x11maroon"] = rgb_t(176, 48, 96);
    colors()["x11purple"] = rgb_t(160, 32, 240);
    colors()["yellow"] = rgb_t(255, 255, 0);
    colors()["yellow1"] = rgb_t(255, 255, 0);
    colors()["yellow2"] = rgb_t(238, 238, 0);
    colors()["yellow3"] = rgb_t(205, 205, 0);
    colors()["yellow4"] = rgb_t(139, 139, 0);
    colors()["yellowgreen"] = rgb_t(154, 205, 50);

    // SVG colors

    colors()["aliceblue"] = rgb_t(240, 248, 255);
    colors()["antiquewhite"] = rgb_t(250, 235, 215);
    colors()["aqua"] = rgb_t(0, 255, 255);
    colors()["aquamarine"] = rgb_t(127, 255, 212);
    colors()["azure"] = rgb_t(240, 255, 255);
    colors()["beige"] = rgb_t(245, 245, 220);
    colors()["bisque"] = rgb_t(255, 228, 196);
    colors()["black"] = rgb_t(0, 0, 0);
    colors()["blanchedalmond"] = rgb_t(255, 235, 205);
    colors()["blue"] = rgb_t(0, 0, 255);
    colors()["blueviolet"] = rgb_t(138, 43, 226);
    colors()["brown"] = rgb_t(165, 42, 42);
    colors()["burlywood"] = rgb_t(222, 184, 135);
    colors()["cadetblue"] = rgb_t(95, 158, 160);
    colors()["chartreuse"] = rgb_t(127, 255, 0);
    colors()["chocolate"] = rgb_t(210, 105, 30);
    colors()["coral"] = rgb_t(255, 127, 80);
    colors()["cornflowerblue"] = rgb_t(100, 149, 237);
    colors()["cornsilk"] = rgb_t(255, 248, 220);
    colors()["crimson"] = rgb_t(220, 20, 60);
    colors()["cyan"] = rgb_t(0, 255, 255);
    colors()["darkblue"] = rgb_t(0, 0, 139);
    colors()["darkcyan"] = rgb_t(0, 139, 139);
    colors()["darkgoldenrod"] = rgb_t(184, 134, 11);
    colors()["darkgray"] = rgb_t(169, 169, 169);
    colors()["darkgreen"] = rgb_t(0, 100, 0);
    colors()["darkgrey"] = rgb_t(169, 169, 169);
    colors()["darkkhaki"] = rgb_t(189, 183, 107);
    colors()["darkmagenta"] = rgb_t(139, 0, 139);
    colors()["darkolivegreen"] = rgb_t(85, 107, 47);
    colors()["darkorange"] = rgb_t(255, 140, 0);
    colors()["darkorchid"] = rgb_t(153, 50, 204);
    colors()["darkred"] = rgb_t(139, 0, 0);
    colors()["darksalmon"] = rgb_t(233, 150, 122);
    colors()["darkseagreen"] = rgb_t(143, 188, 143);
    colors()["darkslateblue"] = rgb_t(72, 61, 139);
    colors()["darkslategray"] = rgb_t(47, 79, 79);
    colors()["darkslategrey"] = rgb_t(47, 79, 79);
    colors()["darkturquoise"] = rgb_t(0, 206, 209);
    colors()["darkviolet"] = rgb_t(148, 0, 211);
    colors()["deeppink"] = rgb_t(255, 20, 147);
    colors()["deepskyblue"] = rgb_t(0, 191, 255);
    colors()["dimgray"] = rgb_t(105, 105, 105);
    colors()["dimgrey"] = rgb_t(105, 105, 105);
    colors()["dodgerblue"] = rgb_t(30, 144, 255);
    colors()["firebrick"] = rgb_t(178, 34, 34);
    colors()["floralwhite"] = rgb_t(255, 250, 240);
    colors()["forestgreen"] = rgb_t(34, 139, 34);
    colors()["fuchsia"] = rgb_t(255, 0, 255);
    colors()["gainsboro"] = rgb_t(220, 220, 220);
    colors()["ghostwhite"] = rgb_t(248, 248, 255);
    colors()["gold"] = rgb_t(255, 215, 0);
    colors()["goldenrod"] = rgb_t(218, 165, 32);
    colors()["gray"] = rgb_t(128, 128, 128);
    colors()["grey"] = rgb_t(128, 128, 128);
    colors()["green"] = rgb_t(0, 128, 0);
    colors()["greenyellow"] = rgb_t(173, 255, 47);
    colors()["honeydew"] = rgb_t(240, 255, 240);
    colors()["hotpink"] = rgb_t(255, 105, 180);
    colors()["indianred"] = rgb_t(205, 92, 92);
    colors()["indigo"] = rgb_t(75, 0, 130);
    colors()["ivory"] = rgb_t(255, 255, 240);
    colors()["khaki"] = rgb_t(240, 230, 140);
    colors()["lavender"] = rgb_t(230, 230, 250);
    colors()["lavenderblush"] = rgb_t(255, 240, 245);
    colors()["lawngreen"] = rgb_t(124, 252, 0);
    colors()["lemonchiffon"] = rgb_t(255, 250, 205);
    colors()["lightblue"] = rgb_t(173, 216, 230);
    colors()["lightcoral"] = rgb_t(240, 128, 128);
    colors()["lightcyan"] = rgb_t(224, 255, 255);
    colors()["lightgoldenrodyellow"] = rgb_t(250, 250, 210);
    colors()["lightgray"] = rgb_t(211, 211, 211);
    colors()["lightgreen"] = rgb_t(144, 238, 144);
    colors()["lightgrey"] = rgb_t(211, 211, 211);
    colors()["lightpink"] = rgb_t(255, 182, 193);
    colors()["lightsalmon"] = rgb_t(255, 160, 122);
    colors()["lightseagreen"] = rgb_t(32, 178, 170);
    colors()["lightskyblue"] = rgb_t(135, 206, 250);
    colors()["lightslategray"] = rgb_t(119, 136, 153);
    colors()["lightslategrey"] = rgb_t(119, 136, 153);
    colors()["lightsteelblue"] = rgb_t(176, 196, 222);
    colors()["lightyellow"] = rgb_t(255, 255, 224);
    colors()["lime"] = rgb_t(0, 255, 0);
    colors()["limegreen"] = rgb_t(50, 205, 50);
    colors()["linen"] = rgb_t(250, 240, 230);
    colors()["magenta"] = rgb_t(255, 0, 255);
    colors()["maroon"] = rgb_t(128, 0, 0);
    colors()["mediumaquamarine"] = rgb_t(102, 205, 170);
    colors()["mediumblue"] = rgb_t(0, 0, 205);
    colors()["mediumorchid"] = rgb_t(186, 85, 211);
    colors()["mediumpurple"] = rgb_t(147, 112, 219);
    colors()["mediumseagreen"] = rgb_t(60, 179, 113);
    colors()["mediumslateblue"] = rgb_t(123, 104, 238);
    colors()["mediumspringgreen"] = rgb_t(0, 250, 154);
    colors()["mediumturquoise"] = rgb_t(72, 209, 204);
    colors()["mediumvioletred"] = rgb_t(199, 21, 133);
    colors()["midnightblue"] = rgb_t(25, 25, 112);
    colors()["mintcream"] = rgb_t(245, 255, 250);
    colors()["mistyrose"] = rgb_t(255, 228, 225);
    colors()["moccasin"] = rgb_t(255, 228, 181);
    colors()["navajowhite"] = rgb_t(255, 222, 173);
    colors()["navy"] = rgb_t(0, 0, 128);
    colors()["oldlace"] = rgb_t(253, 245, 230);
    colors()["olive"] = rgb_t(128, 128, 0);
    colors()["olivedrab"] = rgb_t(107, 142, 35);
    colors()["orange"] = rgb_t(255, 165, 0);
    colors()["orangered"] = rgb_t(255, 69, 0);
    colors()["orchid"] = rgb_t(218, 112, 214);
    colors()["palegoldenrod"] = rgb_t(238, 232, 170);
    colors()["palegreen"] = rgb_t(152, 251, 152);
    colors()["paleturquoise"] = rgb_t(175, 238, 238);
    colors()["palevioletred"] = rgb_t(219, 112, 147);
    colors()["papayawhip"] = rgb_t(255, 239, 213);
    colors()["peachpuff"] = rgb_t(255, 218, 185);
    colors()["peru"] = rgb_t(205, 133, 63);
    colors()["pink"] = rgb_t(255, 192, 203);
    colors()["plum"] = rgb_t(221, 160, 221);
    colors()["powderblue"] = rgb_t(176, 224, 230);
    colors()["purple"] = rgb_t(128, 0, 128);
    colors()["red"] = rgb_t(255, 0, 0);
    colors()["rosybrown"] = rgb_t(188, 143, 143);
    colors()["royalblue"] = rgb_t(65, 105, 225);
    colors()["saddlebrown"] = rgb_t(139, 69, 19);
    colors()["salmon"] = rgb_t(250, 128, 114);
    colors()["sandybrown"] = rgb_t(244, 164, 96);
    colors()["seagreen"] = rgb_t(46, 139, 87);
    colors()["seashell"] = rgb_t(255, 245, 238);
    colors()["sienna"] = rgb_t(160, 82, 45);
    colors()["silver"] = rgb_t(192, 192, 192);
    colors()["skyblue"] = rgb_t(135, 206, 235);
    colors()["slateblue"] = rgb_t(106, 90, 205);
    colors()["slategray"] = rgb_t(112, 128, 144);
    colors()["slategrey"] = rgb_t(112, 128, 144);
    colors()["snow"] = rgb_t(255, 250, 250);
    colors()["springgreen"] = rgb_t(0, 255, 127);
    colors()["steelblue"] = rgb_t(70, 130, 180);
    colors()["tan"] = rgb_t(210, 180, 140);
    colors()["teal"] = rgb_t(0, 128, 128);
    colors()["thistle"] = rgb_t(216, 191, 216);
    colors()["tomato"] = rgb_t(255, 99, 71);
    colors()["turquoise"] = rgb_t(64, 224, 208);
    colors()["violet"] = rgb_t(238, 130, 238);
    colors()["wheat"] = rgb_t(245, 222, 179);
    colors()["white"] = rgb_t(255, 255, 255);
    colors()["whitesmoke"] = rgb_t(245, 245, 245);
    colors()["yellow"] = rgb_t(255, 255, 0);
    colors()["yellowgreen"] = rgb_t(154, 205, 50);
}

}  // namespace common
}  // namespace omnetpp

#ifndef __FIGUREAPI_FIGURES_H_
#define __FIGUREAPI_FIGURES_H_

#include <omnetpp.h>

using namespace omnetpp;


template <typename T>
static std::string getEnumString(T value)
{
    auto cenum = cEnum::find(opp_typename(typeid(T)));
    return cenum ? cenum->getStringFor(value) : "unknown";
}


class AxesFigure : public cGroupFigure {
public:
    explicit AxesFigure(const char *name = "axes") : cGroupFigure(name) {
        cLineFigure *horiz = new cLineFigure("x_axis");
        horiz->setStart(Point(0, 0));
        horiz->setEnd(Point(10, 0));
        horiz->setEndArrowhead(cFigure::ARROW_BARBED);
        horiz->setLineOpacity(0.75);
        horiz->setLineColor("red");
        horiz->setLineWidth(2);
        addFigure(horiz);

        cLineFigure *vert = new cLineFigure("y_axis");
        vert->setStart(Point(0, 0));
        vert->setEnd(Point(0, 10));
        vert->setEndArrowhead(cFigure::ARROW_BARBED);
        vert->setLineOpacity(0.75);
        vert->setLineColor("green");
        vert->setLineWidth(2);
        addFigure(vert);
    }
};

class GridFigure : public cGroupFigure {
public:
    explicit GridFigure(const char *name, const char *color) : cGroupFigure(name) {
        for (int i = 0; i <= 20; ++i) {
            cLineFigure *horiz = new cLineFigure(("line_horiz_" + std::to_string(i)).c_str());
            horiz->setStart({0.0, i*10.0});
            horiz->setEnd({300.0, i*10.0});
            horiz->setLineOpacity(0.75);
            horiz->setLineColor(color);
            addFigure(horiz);
        }
        for (int i = 0; i <= 30; ++i) {
            cLineFigure *vert = new cLineFigure(("line_vert_" + std::to_string(i)).c_str());
            vert->setStart({i*10.0, 0.0});
            vert->setEnd({i*10.0, 200.0});
            vert->setLineOpacity(0.75);
            vert->setLineColor(color);
            addFigure(vert);
        }
    }
};


#endif

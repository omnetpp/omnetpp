//==========================================================================
//  HISTOGRAMINSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cmath>
#include "omnetpp/cabstracthistogram.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspectorfactory.h"
#include "histograminspector.h"

namespace omnetpp {
namespace tkenv {

void _dummy_for_histograminspector() {}

class HistogramInspectorFactory : public InspectorFactory
{
  public:
    HistogramInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cAbstractHistogram *>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector() override { return new HistogramInspector(this); }
};

Register_InspectorFactory(HistogramInspectorFactory);

HistogramInspector::HistogramInspector(InspectorFactory *f) : Inspector(f)
{
}

void HistogramInspector::createWindow(const char *window, const char *geometry)
{
    Inspector::createWindow(window, geometry);

    strcpy(canvas, windowName);
    strcat(canvas, ".main.canvas");

    CHK(Tcl_VarEval(interp, "createHistogramInspector ", windowName, " ", TclQuotedString(geometry).get(), TCL_NULL));
}

void HistogramInspector::useWindow(const char *window)
{
    Inspector::useWindow(window);

    strcpy(canvas, windowName);
    strcat(canvas, ".main.canvas");
}

void HistogramInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    cAbstractHistogram *distr = static_cast<cAbstractHistogram *>(object);

    char buf[80];
    generalInfo(buf);
    CHK(Tcl_VarEval(interp, windowName, ".bot.info config -text {", buf, "}", TCL_NULL));

    // can we draw anything at all?
    if (!distr->binsAlreadySetUp() || distr->getNumBins() == 0)
        return;

    long num_vals = distr->getCount();
    int basepts = distr->getNumBins()+1;
    int bin;
    double cell_lower, cell_upper;

    double xmin = distr->getBinEdge(0);
    double xrange = distr->getBinEdge(basepts-1) - xmin;

    // determine maximum height (will be used for y scaling)
    double ymax = -1.0;  // a good start because all y values are >=0
    cell_upper = distr->getBinEdge(0);
    for (bin = 0; bin < basepts-1; bin++) {
        // get bin
        cell_lower = cell_upper;
        cell_upper = distr->getBinEdge(bin+1);
        // calculate height
        double y = distr->getBinValue(bin) / (double)(num_vals) / (cell_upper-cell_lower);
        if (y > ymax)
            ymax = y;
    }

    // get canvas size
    CHK(Tcl_VarEval(interp, "winfo width ", canvas, TCL_NULL));
    int canvaswidth = atoi(Tcl_GetStringResult(interp));
    CHK(Tcl_VarEval(interp, "winfo height ", canvas, TCL_NULL));
    int canvasheight = atoi(Tcl_GetStringResult(interp));

    // temporarily define X() and Y() coordinate translation macros
#define X(x)    (int)(10+((x)-xmin)*((long)canvaswidth-20)/xrange)
#define Y(y)    (int)(canvasheight-10-(y)*((long)canvasheight-20)/ymax)

    // delete previous drawing
    CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));

    // draw the histogram
    cell_upper = distr->getBinEdge(0);
    for (bin = 0; bin < basepts-1; bin++) {
        char tag[16];
        sprintf(tag, "bin%d", bin);

        // get bin
        cell_lower = cell_upper;
        cell_upper = distr->getBinEdge(bin+1);
        // calculate height
        double y = distr->getBinValue(bin) / (double)(num_vals) / (cell_upper-cell_lower);
        // prepare rectangle coordinates
        char coords[64];
        sprintf(coords, "%d %d %d %d", X(cell_lower), Y(0), X(cell_upper), Y(y));
        // draw rectangle
        CHK(Tcl_VarEval(interp, canvas,
                        " create rect ", coords, " -tag ", tag,
                        " -fill black -outline black", TCL_NULL));
    }
#undef X
#undef Y
}

void HistogramInspector::generalInfo(char *buf)
{
    cAbstractHistogram *d = static_cast<cAbstractHistogram *>(object);
    if (!d->binsAlreadySetUp())
        sprintf(buf, "(collecting initial values, N=%ld)", d->getCount());
    else
        sprintf(buf, "Histogram: (%g...%g)  N=%ld  #bins=%d",
                d->getBinEdge(0), d->getBinEdge(d->getNumBins()),
                d->getCount(),
                d->getNumBins()
                );
}

void HistogramInspector::getCellInfo(char *buf, int bin)
{
    cAbstractHistogram *d = static_cast<cAbstractHistogram *>(object);
    double count = d->getBinValue(bin);
    double cell_lower = d->getBinEdge(bin);
    double cell_upper = d->getBinEdge(bin+1);
    sprintf(buf, "Cell #%d:  (%g...%g)  n=%g  PDF=%g",
            bin,
            cell_lower, cell_upper,
            count,
            count / (double)(d->getCount()) / (cell_upper-cell_lower)
            );
}

int HistogramInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
        return TCL_ERROR;
    }

    if (strcmp(argv[0], "bin") == 0) {  // 'opp_inspectorcommand <inspector> bin ...'
        if (argc > 2) {
            Tcl_SetResult(interp, TCLCONST("wrong argcount"), TCL_STATIC);
            return TCL_ERROR;
        }

        char buf[128];
        if (argc == 1)
            generalInfo(buf);
        else
            getCellInfo(buf, atoi(argv[1]));
        Tcl_SetResult(interp, buf, TCL_VOLATILE);
        return TCL_OK;
    }

    return Inspector::inspectorCommand(argc, argv);
}

}  // namespace tkenv
}  // namespace omnetpp


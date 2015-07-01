//==========================================================================
//  CANVASINSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "common/stringutil.h"
#include "canvasinspector.h"
#include "figurerenderers.h"
#include "tklib.h"
#include "qtutil.h"
#include "inspectorfactory.h"
#include "canvasrenderer.h"
#include "graphicsscene.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGridLayout>

using namespace OPP::common;

namespace omnetpp {
namespace qtenv {

class CanvasInspectorFactory : public InspectorFactory
{
  public:
    CanvasInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cCanvas *>(obj) != nullptr; }
    int getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new CanvasInspector(parent, isTopLevel, this); }
};

Register_InspectorFactory(CanvasInspectorFactory);

CanvasInspector::CanvasInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
    canvasRenderer = new CanvasRenderer();

    graphicsView = new QGraphicsView(this);
    graphicsView->setScene(new QGraphicsScene(graphicsView));
    canvasRenderer->setQtCanvas(graphicsView->scene(), getCanvas());

    auto layout = new QGridLayout(this);
    layout->addWidget(graphicsView, 0, 0, 1, 1);
    layout->setMargin(0);
}

CanvasInspector::~CanvasInspector()
{
    delete canvasRenderer;
}

void CanvasInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    canvasRenderer->setCanvas(getCanvas());

    CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));

    if (object) {
        FigureRenderingHints hint;
        canvasRenderer->redraw(&hint);  // TODO CHK(Tcl_VarEval(interp, "CanvasInspector:onSetObject ", windowName, nullptr ));
    }
}

void CanvasInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    updateBackgroundColor();

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->refresh(&hints);
}

void CanvasInspector::redraw()
{
    updateBackgroundColor();

    FigureRenderingHints hints;
    fillFigureRenderingHints(&hints);
    canvasRenderer->redraw(&hints);
}

void CanvasInspector::clearObjectChangeFlags()
{
    if (object)
        getCanvas()->getRootFigure()->clearChangeFlags();
}

cCanvas *CanvasInspector::getCanvas()
{
    return static_cast<cCanvas *>(object);
}

void CanvasInspector::fillFigureRenderingHints(FigureRenderingHints *hints)
{
    const char *s;

    // read $inspectordata($c:zoomfactor)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":zoomfactor").c_str()), TCL_GLOBAL_ONLY);
    hints->zoom = opp_atof(s);

    // read $inspectordata($c:imagesizefactor)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":imagesizefactor").c_str()), TCL_GLOBAL_ONLY);
    hints->iconMagnification = opp_atof(s);

    // read $inspectordata($c:showlabels)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":showlabels").c_str()), TCL_GLOBAL_ONLY);
    hints->showSubmoduleLabels = opp_atol(s) != 0;

    // read $inspectordata($c:showarrowheads)
    s = Tcl_GetVar2(interp, "inspectordata", TCLCONST((std::string(canvas)+":showarrowheads").c_str()), TCL_GLOBAL_ONLY);
    hints->showArrowHeads = opp_atol(s) != 0;

    Tcl_Eval(interp, "font actual CanvasFont -family");
    hints->defaultFont = Tcl_GetStringResult(interp);

    Tcl_Eval(interp, "font actual CanvasFont -size");
    s = Tcl_GetStringResult(interp);
    hints->defaultFontSize = opp_atol(s) * 16 / 10;  // FIXME figure out conversion factor (point to pixel?)...
}

void CanvasInspector::updateBackgroundColor()
{
    cCanvas *canvas = getCanvas();
    if (canvas) {
        char buf[16];
        cFigure::Color color = canvas->getBackgroundColor();
        sprintf(buf, "#%2.2x%2.2x%2.2x", color.red, color.green, color.blue);
        CHK(Tcl_VarEval(interp, this->canvas, " config -bg {", buf, "}", TCL_NULL));
    }
}

int CanvasInspector::inspectorCommand(int argc, const char **argv)
{
    if (argc < 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    E_TRY
    if (strcmp(argv[0], "redraw") == 0) {
        redraw();
        return TCL_OK;
    }
    if (strcmp(argv[0], "getalltags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getAllTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "getenabledtags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getEnabledTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "getexcepttags") == 0) {
        Tcl_SetResult(interp, TCLCONST(canvasRenderer->getExceptTags().c_str()), TCL_VOLATILE);
        return TCL_OK;
    }
    if (strcmp(argv[0], "setenabledtags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setEnabledTags(argv[1]);
        return TCL_OK;
    }
    if (strcmp(argv[0], "setexcepttags") == 0) {
        if (argc != 2) {
            Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
            return TCL_ERROR;
        }
        canvasRenderer->setExceptTags(argv[1]);
        return TCL_OK;
    }
    E_CATCH

    return Inspector::inspectorCommand(argc, argv);
}

} // namespace omnetpp
} // namespace omnetpp


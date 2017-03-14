//==========================================================================
//  GATEINSPECTOR.CC - part of
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
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "omnetpp/cchannel.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/cfutureeventset.h"
#include "gateinspector.h"
#include "qtenv.h"
#include "qtutil.h"
#include "inspectorfactory.h"

namespace omnetpp {
namespace qtenv {

void _dummy_for_gateinspector() {}

class GateInspectorFactory : public InspectorFactory
{
  public:
    GateInspectorFactory(const char *name) : InspectorFactory(name) {}

    bool supportsObject(cObject *obj) override { return dynamic_cast<cGate *>(obj) != nullptr; }
    InspectorType getInspectorType() override { return INSP_GRAPHICAL; }
    double getQualityAsDefault(cObject *object) override { return 3.0; }
    Inspector *createInspector(QWidget *parent, bool isTopLevel) override { return new GateInspector(parent, isTopLevel, this); }
};

// TODO Register_InspectorFactory(GateInspectorFactory);

GateInspector::GateInspector(QWidget *parent, bool isTopLevel, InspectorFactory *f) : Inspector(parent, isTopLevel, f)
{
}

void GateInspector::doSetObject(cObject *obj)
{
    if (obj == object)
        return;

    Inspector::doSetObject(obj);

    redraw();
}

void GateInspector::redraw()
{
    cGate *gate = (cGate *)object;

    //TCLKILL CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));

    // draw modules
    int k = 0;
    int xsiz = 0;
    char prevdir = ' ';
    cGate *g;
    for (g = gate->getPathStartGate(); g != nullptr; g = g->getNextGate(), k++) {
        if (g->getType() == prevdir)
            xsiz += (g->getType() == cGate::OUTPUT) ? 1 : -1;
        else
            prevdir = g->getType();

        /*TCLKILL char modptr[32], gateptr[32], kstr[16], xstr[16], dir[2];
        ptrToStr(g->getOwnerModule(), modptr);
        ptrToStr(g, gateptr);
        sprintf(kstr, "%d", k);
        sprintf(xstr, "%d", xsiz);
        dir[0] = g->getType();
        dir[1] = 0;
        CHK(Tcl_VarEval(interp, "GateInspector:drawModuleGate ",
                        canvas, " ",
                        modptr, " ",
                        gateptr, " ",
                        "{", g->getOwnerModule()->getFullPath().c_str(), "} ",
                        "{", g->getFullName(), "} ",
                        kstr, " ",
                        xstr, " ",
                        dir, " ",
                        g == gate ? "1" : "0",
                        TCL_NULL));
                        */
    }

    // draw connections
    for (g = gate->getPathStartGate(); g->getNextGate() != nullptr; g = g->getNextGate()) {
        /*TCLKILL char srcgateptr[32], destgateptr[32], chanptr[32];
        ptrToStr(g, srcgateptr);
        ptrToStr(g->getNextGate(), destgateptr);
        cChannel *chan = g->getChannel();
        ptrToStr(chan, chanptr);
        const char *dispstr = (chan && chan->hasDisplayString() && chan->parametersFinalized()) ? chan->getDisplayString().str() : "";
        CHK(Tcl_VarEval(interp, "GateInspector:drawConnection ",
                        canvas, " ",
                        srcgateptr, " ",
                        destgateptr, " ",
                        chanptr, " ",
                        TclQuotedString(chan ? chan->str().c_str() : "").get(), " ",
                        TclQuotedString(dispstr).get(), " ",
                        TCL_NULL));*/
    }

    // loop through all messages in the event queue
    refresh();
}

void GateInspector::refresh()
{
    Inspector::refresh();

    if (!object) {
        //TCLKILL CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
        return;
    }

    cGate *gate = static_cast<cGate *>(object);

    // redraw modules only on explicit request

    // loop through all messages in the event queue
    //TCLKILL CHK(Tcl_VarEval(interp, canvas, " delete msg msgname", TCL_NULL));
    cGate *destGate = gate->getPathEndGate();

    cFutureEventSet *fes = getSimulation()->getFES();
    int fesLen = fes->getLength();
    for (int i = 0; i < fesLen; i++) {
        cEvent *event = fes->get(i);
        if (!event->isMessage())
            continue;
        cMessage *msg = (cMessage *)event;

        if (msg->getArrivalGate() == destGate) {
            cGate *gate = msg->getArrivalGate();
            if (gate)
                gate = gate->getPreviousGate();
            if (gate) {
                char msgptr[32], gateptr[32];
                ptrToStr(msg, msgptr);
                ptrToStr(gate, gateptr);
                /*TCLKILL CHK(Tcl_VarEval(interp, "ModuleInspector:drawMessageOnGate ",
                                canvas, " ",
                                gateptr, " ",
                                msgptr,
                                TCL_NULL));*/
            }
        }
    }
}

void GateInspector::displayStringChanged(cGate *gate)
{
    // XXX should defer redraw (via redraw_needed) to avoid "flickering"
}

}  // namespace qtenv
}  // namespace omnetpp


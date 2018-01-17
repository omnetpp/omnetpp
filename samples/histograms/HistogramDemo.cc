//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Demonstrates histogram classes; see NED file for more info.
 */
class HistogramDemo : public cSimpleModule
{
  public:
    HistogramDemo() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(HistogramDemo);

void HistogramDemo::activity()
{
    int numObs = par("numObservations");
    int numBins = par("numBins");
    bool useFixedRange = par("useFixedRange");
    int numFirstVals = par("numFirstVals");
    double rangeExtFactor = par("rangeExtFactor");

    int i = 0;
    WATCH(i);

    EV << "Creating density estimation objects:\n";
    EV << "cHistogram, cPSquare, cKSplit.\n";
    EV << "Parameters:\n";
    EV << " number of bins: " << numBins << endl;
    EV << (useFixedRange ? " range: [0,100)" : "automatic range estimation") << endl;
    if (!useFixedRange) {
        EV << " observations used for range estimation: " << numFirstVals << endl;
        EV << " range will be extended by " << rangeExtFactor << " times" << endl;
    }
    EV << endl;

    cHistogram hist("Histogram");
    cPSquare psquare("PSquare", numBins);
    cKSplit ksplit("K-Split");

    if (useFixedRange) {
        hist.setStrategy(new cFixedRangeHistogramStrategy(0, 100));
        ksplit.setRange(0, 100);
    }
    else {
        auto strategy = new cAutoRangeHistogramStrategy(0, NAN, numBins);
        strategy->setNumToPrecollect(numFirstVals);
        strategy->setRangeExtensionFactor(rangeExtFactor);
        hist.setStrategy(strategy);
        ksplit.setRangeAutoUpper(0.0, numFirstVals, rangeExtFactor);
    }

    EV << "If Qtenv is used (it is the default), you may click the Children mode\n"
          "button of the object inspector, and double-click the items to open\n"
          "graphical inspectors for the distributions.\n\n";

    EV << endl;
    wait(0);

    EV << "Filling objects with " << numObs << " random observations...\n";
    EV << "(exponential(30) with P=0.5 and normal(80, 10) with P=0.5)\n";
    for (i = 0; i < numObs; i++) {
        double d = (intrand(2) == 0) ? exponential(30) : normal(80, 10);

        EV << " adding " << d << endl;
        hist.collect(d);
        psquare.collect(d);
        ksplit.collect(d);

        wait(1);
    }

    EV << endl;
    EV << "Cells in ksplit:\n";
    for (i = 0; i < ksplit.getNumBins(); i++) {
        EV << " cell " << i << ":";
        EV << " [" << ksplit.getBinEdge(i) << "," << ksplit.getBinEdge(i+1) << "]";
        EV << "  n=" << ksplit.getBinValue(i);
        EV << "  PDF=" << ksplit.getBinPDF(i) << endl;
    }

    EV << endl;
    EV << "Writing snapshot file...\n";
    snapshot(this);

    psquare.record();
    ksplit.record();
    hist.record();

    endSimulation();
}


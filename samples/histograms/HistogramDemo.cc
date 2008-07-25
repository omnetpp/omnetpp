//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

#define STACKSIZE 16384


/**
 * Demonstrates histogram classes; see NED file for more info.
 */
class HistogramDemo : public cSimpleModule
{
  public:
    HistogramDemo() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

Define_Module( HistogramDemo );

void HistogramDemo::activity()
{
    int numObs = par("numObservations");
    int numCells = par("numCells");
    bool useFixedRange = par("useFixedRange");
    int numFirstVals = par("numFirstVals");
    double rangeExtFactor = par("rangeExtFactor");

    int i=0;
    WATCH(i);

    EV << "Creating 5 distribution approximation objects, of types:\n";
    EV << "cDoubleHistogram, cLongHistogram, cVarHistogram, cPSquare, cKSplit.\n";
    EV << "Parameters:\n";
    EV << " number of cells: " << numCells << endl;
    EV << (useFixedRange ? " range: [0,100)" : "automatic range estimation") << endl;
    if (!useFixedRange)
    {
        EV << " observations used for range estimation: " << numFirstVals << endl;
        EV << " range will be extended by " << rangeExtFactor << " times" << endl;
    }
    EV << endl;

    cDoubleHistogram dblhist("DoubleHistogram", numCells);
    cLongHistogram longhist("LongHistogram", numCells);
    cVarHistogram varhist("VarHistogram", numCells,HIST_TR_AUTO_EPC_DBL);
    cPSquare psquare("PSquare", numCells);
    cKSplit ksplit("K-Split");

    if (useFixedRange)
    {
        dblhist.setRange(0,100);
        longhist.setRange(0,100);
        ksplit.setRange(0,100);
    }
    else
    {
        // 0.0 is lower limit
        dblhist.setRangeAutoUpper(0.0, numFirstVals, rangeExtFactor);
        longhist.setRangeAutoUpper(0.0, numFirstVals, rangeExtFactor);
        ksplit.setRangeAutoUpper(0.0, numFirstVals, rangeExtFactor);
    }
    varhist.setNumFirstVals(numFirstVals);

    FILE *f = fopen("hist.dat","r");
    if (f && ev.askYesNo("HIST: Saved histogram file `hist.dat' found,"
                         " load it and continue collecting from there?"))
    {
        longhist.loadFromFile(f);
        dblhist.loadFromFile(f);
        psquare.loadFromFile(f);
        varhist.loadFromFile(f);
        ksplit.loadFromFile(f);
        fclose(f);
    }

    EV << "If Tkenv is used (it is the default), you may click the Contents tab\n"
          "of the module inspector window, and double-click the items to open\n"
          "graphical inspector for the distributions.\n\n";
    EV << "An alternative is to load 'inspect.lst' by selecting Options|Load inspector list\n"
          "from the menu; this will also open the inspector windows.\n";

    EV << endl;
    wait(0);

    EV << "Filling objects with " << numObs << " random observations...\n";
    EV << "(exponential(30) with P=0.5 and normal(80, 10) with P=0.5)\n";
    for (i=0; i<numObs; i++)
    {
        double d = (intrand(2)==0) ? exponential(30) : normal(80, 10);
        EV << " adding " << d << endl;

        longhist.collect( d );
        dblhist.collect( d );
        psquare.collect( d );
        ksplit.collect( d );
        varhist.collect( d );

        wait(1);
    }

    EV << endl;
    EV << "Cells in ksplit:\n";
    for (i=0; i<ksplit.getNumCells(); i++)
    {
        EV << " cell " << i << ":";
        EV << " [" << ksplit.getBasepoint(i) << "," << ksplit.getBasepoint(i+1) << "]";
        EV << "  n=" << ksplit.getCellValue(i);
        EV << "  PDF=" << ksplit.getCellPDF(i) << endl;
    }

    EV << endl;
    EV << "Writing snapshot file...\n";
    snapshot(this);

    EV << "Saving all four objects to `hist.dat'...\n";
    f = fopen("hist.dat","w");
    longhist.saveToFile(f);
    dblhist.saveToFile(f);
    psquare.saveToFile(f);
    varhist.saveToFile(f);
    ksplit.saveToFile(f);
    fclose(f);

    longhist.record();
    dblhist.record();
    psquare.record();
    ksplit.record();
    varhist.record();

    endSimulation();
}


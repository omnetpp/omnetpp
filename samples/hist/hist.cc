//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
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
    int numobs = par("num_obs");
    int numcells = par("num_cells");
    int fixed_range = par("fixed_range");
    int numfirst = par("num_first");
    double range_ext = par("range_ext");

    int i=0;
    WATCH(i);

    ev << "Creating 5 distribution approximation objects, of types:\n";
    ev << "cDoubleHistogram, cLongHistogram, cVarHistogram, cPSquare, cKSplit.\n";
    ev << "Parameters:\n";
    ev << " number of cells: " << numcells << endl;
    ev << (fixed_range ? " range: [0,100)" : "automatic range estimation") << endl;
    if (!fixed_range)
    {
        ev << " observations used for range estimation: " << numfirst << endl;
        ev << " range will be extended by " << range_ext << " times" << endl;
    }
    ev << endl;

    cDoubleHistogram dblhist("DoubleHistogram", numcells);
    cLongHistogram longhist("LongHistogram", numcells);
    cVarHistogram varhist("VarHistogram", numcells,HIST_TR_AUTO_EPC_DBL);
    cPSquare psquare("PSquare", numcells);
    cKSplit ksplit("K-Split");

    FILE *f;

    if (fixed_range)
    {
        dblhist.setRange(0,100);
        longhist.setRange(0,100);
        ksplit.setRange(0,100);
    }
    else
    {
        // 0.0 is lower limit
        dblhist.setRangeAutoUpper(0.0, numfirst, range_ext);
        longhist.setRangeAutoUpper(0.0, numfirst, range_ext);
        ksplit.setRangeAutoUpper(0.0, numfirst, range_ext);
    }
    varhist.setNumFirstVals(numfirst);

    f=fopen("hist.dat","r");
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

    ev << "If Tkenv is used (it is the default), you may click the Objects/Watches tab\n"
          "in the module inspector window and double-click the items to open graphical\n"
          "inspector windows for the distributions.\n\n";
    ev << "An alternative is to load 'inspect.lst' by selecting Options|Load inspector list\n"
          "from the menu; this will also open the inspector windows.\n";

    ev << endl;
    wait(0);

    ev << "Filling objects with " << numobs << " random observations...\n";
    ev << "(exponential(30) with P=0.5 and normal(80, 10) with P=0.5)\n";
    for (i=0; i<numobs; i++)
    {
        double d = (intrand(2)==0) ? exponential(30) : normal(80, 10);
        ev << " adding " << d << endl;

        longhist.collect( d );
        dblhist.collect( d );
        psquare.collect( d );
        ksplit.collect( d );
        varhist.collect( d );

        wait(1);
    }

    ev << endl;
    ev << "Cells in ksplit:\n";
    for (i=0; i<ksplit.cells(); i++)
    {
        ev << " cell " << i << ":";
        ev << " [" << ksplit.basepoint(i) << "," << ksplit.basepoint(i+1) << "]";
        ev << "  n=" << ksplit.cell(i);
        ev << "  PDF=" << ksplit.cellPDF(i) << endl;
    }

    ev << endl;
    ev << "Writing snapshot file...\n";
    snapshot(this);

    ev << "Saving all four objects to `hist.dat'...\n";
    f=fopen("hist.dat","w");
    longhist.saveToFile(f);
    dblhist.saveToFile(f);
    psquare.saveToFile(f);
    varhist.saveToFile(f);
    ksplit.saveToFile(f);
    fclose(f);

    endSimulation();
}


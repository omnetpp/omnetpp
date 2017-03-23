#include <omnetpp.h>

using namespace omnetpp;

class ChartInspectorTester : public cSimpleModule
{

  public:
    ChartInspectorTester() : cSimpleModule(16384) {}
    virtual void activity();
};

Define_Module(ChartInspectorTester);

void ChartInspectorTester::activity()
{
    int numCells = 10;
    cOutVector vector("vector");
    cDoubleHistogram dblhist("DoubleHistogram", numCells);
    cLongHistogram longhist("LongHistogram", numCells);
    cVarHistogram varhist("VarHistogram", numCells, cVarHistogram::HIST_TR_AUTO_EPC_DBL);
    cPSquare psquare("PSquare", numCells);
    cKSplit ksplit("K-Split");

    while (true) {
        double value = par("value").doubleValue();
        vector.record(value);
        dblhist.collect(value);
        longhist.collect(value);
        varhist.collect(value);
        psquare.collect(value);
        ksplit.collect(value);
        wait(par("deltaT").doubleValue());
    }
}

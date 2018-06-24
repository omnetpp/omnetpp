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
    cHistogram hist("Histogram");
    cPSquare psquare("PSquare", numCells);
    cKSplit ksplit("K-Split");

    while (true) {
        double value = par("value").doubleValue();
        vector.record(value);
        hist.collect(value);
        psquare.collect(value);
        ksplit.collect(value);
        wait(par("deltaT").doubleValue());
    }
}

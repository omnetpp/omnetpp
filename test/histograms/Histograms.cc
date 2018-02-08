//
// This file is part of OMNeT++/OMNEST.
//
// Copyright (C) 1992-2018 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

using namespace omnetpp;

/**
 * Exercise histogram classes; see NED file for more info.
 */
class Histograms : public cSimpleModule
{
  protected:
    int count;
    bool weighted;
    std::vector<cAbstractHistogram*> statisticObjects;
    cOutVector valuesVector;
    cOutVector weightsVector;
    simsignal_t unweightedValueSignal;
  protected:
    void addHistogram(const char *name, cIHistogramStrategy *strategy);
    void createStatisticObjects();
  public:
    Histograms() {}
    virtual ~Histograms();
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
};

Define_Module(Histograms);

void Histograms::initialize()
{
    valuesVector.setName("values");
    weightsVector.setName("weights");

    count = par("count");
    weighted = par("weighted");

    createStatisticObjects();

    unweightedValueSignal = registerSignal("unweightedValue");

    EV << "Created " << statisticObjects.size() << " statistics objects.\n\n";

    EV << "If Qtenv is used (it is the default), you may click the Children mode\n"
          "button of the object inspector, and double-click the items to open\n"
          "graphical inspectors for the distributions.\n\n";

    EV << "Collecting variables from: " << par("variable").str() << "\n";

    scheduleAt(0, new cMessage);
}

struct AutoRangeStrategyBuilder {
    cAutoRangeHistogramStrategy *strategy = new cAutoRangeHistogramStrategy();
    AutoRangeStrategyBuilder(cHistogram::Mode mode=cHistogram::MODE_AUTO) {
        strategy->setMode(mode);
    }
    AutoRangeStrategyBuilder& setRange(double lo, double hi, double rangeExtensionFactor=NAN) {
        strategy->setRangeHint(lo, hi);
        if (!std::isnan(rangeExtensionFactor))
            strategy->setRangeExtensionFactor(rangeExtensionFactor);
        return *this;
    }
    AutoRangeStrategyBuilder& setNumBins(int numBins, int maxNumBins=10000) {
        strategy->setNumBinsHint(numBins);
        strategy->setMaxNumBins(maxNumBins);
        return *this;
    }
    AutoRangeStrategyBuilder& setBinSize(double binSize, bool binSizeRounding) {
        strategy->setBinSizeHint(binSize);
        strategy->setBinSizeRounding(binSizeRounding);
        return *this;
    }
    AutoRangeStrategyBuilder& setAutoExtend(bool autoExtend, bool binMerging=true) {
        strategy->setAutoExtend(autoExtend);
        strategy->setBinMerging(binMerging);
        return *this;
    }
    cAutoRangeHistogramStrategy *get() {return strategy;}
};

void Histograms::addHistogram(const char *name, cIHistogramStrategy *strategy)
{
    statisticObjects.push_back(new cHistogram(name, strategy, weighted));
}

void Histograms::createStatisticObjects()
{
    if (!weighted) {
        statisticObjects.push_back(new cPSquare("psquare-default"));
        statisticObjects.push_back(new cPSquare("psquare bins=100", 100));
    }

    statisticObjects.push_back(new cKSplit("ksplit-default", weighted));

    addHistogram("Default", new cDefaultHistogramStrategy());
    addHistogram("Default bins=5", new cDefaultHistogramStrategy(5));
    addHistogram("Default bins=100", new cDefaultHistogramStrategy(100));

    addHistogram("FixedRange [0,10), bins=40, reals", new cFixedRangeHistogramStrategy(0, 10, 40, cHistogram::MODE_REALS));
    addHistogram("FixedRange [0,100), bins=25, integers", new cFixedRangeHistogramStrategy(0, 100, 25, cHistogram::MODE_INTEGERS));
    addHistogram("FixedRange [10,100), bins=9, reals", new cFixedRangeHistogramStrategy(10, 100, 9, cHistogram::MODE_REALS));

    addHistogram("AutoRange default", new cAutoRangeHistogramStrategy());
    addHistogram("AutoRange bins=5", new cAutoRangeHistogramStrategy(5));
    addHistogram("AutoRange bins=500", new cAutoRangeHistogramStrategy(500));
    addHistogram("AutoRange bins=5, integers", new cAutoRangeHistogramStrategy(5, cHistogram::MODE_INTEGERS));
    addHistogram("AutoRange bins=500, integers", new cAutoRangeHistogramStrategy(500, cHistogram::MODE_INTEGERS));

    addHistogram("AutoRange [0,100)", AutoRangeStrategyBuilder().setRange(0, 100).get());
    addHistogram("AutoRange [10,auto)", AutoRangeStrategyBuilder().setRange(10, NAN).get());
    addHistogram("AutoRange [-1.5,1.5) numBins=10", AutoRangeStrategyBuilder().setRange(-1.5, 1.5).setNumBins(10).get());
    addHistogram("AutoRange [auto,20)", AutoRangeStrategyBuilder().setRange(NAN,20).get());
    addHistogram("AutoRange rangeExt=2.0", AutoRangeStrategyBuilder().setRange(NAN, NAN, 2.0).get());
    addHistogram("AutoRange rangeExt=0.8", AutoRangeStrategyBuilder().setRange(NAN, NAN, 0.8).get());

    addHistogram("AutoRange binSizeRounding=false", AutoRangeStrategyBuilder().setBinSize(NAN, true).get());
    addHistogram("AutoRange binSize=2.5", AutoRangeStrategyBuilder().setBinSize(2.5, false).get());

    addHistogram("AutoRange autoExtend=false", AutoRangeStrategyBuilder().setAutoExtend(false).get());
    addHistogram("AutoRange autoExtend=true, binMerging=false", AutoRangeStrategyBuilder().setAutoExtend(true, false).get());

    addHistogram("AutoRange numBins=1", AutoRangeStrategyBuilder().setNumBins(1).get());
    addHistogram("AutoRange numBins=1 autoExtend=false", AutoRangeStrategyBuilder().setNumBins(1).setAutoExtend(false).get());

    addHistogram("AutoRange [10,20) numBins=1 autoExtend=false)", AutoRangeStrategyBuilder().setRange(10,20).setNumBins(1).setAutoExtend(false).get());
}


Histograms::~Histograms()
{
    for (auto stat : statisticObjects)
        delete stat;
}

void Histograms::handleMessage(cMessage *msg)
{
    if (statisticObjects[0]->getCount() == count) {
        EV << "done" << endl;
        delete msg;
        return;
    }

    if (!weighted) {
        double value = par("variable");
        EV << "collecting value=" << value << endl;

        valuesVector.record(value);
        emit(unweightedValueSignal, value);

        for (auto *statistic : statisticObjects)
            statistic->collect(value);
    }
    else {
        double value = par("variable");
        double weight = par("weight");
        EV << "collecting value=" << value << " with weight=" << weight << endl;

        valuesVector.record(value);
        weightsVector.record(weight);
        emit(unweightedValueSignal, value);

        for (auto *statistic : statisticObjects)
            statistic->collectWeighted(value, weight);
    }

    scheduleAt(simTime() + 0.001, msg);
}

void Histograms::finish()
{
    for (auto *statistic : statisticObjects)
        if (auto *histogram = dynamic_cast<cHistogram*>(statistic))
            histogram->assertSanity();

    cAbstractHistogram *stat = statisticObjects.front();
    EV << "Bins in " << stat->getName() << ":\n";
    for (int i = 0; i < stat->getNumBins(); i++) {
        EV << " bin " << i << ":";
        EV << " [" << stat->getBinEdge(i) << "," << stat->getBinEdge(i+1) << ") ";
        EV << "  w=" << stat->getBinValue(i) << " pdf=" << stat->getBinPDF(i) << endl;
    }

    EV << "Writing snapshot file...\n";
    snapshot(this);

    EV << "Recording histograms...\n";
    for (auto *statistic : statisticObjects)
        statistic->record();
}


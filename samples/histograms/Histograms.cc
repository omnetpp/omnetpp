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

    addHistogram("FixedRange [-10,10), bins=40, reals", new cFixedRangeHistogramStrategy(-10, 10, 40, cHistogram::MODE_REALS));

    addHistogram("AutoRange default", new cAutoRangeHistogramStrategy());
    addHistogram("AutoRange bins=100, integers", new cAutoRangeHistogramStrategy(100, cHistogram::MODE_INTEGERS));

    cAutoRangeHistogramStrategy *rangeStrategy = new cAutoRangeHistogramStrategy();
    rangeStrategy->setRangeHint(5, NAN);
    addHistogram("AutoRange [5,auto)", rangeStrategy);

    cAutoRangeHistogramStrategy *binSizeStrategy = new cAutoRangeHistogramStrategy();
    binSizeStrategy->setBinSizeHint(2);
    binSizeStrategy->setBinSizeRounding(false);
    addHistogram("AutoRange binSize=2", binSizeStrategy);

    cAutoRangeHistogramStrategy *fixedBinsStrategy = new cAutoRangeHistogramStrategy();
    fixedBinsStrategy->setRangeHint(5,15);
    fixedBinsStrategy->setNumBinsHint(5);
    fixedBinsStrategy->setAutoExtend(false);
    addHistogram("AutoRange [5,15) numBins=5 autoExtend=false)", fixedBinsStrategy);
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


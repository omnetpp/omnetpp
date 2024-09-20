#include <omnetpp.h>
#include "SimtimeFigure.h"

using namespace omnetpp;

/**
 * Test robustness of Tkenv/Qtenv with regard to exceptions. After exception,
 * refreshDisplay() should not be called.
 */
class RobustnessTester : public cSimpleModule
{
    private:
        // parameters
        int raiseErrorAtEventNumber;
        int raiseErrorAtRefreshNumber;
        int raiseFigureRefreshErrorOnEventNumber;
        bool raiseErrorInInitialize;
        bool raiseErrorInFinish;

        // state
        bool intializeDone = false;
        bool eventsDone = false;
        bool finishDone = false;
        mutable int refreshCount = 0;
        mutable bool exceptionThrown = false;

    public:
        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() const override;
        void throwException(const char *txt) const;
};

Define_Module(RobustnessTester);

void RobustnessTester::initialize()
{
    if (!hasGUI())
        throw cRuntimeError("This test should be run under a GUI like Qtenv");

    intializeDone = true;
    raiseErrorAtEventNumber = par("raiseErrorAtEventNumber");
    raiseErrorAtRefreshNumber = par("raiseErrorAtRefreshNumber");
    raiseFigureRefreshErrorOnEventNumber = par("raiseFigureRefreshErrorOnEventNumber");
    raiseErrorInInitialize = par("raiseErrorInInitialize");
    raiseErrorInFinish = par("raiseErrorInFinish");

    scheduleAt(0, new cMessage("timer"));

    if (raiseErrorInInitialize)
        throwException("BANGGG from initialize()");
}

void RobustnessTester::finish()
{
    finishDone = true;
    EV << "total " << refreshCount << " refresh calls\n";

    if (raiseErrorInFinish)
        throwException("BANGGG from finish()");
}

void RobustnessTester::handleMessage(cMessage *msg)
{
    eventsDone = true;

    if (refreshCount < 20)
        scheduleAt(simTime() + 0.01, msg);
    else
        delete msg;

    if (getSimulation()->getEventNumber() == raiseErrorAtEventNumber)
        throwException("BANGGG from handleMessage()");

    if (getSimulation()->getEventNumber() == raiseFigureRefreshErrorOnEventNumber)
        check_and_cast<SimtimeFigure*>(getSystemModule()->getCanvas()->getFigure("simtime"))->setThrowExceptionInRefresh(true);
}

void RobustnessTester::refreshDisplay() const
{
    if (exceptionThrown)
        throw cRuntimeError("*** GUI ERROR *** refreshDisplay() invoked after a simulation error. This should not have happened");

    refreshCount++;

    std::stringstream os;
    os << "refresh #" << refreshCount << ", ";
    if (finishDone)
        os << "after finish()";
    else if (eventsDone)
        os <<"at event #" << getSimulation()->getEventNumber() << " t=" << simTime();
    else if (intializeDone)
        os << "after initialize()";
    else
        os << "ERROR! too early, initialize() not yet called";
    getDisplayString().setTagArg("t", 0, os.str().c_str());
    EV << os.str() << endl;

    if (refreshCount == raiseErrorAtRefreshNumber)
        throwException("BANGGG from refreshDisplay()");
}

void RobustnessTester::throwException(const char *s) const
{
    exceptionThrown = true;
    throw cRuntimeError("%s", s);
}


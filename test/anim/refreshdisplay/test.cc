#include <omnetpp.h>

using namespace omnetpp;

class RefreshDisplay : public cSimpleModule
{
    private:
        // parameters
        int raiseErrorAtEventNumber;
        int raiseErrorAtRefreshNumber;
        bool raiseErrorInInitialize;
        bool raiseErrorInFinish;

        // state
        bool intializeDone = false;
        bool eventsDone = false;
        bool finishDone = false;
        int refreshCount = 0;
        bool exceptionThrown = false;

    public:
        virtual void initialize() override;
        virtual void finish() override;
        virtual void handleMessage(cMessage *msg) override;
        virtual void refreshDisplay() override;
        void throwException(const char *txt);
};

Define_Module(RefreshDisplay);

void RefreshDisplay::initialize()
{
    intializeDone = true;
    raiseErrorAtEventNumber = par("raiseErrorAtEventNumber");
    raiseErrorAtRefreshNumber = par("raiseErrorAtRefreshNumber");
    raiseErrorInInitialize = par("raiseErrorInInitialize");
    raiseErrorInFinish = par("raiseErrorInFinish");

    scheduleAt(0, new cMessage("timer"));

    if (raiseErrorInInitialize)
        throwException("BANGGG from initialize()");
}

void RefreshDisplay::finish()
{
    finishDone = true;
    EV << "total " << refreshCount << " refresh calls\n";

    if (raiseErrorInFinish)
        throwException("BANGGG from finish()");
}

void RefreshDisplay::handleMessage(cMessage *msg)
{
    eventsDone = true;
    
    if (refreshCount < 20)
        scheduleAt(simTime() + 0.01, msg);

    if (getSimulation()->getEventNumber() == raiseErrorAtEventNumber)
        throwException("BANGGG from handleMessage()");
}

void RefreshDisplay::refreshDisplay()
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

void RefreshDisplay::throwException(const char *s)
{
    exceptionThrown = true;
    throw cRuntimeError(s);
}


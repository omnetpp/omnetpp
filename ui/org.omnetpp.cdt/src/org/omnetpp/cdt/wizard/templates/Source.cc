{{copyright}}


#include <omnetpp.h>

/**
 * Generates messages; see NED file for more info.
 */
class Source : public cSimpleModule
{
  private:
    cMessage *timerMessage;

  public:
     Source();
     virtual ~Source();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Source);

Source::Source()
{
    timerMessage = NULL;
}

Source::~Source()
{
    cancelAndDelete(timerMessage);
}

void Source::initialize()
{
    timerMessage = new cMessage("timer");
    scheduleAt(simTime(), timerMessage);
}

void Source::handleMessage(cMessage *msg)
{
    ASSERT(msg==timerMessage);

    cMessage *job = new cMessage("job");
    send(job, "out");

    scheduleAt(simTime()+par("sendIaTime").doubleValue(), timerMessage);
}


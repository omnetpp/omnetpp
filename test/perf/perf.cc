#include <omnetpp.h>


class Timer
{
  private:
    clock_t starttime;
    clock_t endtime;
  public:
    Timer() {}
    void start() {starttime=clock();}
    void stop() {endtime=clock();}
    double get() {return (endtime-starttime)/(double)CLOCKS_PER_SEC;}
};


//---------------

class Queue_1 : public cSimpleModule
{
  protected:
    int repCount;
    Timer tmr;
  public:
    Module_Class_Members(Queue_1,cSimpleModule,32768);
    virtual void activity();
    virtual void finish();
};

Define_Module(Queue_1);

void Queue_1::activity()
{
    cQueue q;
    repCount = par("repCount");

    // fill queue to desired level
    int qLevel = par("qLevel");
    for (int k=0; k<qLevel; k++)
        q.insert(new cMessage());
    q.insert(new cMessage());

    // exercise queue and measure time
    tmr.start();
    for (int i=0; i<repCount; i++)
    {
        cMessage *msg = (cMessage *) q.pop();
        q.insert(msg);
    }
    tmr.stop();
}

void Queue_1::finish()
{
    ev << "T=" << 1000000*tmr.get()/repCount << " us per cycle\n";
}


//---------------

class Schedule_1 : public cSimpleModule
{
  protected:
    int repCount;
    int count;
    Timer tmr;
  public:
    Module_Class_Members(Schedule_1,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(Schedule_1);

void Schedule_1::initialize()
{
    repCount = par("repCount");
    count = repCount;
    scheduleAt(0.0, new cMessage());
    tmr.start();
}

void Schedule_1::handleMessage(cMessage *msg)
{
    if (--count)
        scheduleAt(0.0, msg);
}

void Schedule_1::finish()
{
    tmr.stop();
    ev << "T=" << 1000000*tmr.get()/repCount << " us per cycle\n";
}

//--------------

class Send_1 : public cSimpleModule
{
  protected:
    int repCount;
    cGate *g;
    int count;
    Timer tmr;
  public:
    Module_Class_Members(Send_1,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

Define_Module(Send_1);

void Send_1::initialize()
{
    repCount = par("repCount");
    count = repCount;
    g = gate("out");
    send(new cMessage(), g);
    tmr.start();
}

void Send_1::handleMessage(cMessage *msg)
{
    if (--count)
        send(msg, g);
}

void Send_1::finish()
{
    tmr.stop();
    ev << "T=" << 1000000*tmr.get()/repCount << " us per cycle\n";
}


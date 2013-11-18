#include <omnetpp.h>

namespace testlib {

class SignalSource : public cSimpleModule
{
  protected:
    int count;
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void emitSignal(cXMLElement *elem);
};

Define_Module(SignalSource);

void SignalSource::initialize()
{
    count = 0;
    cXMLElement *script = par("script");
    for (cXMLElement *elem=script->getFirstChild(); elem; elem = elem->getNextSibling())
    {
        const char *atAttr = elem->getAttribute("at");
        if (!atAttr)
            error("attribute 'at' missing at %s", elem->getSourceLocation());

        if (strcmp(atAttr, "init")==0)
        {
            emitSignal(elem);
        }
        else
        {
            // schedule self-message
            simtime_t t = STR_SIMTIME(atAttr);
            cMessage *msg = new cMessage();
            msg->setContextPointer(elem);
            scheduleAt(t, msg);
        }
    }
}

void SignalSource::handleMessage(cMessage *msg)
{
    cXMLElement *elem = (cXMLElement *)msg->getContextPointer();
    emitSignal(elem);
    delete msg;
}

void SignalSource::finish()
{
    cXMLElement *script = par("script");
    for (cXMLElement *elem=script->getFirstChild(); elem; elem = elem->getNextSibling())
    {
        const char *atAttr = elem->getAttribute("at");
        if (strcmp(atAttr, "finish")==0)
            emitSignal(elem);
    }
    EV << "emitted " << count << " signals\n";
}

void SignalSource::emitSignal(cXMLElement *elem)
{
    // signal name, type, value, timestamp
    const char *name = elem->getAttribute("name");
    const char *type = elem->getAttribute("type");
    const char *value = elem->getAttribute("value");
    const char *timestamp = elem->getAttribute("timestamp");
    ASSERT(name!=NULL);
    ASSERT(type!=NULL);
    ASSERT(value!=NULL);

    EV << "t=" << simTime() << "s: emit \"" << name << "\" type=" << type << " value=" << value;
    if (timestamp) EV << " timestamp=" << timestamp << "s";
    EV << "\n";

    simsignal_t signalID = registerSignal(name);
    cObject *obj = NULL;

    try {
        if (!timestamp)
        {
            if (strcmp(type, "bool")==0)
                emit(signalID, value[0]=='t');
            else if (strcmp(type, "long")==0)
                emit(signalID, strtol(value, NULL, 10));
            else if (strcmp(type, "unsigned long")==0)
                emit(signalID, strtoul(value, NULL, 10));
            else if (strcmp(type, "double")==0)
                emit(signalID, strtod(value, NULL));
            else if (strcmp(type, "simtime_t")==0)
                emit(signalID, STR_SIMTIME(value));
            else if (strcmp(type, "string")==0)
                emit(signalID, value);
            else if (strcmp(type, "NULL")==0)
                emit(signalID, (cObject*)0);
            else  // interpret as class name
                emit(signalID, obj = createOne(type));
        }
        else
        {
            simtime_t t = STR_SIMTIME(timestamp);
            cTimestampedValue tsval;
            if (strcmp(type, "bool")==0)
                tsval.set(t, value[0]=='t');
            else if (strcmp(type, "long")==0)
                tsval.set(t, strtol(value, NULL, 10));
            else if (strcmp(type, "unsigned long")==0)
                tsval.set(t, strtoul(value, NULL, 10));
            else if (strcmp(type, "double")==0)
                tsval.set(t, strtod(value, NULL));
            else if (strcmp(type, "simtime_t")==0)
                tsval.set(t, STR_SIMTIME(value));
            else if (strcmp(type, "string")==0)
                tsval.set(t, value);
            else if (strcmp(type, "NULL")==0)
                tsval.set(t, (cObject*)0);
            else  // interpret as class name
                tsval.set(t, obj = createOne(type));
            emit(signalID, &tsval);
        }
    }
    catch (std::exception& e)
    {
        EV << "ERROR: " << e.what() << "\n";
    }

    if (obj)
        delete obj;

    count++;
}

}

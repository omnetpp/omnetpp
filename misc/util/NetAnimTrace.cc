#include <fstream>
#include <omnetpp.h>

/**
 * Records a NetAnim trace. See NED file for more information.
 *
 * @author andras
 */
class NetAnimTrace : public cSimpleModule, protected cListener
{
  protected:
    static simsignal_t messageSentSignal;
    std::ofstream f;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void dump();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
    virtual bool isRelevantModule(cModule *mod);
    virtual void resolveNodeCoordinates(cModule *mod, double& x, double& y);
    virtual void addNode(cModule *mod);
    virtual void addLink(cGate *gate);
};

simsignal_t NetAnimTrace::messageSentSignal;

Define_Module(NetAnimTrace);

void NetAnimTrace::initialize()
{
    if (!par("enabled").boolValue())
        return;

    const char *filename = par("filename");
    f.open(filename, std::ios::out | std::ios::trunc);
    if (f.fail())
        throw cRuntimeError("Cannot open file \"%s\" for writing", filename);

    dump();

    messageSentSignal  = registerSignal("messageSent");
    simulation.getSystemModule()->subscribe(POST_MODEL_CHANGE, this);
    simulation.getSystemModule()->subscribe(messageSentSignal, this);
}

void NetAnimTrace::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not handle messages");
}

void NetAnimTrace::finish()
{
    f.close();
}

void NetAnimTrace::dump()
{
    cModule *parent = simulation.getSystemModule();
    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
        if (it() != this)
            addNode(it());
    for (cModule::SubmoduleIterator it(parent); !it.end(); it++)
        if (it() != this)
            for (cModule::GateIterator ig(it()); !ig.end(); ig++)
                if (ig()->getType()==cGate::OUTPUT && ig()->getNextGate())
                    addLink(ig());
}

void NetAnimTrace::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    if (signalID == messageSentSignal && source->isChannel())
    {
        // record a "packet sent" line
        cChannel *channel = (cChannel *)source;
        cModule *srcModule = channel->getSourceGate()->getOwnerModule();
        if (isRelevantModule(srcModule))
        {
            cModule *destModule = channel->getSourceGate()->getNextGate()->getOwnerModule();
            cChannel::MessageSentSignalValue *v = check_and_cast<cChannel::MessageSentSignalValue *>(obj);
            cChannel::result_t *result = v->getChannelResult();
            simtime_t fbTx = v->getTimestamp(signalID);
            simtime_t lbTx = fbTx + result->duration;
            simtime_t fbRx = fbTx + result->delay;
            simtime_t lbRx = lbTx + result->delay;
            f << fbTx << " P " << srcModule->getId() << " " << destModule->getId() << " " << lbTx << " " << fbRx << " " << lbRx << "\n";
        }
    }
    else if (signalID == POST_MODEL_CHANGE)
    {
        // record dynamic "node created" and "link created" lines.
        // note: at the time of writing, NetAnim did not support "link removed" and "node removed" lines
        if (dynamic_cast<cPostModuleAddNotification *>(obj))
        {
            cPostModuleAddNotification *notification = (cPostModuleAddNotification *)obj;
            if (isRelevantModule(notification->module))
                addNode(notification->module);
        }
        else if (dynamic_cast<cPostGateConnectNotification *>(obj))
        {
            cPostGateConnectNotification *notification = (cPostGateConnectNotification *)obj;
            if (isRelevantModule(notification->gate->getOwnerModule()))
                addLink(notification->gate);
        }
    }
}

bool NetAnimTrace::isRelevantModule(cModule *mod)
{
    return mod->getParentModule() == simulation.getSystemModule();
}

void NetAnimTrace::addNode(cModule *mod)
{
    double x, y;
    resolveNodeCoordinates(mod, x, y);
    f << simTime() << " N " << mod->getId() << " " << x << " " << y << "\n";
}

void NetAnimTrace::addLink(cGate *gate)
{
    f << simTime() << " L " << gate->getOwnerModule()->getId() << " " << gate->getNextGate()->getOwnerModule()->getId() << "\n";
}

double toDouble(const char *s, double defaultValue)
{
   if (!s || !*s)
       return defaultValue;
   char *end;
   double d = strtod(s, &end);
   return (end && *end) ? 0.0 : d; // return 0.0 on error, instead of throwing an exception
}

void NetAnimTrace::resolveNodeCoordinates(cModule *submod, double& x, double& y)
{
    // choose some defaults
    x = 600 * dblrand();
    y = 400 * dblrand();

    // and be content with them if there is no "p" tag in the display string
    cDisplayString& ds = submod->getDisplayString();
    if (!ds.containsTag("p"))
        return;

    // the following code is based on Tkenv (modinsp.cc, getSubmoduleCoords())

    // read x,y coordinates from "p" tag
    x = toDouble(ds.getTagArg("p",0), x);
    y = toDouble(ds.getTagArg("p",1), y);

    double sx = 20;
    double sy = 20;

    const char *layout = ds.getTagArg("p",2); // matrix, row, column, ring, exact etc.

    // modify x,y using predefined layouts
    if (!layout || !*layout)
    {
        // we're happy
    }
    else if (!strcmp(layout,"e") || !strcmp(layout,"x") || !strcmp(layout,"exact"))
    {
        int dx = toDouble(ds.getTagArg("p",3), 0);
        int dy = toDouble(ds.getTagArg("p",4), 0);
        x += dx;
        y += dy;
    }
    else if (!strcmp(layout,"r") || !strcmp(layout,"row"))
    {
        int dx = toDouble(ds.getTagArg("p",3), 2*sx);
        x += submod->getIndex()*dx;
    }
    else if (!strcmp(layout,"c") || !strcmp(layout,"col") || !strcmp(layout,"column"))
    {
        int dy = toDouble(ds.getTagArg("p",3), 2*sy);
        y += submod->getIndex()*dy;
    }
    else if (!strcmp(layout,"m") || !strcmp(layout,"matrix"))
    {
        int columns = toDouble(ds.getTagArg("p",3), 5);
        int dx = toDouble(ds.getTagArg("p",4), 2*sx);
        int dy = toDouble(ds.getTagArg("p",5), 2*sy);
        x += (submod->getIndex() % columns)*dx;
        y += (submod->getIndex() / columns)*dy;
    }
    else if (!strcmp(layout,"i") || !strcmp(layout,"ri") || !strcmp(layout,"ring"))
    {
        int rx = toDouble(ds.getTagArg("p",3), (sx+sy)*submod->size()/4);
        int ry = toDouble(ds.getTagArg("p",4), rx);

        x += (int) floor(rx - rx*sin(submod->getIndex()*2*PI/submod->size()));
        y += (int) floor(ry - ry*cos(submod->getIndex()*2*PI/submod->size()));
    }
    else
    {
        throw cRuntimeError("invalid layout `%s' in `p' tag of display string", layout);
    }
}



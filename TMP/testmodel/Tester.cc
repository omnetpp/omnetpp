#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <omnetpp.h>
#include "C:\home\omnetpp40\gitrepo\modelchange.h"  //XXX


#ifdef _WIN32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

std::string stringf(const char *format, ...)
{
    const int BUFLEN = 1024;
    char buffer[BUFLEN];
    va_list va;
    va_start(va, format);
    vsnprintf(buffer, BUFLEN, format, va);
    buffer[BUFLEN-1] = '\0';
    va_end(va);
    return buffer;
}


class Tester : public cSimpleModule, public cListener
{
  private:
    std::vector<cModule*> nodes;
    std::vector<cGate*> connections;
  public:
    Tester() : cSimpleModule(16384) {}
    virtual void activity();
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);
};

Define_Module(Tester);

//FIXME update test/anim/dynamic with this code!!!!

inline void disconnectOutside(cGate *g) //XXX add this method into cGate!
{
    if (g->getType()==cGate::OUTPUT)
        g->disconnect();
    else
        g->getPreviousGate()->disconnect();
}

void Tester::activity()
{
    cModuleType *type = cModuleType::find("TestNode");
    simsignal_t signalIDs[] = {10,63,64,100};
    int numSignalIDs = 4;
    simulation.getSystemModule()->subscribe(PRE_MODEL_CHANGE, this);
    simulation.getSystemModule()->subscribe(POST_MODEL_CHANGE, this);

    for (int i=1; true; i++)
    {
        if (intuniform(0,20)==0)
        {
            // create a node
            int k = intuniform(0, nodes.size());
            cModule *parent = k==nodes.size() ? simulation.getSystemModule() : nodes[k];
            ASSERT(parent!=NULL);
            std::string name = stringf("node%d",i);
            ev << "CREATING NODE " << parent->getFullPath() << "." << name << "\n";
            cModule *node = type->createScheduleInit(name.c_str(), parent);
            nodes.push_back(node);
        }

        if (!nodes.empty() && intuniform(0,20)==0)
        {
            // delete a node
            int k = intuniform(0, nodes.size()-1);
            ev << "DELETING NODE " << nodes[k]->getFullPath() << "\n";
            nodes[k]->deleteModule();
        }

        if ((int)nodes.size()>=2 && intuniform(0,20)==0)
        {
            // reparent node
            int m = intuniform(0, nodes.size()-1);
            int p = intuniform(0, nodes.size()-2);
            if (p>=m) p++;
            // avoid circularity: swap them if p is in the subtree of m
            for (cModule *pAnc = nodes[p]; pAnc; pAnc = pAnc->getParentModule())
                if (pAnc == nodes[m])
                    {int tmp = m; m = p; p = tmp; break;}
            ev << "REPARENTING NODE " << nodes[m]->getFullPath() << ", new parent: " << nodes[p]->getFullPath() << "\n";
            // node must be disconnected for reparenting
            for (cModule::GateIterator i(nodes[m]); !i.end(); i++)
                if (i()->isConnectedOutside())
                    disconnectOutside(i());
            nodes[m]->changeParentTo(nodes[p]);
        }

        if ((int)nodes.size()>=2 && intuniform(0,5)==0)
        {
            // create connection
            int s = intuniform(0, nodes.size()-1);
            int t = intuniform(0, nodes.size()-2);
            if (t>=s) t++;
            ev << "CREATING CONNECTION " << nodes[s]->getFullPath() << "-->" << nodes[t]->getFullPath() << "\n";
            cGate *gs = nodes[s]->addGate(stringf("gate%do",i).c_str(), cGate::OUTPUT);
            cGate *gt = nodes[t]->addGate(stringf("gate%di",i).c_str(), cGate::INPUT);
            gs->connectTo(gt, cDatarateChannel::create("channel"));
            connections.push_back(gs);
        }

        if (!connections.empty() && intuniform(0,5)==0)
        {
            // delete connection (delete gates as well, to facilitate reparent!)
            int k = intuniform(0, connections.size()-1);
            ev << "REMOVING CONNECTION " << connections[k]->getFullPath() << " --> ...\n";
            connections[k]->disconnect();
        }

        if (!nodes.empty() && intuniform(0,3)==0)
        {
            // subscribe a random module or channel to a random signal
            int k = intuniform(0, nodes.size()+connections.size()-1);
            cComponent *c = k<nodes.size() ? (cComponent*)nodes[k] : (cComponent*)(connections[k-nodes.size()]->getChannel());
            simsignal_t signalID = signalIDs[intuniform(0,numSignalIDs-1)];
            ev << "SUBSCRIBING MODULE/CHANNEL " << c->getFullPath() << " to signal " << signalID << "\n";
            c->subscribe(signalID, new cListener());
        }

        if (!nodes.empty() && intuniform(0,2)==0)
        {
            // unsubscribe a randomly chosen listener
            int k = intuniform(0, nodes.size()+connections.size()-1);
            cComponent *c = k<nodes.size() ? (cComponent*)nodes[k] : (cComponent*)(connections[k-nodes.size()]->getChannel());
            std::vector<int> signals = c->getLocalListenedSignals();
            if (!signals.empty()) {
                simsignal_t signalID = signals[intrand(signals.size())];
                std::vector<cIListener*> listeners = c->getLocalSignalListeners(signalID);
                cIListener *listener = listeners[intrand(listeners.size())];
                ev << "UNSUBSCRIBING MODULE/CHANNEL " << c->getFullPath() << " from signal " << signalID << "\n";
                c->unsubscribe(signalID, listener);
                delete listener;
            }
       }

       wait(0.2);
    }
}

void Tester::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    Enter_Method_Silent();

    EV << "Got " << obj->getClassName() << " " << obj->info() << " from " << source->getFullPath() << "\n";

    simulation.getSystemModule()->checkConsistencyRec();

    if (dynamic_cast<cPostModuleDeleteNotification*>(obj)) {
        EV << "  - LISTENER: NODE DELETED\n";
        cPostModuleDeleteNotification *data = (cPostModuleDeleteNotification *)obj;
        std::vector<cModule*>::iterator it = std::find(nodes.begin(), nodes.end(), data->module);
        ASSERT(it!=nodes.end());
        nodes.erase(it);
    }
    else if (dynamic_cast<cPostGateDisconnectNotification *>(obj)) {
        EV << "  - LISTENER: CONN DELETED\n";
        cPostGateDisconnectNotification *data = (cPostGateDisconnectNotification *)obj;
        std::vector<cGate*>::iterator it = std::find(connections.begin(), connections.end(), data->gate);
        ASSERT(it!=connections.end());
        connections.erase(it);
    }
}


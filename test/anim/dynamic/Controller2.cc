#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include <algorithm>
#include <omnetpp.h>

#ifdef _WIN32
#ifndef vsnprintf
#define vsnprintf    _vsnprintf
#endif
#endif

using namespace omnetpp;

_OPP_GNU_ATTRIBUTE(format(printf, 1, 2))
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

class Controller2 : public cSimpleModule
{
  protected:
    int step;
    cModuleType *type;
    std::vector<cModule *> nodes;
    std::vector<cGate *> connections;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Controller2);

void Controller2::initialize()
{
    step = 0;
    type = cModuleType::find("Node");
    scheduleAt(0, new cMessage("timer"));
}

void Controller2::handleMessage(cMessage *msg)
{
    // create a node
    if (intuniform(0, 20) == 0) {
        cModule *node = type->createScheduleInit(stringf("node%d", step).c_str(), getParentModule());
        nodes.push_back(node);
        EV << "node CREATED as node " << (nodes.size()-1) << "\n";
        cDisplayString& d = node->getDisplayString();
        d.setTagArg("p", 0, intuniform(0, 600));
        d.setTagArg("p", 1, intuniform(0, 400));
        d.setTagArg("t", 0, "NEW");
    }

    // delete a node
    if (!nodes.empty() && intuniform(0, 20) == 0) {
        int k = intuniform(0, nodes.size()-1);
        cModule *node = nodes[k];
        EV << "node " << node->getFullName() << ": DELETED\n";
        for (unsigned int i = 0; i < connections.size(); i++) {
            if (connections[i]->getOwnerModule() == node || connections[i]->getNextGate()->getOwnerModule() == node) {
                connections.erase(connections.begin()+i);
                i--;
            }
        }
        node->deleteModule();
        nodes.erase(nodes.begin()+k);
    }

    // create connection
    if ((int)nodes.size() >= 2 && intuniform(0, 5) == 0) {
        int s = intuniform(0, nodes.size()-1);
        int t = intuniform(0, nodes.size()-2);
        if (t >= s)
            t++;
        EV << "creating connection: " << nodes[s]->getFullName() << " --> " << nodes[t]->getFullName() << "\n";
        cGate *gs = nodes[s]->addGate(stringf("gate%do", step).c_str(), cGate::OUTPUT);
        cGate *gt = nodes[t]->addGate(stringf("gate%di", step).c_str(), cGate::INPUT);
        gs->connectTo(gt);
        connections.push_back(gs);
    }

    // delete connection
    if (!connections.empty() && intuniform(0, 5) == 0) {
        int k = intuniform(0, connections.size()-1);
        EV << "connection " << connections[k]->getFullPath() << ": DELETED\n";
        connections[k]->disconnect();
        connections.erase(connections.begin()+k);
    }

    // move
    if (!nodes.empty() && intuniform(0, 1) == 0) {
        int k = intuniform(0, nodes.size()-1);
        EV << "node " << nodes[k]->getFullName() << ": position change\n";
        cDisplayString& d = nodes[k]->getDisplayString();
        int x = atoi(d.getTagArg("p", 0));
        int y = atoi(d.getTagArg("p", 1));
        int newX = std::max(0, std::min(600, x + intuniform(-20, 20)));
        int newY = std::max(0, std::min(400, y + intuniform(-20, 20)));
        d.setTagArg("p", 0, newX);
        d.setTagArg("p", 1, newY);
    }

    // change text
    if (!nodes.empty() && intuniform(0, 5) == 0) {
        int k = intuniform(0, nodes.size()-1);
        EV << "node " << nodes[k]->getFullName() << ": text change\n";
        cDisplayString& d = nodes[k]->getDisplayString();
        d.setTagArg("t", 0, stringf("rng=%d", intuniform(0, 100000)).c_str());
    }

    // change icon size
    if (!nodes.empty() && intuniform(0, 10) == 0) {
        int k = intuniform(0, nodes.size()-1);
        EV << "node " << nodes[k]->getFullName() << ": icon size change\n";
        cDisplayString& d = nodes[k]->getDisplayString();
        static const char *sizes[] = {"vs", "s", ""};
        d.setTagArg("is", 0, sizes[intuniform(0, 2)]);
    }

    // change color
    if (!nodes.empty() && intuniform(0, 3) == 0) {
        int k = intuniform(0, nodes.size()-1);
        EV << "node " << nodes[k]->getFullName() << ": color change\n";
        cDisplayString& d = nodes[k]->getDisplayString();
        static const char *colors[] = {"", "red", "green", "blue", "yellow"};
        d.setTagArg("step", 1, colors[intuniform(0, 4)]);
    }

    // change i2
    if (!nodes.empty() && intuniform(0, 3) == 0) {
        int k = intuniform(0, nodes.size()-1);
        EV << "node " << nodes[k]->getFullName() << ": i2 change\n";
        cDisplayString& d = nodes[k]->getDisplayString();
        static const char *images[] = {"", "status/ctrl", "status/busy"};
        d.setTagArg("i2", 0, images[intuniform(0, 2)]);
    }

    step++;
    scheduleAt(simTime()+0.2, msg);
}


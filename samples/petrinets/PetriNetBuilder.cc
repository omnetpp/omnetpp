//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include <omnetpp.h>

/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
class PetriNetBuilder : public cSimpleModule
{
  protected:
    void connect(cGate *src, cGate *dest, double delay, double ber, double datarate);
    void buildNetwork(cModule *parent);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(PetriNetBuilder);


void PetriNetBuilder::initialize()
{
    // build the network in event 1, because it is undefined whether the simkernel
    // will implicitly initialize modules created *during* initialization, or this needs
    // to be done manually.
    scheduleAt(0, new cMessage());
}

void PetriNetBuilder::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
        error("This modules does not process messages.");

    delete msg;
    buildNetwork(getParentModule());
}


void PetriNetBuilder::buildNetwork(cModule *parent)
{
    cXMLElement *root = par("pnmlFile");
    const char *netId = par("id");

    // find the <net> element that contains the network
    cXMLElement *net;
    if (*netId)
        net = root->getElementById(netId);
    else if (strcmp(root->getTagName(),"net")==0)
        net = root;
    else
        net = root->getFirstChildWithTag("net");
    if (!net)
        error("Petri net description not found in %s", root->getSourceLocation());

    // find module types
    cModuleType *placeModuleType = cModuleType::find("Place"); //XXX parameter
    if (!placeModuleType)
        throw cRuntimeError("module type `%s' not found", "Place");//XXX parameter
    cModuleType *transitionModuleType = cModuleType::find("Transition"); //XXX parameter
    if (!transitionModuleType)
        throw cRuntimeError("module type `%s' not found", "Transition"); //XXX parameter

    cModule *parent = getParentModule();

    // create places
    cXMLElementList places = net->getChildrenByTagName("place");
    for (int i=0; i<(int)places.size(); i++)
    {
        cXMLElement *place = places[i];
        const char *name = place->getAttribute("id");
        cModule *placeModule = placeModuleType->create(name, parent);
        //XXX placeModule->finalizeParameters();
    }

    // create transitions
    cXMLElementList transitions = net->getChildrenByTagName("transition");
    for (int i=0; i<(int)transitions.size(); i++)
    {
        cXMLElement *transition = transitions[i];
        const char *name = transition->getAttribute("id");
        cModule *transitionModule = transitionModuleType->create(name, parent);
        //XXX transitionModule->finalizeParameters();
    }

    // create arcs
    cXMLElementList arcs = net->getChildrenByTagName("arc");
    for (int i=0; i<(int)arcs.size(); i++)
    {
        cXMLElement *arc = arcs[i];
        const char *name = arc->getAttribute("id");
        const char *source = arc->getAttribute("source");
        const char *target = arc->getAttribute("target");
        cChannel *arcChannel = arcType->create(name, parent);
        //XXX arcChannel->finalizeParameters();
    }
    std::map<long,cModule *>::iterator it;

    // final touches: buildinside, initialize()
    for (it=nodeid2mod.begin(); it!=nodeid2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->buildInside();
    }

    // the following is not entirely OK regarding multi-stage init...
    for (it=nodeid2mod.begin(); it!=nodeid2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->callInitialize();
    }
}

void PetriNetBuilder::connect(cGate *src, cGate *dest, double delay, double ber, double datarate)
{
    cDatarateChannel *channel = NULL;
    if (delay>0 || ber>0 || datarate>0)
    {
        channel = cDatarateChannel::create("channel");
        if (delay>0)
            channel->setDelay(delay);
        if (ber>0)
            channel->setBitErrorRate(ber);
        if (datarate>0)
            channel->setDatarate(datarate);
    }
    src->connectTo(dest, channel);
}


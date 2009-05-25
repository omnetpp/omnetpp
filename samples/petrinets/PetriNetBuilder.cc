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
    const char *placeTypeName = "Place"; //XXX parameter
    const char *transitionTypeName = "Transition"; //XXX parameter
    const char *arcTypeName = "Arc"; //XXX parameter
    cModuleType *placeModuleType = cModuleType::find(placeTypeName);
    if (!placeModuleType)
        throw cRuntimeError("module type `%s' not found", placeTypeName);
    cModuleType *transitionModuleType = cModuleType::find(transitionTypeName);
    if (!transitionModuleType)
        throw cRuntimeError("module type `%s' not found", transitionTypeName);
    cChannelType *arcChannelType = cModuleType::find(arcTypeName);
    if (!arcChannelType)
        throw cRuntimeError("channel type `%s' not found", arcTypeName);

    cModule *parent = getParentModule();
    std::map<std::string,cModule*> id2mod;

    // create places
    cXMLElementList places = net->getChildrenByTagName("place");
    for (int i=0; i<(int)places.size(); i++)
    {
        cXMLElement *place = places[i];
        const char *id = place->getAttribute("id");
        const char *name = id;
        cModule *placeModule = placeModuleType->create(name, parent);
        placeModule->finalizeParameters();
        id2mod[id] = placeModule;
    }

    // create transitions
    cXMLElementList transitions = net->getChildrenByTagName("transition");
    for (int i=0; i<(int)transitions.size(); i++)
    {
        cXMLElement *transition = transitions[i];
        const char *id = transition->getAttribute("id");
        const char *name = id;
        cModule *transitionModule = transitionModuleType->create(name, parent);
        transitionModule->finalizeParameters();
        id2mod[id] = transitionModule;
    }

    // create arcs
    cXMLElementList arcs = net->getChildrenByTagName("arc");
    for (int i=0; i<(int)arcs.size(); i++)
    {
        cXMLElement *arc = arcs[i];
        const char *name = arc->getAttribute("id");
        const char *source = arc->getAttribute("source");
        const char *target = arc->getAttribute("target");
        cModule *sourceModule = id2mod[source]; //XXX check it exists!
        cModule *targetModule = id2mod[target]; //XXX check it exists!
        cGate *srcGate = sourceModule->getOrCreateFirstUnconnectedGate("out", 0, false, true);
        cGate *destGate = targetModule->getOrCreateFirstUnconnectedGate("in", 0, false, true);

        cChannel *arcChannel = arcType->create(name, parent);
        srcGate->connectTo(destGate, arcChannel);
        arcChannel->finalizeParameters();
    }

    std::map<std::string,cModule *>::iterator it;

    // final touches: buildinside, initialize()
    for (it=id2mod.begin(); it!=id2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->buildInside();
    }

    // the following is not entirely OK regarding multi-stage init...
    for (it=id2mod.begin(); it!=id2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->callInitialize();
    }
}



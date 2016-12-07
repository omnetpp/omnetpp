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
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void buildNetwork(cModule *parent);
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

inline const char *getTextFrom(cXMLElement *node, const char *xpath, const char *defaultValue)
{
    cXMLElement *element = node->getElementByPath(xpath);
    const char *s = element ? element->getNodeValue() : NULL;
    return s ? s : defaultValue;
}

inline const char *getAttributeFrom(cXMLElement *node, const char *xpath, const char *attrName, const char *defaultValue)
{
    cXMLElement *element = node->getElementByPath(xpath);
    const char *s = element ? element->getAttribute(attrName) : NULL;
    return s ? s : defaultValue;
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
    cChannelType *arcChannelType = cChannelType::find(arcTypeName);
    if (!arcChannelType)
        throw cRuntimeError("channel type `%s' not found", arcTypeName);

    std::map<std::string,cModule*> id2mod;

    EV << "module PetriNet {\n";
    EV << "    submodules:\n";

    // create places
    cXMLElementList places = net->getChildrenByTagName("place");
    for (int i=0; i<(int)places.size(); i++)
    {
        cXMLElement *place = places[i];
        const char *id = place->getAttribute("id");
        const char *name = getTextFrom(place, "name/text", id);
        cModule *placeModule = placeModuleType->create(name, parent);
        placeModule->finalizeParameters();

        const char *xPos = getAttributeFrom(place, "graphics/position", "x", "");
        const char *yPos = getAttributeFrom(place, "graphics/position", "y", "");
        placeModule->getDisplayString().setTagArg("p", 0, xPos);
        placeModule->getDisplayString().setTagArg("p", 1, yPos);

        const char *marking = getTextFrom(place, "initialMarking/text", NULL);
        if (marking)
            placeModule->par("numInitialTokens").parse(marking);

        id2mod[id] = placeModule;

        EV << "        " << placeModule->getFullName() << " : " << placeModuleType->getFullName() << " {\n";
        if (*xPos || *yPos)
            EV << "            @display(\"p=" << xPos << "," << yPos << "\")\n";
        if (marking)
            EV << "            numInitialTokens = " << marking << ";\n";
        EV << "        }\n";
    }

    // create transitions
    cXMLElementList transitions = net->getChildrenByTagName("transition");
    for (int i=0; i<(int)transitions.size(); i++)
    {
        cXMLElement *transition = transitions[i];
        const char *id = transition->getAttribute("id");
        const char *name = getTextFrom(transition, "name/text", id);
        cModule *transitionModule = transitionModuleType->create(name, parent);
        transitionModule->finalizeParameters();

        const char *xPos = getAttributeFrom(transition, "graphics/position", "x", "");
        const char *yPos = getAttributeFrom(transition, "graphics/position", "y", "");
        transitionModule->getDisplayString().setTagArg("p", 0, xPos);
        transitionModule->getDisplayString().setTagArg("p", 1, yPos);

        id2mod[id] = transitionModule;

        EV << "        " << transitionModule->getFullName() << " : " << transitionModuleType->getFullName() << " {\n";
        if (*xPos || *yPos)
            EV << "            @display(\"p=" << xPos << "," << yPos << "\")\n";
        EV << "        }\n";
    }

    // create arcs
    EV << "    connections:\n";
    cXMLElementList arcs = net->getChildrenByTagName("arc");
    for (int i=0; i<(int)arcs.size(); i++)
    {
        cXMLElement *arc = arcs[i];
        const char *name = arc->getAttribute("id");
        const char *source = arc->getAttribute("source");
        const char *target = arc->getAttribute("target");

        ASSERT(id2mod.find(source)!=id2mod.end() && id2mod.find(target)!=id2mod.end());
        cModule *sourceModule = id2mod[source];
        cModule *targetModule = id2mod[target];
        cGate *sourceGate = sourceModule->getOrCreateFirstUnconnectedGate("out", 0, false, true);
        cGate *targetGate = targetModule->getOrCreateFirstUnconnectedGate("in", 0, false, true);

        cChannel *arcChannel = arcChannelType->create(name);
        sourceGate->connectTo(targetGate, arcChannel, true);

        EV << "        " << sourceModule->getFullName() << ".out++"
           << " --> " << arcChannelType->getName() << " --> "
           << targetModule->getFullName() << ".in++\n";
    }

    EV << "}\n";

    std::map<std::string,cModule *>::iterator it;

    // final touches: buildinside, initialize()
    for (it=id2mod.begin(); it!=id2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->buildInside();
    }

    // multi-stage init
    bool more = true;
    //TODO initialize channels too!!!
    for (int stage=0; more; stage++) {
        more = false;
        for (it=id2mod.begin(); it!=id2mod.end(); ++it) {
            cModule *mod = it->second;
            if (mod->callInitialize(stage))
                more = true;
        }
    }

}



//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <regex>
#include <omnetpp.h>
#include "Transition.h"
#include "TransitionScheduler.h"

using namespace omnetpp;

/**
 * Builds a network dynamically, with the topology coming from a
 * PNML file.
 */
class PetriNetBuilder : public cSimpleModule
{
  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void buildNetwork(cModule *parent);
};

Define_Module(PetriNetBuilder);


void PetriNetBuilder::initialize()
{
    buildNetwork(getParentModule());
}

void PetriNetBuilder::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This modules does not process messages.");
}

inline const char *getTextFrom(cXMLElement *node, const char *xpath, const char *defaultValue)
{
    cXMLElement *element = node->getElementByPath(xpath);
    const char *s = element ? element->getNodeValue() : nullptr;
    return s ? s : defaultValue;
}

inline const char *getAttributeFrom(cXMLElement *node, const char *xpath, const char *attrName, const char *defaultValue)
{
    cXMLElement *element = node->getElementByPath(xpath);
    const char *s = element ? element->getAttribute(attrName) : nullptr;
    return s ? s : defaultValue;
}

inline int parseInt(const char *s)
{
    return std::stoi(s);
}

static std::string makeUnique(const char *name, cModule *parent)
{
    if (!parent->hasSubmodule(name) && !parent->hasSubmoduleVector(name))
        return name;
    int k = 1;
    std::cmatch match;
    if (std::regex_match(name, match, std::regex("[0-9]+$")))
        k = std::stoi(match[0]);
    while (true) {
        std::string newName = std::regex_replace(name, std::regex("[0-9]*$"), std::to_string(++k));
        if (!parent->hasSubmodule(newName.c_str()) && !parent->hasSubmoduleVector(newName.c_str()))
            return newName;
    }
}

void PetriNetBuilder::buildNetwork(cModule *parent)
{
    cXMLElement *root = par("pnmlFile");
    const char *netId = par("id");
    int xOrigin = 100;
    int yOrigin = 80;

    // find the <net> element that contains the network
    cXMLElement *net;
    if (*netId)
        net = root->getElementById(netId);
    else if (strcmp(root->getTagName(),"net")==0)
        net = root;
    else
        net = root->getFirstChildWithTag("net");
    if (!net)
        throw cRuntimeError("Petri net description not found in %s", root->getSourceLocation());

    // find module types
    const char *placeTypeName = par("placeNedType");
    const char *transitionTypeName = par("transitionNedType");
    const char *arcTypeName = par("arcNedType");
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
    for (int i = 0; i < (int)places.size(); i++) {
        cXMLElement *place = places[i];
        const char *id = place->getAttribute("id");
        const char *name = getTextFrom(place, "name/text", id);
        cModule *placeModule = placeModuleType->create(makeUnique(name,parent).c_str(), parent);

        const char *xPos = getAttributeFrom(place, "graphics/position", "x", "");
        const char *yPos = getAttributeFrom(place, "graphics/position", "y", "");
        placeModule->getDisplayString().setTagArg("p", 0, xOrigin + parseInt(xPos));
        placeModule->getDisplayString().setTagArg("p", 1, yOrigin + parseInt(yPos));

        const char *marking = getTextFrom(place, "initialMarking/text", nullptr);
        if (marking)
            placeModule->par("numInitialTokens").parse(marking);
        placeModule->finalizeParameters();

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
    std::map<std::string,int> counts; // some files contain multiple transitions with the same name at the same place, nudge them to make them stand out
    for (int i = 0; i < (int)transitions.size(); i++) {
        cXMLElement *transition = transitions[i];
        const char *id = transition->getAttribute("id");
        const char *name = getTextFrom(transition, "name/text", id);
        cModule *transitionModule = transitionModuleType->create(makeUnique(name,parent).c_str(), parent);
        transitionModule->finalizeParameters();

        if (counts.find(name) == counts.end())
            counts[name] = 1;
        else
            counts[name]++;
        int nudge = 2 * (counts[name]-1);

        const char *xPos = getAttributeFrom(transition, "graphics/position", "x", "");
        const char *yPos = getAttributeFrom(transition, "graphics/position", "y", "");
        transitionModule->getDisplayString().setTagArg("p", 0, xOrigin + parseInt(xPos) + nudge);
        transitionModule->getDisplayString().setTagArg("p", 1, yOrigin + parseInt(yPos) + nudge);

        id2mod[id] = transitionModule;

        EV << "        " << transitionModule->getFullName() << " : " << transitionModuleType->getFullName() << " {\n";
        if (*xPos || *yPos)
            EV << "            @display(\"p=" << xPos << "," << yPos << "\")\n";
        EV << "        }\n";
    }

    // create arcs
    EV << "    connections:\n";
    cXMLElementList arcs = net->getChildrenByTagName("arc");
    for (int i = 0; i < (int)arcs.size(); i++) {
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
        sourceGate->getChannel()->finalizeParameters();

        EV << "        " << sourceModule->getFullName() << ".out++"
           << " --> " << arcChannelType->getName() << " --> "
           << targetModule->getFullName() << ".in++\n";
    }

    EV << "}\n";

    // Note: Newly created modules and channels need to be initialized. Currently it is
    // done implicitly: since this function is called in init stage 0, the OMNeT++ kernel
    // will "fall through" to initializing the newly created modules as well.

}



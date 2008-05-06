//==========================================================================
//  NETBUILDER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <omnetpp.h>


/**
 * Builds a network dynamically, with the topology coming from a
 * text file.
 */
class NetBuilder : public cSimpleModule
{
  protected:
    void addLinkAttributes(cGate *src, double delay, double error, double datarate);
    void buildNetwork(cModule *parent);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(NetBuilder);


void NetBuilder::initialize()
{
    // build the network in event 1, because it is undefined whether the simkernel 
    // will implicitly initialize modules created *during* initialization, or this needs
    // to be done manually. 
    scheduleAt(0, new cMessage());
}

void NetBuilder::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
        error("This modules does not process messages.");

    delete msg;
    buildNetwork(parentModule());
}

void NetBuilder::addLinkAttributes(cGate *src, double delay, double error, double datarate)
{
    if (delay>0 || error>0 || datarate>0)
    {
        cBasicChannel *channel = cChannelType::createBasicChannel("channel", src->ownerModule());
        if (delay>0)
            channel->setDelay(delay);
        if (error>0)
            channel->setError(error);
        if (datarate>0)
            channel->setDatarate(datarate);
        channel->finalizeParameters();
        src->setChannel(channel);
    }
}

void NetBuilder::buildNetwork(cModule *parent)
{
    std::map<long,cModule *> nodeid2mod;
    std::string line;

    std::fstream modulesFile(par("modulesFile").stringValue(), std::ios::in);
    while(getline(modulesFile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError("wrong line in module file: 3 items required, line: \"%s\"", line.c_str());

        // get fields from tokens
        long nodeid = atol(tokens[0].c_str());
        const char *name = tokens[1].c_str();
        const char *modtypename = tokens[2].c_str();
        ev << "NODE id=" << nodeid << " name=" << name << " type=" << modtypename << "\n";

        // create module
        cModuleType *modtype = cModuleType::find(modtypename);
        if (!modtype)
            throw cRuntimeError("module type `%s' for node `%s' not found", modtypename, name);
        cModule *mod = modtype->create(name, parent);
        nodeid2mod[nodeid] = mod;
    }

    // set parameters
    std::fstream paramsFile(par("parametersFile").stringValue(), std::ios::in);
    while(getline(paramsFile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 3)
            throw cRuntimeError("wrong line in parameters file: 3 items required, line: \"%s\"", line.c_str());

        // get fields from tokens
        long nodeid = atol(tokens[0].c_str());
        const char *parname = tokens[1].c_str();
        const char *value = tokens[2].c_str();

        if (nodeid2mod.find(nodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in parameters file: node with id=%ld not found", nodeid);

        // look up module and set its parameter
        cModule *mod = nodeid2mod[nodeid];
        cPar& par = mod->par(parname); // will throw an error if no such param
        par.parse(value);
    }

    std::map<long,cModule *>::iterator it;
    for (it=nodeid2mod.begin(); it!=nodeid2mod.end(); ++it)
    {
        cModule *mod = it->second;
        mod->finalizeParameters();
    }

    // read and create connections
    std::fstream connectionsFile(par("connectionsFile").stringValue(), std::ios::in);
    while(getline(connectionsFile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() != 5)
            throw cRuntimeError("wrong line in parameters file: 5 items required, line: \"%s\"", line.c_str());

        // get fields from tokens
        long srcnodeid = atol(tokens[0].c_str());
        long destnodeid = atol(tokens[1].c_str());
        double delay = tokens[2]!="-" ? atof(tokens[2].c_str()) : -1;
        double error = tokens[3]!="-" ? atof(tokens[3].c_str()) : -1;
        double datarate = tokens[4]!="-" ? atof(tokens[4].c_str()) : -1;

        if (nodeid2mod.find(srcnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in connections file: node with id=%ld not found", srcnodeid);
        if (nodeid2mod.find(destnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in connections file: node with id=%ld not found", destnodeid);

        cModule *srcmod = nodeid2mod[srcnodeid];
        cModule *destmod = nodeid2mod[destnodeid];

        cGate *srcIn, *srcOut, *destIn, *destOut;
        srcmod->getOrCreateFirstUnconnectedGatePair("port", false, true, srcIn, srcOut);
        destmod->getOrCreateFirstUnconnectedGatePair("port", false, true, destIn, destOut);

        // connect
        srcOut->connectTo(destIn);
        destOut->connectTo(srcIn);
        addLinkAttributes(srcOut, delay, error, datarate);
        addLinkAttributes(destOut, delay, error, datarate);
    }

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



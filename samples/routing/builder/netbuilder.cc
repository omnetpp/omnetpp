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
    void readAndBuild(cModule *parent);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(NetBuilder);

void NetBuilder::initialize()
{
    cModule *mod = parentModule();
    readAndBuild(mod);
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

void NetBuilder::readAndBuild(cModule *parentmod)
{
    std::map<long,cModule *> nodeid2mod;
    std::string line;

    std::fstream modulesFile(par("modulesFile").stringValue(), std::ios::in);
    while(getline(modulesFile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;

        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() < 3)
            throw cRuntimeError("wrong line in module file: less than 3 items in line: \"%s\"", line.c_str());

        // get fields from tokens
        long nodeid = atol(tokens[0].c_str());
        const char *name = tokens[1].c_str();
        const char *modtypename = tokens[2].c_str();
        ev << "NODE id=" << nodeid << " name=" << name << " type=" << modtypename << "\n";

        // create module
        cModuleType *modtype = cModuleType::find(modtypename);
        if (!modtype)
            throw cRuntimeError("module type `%s' for node `%s' not found", modtypename, name);
        cModule *mod = modtype->create(name, parentmod);
        nodeid2mod[nodeid] = mod;
    }

    // set parameters
    std::fstream paramsFile(par("parametersFile").stringValue(), std::ios::in);
    while(getline(paramsFile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() < 3)
            throw cRuntimeError("wrong line in parameters file: less than 3 items in line: \"%s\"", line.c_str());

        // get fields from tokens
        long nodeid = atol(tokens[0].c_str());
        const char *parname = tokens[1].c_str();
        const char *value = tokens[2].c_str();

        if (nodeid2mod.find(nodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in parameters file: module id %ld not found", nodeid);

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
    std::fstream connectionsfile(par("connectionsFile").stringValue(), std::ios::in);
    while(getline(connectionsfile, line, '\n'))
    {
        if (line.empty() || line[0] == '#')
            continue;
        std::vector<std::string> tokens = cStringTokenizer(line.c_str()).asVector();
        if (tokens.size() < 6)
            throw cRuntimeError("wrong line in parameters file: less than 6 items in line: \"%s\"", line.c_str());

        // get fields from tokens
        long srcnodeid = atol(tokens[0].c_str());
        long destnodeid = atol(tokens[1].c_str());
        bool duplex = strcmp(tokens[2].c_str(),"0")!=0;
        double delay = tokens[3]!="-" ? atof(tokens[3].c_str()) : -1;
        double error = tokens[4]!="-" ? atof(tokens[4].c_str()) : -1;
        double datarate = tokens[5]!="-" ? atof(tokens[5].c_str()) : -1;

        if (nodeid2mod.find(srcnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in parameters file: module id %ld not found", srcnodeid);
        if (nodeid2mod.find(destnodeid) == nodeid2mod.end())
            throw cRuntimeError("wrong line in parameters file: module id %ld not found", destnodeid);

        cModule *srcmod = nodeid2mod[srcnodeid];
        cModule *destmod = nodeid2mod[destnodeid];

        cGate *srcIn, *srcOut, *destIn, *destOut;
        srcmod->getOrCreateFirstUnconnectedGatePair("port", false, true, srcIn, srcOut);
        destmod->getOrCreateFirstUnconnectedGatePair("port", false, true, destIn, destOut);

        // connect
        srcOut->connectTo(destIn);
        addLinkAttributes(srcOut, delay, error, datarate);
        if (duplex)
        {
            destOut->connectTo(srcIn);
            addLinkAttributes(destOut, delay, error, datarate);
        }
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

void NetBuilder::handleMessage(cMessage *)
{
    error("This modules does not process messages.");
}


//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>


class ChainNode : public cSimpleModule
{
    Module_Class_Members(ChainNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(ChainNode);


class MeshNode : public cSimpleModule
{
    Module_Class_Members(MeshNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(MeshNode);


class StarNode : public cSimpleModule
{
    Module_Class_Members(StarNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(StarNode);


class HubNode : public cSimpleModule
{
    Module_Class_Members(HubNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(HubNode);


class RndNode : public cSimpleModule
{
    Module_Class_Members(RndNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(RndNode);


class FullGraphNode : public cSimpleModule
{
    Module_Class_Members(FullGraphNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(FullGraphNode);


class HexGridNode : public cSimpleModule
{
    Module_Class_Members(HexGridNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(HexGridNode);


class BinaryTreeNode : public cSimpleModule
{
    Module_Class_Members(BinaryTreeNode,cSimpleModule,0)
    virtual void handleMessage(cMessage *) {}
};

Define_Module(BinaryTreeNode);


//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>


class ChainNode : public cSimpleModule
{
  public:
    ChainNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(ChainNode);


class MeshNode : public cSimpleModule
{
  public:
    MeshNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(MeshNode);


class StarNode : public cSimpleModule
{
  public:
    StarNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(StarNode);


class HubNode : public cSimpleModule
{
  public:
    HubNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(HubNode);


class RndNode : public cSimpleModule
{
  public:
    RndNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(RndNode);


class FullGraphNode : public cSimpleModule
{
  public:
    FullGraphNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(FullGraphNode);


class HexGridNode : public cSimpleModule
{
  public:
    HexGridNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(HexGridNode);


class BinaryTreeNode : public cSimpleModule
{
  public:
    BinaryTreeNode() {}   //NEWCTOR
    virtual void handleMessage(cMessage *) {}
};

Define_Module(BinaryTreeNode);


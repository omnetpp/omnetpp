//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

class DoNothing : public cSimpleModule
{
  protected:
    virtual void handleMessage(cMessage *) {}
};

class ChainNode : public DoNothing {};
Define_Module(ChainNode);

class MeshNode : public DoNothing {};
Define_Module(MeshNode);

class StarNode : public DoNothing {};
Define_Module(StarNode);

class HubNode : public DoNothing {};
Define_Module(HubNode);

class RndNode : public DoNothing {};
Define_Module(RndNode);

class FullGraphNode : public DoNothing {};
Define_Module(FullGraphNode);

class HexGridNode : public DoNothing {};
Define_Module(HexGridNode);

class BinaryTreeNode : public DoNothing {};
Define_Module(BinaryTreeNode);


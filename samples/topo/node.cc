//
// file: node.cc
//
// used for NED testing
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

// log2 for integers>=1 -- needed by BinaryTree placement
double intlog2(double a)
{
    long aa = (long)a;
    int k=0;
    while (aa>1)
    {
       aa = aa >> 1;
       k++;
    }
    return k;
}

Define_Function(intlog2,1);


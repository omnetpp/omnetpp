//==========================================================================
//   CTOPO.H  - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//     cTopology : network topology to find shortest paths etc.
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CTOPO_H
#define __CTOPO_H

#include "cobject.h"

class cPar;


#define INFINITY  HUGE_VAL

class sTopoLink;
class sTopoLinkIn;
class sTopoLinkOut;


class sTopoNode
{
    friend class cTopology;
  private:
    int module_id;
    double wgt;
    bool enabl;

    int num_in_links;
    sTopoLink **in_links;
    int num_out_links;
    sTopoLink *out_links;

    // variables used by the shortest-path algorithms
    bool known;
    double dist;
    sTopoLink *out_path;

  public:
    int moduleId()              {return module_id;}
    cModule *module()           {return &simulation[module_id];}

    double weight()             {return wgt;}
    void setWeight(double d)    {wgt=d;}
    bool enabled()              {return enabl;}
    void enable()               {enabl=TRUE;}
    void disable()              {enabl=FALSE;}

    int inLinks()               {return num_in_links;}
    sTopoLinkIn *in(int i);
    int outLinks()              {return num_out_links;}
    sTopoLinkOut *out(int i);

    // shortest path extraction (prepared for multiple paths...):
    double distanceToTarget()   {return dist;}
    int paths()                 {return out_path?1:0;}
    sTopoLinkOut *path(int)     {return (sTopoLinkOut *)out_path;}
};

class sTopoLink
{
    friend class cTopology;
  protected:
    sTopoNode *src_node;
    int src_gate;
    sTopoNode *dest_node;
    int dest_gate;
    double wgt;
    bool enabl;
  public:
    double weight()             {return wgt;}
    void setWeight(double d)    {wgt=d;}
    bool enabled()              {return enabl;}
    void enable()               {enabl=TRUE;}
    void disable()              {enabl=FALSE;}
};

// sTopoLinkIn, sTopoLinkOut: two different 'user interfaces' for sTopoLink

class sTopoLinkIn : public sTopoLink
{
  public:
    sTopoNode *remoteNode()  {return src_node;}
    int remoteGateId()       {return src_gate;}
    int localGateId()        {return dest_gate;}
    cGate *remoteGate()      {return src_node->module()->gate(src_gate);}
    cGate *localGate()       {return dest_node->module()->gate(dest_gate);}
};

class sTopoLinkOut : public sTopoLink
{
  public:
    sTopoNode *remoteNode()  {return dest_node;}
    int remoteGateId()       {return dest_gate;}
    int localGateId()        {return src_gate;}
    cGate *remoteGate()      {return dest_node->module()->gate(dest_gate);}
    cGate *localGate()       {return src_node->module()->gate(src_gate);}
};


class cTopology : public cObject
{
  protected:
    int num_nodes;
    sTopoNode *nodev;
    sTopoNode *target;

  public:
    explicit cTopology(const char *name=NULL);
    cTopology(cTopology& topo);
    virtual ~cTopology();

     // redefined functions
     virtual const char *className()  {return "cTopology";}
     virtual cObject *dup()     {return new cTopology(*this);}
     virtual void info(char *buf);
     virtual const char *inspectorFactoryName() {return "cTopologyIFC";}

     virtual int netPack();
     virtual int netUnpack();

     cTopology& operator=(cTopology& topo);

     // delete data structure
     void clear();

     // extract...() functions build topology from the model.
     // User can select which modules to include. All connections
     // between those modules will be in the topology. Connections can
     // cross compound module boundaries.

     // Include modules for which the passed selfunc() returns nonzero.
     void extractFromNetwork(int (*selfunc)(cModule *,void *), void *data=NULL);

     // Include modules with given types (classnames). List of types must be
     // terminated by NULL.
     void extractByModuleType(char *type1,...);

     // Include modules with given parameter and (optionally) given value.
     void extractByParameter(char *parname, cPar *value=NULL);

     // Functions to examine topology by hand. Users also need to rely on
     // sTopoNode and sTopoLink member functions.
     int nodes()  {return num_nodes;}
     sTopoNode *node(int i);
     sTopoNode *nodeFor(cModule *mod);

     // Algorithms to find shortest paths:
     void unweightedSingleShortestPathsTo(sTopoNode *target);

     // to be implemented:
     //void unweightedMultiShortestPathsTo(sTopoNode *target);
     //void weightedSingleShortestPathsTo(sTopoNode *target);
     //void weightedMultiShortestPathsTo(sTopoNode *target);

     sTopoNode *targetNode() {return target;}

};

#endif


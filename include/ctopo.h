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
  Copyright (C) 1992-2001 Andras Varga
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



/**
 * MISSINGDOC: sTopoNode
 */
class SIM_API sTopoNode
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

    /**
     * FIXME: variables used by the shortest-path algorithms
     */
    int moduleId()              {return module_id;}

    /**
     * MISSINGDOC: sTopoNode:cModule*module()
     */
    cModule *module()           {return &simulation[module_id];}


    /**
     * MISSINGDOC: sTopoNode:double weight()
     */
    double weight()             {return wgt;}

    /**
     * MISSINGDOC: sTopoNode:void setWeight(double)
     */
    void setWeight(double d)    {wgt=d;}

    /**
     * MISSINGDOC: sTopoNode:bool enabled()
     */
    bool enabled()              {return enabl;}

    /**
     * MISSINGDOC: sTopoNode:void enable()
     */
    void enable()               {enabl=true;}

    /**
     * MISSINGDOC: sTopoNode:void disable()
     */
    void disable()              {enabl=false;}


    /**
     * MISSINGDOC: sTopoNode:int inLinks()
     */
    int inLinks()               {return num_in_links;}

    /**
     * MISSINGDOC: sTopoNode:sTopoLinkIn*in(int)
     */
    sTopoLinkIn *in(int i);

    /**
     * MISSINGDOC: sTopoNode:int outLinks()
     */
    int outLinks()              {return num_out_links;}

    /**
     * MISSINGDOC: sTopoNode:sTopoLinkOut*out(int)
     */
    sTopoLinkOut *out(int i);

    // shortest path extraction (prepared for multiple paths...):

    /**
     * FIXME: shortest path extraction (prepared for multiple paths...):
     */
    double distanceToTarget()   {return dist;}

    /**
     * MISSINGDOC: sTopoNode:int paths()
     */
    int paths()                 {return out_path?1:0;}

    /**
     * MISSINGDOC: sTopoNode:sTopoLinkOut*path()
     */
    sTopoLinkOut *path(int)     {return (sTopoLinkOut *)out_path;}
};


/**
 * MISSINGDOC: sTopoLink
 */
class SIM_API sTopoLink
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

    /**
     * MISSINGDOC: sTopoLink:double weight()
     */
    double weight()             {return wgt;}

    /**
     * MISSINGDOC: sTopoLink:void setWeight(double)
     */
    void setWeight(double d)    {wgt=d;}

    /**
     * MISSINGDOC: sTopoLink:bool enabled()
     */
    bool enabled()              {return enabl;}

    /**
     * MISSINGDOC: sTopoLink:void enable()
     */
    void enable()               {enabl=true;}

    /**
     * MISSINGDOC: sTopoLink:void disable()
     */
    void disable()              {enabl=false;}
};

// sTopoLinkIn, sTopoLinkOut: two different 'user interfaces' for sTopoLink


/**
 * FIXME: sTopoLinkIn, sTopoLinkOut: two different 'user interfaces' for sTopoLink
 */
class SIM_API sTopoLinkIn : public sTopoLink
{
  public:

    /**
     * MISSINGDOC: sTopoLinkIn:sTopoNode*remoteNode()
     */
    sTopoNode *remoteNode()  {return src_node;}

    /**
     * MISSINGDOC: sTopoLinkIn:int remoteGateId()
     */
    int remoteGateId()       {return src_gate;}

    /**
     * MISSINGDOC: sTopoLinkIn:int localGateId()
     */
    int localGateId()        {return dest_gate;}

    /**
     * MISSINGDOC: sTopoLinkIn:cGate*remoteGate()
     */
    cGate *remoteGate()      {return src_node->module()->gate(src_gate);}

    /**
     * MISSINGDOC: sTopoLinkIn:cGate*localGate()
     */
    cGate *localGate()       {return dest_node->module()->gate(dest_gate);}
};


/**
 * MISSINGDOC: sTopoLinkOut
 */
class SIM_API sTopoLinkOut : public sTopoLink
{
  public:

    /**
     * MISSINGDOC: sTopoLinkOut:sTopoNode*remoteNode()
     */
    sTopoNode *remoteNode()  {return dest_node;}

    /**
     * MISSINGDOC: sTopoLinkOut:int remoteGateId()
     */
    int remoteGateId()       {return dest_gate;}

    /**
     * MISSINGDOC: sTopoLinkOut:int localGateId()
     */
    int localGateId()        {return src_gate;}

    /**
     * MISSINGDOC: sTopoLinkOut:cGate*remoteGate()
     */
    cGate *remoteGate()      {return dest_node->module()->gate(dest_gate);}

    /**
     * MISSINGDOC: sTopoLinkOut:cGate*localGate()
     */
    cGate *localGate()       {return src_node->module()->gate(src_gate);}
};



/**
 * MISSINGDOC: cTopology
 */
class SIM_API cTopology : public cObject
{
  protected:
    int num_nodes;
    sTopoNode *nodev;
    sTopoNode *target;

  public:

    /**
     * MISSINGDOC: cTopology:cTopology(char*)
     */
    explicit cTopology(const char *name=NULL);

    /**
     * MISSINGDOC: cTopology:cTopology(cTopology&)
     */
    cTopology(cTopology& topo);

    /**
     * MISSINGDOC: cTopology:~cTopology()
     */
    virtual ~cTopology();

     // redefined functions

    /**
     * FIXME: redefined functions
     */
     virtual const char *className() const {return "cTopology";}

    /**
     * MISSINGDOC: cTopology:cObject*dup()
     */
     virtual cObject *dup()     {return new cTopology(*this);}

    /**
     * MISSINGDOC: cTopology:void info(char*)
     */
     virtual void info(char *buf);

    /**
     * MISSINGDOC: cTopology:char*inspectorFactoryName()
     */
     virtual const char *inspectorFactoryName() const {return "cTopologyIFC";}


    /**
     * MISSINGDOC: cTopology:int netPack()
     */
     virtual int netPack();

    /**
     * MISSINGDOC: cTopology:int netUnpack()
     */
     virtual int netUnpack();


    /**
     * MISSINGDOC: cTopology:cTopology&operator=(cTopology&)
     */
     cTopology& operator=(cTopology& topo);

     // delete data structure

    /**
     * FIXME: delete data structure
     */
     void clear();

     // extract...() functions build topology from the model.
     // User can select which modules to include. All connections
     // between those modules will be in the topology. Connections can
     // cross compound module boundaries.

     // Include modules for which the passed selfunc() returns nonzero.

    /**
     * FIXME: extract...() functions build topology from the model.
     * User can select which modules to include. All connections
     * between those modules will be in the topology. Connections can
     * cross compound module boundaries.
     * Include modules for which the passed selfunc() returns nonzero.
     */
     void extractFromNetwork(int (*selfunc)(cModule *,void *), void *data=NULL);

     // Include modules with given types (classnames). List of types must be
     // terminated by NULL.

    /**
     * FIXME: Include modules with given types (classnames). List of types must be
     * terminated by NULL.
     */
     void extractByModuleType(const char *type1,...);

     // Include modules with given parameter and (optionally) given value.

    /**
     * FIXME: Include modules with given parameter and (optionally) given value.
     */
     void extractByParameter(const char *parname, cPar *value=NULL);

     // Functions to examine topology by hand. Users also need to rely on
     // sTopoNode and sTopoLink member functions.

    /**
     * FIXME: Functions to examine topology by hand. Users also need to rely on
     * sTopoNode and sTopoLink member functions.
     */
     int nodes()  {return num_nodes;}

    /**
     * MISSINGDOC: cTopology:sTopoNode*node(int)
     */
     sTopoNode *node(int i);

    /**
     * MISSINGDOC: cTopology:sTopoNode*nodeFor(cModule*)
     */
     sTopoNode *nodeFor(cModule *mod);

     // Algorithms to find shortest paths:

    /**
     * FIXME: Algorithms to find shortest paths:
     */
     void unweightedSingleShortestPathsTo(sTopoNode *target);

     // to be implemented:
     //void unweightedMultiShortestPathsTo(sTopoNode *target);
     //void weightedSingleShortestPathsTo(sTopoNode *target);
     //void weightedMultiShortestPathsTo(sTopoNode *target);


    /**
     * FIXME: to be implemented:
     *      //void unweightedMultiShortestPathsTo(sTopoNode *target);
     *      //void weightedSingleShortestPathsTo(sTopoNode *target);
     *      //void weightedMultiShortestPathsTo(sTopoNode *target);
     */
     sTopoNode *targetNode() {return target;}

};

#endif

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
 * Supporting class for cTopology, represents a node in the graph.
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
    /** @name Node attributes: weight, enabled state, correspondence to modules. */
    //@{

    /**
     * Returns the ID of the network module to which this node corresponds.
     */
    int moduleId()              {return module_id;}

    /**
     * Returns the pointer to the network module to which this node corresponds.
     */
    cModule *module()           {return &simulation[module_id];}

    /**
     * Returns the weight of this node. Weight is used with the
     * weighted shortest path finder methods of cTopology.
     */
    double weight()             {return wgt;}

    /**
     * Sets the weight of this node. Weight is used with the
     * weighted shortest path finder methods of cTopology.
     */
    void setWeight(double d)    {wgt=d;}

    /**
     * Returns true of this node is enabled. This has significance
     * with the shortest path finder methods of cTopology.
     */
    bool enabled()              {return enabl;}

    /**
     * Enable this node. This has significance with the shortest path
     * finder methods of cTopology.
     */
    void enable()               {enabl=true;}

    /**
     * Disable this node. This has significance with the shortest path
     * finder methods of cTopology.
     */
    void disable()              {enabl=false;}
    //@}

    /** @name Node connectivity. */
    //@{

    /**
     * Returns the number of incoming links to this graph node.
     */
    int inLinks()               {return num_in_links;}

    /**
     * Returns ith incoming link of graph node.
     */
    sTopoLinkIn *in(int i);

    /**
     * Returns the number of outgoing links from this graph node.
     */
    int outLinks()              {return num_out_links;}

    /**
     * Returns ith outgoing link of graph node.
     */
    sTopoLinkOut *out(int i);
    //@}

    /** @name Result of shortest path extraction. */
    //@{

    /**
     * Returns the distance of this node to the target node.
     */
    double distanceToTarget()   {return dist;}

    /**
     * Returns the number of shortest paths towards the target node.
     * (There might be several paths with the same lengths.)
     */
    int paths()                 {return out_path?1:0;}

    /**
     * Returns the next link in the ith shortest paths towards the
     * target node. (There might be several paths with the same
     * lengths.)
     */
    sTopoLinkOut *path(int)     {return (sTopoLinkOut *)out_path;}
    //@}
};


/**
 * Supporting class for cTopology, represents a link in the graph.
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
     * Returns the weight of this link. Weight is used with the
     * weighted shortest path finder methods of cTopology.
     */
    double weight()             {return wgt;}

    /**
     * Sets the weight of this link. Weight is used with the
     * weighted shortest path finder methods of cTopology.
     */
    void setWeight(double d)    {wgt=d;}

    /**
     * Returns true of this link is enabled. This has significance
     * with the shortest path finder methods of cTopology.
     */
    bool enabled()              {return enabl;}

    /**
     * Enables this link. This has significance with the shortest path
     * finder methods of cTopology.
     */
    void enable()               {enabl=true;}

    /**
     * Disables this link. This has significance with the shortest path
     * finder methods of cTopology.
     */
    void disable()              {enabl=false;}
};


/**
 * Supporting class for cTopology.
 *
 * While navigating the graph stored in a cTopology, sTopoNode's methods return
 * sTopoLinkIn and sTopoLinkOut objects, which are 'aliases' to sTopoLink objects.
 * sTopoLinkIn and sTopoLinkOut provide convenience functions that return the
 * 'local' and 'remote' end of the connection when traversing the topology.
 */
class SIM_API sTopoLinkIn : public sTopoLink
{
  public:
    /**
     * Returns the node at the remote end of this connection.
     *
     * Note: There's no corresponding localNode() method: the local node of
     * this connection is the sTopoNode object whose method returned
     * this sTopoLinkIn object.
     */
    sTopoNode *remoteNode()  {return src_node;}

    /**
     * Returns the gate ID at the remote end of this connection.
     */
    int remoteGateId()       {return src_gate;}

    /**
     * Returns the gate ID at the local end of this connection.
     */
    int localGateId()        {return dest_gate;}

    /**
     * Returns the gate at the remote end of this connection.
     */
    cGate *remoteGate()      {return src_node->module()->gate(src_gate);}

    /**
     * Returns the gate at the local end of this connection.
     */
    cGate *localGate()       {return dest_node->module()->gate(dest_gate);}
};


/**
 * Supporting class for cTopology.
 *
 * While navigating the graph stored in a cTopology, sTopoNode's methods return
 * sTopoLinkIn and sTopoLinkOut objects, which are 'aliases' to sTopoLink objects.
 * sTopoLinkIn and sTopoLinkOut provide convenience functions that return the
 * 'local' and 'renote' end of the connection when traversing the topology.
 */
class SIM_API sTopoLinkOut : public sTopoLink
{
  public:
    /**
     * Returns the node at the remote end of this connection.
     *
     * Note: There's no corresponding localNode() method: the local node of
     * this connection is the sTopoNode object whose method returned
     * this sTopoLinkIn object.
     */
    sTopoNode *remoteNode()  {return dest_node;}

    /**
     * Returns the gate ID at the remote end of this connection.
     */
    int remoteGateId()       {return dest_gate;}

    /**
     * Returns the gate ID at the local end of this connection.
     */
    int localGateId()        {return src_gate;}

    /**
     * Returns the gate at the remote end of this connection.
     */
    cGate *remoteGate()      {return dest_node->module()->gate(dest_gate);}

    /**
     * Returns the gate at the local end of this connection.
     */
    cGate *localGate()       {return src_node->module()->gate(src_gate);}
};


/**
 * Routing support. The cTopology class was designed primarily to support
 * routing in telecommunication or multiprocessor networks.
 *
 * A cTopology object stores an abstract representation of the network
 * in graph form:
 * <UL>
 *   <LI> each cTopology node corresponds to a module (simple or compound), and
 *   <LI> each cTopology edge corresponds to a link or series of connecting links.
 * </UL>
 *
 * You can specify which modules (either simple or compound) you want to
 * include in the graph. The graph will include all connections among the
 * selected modules. In the graph, all nodes are at the same level, there's
 * no submodule nesting. Connections which span across compound module
 * boundaries are also represented as one graph edge. Graph edges are directed,
 * just as module gates are.
 *
 * @ingroup SimSupport
 */
class SIM_API cTopology : public cObject
{
  protected:
    int num_nodes;
    sTopoNode *nodev;
    sTopoNode *target;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    explicit cTopology(const char *name=NULL);

    /**
     * Copy constructor.
     */
    cTopology(cTopology& topo);

    /**
     * Destructor.
     */
    virtual ~cTopology();

    /**
     * Assignment operator. The name member doesn't get copied; see cObject's operator=() for more details.
     */
    cTopology& operator=(cTopology& topo);
    //@}

    /** @name Redefined cObject member functions. */
    //@{

    /**
     * Returns pointer to a string containing the class name, "cTopology".
     */
    virtual const char *className() const {return "cTopology";}

    /**
     * Creates and returns an exact copy of this object.
     * See cObject for more details.
     */
    virtual cObject *dup()     {return new cTopology(*this);}

    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     * See cObject for more details.
     */
    virtual void info(char *buf);

    /**
     * Returns the name of the inspector factory class associated with this class.
     * See cObject for more details.
     */
    virtual const char *inspectorFactoryName() const {return "cTopologyIFC";}

    /**
     * Serializes the object into a PVM or MPI send buffer.
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netPack();

    /**
     * Deserializes the object from a PVM or MPI receive buffer
     * Used by the simulation kernel for parallel execution.
     * See cObject for more details.
     */
    virtual int netUnpack();
    //@}

    /** @name Extracting the topology from a network.
     *
     * extract...() functions build topology from the model.
     * User can select which modules to include. All connections
     * between those modules will be in the topology. Connections can
     * cross compound module boundaries.
     */
    //@{

    /**
     * Extracts model topology by a user-defined criteria. Includes into the graph
     * modules for which the passed selfunc() returns nonzero.
     */
    void extractFromNetwork(int (*selfunc)(cModule *,void *), void *data=NULL);

    /**
     * Extracts model topology by module type (classnames). Includes into
     * the graph all modules whose className() is one of the strings
     * listed as arguments. The argument list must be terminated by NULL.
     */
    void extractByModuleType(const char *type1,...);

    /**
     * Extracts model topology by parameter value. Includes into the graph
     * modules which have a parameter with the given name and (optionally)
     * the given value.
     */
    void extractByParameter(const char *parname, cPar *value=NULL);

    /**
     * Deletes the topology stored in the object.
     */
    void clear();
    //@}

    /** @name Functions to examine topology by hand.
     *
     * Users also need to rely on sTopoNode and sTopoLink member functions
     * to explore the graph stored in the object.
     */
    //@{

    /**
     * Returns the number of nodes in the graph.
     */
    int nodes()  {return num_nodes;}

    /**
     * Returns pointer to the ith node in the graph. sTopoNode's methods
     * can be used to further examine the node's connectivity, etc.
     */
    sTopoNode *node(int i);

    /**
     * Returns the graph node which corresponds to the given module in the
     * network. If no graph node corresponds to the module, the method returns
     * NULL. This method assumes that the topology corresponds to the
     * network, that is, it was probably created with one of the
     * extract...() functions.
     */
    sTopoNode *nodeFor(cModule *mod);
    //@}

    /** @name Algorithms to find shortest paths.
     *
     * To be implemented:
     *    -  void unweightedMultiShortestPathsTo(sTopoNode *target);
     *    -  void weightedSingleShortestPathsTo(sTopoNode *target);
     *    -  void weightedMultiShortestPathsTo(sTopoNode *target);
     */
    //@{

    /**
     * Apply the Dijkstra algorithm to find all shortest paths to the given
     * graph node. The paths found can be extracted via sTopoNode's methods.
     */
    void unweightedSingleShortestPathsTo(sTopoNode *target);

    /**
     * Returns the node that was passed to the most recently called
     * shortest path finding function.
     */
    sTopoNode *targetNode() {return target;}
    //@}
};

#endif

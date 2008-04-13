//=========================================================================
//  CTOPO.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//     cTopology : network topology to find shortest paths etc.
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <deque>
#include <algorithm>
#include "ctopology.h"
#include "cpar.h"
#include "globals.h"
#include "cexception.h"
#include "patternmatcher.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

Register_Class(cTopology);


cTopology::LinkIn *cTopology::Node::in(int i)
{
    if (i<0 || i>=num_in_links)
        throw cRuntimeError("cTopology::Node: invalid in() index %d",i);
    return (cTopology::LinkIn *)in_links[i];
}

cTopology::LinkOut *cTopology::Node::out(int i)
{
    if (i<0 || i>=num_out_links)
        throw cRuntimeError("cTopology::Node: invalid out() index %d",i);
    return (cTopology::LinkOut *)(out_links+i);
}

//----

cTopology::cTopology(const char *name) : cOwnedObject(name)
{
    num_nodes = 0;
    nodev = NULL;
}

cTopology::cTopology(const cTopology& topo) : cOwnedObject()
{
    nodev = NULL;
    setName(topo.name());
    cTopology::operator=(topo);
}

cTopology::~cTopology()
{
    clear();
}

std::string cTopology::info() const
{
    std::stringstream out;
    out << "n=" << num_nodes;
    return out.str();
}

void cTopology::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netPack() not implemented");
}

void cTopology::netUnpack(cCommBuffer *buffer)
{
    throw cRuntimeError(this,"netUnpack() not implemented");
}

cTopology& cTopology::operator=(const cTopology&)
{
    throw cRuntimeError(this,"operator= not implemented yet");
}

void cTopology::clear()
{
    for (int i=0; i<num_nodes; i++)
    {
        delete [] nodev[i].in_links;
        delete [] nodev[i].out_links;
    }
    delete [] nodev;

    num_nodes = 0;
    nodev = NULL;
}

//---

static bool selectByModuleName(cModule *mod, void *data)
{
    // actually, this is selectByModuleFullPathPattern()
    const std::vector<std::string>& v = *(const std::vector<std::string> *)data;
    std::string path = mod->fullPath();
    for (int i=0; i<v.size(); i++)
        if (PatternMatcher(v[i].c_str(), true, true, true).matches(path.c_str()))
            return true;
    return false;
}

static bool selectByNedTypeName(cModule *mod, void *data)
{
    const std::vector<std::string>& v = *(const std::vector<std::string> *)data;
    return std::find(v.begin(), v.end(), mod->nedTypeName()) != v.end();
}

static bool selectByParameter(cModule *mod, void *data)
{
    struct ParamData {const char *name; const char *value;};
    ParamData *d = (ParamData *)data;
    //FIXME if parameter is string type, check against stringValue() as well!!! (quote marks!!)
    return mod->hasPar(d->name) && (d->value==NULL || mod->par(d->name).str()==std::string(d->value));
}

//---

void cTopology::extractByModuleName(const std::vector<std::string>& fullPathPatterns)
{
    extractFromNetwork(selectByModuleName, (void *)&fullPathPatterns);
}

void cTopology::extractByNedTypeName(const std::vector<std::string>& nedTypeNames)
{
    extractFromNetwork(selectByNedTypeName, (void *)&nedTypeNames);
}

void cTopology::extractByParameter(const char *paramName, const char *paramValue)
{
    struct {const char *name; const char *value;} data = {paramName, paramValue};
    extractFromNetwork(selectByParameter, (void *)&data);
}

//---

static bool selectByPredicate(cModule *mod, void *data)
{
    cTopology::Predicate *predicate = (cTopology::Predicate *)data;
    return predicate->matches(mod);
}

void cTopology::extractFromNetwork(Predicate *predicate)
{
    extractFromNetwork(selectByPredicate, (void *)predicate);
}

void cTopology::extractFromNetwork(bool (*selfunc)(cModule *,void *), void *data)
{
    clear();

    int mod_id, gate_id;

    Node *temp_nodev = new Node[simulation.lastModuleId()];

    // Loop through all modules and find those which have the required
    // parameter with the (optionally) required value.
    int k=0;
    for (mod_id=0; mod_id<=simulation.lastModuleId(); mod_id++)
    {
        cModule *mod = simulation.module(mod_id);
        if (mod && selfunc(mod,data))
        {
            // ith module is OK, insert into nodev[]
            temp_nodev[k].module_id = mod_id;
            temp_nodev[k].wgt = 0;
            temp_nodev[k].enabl = true;

            // init auxiliary variables
            temp_nodev[k].known = 0;
            temp_nodev[k].dist = INFINITY;
            temp_nodev[k].out_path = NULL;

            // create in_links[] arrays (big enough...)
            temp_nodev[k].num_in_links = 0;
            temp_nodev[k].in_links = new cTopology::Link *[mod->gates()];

            k++;
        }
    }
    num_nodes = k;

    nodev = new Node[num_nodes];
    memcpy(nodev,temp_nodev,num_nodes*sizeof(Node));
    delete [] temp_nodev;

    // Discover out neighbors too.
    for (k=0; k<num_nodes; k++)
    {
        // Loop through all its gates and find those which come
        // from or go to modules included in the topology.

        cModule *mod = simulation.module(nodev[k].module_id);
        cTopology::Link *temp_out_links = new cTopology::Link[mod->gates()];

        int n_out=0;
        for (gate_id=0; gate_id<mod->gates(); gate_id++)
        {
            cGate *gate = mod->gate(gate_id);
            if (!gate || gate->type()!='O') continue;

            // follow path
            do {
                gate = gate->toGate();
            }
            while(gate && !selfunc(gate->ownerModule(),data));

            // if we arrived in a module in the topology, record it.
            if (gate)
            {
                temp_out_links[n_out].src_node = nodev+k;
                temp_out_links[n_out].src_gate = gate_id;
                temp_out_links[n_out].dest_node = nodeFor(gate->ownerModule());
                temp_out_links[n_out].dest_gate = gate->id();
                temp_out_links[n_out].wgt = 1.0;
                temp_out_links[n_out].enabl = true;
                n_out++;
            }
        }
        nodev[k].num_out_links = n_out;

        nodev[k].out_links = new cTopology::Link[n_out];
        memcpy(nodev[k].out_links,temp_out_links,n_out*sizeof(cTopology::Link));
        delete [] temp_out_links;
    }

    // fill in_links[] arrays
    for (k=0; k<num_nodes; k++)
    {
        for (int l=0; l<nodev[k].num_out_links; l++)
        {
            cTopology::Link *link = &nodev[k].out_links[l];
            link->dest_node->in_links[link->dest_node->num_in_links++] = link;
        }
    }
}

cTopology::Node *cTopology::node(int i)
{
    if (i<0 || i>=num_nodes)
        throw cRuntimeError(this,"invalid node index %d",i);
    return nodev+i;
}

cTopology::Node *cTopology::nodeFor(cModule *mod)
{
    // binary search can be done because nodev[] is ordered

    int lo, up, index;
    for ( lo=0, up=num_nodes, index=(lo+up)/2;
          lo<index;
          index=(lo+up)/2 )
    {
        // cycle invariant: nodev[lo].mod_id <= mod->id() < nodev[up].mod_id
        if (mod->id() < nodev[index].module_id)
             up = index;
        else
             lo = index;
    }
    return (mod->id() == nodev[index].module_id) ? nodev+index : NULL;
}

void cTopology::unweightedSingleShortestPathsTo(Node *_target)
{
    // multiple paths not supported :-(

    if (!_target)
        throw cRuntimeError(this,"..ShortestPathTo(): target node is NULL");
    target = _target;

    for (int i=0; i<num_nodes; i++)
    {
       nodev[i].known = false;   // not really needed for unweighted
       nodev[i].dist = INFINITY;
       nodev[i].out_path = NULL;
    }
    target->dist = 0;

    std::deque<Node*> q;

    q.push_back(target);

    while (!q.empty())
    {
       Node *v = q.front();
       q.pop_front();

       // for each w adjacent to v...
       for (int i=0; i<v->num_in_links; i++)
       {
           if (!(v->in_links[i]->enabl)) continue;

           Node *w = v->in_links[i]->src_node;
           if (!w->enabl) continue;

           if (w->dist == INFINITY)
           {
               w->dist = v->dist + 1;
               w->out_path = v->in_links[i];
               q.push_back(w);
           }
       }
    }
}


/* to be adapted:
void cTopology::weightedSingleShortestPathsTo(Node *_target)
{
    if (!_target)
        throw cRuntimeError(this,"..ShortestPathTo(): target node is NULL");

    target = _target;

    void Dijstra( Table t)

    Vertex v,w;

    for (;;)
    {
       v = smallest unknown distance vertex; // priority queue-val!
                                             // az unknown-ok vannak benne
                                             // priority q = pl. binary heap
       if (v == NO_VERTEX) break;

       t[v].known = true;                    // delete_min()
       for (each w adjacent to v)
       {
           if (!t[v].known  &&  t[v].dist+C(w,v) < t[w].dist)
           {
               decrease( t[w].dist  to  t[v].dist+C(w,v); // decrease_key!
                  // belekavar a prio q-ba!
                  // megoldas lehet, hogy nem vesszuk ki a sorbol,
                  // hanem meg egyszer betesszuk
                  // ekkor fent a delete_min()-ne'l kell vadaszni unknown-okra!
               t[w].path = v;
           }
       }
    }
}
*/

//=========================================================================
//  CTOPOLOGY.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//     cTopology : network topology to find shortest paths etc.
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <deque>
#include <list>
#include <algorithm>
#include <sstream>
#include <functional>
#include "common/patternmatcher.h"
#include "common/stringutil.h"
#include "omnetpp/ctopology.h"
#include "omnetpp/cpar.h"
#include "omnetpp/globals.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cproperty.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

using namespace omnetpp::common;

namespace omnetpp {

Register_Class(cTopology);


cTopology::LinkIn *cTopology::Node::getLinkIn(int i)
{
    if (i < 0 || i >= (int)inLinks.size())
        throw cRuntimeError("cTopology::Node::getLinkIn: Invalid link index %d", i);
    return (cTopology::LinkIn *)inLinks[i];
}

cTopology::LinkOut *cTopology::Node::getLinkOut(int i)
{
    if (i < 0 || i >= (int)outLinks.size())
        throw cRuntimeError("cTopology::Node::getLinkOut: Invalid index %d", i);
    return (cTopology::LinkOut *)outLinks[i];
}

//----

cTopology::cTopology(const char *name) : cOwnedObject(name)
{
}

cTopology::cTopology(const cTopology& topo) : cOwnedObject(topo)
{
    throw cRuntimeError(this, "Copy ctor not implemented yet");
}

cTopology::~cTopology()
{
    clear();
}

std::string cTopology::str() const
{
    std::stringstream out;
    out << "n=" << nodes.size();
    return out.str();
}

cTopology& cTopology::operator=(const cTopology&)
{
    throw cRuntimeError(this, "operator= not implemented yet");
}

void cTopology::clear()
{
    for (auto & node : nodes) {
        for (int j = 0; j < (int)node->outLinks.size(); j++)
            delete node->outLinks[j];  // delete links from their source side
        delete node;
    }
    nodes.clear();
}

//---

static bool selectByModulePath(cModule *mod, void *data)
{
    // actually, this is selectByModuleFullPathPattern()
    const std::vector<std::string>& patterns = *(const std::vector<std::string> *)data;
    std::string path = mod->getFullPath();
    for (const std::string & pattern : patterns)
        if (PatternMatcher(pattern.c_str(), true, true, true).matches(path.c_str()))
            return true;

    return false;
}

static bool selectByNedTypeName(cModule *mod, void *data)
{
    const std::vector<std::string>& typeNames = *(const std::vector<std::string> *)data;
    return std::find(typeNames.begin(), typeNames.end(), mod->getNedTypeName()) != typeNames.end();
}

static bool selectByProperty(cModule *mod, void *data)
{
    struct ParamData {const char *name; const char *value;};
    ParamData *d = (ParamData *)data;
    cProperty *prop = mod->getProperties()->get(d->name);
    if (!prop)
        return false;
    const char *value = prop->getValue(cProperty::DEFAULTKEY, 0);
    if (d->value)
        return opp_strcmp(value, d->value) == 0;
    else
        return opp_strcmp(value, "false") != 0;
}

static bool selectByParameter(cModule *mod, void *data)
{
    struct PropertyData{const char *name; const char *value;};
    PropertyData *d = (PropertyData *)data;
    return mod->hasPar(d->name) && (d->value == nullptr || mod->par(d->name).str() == std::string(d->value));
}

//---

void cTopology::extractByModulePath(const std::vector<std::string>& fullPathPatterns)
{
    extractFromNetwork(selectByModulePath, (void *)&fullPathPatterns);
}

void cTopology::extractByNedTypeName(const std::vector<std::string>& nedTypeNames)
{
    extractFromNetwork(selectByNedTypeName, (void *)&nedTypeNames);
}

void cTopology::extractByProperty(const char *propertyName, const char *value)
{
    struct {const char *name; const char *value;} data = {propertyName, value};
    extractFromNetwork(selectByProperty, (void *)&data);
}

void cTopology::extractByParameter(const char *paramName, const char *paramValue)
{
    struct {const char *name; const char *value;} data = {paramName, paramValue};
    extractFromNetwork(selectByParameter, (void *)&data);
}

//---

class LambdaPredicate : public cTopology::Predicate
{
  private:
    std::function<bool(cModule *)> func;
  public:
    LambdaPredicate(std::function<bool(cModule *)> func) : func(func) {}
    virtual bool matches(cModule *module) override { return func(module); }

};

void cTopology::extractFromNetwork(std::function<bool(cModule *)> predicate)
{
    LambdaPredicate wrappedLambda(predicate);
    extractFromNetwork(&wrappedLambda);
}

static bool selectByPredicate(cModule *mod, void *data)
{
    cTopology::Predicate *predicate = (cTopology::Predicate *)data;
    return predicate->matches(mod);
}

void cTopology::extractFromNetwork(Predicate *predicate)
{
    extractFromNetwork(selectByPredicate, (void *)predicate);
}

void cTopology::extractFromNetwork(bool (*predicate)(cModule *, void *), void *data)
{
    clear();

    // Loop through all modules and find those that satisfy the criteria
    for (int moduleId = 0; moduleId <= getSimulation()->getLastComponentId(); moduleId++) {
        cModule *module = getSimulation()->getModule(moduleId);
        if (module && predicate(module, data)) {
            Node *node = createNode(module);
            nodes.push_back(node);
        }
    }

    // Discover out neighbors too.
    for (auto node : nodes) {
        // Loop through all its gates and find those which come
        // from or go to modules included in the topology.
        cModule *module = getSimulation()->getModule(node->moduleId);

        for (cModule::GateIterator it(module); !it.end(); ++it) {
            cGate *gate = *it;

            // follow path
            cGate *srcGate = gate;
            do {
                gate = gate->getNextGate();
            } while (gate && !predicate(gate->getOwnerModule(), data));

            // if we arrived at a module in the topology, record it.
            if (gate) {
                Link *link = createLink();
                link->srcNode = node;
                link->srcGateId = srcGate->getId();
                link->destNode = getNodeFor(gate->getOwnerModule());
                link->destGateId = gate->getId();
                node->outLinks.push_back(link);
            }
        }
    }

    // fill inLinks vectors
    for (auto & node : nodes) {
        for (int l = 0; l < (int)node->outLinks.size(); l++) {
            cTopology::Link *link = node->outLinks[l];
            link->destNode->inLinks.push_back(link);
        }
    }
}

int cTopology::addNode(Node *node)
{
    if (node->moduleId == -1) {
        // elements without module ID are stored at the end
        nodes.push_back(node);
        return nodes.size()-1;
    }
    else {
        // must find an insertion point because nodes[] is ordered by module ID
        std::vector<Node *>::iterator it = std::lower_bound(nodes.begin(), nodes.end(), node, lessByModuleId);
        it = nodes.insert(it, node);
        return it - nodes.begin();
    }
}

void cTopology::deleteNode(Node *node)
{
    // remove outgoing links
    for (auto link : node->outLinks) {
        unlinkFromDestNode(link);
        delete link;
    }
    node->outLinks.clear();

    // remove incoming links
    for (auto link : node->inLinks) {
        unlinkFromSourceNode(link);
        delete link;
    }
    node->inLinks.clear();

    // remove from nodes[]
    std::vector<Node *>::iterator it = std::find(nodes.begin(), nodes.end(), node);
    ASSERT(it != nodes.end());
    nodes.erase(it);

    delete node;
}

void cTopology::addLink(Link *link, Node *srcNode, Node *destNode)
{
    // remove from graph if it's already in
    if (link->srcNode)
        unlinkFromSourceNode(link);
    if (link->destNode)
        unlinkFromDestNode(link);

    // insert
    if (link->srcNode != srcNode)
        link->srcGateId = -1;
    if (link->destNode != destNode)
        link->destGateId = -1;
    link->srcNode = srcNode;
    link->destNode = destNode;
    srcNode->outLinks.push_back(link);
    destNode->inLinks.push_back(link);
}

void cTopology::addLink(Link *link, cGate *srcGate, cGate *destGate)
{
    // remove from graph if it's already in
    if (link->srcNode)
        unlinkFromSourceNode(link);
    if (link->destNode)
        unlinkFromDestNode(link);

    // insert
    Node *srcNode = getNodeFor(srcGate->getOwnerModule());
    Node *destNode = getNodeFor(destGate->getOwnerModule());
    if (!srcNode)
        throw cRuntimeError("cTopology::addLink: Module of source gate \"%s\" is not in the graph", srcGate->getFullPath().c_str());
    if (!destNode)
        throw cRuntimeError("cTopology::addLink: Module of destination gate \"%s\" is not in the graph", destGate->getFullPath().c_str());
    link->srcNode = srcNode;
    link->destNode = destNode;
    link->srcGateId = srcGate->getId();
    link->destGateId = destGate->getId();
    srcNode->outLinks.push_back(link);
    destNode->inLinks.push_back(link);
}

void cTopology::deleteLink(Link *link)
{
    unlinkFromSourceNode(link);
    unlinkFromDestNode(link);
    delete link;
}

void cTopology::unlinkFromSourceNode(Link *link)
{
    std::vector<Link *>& srcOutLinks = link->srcNode->outLinks;
    std::vector<Link *>::iterator it = std::find(srcOutLinks.begin(), srcOutLinks.end(), link);
    ASSERT(it != srcOutLinks.end());
    srcOutLinks.erase(it);
}

void cTopology::unlinkFromDestNode(Link *link)
{
    std::vector<Link *>& destInLinks = link->destNode->inLinks;
    std::vector<Link *>::iterator it = std::find(destInLinks.begin(), destInLinks.end(), link);
    ASSERT(it != destInLinks.end());
    destInLinks.erase(it);
}

cTopology::Node *cTopology::getNode(int i) const
{
    if (i < 0 || i >= (int)nodes.size())
        throw cRuntimeError(this, "Invalid node index %d", i);
    return nodes[i];
}

cTopology::Node *cTopology::getNodeFor(cModule *mod) const
{
    // binary search because nodes[] is ordered by module ID
    Node tmpNode(mod->getId());
    auto it = std::lower_bound(nodes.begin(), nodes.end(), &tmpNode, lessByModuleId);
    return it == nodes.end() || (*it)->moduleId != mod->getId() ? nullptr : *it;
}

void cTopology::calculateUnweightedSingleShortestPathsTo(Node *_target)
{
    // multiple paths not supported :-(

    if (!_target)
        throw cRuntimeError(this, "..ShortestPathTo(): Target node is nullptr");
    target = _target;

    for (auto & node : nodes) {
        node->dist = INFINITY;
        node->outPath = nullptr;
    }
    target->dist = 0;

    std::deque<Node *> q;

    q.push_back(target);

    while (!q.empty()) {
        Node *v = q.front();
        q.pop_front();

        // for each w adjacent to v...
        for (int i = 0; i < (int)v->inLinks.size(); i++) {
            if (!v->inLinks[i]->enabled)
                continue;

            Node *w = v->inLinks[i]->srcNode;
            if (!w->enabled)
                continue;

            if (w->dist == INFINITY) {
                w->dist = v->dist+1;
                w->outPath = v->inLinks[i];
                q.push_back(w);
            }
        }
    }
}

void cTopology::calculateWeightedSingleShortestPathsTo(Node *_target)
{
    if (!_target)
        throw cRuntimeError(this, "..ShortestPathTo(): Target node is nullptr");
    target = _target;

    // clean path infos
    for (auto & node : nodes) {
        node->dist = INFINITY;
        node->outPath = nullptr;
    }

    target->dist = 0;

    std::list<Node *> q;

    q.push_back(target);

    while (!q.empty()) {
        Node *dest = q.front();
        q.pop_front();

        ASSERT(dest->getWeight() >= 0.0);

        // for each w adjacent to v...
        for (int i = 0; i < dest->getNumInLinks(); i++) {
            if (!(dest->getLinkIn(i)->isEnabled()))
                continue;

            Node *src = dest->getLinkIn(i)->getRemoteNode();
            if (!src->isEnabled())
                continue;

            double linkWeight = dest->getLinkIn(i)->getWeight();
            ASSERT(linkWeight > 0.0);

            double newdist = dest->dist + linkWeight;
            if (dest != target)
                newdist += dest->getWeight();  // dest is not the target, uses weight of dest node as price of routing (infinity means dest node doesn't route between interfaces)
            if (newdist != INFINITY && src->dist > newdist) {  // it's a valid shorter path from src to target node
                if (src->dist != INFINITY)
                    q.remove(src);  // src is in the queue
                src->dist = newdist;
                src->outPath = dest->inLinks[i];

                // insert src node to ordered list
                std::list<Node *>::iterator it;
                for (it = q.begin(); it != q.end(); ++it)
                    if ((*it)->dist > newdist)
                        break;

                q.insert(it, src);
            }
        }
    }
}

}  // namespace omnetpp


//=========================================================================
//  COMPOUNDFILTER.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include "util.h"
#include "compoundfilter.h"
#include "filternodes.h"
#include "nodetyperegistry.h"


bool CompoundFilterType::Subfilter::operator==(const CompoundFilterType::Subfilter& other) const
{
    return _nodetype==other._nodetype &&
           _comment==other._comment &&
           _attrassignments==other._attrassignments;
}

const char *CompoundFilterType::name() const
{
    return _name.c_str();
}

const char *CompoundFilterType::description() const
{
    return _description.c_str();
}

void CompoundFilterType::setName(const char *s)
{
    _name = s;
}

void CompoundFilterType::setDescription(const char *s)
{
    _description = s;
}

CompoundFilterType& CompoundFilterType::operator=(const CompoundFilterType& other)
{
    _name = other._name;
    _description = other._description;
    _hidden = other._hidden;
    _attrs = other._attrs;
    _defaults = other._defaults;
    _subfilters = other._subfilters;
    return *this;
}

bool CompoundFilterType::equals(const CompoundFilterType& other)
{
    // ignore name (they're surely different)
    // ignore "hidden" flag too
    return _description==other._description &&
           _attrs==other._attrs &&
           _defaults==other._defaults &&
           _subfilters==other._subfilters;
}

void CompoundFilterType::getAttributes(StringMap& attrs) const
{
    attrs = _attrs;
}

void CompoundFilterType::getAttrDefaults(StringMap& attrs) const
{
    attrs = _defaults;
}

void CompoundFilterType::setAttr(const char *name, const char *desc, const char *defaultval)
{
    _attrs[name] = desc;
    _defaults[name] = defaultval;
}

void CompoundFilterType::removeAttr(const char *name)
{
    _attrs.erase(std::string(name));
    _defaults.erase(std::string(name));
}

int CompoundFilterType::numSubfilters() const
{
    return _subfilters.size();
}

CompoundFilterType::Subfilter& CompoundFilterType::subfilter(int pos)
{
    if (pos<0 || (unsigned)pos>=_subfilters.size())
        throw new Exception("%s: invalid subfilter index %d", name(), pos);
    return _subfilters[pos];
}

void CompoundFilterType::insertSubfilter(int pos, const Subfilter& f)
{
    if (pos<0 || (unsigned)pos>_subfilters.size())
        throw new Exception("%s: invalid subfilter insert index %d", name(), pos);
    _subfilters.insert(_subfilters.begin()+pos, f);
}

void CompoundFilterType::removeSubfilter(int pos)
{
    if (pos<0 || (unsigned)pos>=_subfilters.size())
        throw new Exception("%s: invalid subfilter index %d", name(), pos);
    _subfilters.erase(_subfilters.begin()+pos);
}

Node *CompoundFilterType::create(DataflowManager *mgr, StringMap& attrs) const
{
    CompoundFilter *node = new CompoundFilter();
    node->setNodeType(this);
    node->_attrvalues = attrs;

    int n = numSubfilters();
    if (n==0)
    {
        FilterNode *subnode = new NopNode();
        mgr->addNode(subnode);
        node->first = node->last = subnode;
    }
    Node *prevsubnode = NULL;
    for (int i=0; i<n; i++)
    {
        Subfilter& subfilt = const_cast<CompoundFilterType *>(this)->subfilter(i);

        // get type
        const char *subnodetypename = subfilt.nodeType();
        NodeType *subnodetype = NodeTypeRegistry::instance()->getNodeType(subnodetypename);

        // collect parameters for subfilter
        StringMap subattrs;
        StringMap subattrsall = subfilt.attrAssignments();
        // pick needed attrs
        StringMap allowedattrs;
        subnodetype->getAttributes(allowedattrs);
        for (StringMap::iterator aattr=allowedattrs.begin(); aattr!=allowedattrs.end();++aattr)
            subattrs[aattr->first] = subattrsall[aattr->first];
        // now perform param substitutions
        for (StringMap::iterator subattr1=subattrs.begin(); subattr1!=subattrs.end(); ++subattr1)
        {
            StringMap::iterator attrfound = attrs.find(subattr1->second);
            if (attrfound!=attrs.end())
                subattr1->second = attrfound->second;
        }

        // create and add instance
        FilterNode *subnode = dynamic_cast<FilterNode *>(subnodetype->create(mgr,subattrs));
        if (!subnode)
            throw new Exception("%s: subfilter type %s is not subclassed from FilterNode", name(), subnodetypename);
        mgr->addNode(subnode);
        if (i==0)
            node->first = subnode;
        if (i==n-1)
            node->last = subnode;

        // connect to next one
        if (prevsubnode)
        {
            Port *port1 = prevsubnode->nodeType()->getPort(prevsubnode,"out");
            Port *port2 = subnode->nodeType()->getPort(subnode,"in");
            mgr->connect(port1, port2);
        }
        prevsubnode = subnode;
    }

    return node;
}

Port *CompoundFilterType::getPort(Node *node, const char *name) const
{
    CompoundFilter *compound = dynamic_cast<CompoundFilter *>(node);
    Node *subnode = !strcmp(name,"in") ? compound->firstNode() :
                    !strcmp(name,"out") ? compound->lastNode() :
                    NULL;
    if (!subnode)
        throw new Exception("no such port `%s'", name);
    return subnode->nodeType()->getPort(subnode, name);
}



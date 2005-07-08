//=========================================================================
//  NODETYPEREGISTRY.CC - part of
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
#include "nodetyperegistry.h"

#include "arraybuilder.h"
#include "vectorfilereader.h"
#include "vectorfilewriter.h"
#include "filewriter.h"
#include "windowavg.h"
#include "slidingwinavg.h"
#include "filternodes.h"
#include "mergernodes.h"
#include "xyplotnode.h"
#include "diffquot.h"


NodeTypeRegistry *NodeTypeRegistry::inst;

NodeTypeRegistry *NodeTypeRegistry::instance()
{
    if (!inst)
        inst = new NodeTypeRegistry();
    return inst;
}

void NodeTypeRegistry::add(NodeType *nodetype)
{
    nodeTypeMap[nodetype->name()] = nodetype;
}

void NodeTypeRegistry::remove(NodeType *nodetype)
{
    NodeTypeMap::iterator it = nodeTypeMap.find(nodetype->name());
    if (it!=nodeTypeMap.end())
        nodeTypeMap.erase(it);
}

NodeTypeRegistry::NodeTypeRegistry()
{
    add(new ArrayBuilderNodeType());
    add(new VectorFileReaderNodeType());
    add(new VectorFileWriterNodeType());
    add(new FileWriterNodeType());
    add(new MergerNodeType());
    add(new XYPlotNodeType());

    add(new WindowAverageNodeType());
    add(new SlidingWindowAverageNodeType());
    add(new MovingAverageNodeType());
    add(new DifferenceQuotientNodeType());
    add(new NopNodeType());
    add(new AdderNodeType());
    add(new MultiplierNodeType());
    add(new DividerNodeType());
    add(new ModuloNodeType());
    add(new DifferenceNodeType());
    add(new SumNodeType());
    add(new TimeShiftNodeType());
    add(new LinearTrendNodeType());
    add(new CropNodeType());
    add(new MeanNodeType());
}

NodeTypeRegistry::~NodeTypeRegistry()
{
    for (NodeTypeMap::iterator it=nodeTypeMap.begin(); it!=nodeTypeMap.end(); it++)
        delete it->second;
}

bool NodeTypeRegistry::exists(const char *name)
{
    return nodeTypeMap.find(name)!=nodeTypeMap.end();
}

NodeType *NodeTypeRegistry::getNodeType(const char *name)
{
    NodeTypeMap::iterator it = nodeTypeMap.find(name);
    if (it==nodeTypeMap.end())
        throw new Exception("unknown node type `%s'", name);
    return it->second;
}

void NodeTypeRegistry::getNodeTypes(NodeTypeVector& vect)
{
    for (NodeTypeMap::iterator it=nodeTypeMap.begin(); it!=nodeTypeMap.end(); it++)
        vect.push_back(it->second);
}


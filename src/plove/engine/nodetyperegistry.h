//=========================================================================
//  NODETYPEREGISTRY.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _NODETYPEREGISTRY_H_
#define _NODETYPEREGISTRY_H_

#include <map>
#include <vector>
#include <string>

class NodeType;

typedef std::vector<NodeType*> NodeTypeVector;


/**
 * Factory for all available node types.
 *
 * @see NodeType, Node
 */
class NodeTypeRegistry
{
    protected:
        static NodeTypeRegistry *inst;
        typedef std::map<std::string,NodeType*> NodeTypeMap;
        NodeTypeMap nodeTypeMap;

        NodeTypeRegistry();
        ~NodeTypeRegistry();

    public:
        static NodeTypeRegistry *instance();
        bool exists(const char *name);
        NodeType *getNodeType(const char *name);
        void getNodeTypes(NodeTypeVector& vect);
        void add(NodeType *nodetype);
        void remove(NodeType *nodetype);

};

#endif

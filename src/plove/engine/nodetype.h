//=========================================================================
//  NODETYPE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _NODETYPE_H_
#define _NODETYPE_H_

#include <map>
#include <string>
#include "dataflowmanager.h"

class Node;
class Port;

typedef std::map<std::string,std::string> StringMap;


/**
 * Describes properties of a node class.
 *
 * @see Node, NodeTypeRegistry
 */
class NodeType
{
    protected:
        void checkAttrNames(const StringMap& attr) const;

    public:
        NodeType() {}
        virtual ~NodeType() {}

        /**
         * Returns type name as displayed on the UI (e.g. "mean" or "winavg")
         */
        virtual const char *name() const = 0;

        /**
         * Returns the category of the node (source, sink, filter, etc.)
         */
        virtual const char *category() const = 0;

        /**
         * Returns the description of the node (this can be displayed on the UI.)
         */
        virtual const char *description() const = 0;

        /**
         * Returns true if this node should appear in the UI.
         */
        virtual bool isHidden() const = 0;

        /**
         * Fills the string map with attribute names and their descriptions.
         */
        virtual void getAttributes(StringMap& attrs) const = 0;

        /**
         * Fills the string map with attribute names and their default values.
         * (Doesn't necessarily supply default value for every attribute).
         */
        virtual void getAttrDefaults(StringMap& attrs) const {}

        /**
         * Throws exception if something is not OK. The message in the
         * exception is shown to the user.
         */
        virtual void validateAttrValues(const StringMap& attrs) const {}

        /**
         * Create a node instance of this type.
         */
        virtual Node *create(DataflowManager *, StringMap& attrs) const = 0;

        /**
         * Get the named port of the give node instance.
         */
        virtual Port *getPort(Node *node, const char *portname) const = 0;
};


#endif

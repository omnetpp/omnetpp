//=========================================================================
//  NODETYPE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_NODETYPE_H
#define __OMNETPP_SCAVE_NODETYPE_H

#include <map>
#include <string>
#include <vector>
#include "dataflowmanager.h"

namespace omnetpp {
namespace scave {

class Node;
class Port;

typedef std::map<std::string,std::string> StringMap;
typedef std::vector<std::string> StringVector;


/**
 * Describes properties of a node class.
 *
 * @see Node, NodeTypeRegistry
 */
class SCAVE_API NodeType
{
    protected:
        void checkAttrNames(const StringMap& attr) const;

    public:
        NodeType() {}
        virtual ~NodeType() {}

        /**
         * Returns type name as displayed on the UI (e.g. "mean" or "winavg")
         */
        virtual const char *getName() const = 0;

        /**
         * Returns the category of the node (source, sink, filter, etc.)
         */
        virtual const char *getCategory() const = 0;

        /**
         * Returns the description of the node (this can be displayed on the UI.)
         */
        virtual const char *getDescription() const = 0;

        /**
         * Returns true if this node should not appear in the UI.
         */
        virtual bool isHidden() const = 0;

        /**
         * Fills the string map with the names of all accepted attributes
         * and their descriptions.
         */
        virtual void getAttributes(StringMap& attrs) const = 0;

        /**
         * Fills the string map with attribute names and their default values.
         * (Does not necessarily supply default value for every attribute).
         */
        virtual void getAttrDefaults(StringMap& attrs) const {}

        /**
         * Throws exception if something is not OK. The message in the
         * exception is shown to the user.
         */
        virtual void validateAttrValues(const StringMap& attrs) const {}

        /**
         * Create a node instance of this type. Also adds the node to the
         * data-flow network, so you do not need to call mgr->addNode() afterwards.
         */
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const = 0;

        /**
         * Get the named port of the give node instance.
         */
        virtual Port *getPort(Node *node, const char *portname) const = 0;

        /**
         * Maps input vector attributes to output vector attributes.
         * This method is called for filter nodes only.
         */
        virtual void mapVectorAttributes(/*inout*/StringMap& attrs, /*out*/StringVector& warnings) const;
};


} // namespace scave
}  // namespace omnetpp


#endif

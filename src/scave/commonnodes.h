//=========================================================================
//  COMMONNODES.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _COMMONNODES_H_
#define _COMMONNODES_H_

#include <string>
#include "node.h"
#include "nodetype.h"
#include "filereader.h"

NAMESPACE_BEGIN

#ifdef _MSC_VER
#pragma warning(disable:4355) // 'this' : used in base member initializer list
#endif


/**
 * Base class for one-port producer nodes.
 */
class SCAVE_API SingleSourceNode : public Node
{
    public:
        Port out;
    public:
        SingleSourceNode() : out(this) {}
};


/**
 * Base class for one-port consumer nodes.
 */
class SCAVE_API SingleSinkNode : public Node
{
    public:
        Port in;
    public:
        SingleSinkNode() : in(this) {}
};


/**
 * Base class for processing nodes with one input and one output port.
 */
class SCAVE_API FilterNode : public Node
{
    protected:
        virtual bool finished() const;
    public:
        Port in;
        Port out;
    public:
        FilterNode() : in(this), out(this) {}
};

/**
 * Base class for reading input from file.
 */
class SCAVE_API ReaderNode : public Node
{
    protected:
        std::string filename;
        FileReader reader;
    public:
        ReaderNode(const char* filename, size_t bufferSize)
            : filename(filename), reader(filename, bufferSize) {}
        int64 getFileSize() { return reader.getFileSize(); }
        int64 getNumReadBytes() { return reader.getNumReadBytes(); }
};

/**
 * NodeType for SingleSourceNode.
 */
class SCAVE_API SingleSourceNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "single sources";}
        virtual bool isHidden() const {return true;}
        virtual Port *getPort(Node *node, const char *portname) const;
};


/**
 * NodeType for SingleSinkNode.
 */
class SCAVE_API SingleSinkNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "single sinks";}
        virtual bool isHidden() const {return true;}
        virtual Port *getPort(Node *node, const char *portname) const;
};


/**
 * NodeType for FilterNode.
 */
class SCAVE_API FilterNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "filter";}
        virtual bool isHidden() const {return false;}
        virtual Port *getPort(Node *node, const char *portname) const;
};

/**
 * NodeType for ReaderNode.
 */
class SCAVE_API ReaderNodeType : public NodeType
{
    public:
        virtual const char *getCategory() const {return "reader-node";}
        virtual bool isHidden() const {return true;}
};

NAMESPACE_END


#endif



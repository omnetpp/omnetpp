//==========================================================================
//   OSGUTIL.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_OSGUTIL_H
#define __OMNETPP_OSGUTIL_H

// IMPORTANT:
// DO NOT #include THIS FILE INTO <OMNETPP.H> BECAUSE WE DON'T WANT
// <OMNETPP.H> TO PULL IN OSG HEADERS!

#include "simkerneldefs.h"

#ifdef WITH_OSG

#include <osg/Group>

NAMESPACE_BEGIN

class cObject;

/**
 * osg::Node for defining correspondence to an OMNeT++ object.
 */
class SIM_API OmnetppObjectNode : public osg::Group
{
    protected:
        int componentId; // 0=none
        cObject *object; // object pointer; if componentId!=0, it takes precedence

    protected:
        virtual ~OmnetppObjectNode();

    public:
        OmnetppObjectNode();
        OmnetppObjectNode(cObject* object);
        OmnetppObjectNode(const OmnetppObjectNode& node, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
        META_Node(osg, OmnetppObjectNode);

        cObject* getObject() const;
        void setObject(cObject* obj);
};

NAMESPACE_END

#endif // WITH_OSG

#endif

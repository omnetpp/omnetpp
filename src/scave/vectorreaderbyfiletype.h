//=========================================================================
//  SQLITEVECTORREADER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Tamas Borbely, Zoltan Bojthe
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_VECTORREADERBYFILETYPE_H
#define __OMNETPP_SCAVE_VECTORREADERBYFILETYPE_H

//#include "common/filereader.h"
//#include "common/linetokenizer.h"
#include "node.h"
#include "nodetype.h"
#include "commonnodes.h"
//#include "indexfile.h"
//#include "resultfilemanager.h"


namespace omnetpp {
namespace scave {

class SCAVE_API VectorReaderByFileTypeNodeType : public ReaderNodeType
{
    public:
        virtual const char *getName() const override {return "vectorreaderbyfiletype";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
        virtual Port *getPort(Node *node, const char *portname) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif

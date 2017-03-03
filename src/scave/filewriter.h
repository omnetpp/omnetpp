//=========================================================================
//  FILEWRITER.H - part of
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

#ifndef __OMNETPP_SCAVE_FILEWRITER_H
#define __OMNETPP_SCAVE_FILEWRITER_H

#include <string>
#include "commonnodes.h"

namespace omnetpp {
namespace scave {


/**
 * Processing node which writes data into a file. Every line contains two
 * real numbers (time and value) separated by a tab.
 */
class SCAVE_API FileWriterNode : public SingleSinkNode
{
    private:
        std::string fileName;
        std::string banner;
        FILE *f;
        int prec;
    public:
        FileWriterNode(const char *filename, const char *banner=nullptr);
        virtual ~FileWriterNode();
        void setPrecision(int prec) {this->prec = prec;}
        virtual bool isReady() const override;
        virtual void process() override;
        virtual bool isFinished() const override;
};

class SCAVE_API FileWriterNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *getName() const override {return "filewriter";}
        virtual const char *getDescription() const override;
        virtual void getAttributes(StringMap& attrs) const override;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const override;
};


} // namespace scave
}  // namespace omnetpp


#endif



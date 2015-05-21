//=========================================================================
//  FILEWRITER.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_FILEWRITER_H
#define __OMNETPP_FILEWRITER_H

#include <string>
#include "commonnodes.h"

NAMESPACE_BEGIN


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
        virtual bool isReady() const;
        virtual void process();
        virtual bool isFinished() const;
};

class SCAVE_API FileWriterNodeType : public SingleSinkNodeType
{
    public:
        virtual const char *getName() const {return "filewriter";}
        virtual const char *getDescription() const;
        virtual void getAttributes(StringMap& attrs) const;
        virtual Node *create(DataflowManager *mgr, StringMap& attrs) const;
};


NAMESPACE_END


#endif



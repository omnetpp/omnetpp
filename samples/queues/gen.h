//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>

class Generator : public cSimpleModule
{
    Module_Class_Members(Generator,cSimpleModule,8192)
    virtual void activity();
};

#endif

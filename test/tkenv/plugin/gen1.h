//
// file: gen1.h
//
// This file is part of an OMNeT++/OMNEST demo simulation.
//

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>

class FF1Generator : public cSimpleModule
{
    Module_Class_Members(FF1Generator,cSimpleModule,16384)
    virtual void activity();
    virtual void finish();
};

#endif

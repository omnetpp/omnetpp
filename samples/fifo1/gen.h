//-------------------------------------------------------------
// file: gen.h
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __GEN_H
#define __GEN_H

#include "omnetpp.h"

class Generator : public cSimpleModule
{
    Module_Class_Members(Generator,cSimpleModule,16384)
    virtual void activity();
    virtual void finish();
};

#endif

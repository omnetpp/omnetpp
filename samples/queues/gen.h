//-------------------------------------------------------------
// file: gen.h
//        (part of Queues - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __GEN_H
#define __GEN_H

#include <omnetpp.h>

class Generator : public cSimpleModule
{
    Module_Class_Members(Generator,cSimpleModule,8192)
    virtual void activity();
};

#endif

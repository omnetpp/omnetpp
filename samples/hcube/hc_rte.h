//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifndef __HC_RTE_H
#define __HC_RTE_H

#include <omnetpp.h>

class HCRouter : public cSimpleModule
{
       Module_Class_Members(HCRouter,cSimpleModule,16384)
       virtual void activity();
};

#endif



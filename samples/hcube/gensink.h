//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __GENSINK_H
#define __GENSINK_H

#include <omnetpp.h>

class HCGenerator : public cSimpleModule
{
       Module_Class_Members(HCGenerator,cSimpleModule,16384)
       virtual void activity();
};

class HCSink : public cSimpleModule
{
       Module_Class_Members(HCSink,cSimpleModule,16384)
       virtual void activity();
};

#endif



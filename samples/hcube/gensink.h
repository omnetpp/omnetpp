//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#ifndef __GENSINK_H
#define __GENSINK_H

#include <omnetpp.h>

class HCGenerator : public cSimpleModule
{
  public:
    HCGenerator() : cSimpleModule(16384) {}   //NEWCTOR2
    virtual void activity();
};

class HCSink : public cSimpleModule
{
  public:
    HCSink() : cSimpleModule(16384) {}   //NEWCTOR2
    virtual void activity();
};

#endif



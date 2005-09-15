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

#define STACKSIZE 16384


class HCGenerator : public cSimpleModule
{
  public:
    HCGenerator() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

class HCSink : public cSimpleModule
{
  public:
    HCSink() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

#endif



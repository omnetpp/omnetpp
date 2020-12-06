//==========================================================================
//   COWNINGCONTEXTSWITCHER.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COWNINGCONTEXTSWITCHER_H
#define __OMNETPP_COWNINGCONTEXTSWITCHER_H

#include "omnetpp/csoftowner.h"

namespace omnetpp {

class cOwningContextSwitcher
{
  private:
    cSoftOwner *oldOwner;
  public:
    cOwningContextSwitcher(cSoftOwner *newOwner) : oldOwner(cOwnedObject::getOwningContext()) {cOwnedObject::setOwningContext(newOwner);}
    ~cOwningContextSwitcher() {cOwnedObject::setOwningContext(oldOwner);}
};

class cTemporaryOwner : public cSoftOwner
{
  private:
    cSoftOwner *oldOwner;
  public:
    cTemporaryOwner() : oldOwner(getOwningContext()) {setOwningContext(this);}
    ~cTemporaryOwner() {if (oldOwner) setOwningContext(oldOwner);}
    void yield() {setOwningContext(oldOwner); oldOwner = nullptr;}
    void drop(cOwnedObject *obj) {cSoftOwner::drop(obj);}
};

}  // namespace omnetpp


#endif



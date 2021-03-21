//==========================================================================
//   CTEMPORARYOWNER.H  - part of
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

#ifndef __OMNETPP_CTEMPORARYOWNER_H
#define __OMNETPP_CTEMPORARYOWNER_H

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
  public:
    enum class DtorMode {REPORT_AS_UNDISPOSED, DISPOSE, DROP, ASSERTNONE};
  private:
    DtorMode mode;
    cSoftOwner *oldOwner;
  protected:
    virtual void objectStealingOnDeletion(cOwnedObject *obj) override { /*allow*/ }
  public:
    cTemporaryOwner(DtorMode mode) : mode(mode), oldOwner(getOwningContext()) {setOwningContext(this);}
    ~cTemporaryOwner();
    void restoreOriginalOwner() {setOwningContext(oldOwner); oldOwner = nullptr;}
    void drop(cOwnedObject *obj) {cSoftOwner::drop(obj);}
    DtorMode getDtorMode() const {return mode;}
    void setDtorMode(DtorMode mode) {this->mode = mode;}
};

}  // namespace omnetpp


#endif



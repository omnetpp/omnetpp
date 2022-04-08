//==========================================================================
//   CVALUECONTAINER.H  - part of
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

#ifndef __OMNETPP_CVALUECONTAINER_H
#define __OMNETPP_CVALUECONTAINER_H

#include <string>
#include <vector>
#include "cvalue.h"
#include "cownedobject.h"


namespace omnetpp {

/**
 * @brief Common base class for cValue-based containers.
 *
 * @ingroup Expressions
 */
class SIM_API cValueContainer : public cOwnedObject
{
  private:
    virtual void parsimPack(cCommBuffer *) const override {throw cRuntimeError(this, E_CANTPACK);}
    virtual void parsimUnpack(cCommBuffer *) override {throw cRuntimeError(this, E_CANTPACK);}

  protected:
    void valueCloned(cValue& value, const cValueContainer& other);
    void takeValue(cValue& value);
    void dropValue(cValue& value);
    void dropAndDeleteValue(cValue& value);

  public:
    //@}
    /**
     * Constructor.
     */
    explicit cValueContainer(const char *name=nullptr) : cOwnedObject(name) {}
    //@}
};


}  // namespace omnetpp

#endif



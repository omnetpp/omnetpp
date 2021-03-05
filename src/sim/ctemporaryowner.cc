//==========================================================================
//   CTEMPORARYOWNER.CC  - part of
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

#include "ctemporaryowner.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"

namespace omnetpp {

cTemporaryOwner::~cTemporaryOwner()
{
    if (oldOwner)
        setOwningContext(oldOwner);

    switch (mode) {
        case DtorMode::REPORT_AS_UNDISPOSED: {
            // just leave it to the base class, which does exactly that
            break;
        }
        case DtorMode::DISPOSE: {
            while (getNumOwnedObjects() > 0)
                delete getOwnedObject(0);
            break;
        }
        case DtorMode::DROP: {
            if (getOwningContext() == this)
                getEnvir()->alert((getClassAndFullName() + ": Destructor: Cannot drop objects because getOwningContext()==this").c_str()); // and let base class fall back to printing UNDISPOSED
            else {
                while (getNumOwnedObjects() > 0)
                    drop(getOwnedObject(0));
            }
            break;
        }
        case DtorMode::ASSERTNONE: {
            if (getNumOwnedObjects() > 0)
                getEnvir()->alert((getClassAndFullName() + ": Destructor: getNumOwnedObjects() > 0, should be ==0").c_str()); // and let base class fall back to printing UNDISPOSED
            break;
        }
    }
}

}  // namespace omnetpp


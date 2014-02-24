//==========================================================================
//  INSPFACTORY.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INSPFACTORY_H
#define __INSPFACTORY_H

#include <tk.h>
#include "onstartup.h"
#include "cregistrationlist.h"
#include "inspector.h"
#include "envirbase.h"

NAMESPACE_BEGIN

#define Register_InspectorFactory(FACTORYNAME) \
  EXECUTE_ON_STARTUP(inspectorfactories.getInstance()->add(new FACTORYNAME(#FACTORYNAME));)


/**
 * Serves as a base class for inspector factories of specific classes.
 */
class TKENV_API InspectorFactory : public cNoncopyableOwnedObject
{
  protected:
    virtual Inspector *prepare(Inspector *insp) {insp->setType(getInspectorType()); return insp;}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    InspectorFactory(const char *name) : cNoncopyableOwnedObject(name,false) {}

    /**
     * Destructor.
     */
    virtual ~InspectorFactory() {}
    //@}


    /** @name Inspector creation. */
    //@{
    /**
     * Returns true if this factory can create an inspector for this object.
     * Works with RTTI.
     */
    virtual bool supportsObject(cObject *object) = 0;

    /**
     * Returns type of inspector created by this factory (INSP_* constants).
     * Works with RTTI.
     */
    virtual int getInspectorType() = 0;

    /**
     * Returns "how good" this inspector is as default inspector for this object.
     * Expressed as distance in the hiearchy tree from cObject, i.e.
     *  1.0 for TObjectInspector and TContainerInspector for common objects;
     *  2.0 for TMessageInspector, TWatchInspector, etc, and TContainerInspector for cArray&cQueue;
     *  3.0 for TPacketInspector; ...
     */
    virtual double getQualityAsDefault(cObject *object) = 0;

    /**
     * Creates an inspector.
     */
    virtual Inspector *createInspector() = 0;
    //@}
};

///< List of cInspectorFactory objects.
extern cGlobalRegistrationList inspectorfactories;

/**
 * Find a cInspectorFactory.
 */
InspectorFactory *findInspectorFactoryFor(cObject *obj, int type);

NAMESPACE_END


#endif




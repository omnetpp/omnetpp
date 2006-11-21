//==========================================================================
//  INSPFACTORY.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INSPFACTORY_H
#define __INSPFACTORY_H

#include <tk.h>
#include "onstartup.h"
#include "inspector.h"
#include "omnetapp.h"

#define Register_InspectorFactory(FACTORYNAME) \
  EXECUTE_ON_STARTUP(FACTORYNAME##__ifc, inspectorfactories.instance()->add(new FACTORYNAME(#FACTORYNAME));)


/**
 * Serves as a base class for inspector factories of specific classes.
 */
class cInspectorFactory : public cNoncopyableObject
{
  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * Constructor.
     */
    cInspectorFactory(const char *name) : cNoncopyableObject(name,false) {}

    /**
     * Destructor.
     */
    virtual ~cInspectorFactory() {}
    //@}


    /** @name Inspector creation. */
    //@{
    /**
     * Returns true if this factory can create an inspector for this object.
     * Works with RTTI.
     */
    virtual bool supportsObject(cPolymorphic *object) = 0;

    /**
     * Returns type of inspector created by this factory (INSP_* constants).
     * Works with RTTI.
     */
    virtual int inspectorType() = 0;

    /**
     * Returns "how good" this inspector is as default inspector for this object.
     * Expressed as distance in the hiearchy tree from cPolymorphic, i.e.
     *  1.0 for TObjectInspector and TContainerInspector for common objects;
     *  2.0 for TMessageInspector, TWatchInspector, etc, and TContainerInspector for cArray&cQueue;
     *  3.0 for TPacketInspector; ...
     */
    virtual double qualityAsDefault(cPolymorphic *object) = 0;

    /**
     * Creates an inspector for the object passed. The type and data
     * arguments influence the type of inspector created.
     */
    virtual TInspector *createInspectorFor(cPolymorphic *object,int type,const char *geom,void *data) = 0;
    //@}
};

///< List of cInspectorFactory objects.
extern cRegistrationList inspectorfactories;

/**
 * Find a cInspectorFactory.
 */
cInspectorFactory *findInspectorFactoryFor(cPolymorphic *obj, int type);

#endif




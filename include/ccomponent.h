//==========================================================================
//   CCOMPONENT.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CCOMPONENT_H
#define __CCOMPONENT_H

#include "defs.h"
#include "cobject.h"
#include "cdefaultlist.h"

class cComponentType;
class cProperties;
class cPar;
class cRNG;

/**
 * Common base for module and channel classes: cModule and cChannel.
 * cComponent provides parameters, properties and RNG mapping.
 *
 * @ingroup SimCore
 */
 //FIXME define initialize() here!!!
class SIM_API cComponent : public cDefaultList // noncopyable
{
    friend class cPar; // needs to call handleParameterChange()
  protected:
    cComponentType *componenttype;  // component type object

    cProperties *props;    // if NULL, use the one from the component type
    cPar **paramv; // parameters; num & order determined by the componenttype object

    short rngmapsize;  // size of rngmap array (RNGs>=rngmapsize are mapped one-to-one to physical RNGs)
    int *rngmap;       // maps local RNG numbers (may be NULL if rngmapsize==0)
    bool ev_enabled;   // if output from ev<< is enabled

  public:
    // internal: currently used by Cmdenv
    void setEvEnabled(bool e)  {ev_enabled = e;}
    bool isEvEnabled() {return ev_enabled;}

    // internal: invoked from within cEnvir::getRNGMappingFor(mod)
    void setRNGMap(short size, int *map) {rngmapsize=size; rngmap=map;}

    // internal: sets associated cComponentType for the component;
    // called as part of the creation process.
    virtual void setComponentType(cComponentType *componenttype);

    // internal: return parameter properties from the component type object
    virtual cProperties *defaultParProperties(int k) const;

  protected:
    /**
     * This method is called by the simulation kernel to notify the module
     * that the value of an existing module parameter got changed.
     * Redefining this method allows simple modules to be prepared for
     * parameter changes, e.g. by re-reading the value.
     * This default implementation does nothing.
     *
     * To make it easier to write predictable simple modules, the function does
     * NOT get called during initialize() or finish(). If you need
     * notifications within those two functions as well, add the following
     * code to them:
     *
     * <pre>
     * for (int i=0; i<params(); i++)
     *     handleParameterChange(par(i).name());
     * </pre>
     *
     * Also, one must be extremely careful when changing parameters from inside
     * handleParameterChange(), to avoid creating an infinite notification loop.
     */
    virtual void handleParameterChange(const char *parname);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{
    /**
     * FIXME revise comment!!!
     * Constructor. Note that module objects should not be created directly,
     * only via their cComponentType objects. cComponentType::create() will do
     * all housekeeping tasks associated with module creation (assigning
     * an ID to the module, inserting it into the global <tt>simulation</tt>
     * object (see cSimulation), etc.).
     */
    cComponent(const char *name = NULL);

    /**
     * Destructor.
     */
    virtual ~cComponent();
    //@}

    /** @name Redefined cPolymorphic functions */
    //@{
    /**
     * Lie about the class name: we return the NED type name instead of the
     * real one, that is, "MobileHost" instead of "cCompoundModule" for example.
     */
    virtual const char *className() const;

    /**
     * Serialization not supported, this method is redefined to throw an error.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Serialization not supported, this method is redefined to throw an error.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Misc. */
    //@{
    /**
     * Returns the associated component type.
     */
    cComponentType *componentType() const  {return componenttype;}

    /**
     * Redefined to return true in cModule and subclasses, otherwise returns false.
     */
    virtual bool isModule() const  {return false;}

    /**
     * Returns the global RNG mapped to local RNG number k. For large indices
     * (k >= map size) the global RNG k is returned, provided it exists.
     */
    cRNG *rng(int k) const  {return ev.rng(k<rngmapsize ? rngmap[k] : k);}
    //@}

    /** @name Properties. */
    //@{
    /**
     * Return the properties for this component. See also unlockProperties().
     */
    virtual cProperties *properties();

    /**
     * Allows modification of component properties. By default, properties are
     * locked against modifications, because properties() returns a shared copy.
     * This method creates an own, modifiable copy for this component instance.
     */
    virtual cProperties *unlockProperties();
    //@}

    /** @name Parameters. */
    //@{

    /**
     * Returns total number of the module's parameters.
     */
    virtual int params() const;  // FIXME change to numParams

    /**
     * Returns name of the module parameter identified with its
     * index k. Throws an error if the parameter does not exist.
     */
    virtual const char *parName(int k);

    /**
     * Returns reference to the module parameter identified with its
     * index k. Throws an error if the parameter does not exist.
     */
    virtual cPar& par(int k);

    /**
     * Returns reference to the module parameter specified with its name.
     * Throws an error if the parameter does not exist.
     */
    virtual cPar& par(const char *parname);

    /**
     * Returns index of the module parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}
    //@}
};

#endif


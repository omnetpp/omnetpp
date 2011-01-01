/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Wraps a NED component: a ChannelElement, ChannelInterfaceElement, SimpleModuleElement,
 * CompoundModuleElement or ModuleInterfaceElement subtree; provides easy lookup
 * of its gates, parameters, properties, submodules, inner types.
 * Enables the model element so have some additional cross-model info like a
 * list of names in the inheritance chain, the containing workspace file,
 * inherited parameter, gate, connection and submodule lists.
 *
 * @author rhornig, andras
 */
public interface INedTypeInfo extends INedChangeListener {
	/**
	 * Convenience method: returns the short name of the module/interface/channel/etc.
	 */
	public String getName();

	/**
	 * Returns the fully qualified name for the type. This consists of the package name,
	 * optional enclosing type name (for inner types), and the type name, separated by dot.
	 */
	public String getFullyQualifiedName();

	/**
	 * Returns the name prefix, i.e. the fully qualified name minus the simple name.
	 */
	public String getNamePrefix();

	/**
	 * Returns the package of the NED type, or null for the default package.
	 */
	public String getPackageName();

	/**
	 * Returns the underlying INedElement subtree.
	 */
	public INedTypeElement getNedElement();

	/**
	 * Returns NED file containing the definition.
	 */
	public IFile getNedFile();

	/**
	 * Returns the C++ class name inherited along @class properties or from the root's NED type name.
	 * The namespace is determined using the @namespace properties from the package.ned files.
	 */
    public String getFullyQualifiedCppClassName();

    /**
     * Signals that the info has been invalidated and must be rebuilt next time accessed.
     */
    public void invalidate();

	/**
	 * Causes information about inherited members to be discarded, and later
	 * re-built on demand.
	 */
    public void invalidateInherited();

    /**
     * For single-inheritance types (non-interface types such as modules and channels),
     * this method returns the super type.  Returns null if there is no super 
     * type (type contains no "extends" clause), if the type does not exist, 
     * or there is a cycle in the inheritance chain.
     * 
     * For multiple-interface types (module interface, channel interface), this method 
     * returns null. To obtain the super types of an interface, use the
     * getLocalInterfaces() method.
     * 
     *  @see getInheritanceChain()
     */
    public INedTypeElement getSuperType();

    /**
     * For single-inheritance types (non-interface types such as modules and channels),
     * this method returns the inheritance chain as a list. The list starts
     * with this NED type, and ends with the root.
     *
     * For multiple-interface types (module interface, channel interface), this method 
     * returns a list that contains this type only. To build the inheritance tree of 
     * an interface, use getInterfaces().
     * 
     * @see getSuperType()
     */
    public List<INedTypeInfo> getInheritanceChain();
    
    /**
     * Returns the list of interfaces this type locally extends or implements.
     * That is, this method returns the types in the "extends" clause
     * of interface types, and the types in the "like" clause of non-interface 
     * types.
     */
	public Set<INedTypeElement> getLocalInterfaces();

	/**
     * Returns the list of interfaces this type and its ancestor types and
     * ancestor interfaces extend or implement.
     */
    public Set<INedTypeElement> getInterfaces();

    /** 
     * Returns the map of all locally declared inner types, parameters, gates
     * and submodules.
     */
    public Map<String, INedElement> getLocalMembers();

    /** Parameters declared locally within this type (i.e. where param type is not empty) */
    public Map<String, ParamElementEx> getLocalParamDeclarations();

    /** Parameter nodes within this type where the "value" attribute is filled in */
    public Map<String, ParamElementEx> getLocalParamAssignments();

    /** Parameter nodes within this type */
    public Map<String, ParamElementEx> getLocalParams();

    /** Properties from the local parameters section */
    public Map<String, Map<String, PropertyElementEx>> getLocalProperties();

    /** Property from the local parameters section */
    public PropertyElementEx getLocalProperty(String name, String index);

    /** Gates declared locally within this type (i.e. where gate type is not empty) */
    public Map<String, GateElementEx> getLocalGateDeclarations();

    /** Gate nodes within this type where the "vector size" attribute filled in */
    public Map<String, GateElementEx> getLocalGateSizes();

    /** Inner types declared locally within this type */
    public Map<String, INedTypeElement> getLocalInnerTypes();

    /** Submodules declared locally within this (compound module) type */
    public Map<String, SubmoduleElementEx> getLocalSubmodules();

    /** Module and channel types used locally in this (compound module) type */
    public Set<INedTypeElement> getLocalUsedTypes();

	// same as above, for inherited members as well

    /** XXX ? */
    public Map<String, INedElement> getMembers();

    /** Parameter declarations (i.e. where parameter type is not empty), including inherited ones */
    public Map<String, ParamElementEx> getParamDeclarations();

    /** Parameter nodes where the "value" attribute is filled in, including inherited ones;
     * the most recent one for each parameter
     */
    public Map<String, ParamElementEx> getParamAssignments();

    /** Property nodes, including inherited ones; the most recent one for each property.
     * (Given the special inheritance rules for properties, this may not be what you want;
     * see getPropertyInheritanceChain().
     */
    // TODO: properly implement property: name, index pair
    public Map<String, Map<String, PropertyElementEx>> getProperties();

    /**
     * Returns the property element for the given name and index. The index may be
     * null to get the property without index.
     */
    public PropertyElementEx getProperty(String name, String index);

    /** Gate declarations (i.e. where gate type is not empty), including inherited ones */
    public Map<String, GateElementEx> getGateDeclarations();

    /**
     * Gate nodes where the "vector size" attribute is filled in, including inherited ones;
     * the most recent one for each gate
     */
    public Map<String, GateElementEx> getGateSizes();

    /** All inner types in this type, including inherited ones */
    public Map<String, INedTypeElement> getInnerTypes();

    /** All submodules in this (compound module) type, including inherited ones */
    public Map<String, SubmoduleElementEx> getSubmodules();


	public List<ParamElementEx> getParameterInheritanceChain(String parameterName);
	public List<GateElementEx> getGateInheritanceChain(String gateName);
	public List<PropertyElement> getPropertyInheritanceChain(String propertyName);

}

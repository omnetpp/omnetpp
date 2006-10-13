package org.omnetpp.ned2.model;

import java.util.Collection;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;

/**
 * Wraps a NED component: a ChannelNode, ChannelInterfaceNode, SimpleModuleNode, 
 * CompoundModuleNode or ModuleInterfaceNode subtree; provides easy lookup 
 * of its gates, parameters, properties, submodules, inner types.
 * Enables the model element so have some additional cross-model info like a 
 * list of names in the inheritence chain, the containing workspace file 
 * inherited parameter, gate, connection and submodule lists
 *
 * XXX: stuff like getType(), getDocu(), getValue() etc should be declared on the Ex classes!
 * XXX: getContainingComponent() should be defined on Ex classes as well
 * TODO: - would love to see: reference to the ancestor module (extends)
 *       - extends chain (name and NEDElement list)
 *       - derived list (name and NEDElement) of elements directly or indirectly derived from this element
 *       - methods returning the NEDElement list instead of simple name list (for all methods)
 *       - access to toplevel components via this interface ??? 
 */
public interface INEDTypeInfo {
	/**
	 * Convenience method: returns the name of the module/interface/channel/etc.
	 */
	public String getName();
    
    /**
     * @return The resolver implementation that created this typeInfo object
     */
    public INEDTypeResolver getResolver();
	
	/**
	 * Returns underlying NEDElement subtree.
	 */
	public NEDElement getNEDElement();

	/**
	 * Returns NED file containing the definition. 
	 */
	public IFile getNEDFile();

	/**
	 * Returns the list of nesting NED elements that extend over the given line:column
	 * in the source file. The list contains the component's root node as well.
	 * Returns null if there is no match (the component's declaration doesn't contain
	 * the given line:column, or source region info is missing from the NEDElement tree).
	 */
	public NEDElement[] getNEDElementsAt(int line, int column);
	
	// TODO comment!
	// these methods are for members declared on this component (ie exluding inherited ones)
	// member: gate/param/property/submod/innertype
	public Set<String> getOwnMemberNames();
    public Collection<NEDElement> getOwnMembers();
	public boolean hasOwnMember(String name);
	public NEDElement getOwnMember(String name);
	
	public Set<String> getOwnParamNames();
    public Collection<NEDElement> getOwnParams();
	public boolean hasOwnParam(String name);

	public Set<String> getOwnPropertyNames();
    public Collection<NEDElement> getOwnProperties();
	public boolean hasOwnProperty(String name);
	
	public Set<String> getOwnGateNames();
    public Collection<NEDElement> getOwnGates();
	public boolean hasOwnGate(String name);

    public Set<String> getOwnInnerTypeNames();
    public Collection<NEDElement> getOwnInnerTypes();
	public boolean hasOwnInnerType(String name);

	public Set<String> getOwnSubmodNames();
    public Collection<NEDElement> getOwnSubmods();
	public boolean hasOwnSubmod(String name);

	// same as above, for inherited members as well
	public Set<String> getMemberNames();
    public Collection<NEDElement> getMembers();
	public boolean hasMember(String name);
	public NEDElement getMember(String name);
	
	public Set<String> getParamNames();
    public Collection<NEDElement> getParams();
	public boolean hasParam(String name);

	public Set<String> getPropertyNames();
    public Collection<NEDElement> getProperties();
	public boolean hasProperty(String name);
	public NEDElement getProperty(String name);
	
	public Set<String> getGateNames();
    public Collection<NEDElement> getGates();
    public boolean hasGate(String name);

	public Set<String> getInnerTypeNames();
    public Collection<NEDElement> getInnerTypes();
	public boolean hasInnerType(String name);

	public Set<String> getSubmodNames();
    public Collection<NEDElement> getSubmods();
	public boolean hasSubmod(String name);
    
    /**
     * @return All types derived from this type
     */
    public List<INEDTypeInfo> getAllDerivedTypes();
}

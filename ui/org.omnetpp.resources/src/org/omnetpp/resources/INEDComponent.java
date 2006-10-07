package org.omnetpp.resources;

import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Wraps a NED component: a ChannelNode, ChannelInterfaceNode, SimpleModuleNode, 
 * CompoundModuleNode or ModuleInterfaceNode subtree; provides easy lookup 
 * of its gates, parameters, properties, submodules, inner types.
 * This class is primarily for supporting content assist-style functionality.
 *
 * XXX: stuff like getType(), getDocu(), getValue() etc should be declared on the Ex classes!
 * XXX: getContainingComponent() should be defined on Ex classes as well
 */
public interface INEDComponent {
	/**
	 * Convenience method: returns the name of the module/interface/channel/etc.
	 */
	public String getName();
	
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
	public boolean hasOwnMember(String name);
	public NEDElement getOwnMember(String name);
	
	public Set<String> getOwnParamNames();
	public boolean hasOwnParam(String name);

	public Set<String> getOwnPropertyNames();
	public boolean hasOwnProperty(String name);
	
	public Set<String> getOwnGateNames();
	public boolean hasOwnGate(String name);

	public Set<String> getOwnInnerTypeNames();
	public boolean hasOwnInnerType(String name);

	public Set<String> getOwnSubmodNames();
	public boolean hasOwnSubmod(String name);

	// same as above, for inherited members as well
	public Set<String> getMemberNames();
	public boolean hasMember(String name);
	public NEDElement getMember(String name);
	
	public Set<String> getParamNames();
	public boolean hasParam(String name);

	public Set<String> getPropertyNames();
	public boolean hasProperty(String name);
	public NEDElement getProperty(String name);
	
	public Set<String> getGateNames();
	public boolean hasGate(String name);

	public Set<String> getInnerTypeNames();
	public boolean hasInnerType(String name);

	public Set<String> getSubmodNames();
	public boolean hasSubmod(String name);
}

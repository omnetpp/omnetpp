package org.omnetpp.resources;

import java.util.Set;

import org.omnetpp.ned2.model.NEDElement;

/**
 * Primarily for supporting content assist-style functionality.
 * XXX: stuff like getType(), getDocu(), getValue() etc should be declared on the Ex classes!
 * XXX: getContainingComponent() should be defined on Ex classes as well
 */
public interface INEDComponent {
	public NEDElement getNEDElement();
	
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
	
	public Set<String> getGateNames();
	public boolean hasGate(String name);

	public Set<String> getInnerTypeNames();
	public boolean hasInnerType(String name);

	public Set<String> getSubmodNames();
	public boolean hasSubmod(String name);
}

package org.omnetpp.ned2.model;

import java.util.List;
import java.util.Map;

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
public interface INEDTypeInfo extends INEDChangeListener {
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
     * SIgnals that the info has been invalidated and must be rebuilt next time accessed.
     */
    public void invalidate();
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
    public Map<String, NEDElement> getOwnMembers();
    public Map<String, NEDElement> getOwnParams();
    public Map<String, NEDElement> getOwnParamValues();
    public Map<String, NEDElement> getOwnProperties();
    public Map<String, NEDElement> getOwnGates();
    public Map<String, NEDElement> getOwnGateSizes();
    public Map<String, NEDElement> getOwnInnerTypes();
    public Map<String, NEDElement> getOwnSubmods();

	// same as above, for inherited members as well
    public Map<String, NEDElement> getMembers();
    public Map<String, NEDElement> getParams();
    public Map<String, NEDElement> getParamValues();
    public Map<String, NEDElement> getProperties();
    public Map<String, NEDElement> getGates();
    public Map<String, NEDElement> getGateSizes();
    public Map<String, NEDElement> getInnerTypes();
    public Map<String, NEDElement> getSubmods();
    
    /**
     * @return All types derived from this type
     */
    public List<INEDTypeInfo> getAllDerivedTypes();
}

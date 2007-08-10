package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.notification.INEDChangeListener;

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
 *       - extends chain (name and INEDElement list)
 *       - derived list (name and INEDElement) of elements directly or indirectly derived from this element
 *       - methods returning the INEDElement list instead of simple name list (for all methods)
 *       - access to toplevel components via this interface ???
 */
public interface INEDTypeInfo extends INEDChangeListener {
	/**
	 * Convenience method: returns the name of the module/interface/channel/etc.
	 */
	public String getName();

    /**
     * Returns the resolver implementation that created this typeInfo object
     */
    public INEDTypeResolver getResolver();

	/**
	 * Returns underlying INEDElement subtree.
	 */
	public INEDElement getNEDElement();

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
	 * the given line:column, or source region info is missing from the INEDElement tree).
	 */
	public INEDElement[] getNEDElementsAt(int line, int column);

	// TODO comment!
	// these methods are for members declared on this component (ie exluding inherited ones)
	// member: gate/param/property/submod/innertype
	//XXX why not typed where possible? (ie getParams(): ParamNode instead of INEDElement)
    public Map<String, INEDElement> getOwnMembers();
    public Map<String, INEDElement> getOwnParams();
    public Map<String, INEDElement> getOwnParamValues();
    public Map<String, INEDElement> getOwnProperties();
    public Map<String, INEDElement> getOwnGates();
    public Map<String, INEDElement> getOwnGateSizes();
    public Map<String, INEDElement> getOwnInnerTypes();
    public Map<String, INEDElement> getOwnSubmods();
    public Set<String> getOwnUsedTypes();

	// same as above, for inherited members as well
    public Map<String, INEDElement> getMembers();
    public Map<String, INEDElement> getParams();
    public Map<String, INEDElement> getParamValues();
    public Map<String, INEDElement> getProperties();
    public Map<String, INEDElement> getGates();
    public Map<String, INEDElement> getGateSizes();
    public Map<String, INEDElement> getInnerTypes();
    public Map<String, INEDElement> getSubmods();

    /**
     * Returns the list of all types derived from this type
     */
    public List<INEDTypeInfo> getAllDerivedTypes();
    
    /**
     * Returns the list of all types that are using internally this type (i.e. compound
     * modules that contain submodules or connections with this type)
     */
    public List<INEDTypeInfo> getAllUsingTypes();
}

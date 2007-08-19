package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Wraps a NED component: a ChannelNode, ChannelInterfaceNode, SimpleModuleNode,
 * CompoundModuleNode or ModuleInterfaceNode subtree; provides easy lookup
 * of its gates, parameters, properties, submodules, inner types.
 * Enables the model element so have some additional cross-model info like a
 * list of names in the inheritance chain, the containing workspace file
 * inherited parameter, gate, connection and submodule lists
 *
 * XXX: stuff like getType(), getDocu(), getValue() etc should be declared on the Ex classes!
 * XXX: getContainingComponent() should be defined on Ex classes as well
 * TODO: - would love to see: reference to the ancestor module (extends)
 *       - extends chain (name and INEDElement list)
 *       - derived list (name and INEDElement) of elements directly or indirectly derived from this element
 *       - methods returning the INEDElement list instead of simple name list (for all methods)
 *       - access to toplevel components via this interface ???
 *
 * @author rhornig, andras
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
	public INedTypeNode getNEDElement();

	/**
	 * Returns NED file containing the definition.
	 */
	public IFile getNEDFile();

    /**
     * Signals that the info has been invalidated and must be rebuilt next time accessed.
     */
    public void invalidate();
    
	/**
	 * Returns the list of nesting NED elements that extend over the given line:column
	 * in the source file. The list contains the component's root node as well.
	 * Returns null if there is no match (the component's declaration doesn't contain
	 * the given line:column, or source region info is missing from the INEDElement tree).
	 */
	public INEDElement[] getNEDElementsAt(int line, int column);

	/**
	 * Returns the inheritance chain, starting with this NED type, and 
	 * ending with the root.
	 * 
	 * Only for non-interfaces (i.e. module or channel types): to get the 
	 * inheritance of an interface, use getInterfaces instead.
	 */
	public List<INEDTypeInfo> getExtendsChain();

    /**
     * Returns the list of interfaces this type locally implements.
     */
	public Set<String> getOwnInterfaces();

	/**
     * Returns the list of interfaces this type and its ancestor types and 
     * ancestor interfaces implement.
     */
    public Set<String> getInterfaces();

	// TODO comment!
	// these methods are for members declared on this component (i.e. excluding inherited ones)
	// member: gate/param/property/submod/innertype
    /** */
    public Map<String, INEDElement> getOwnMembers();

    /** Parameters declared locally within this type (i.e. where param type is not empty) */
    public Map<String, ParamNode> getOwnParams();

    /** Parameter nodes within this type with the value attribute filled in */
    public Map<String, ParamNode> getOwnParamValues();

    /** Properties from the local parameters section */
    public Map<String, PropertyNode> getOwnProperties();

    /** Gates declared locally within this type (i.e. where gate type is not empty) */
    public Map<String, GateNode> getOwnGates();

    /** Gate nodes within this type with the vector size attribute filled in */
    public Map<String, GateNode> getOwnGateSizes();

    /** Inner types declared locally within this type */
    public Map<String, INedTypeNode> getOwnInnerTypes();

    /** Submodules declared locally within this (compound module) type */
    public Map<String, SubmoduleNode> getOwnSubmodules();

    /** Module and channel types used locally in this (compound module) type */
    public Set<String> getOwnUsedTypes();

	// same as above, for inherited members as well
    public Map<String, INEDElement> getMembers();

    /** Parameter declarations (i.e. where parameter type is not empty), including inherited ones */
    public Map<String, ParamNode> getParams();

    /** Parameter nodes where the value attribute is filled in, including inherited ones */
    public Map<String, ParamNode> getParamValues();

    /** Property nodes, including inherited ones */
    public Map<String, PropertyNode> getProperties();

    /** Gate declarations (i.e. where gate type is not empty), including inherited ones */
    public Map<String, GateNode> getGates();

    /** Gate nodes where the vector size attribute is filled in, including inherited ones */
    public Map<String, GateNode> getGateSizes();

    /** All inner types in this type, including inherited ones */
    public Map<String, INedTypeNode> getInnerTypes();

    /** All submodules in this (compound module) type, including inherited ones */
    public Map<String, SubmoduleNode> getSubmodules();

    
	public List<ParamNode> getParameterInheritanceChain(String parameterName);
	public List<GateNode> getGateInheritanceChain(String gateName);
	public List<PropertyNode> getPropertyInheritanceChain(String propertyName);
}

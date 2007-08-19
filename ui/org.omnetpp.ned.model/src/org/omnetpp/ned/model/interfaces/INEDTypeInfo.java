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
	 * Returns the inheritance chain, starting with this NED type, and ending with the root.
	 */
	public List<INEDTypeInfo> getExtendsChain();

	/**
	 * Returns the inheritance chain, starting with the root, and ending with this NED type.
	 */
	public List<INEDTypeInfo> getForwardExtendsChain();

	// TODO comment!
	// these methods are for members declared on this component (i.e. excluding inherited ones)
	// member: gate/param/property/submod/innertype
    public Map<String, INEDElement> getOwnMembers();
    public Map<String, ParamNode> getOwnParams();
    public Map<String, ParamNode> getOwnParamValues();
    public Map<String, PropertyNode> getOwnProperties();
    public Map<String, GateNode> getOwnGates();
    public Map<String, GateNode> getOwnGateSizes();
    public Map<String, INedTypeNode> getOwnInnerTypes();
    public Map<String, SubmoduleNode> getOwnSubmods();
    public Set<String> getOwnUsedTypes();

	// same as above, for inherited members as well
    public Map<String, INEDElement> getMembers();
    public Map<String, ParamNode> getParams();
    public Map<String, ParamNode> getParamValues();
    public Map<String, PropertyNode> getProperties();
    public Map<String, GateNode> getGates();
    public Map<String, GateNode> getGateSizes();
    public Map<String, INedTypeNode> getInnerTypes();
    public Map<String, SubmoduleNode> getSubmods();

	public List<ParamNode> getParameterInheritanceChain(String parameterName);
	public List<GateNode> getGateInheritanceChain(String gateName);
	public List<PropertyNode> getPropertyInheritanceChain(String propertyName);

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

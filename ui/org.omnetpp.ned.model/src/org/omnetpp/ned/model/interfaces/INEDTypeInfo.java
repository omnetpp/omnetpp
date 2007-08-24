package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.notification.INEDChangeListener;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Wraps a NED component: a ChannelElement, ChannelInterfaceElement, SimpleModuleElement,
 * CompoundModuleElement or ModuleInterfaceElement subtree; provides easy lookup
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
	 * Returns underlying INEDElement subtree.
	 */
	public INedTypeElement getNEDElement();

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
     * Returns the first ned element that is in the extends clause. Returns NULL
     * if the type does not exist, if the extends clause is missing
     * or there is a cycle in the inheritance chain.
     */
    public INedTypeElement getFirstExtendsRef();

    /**
     * Returns the list of interfaces this type locally implements.
     */
	public Set<String> getLocalInterfaces();

	/**
     * Returns the list of interfaces this type and its ancestor types and
     * ancestor interfaces implement.
     */
    public Set<String> getInterfaces();

    /** XXX ? */
    public Map<String, INEDElement> getLocalMembers();

    /** Parameters declared locally within this type (i.e. where param type is not empty) */
    public Map<String, ParamElementEx> getLocalParamDeclarations();

    /** Parameter nodes within this type where the "value" attribute is filled in */
    public Map<String, ParamElementEx> getLocalParamAssignments();

    /** Properties from the local parameters section */
    public Map<String, PropertyElement> getLocalProperties();

    /** Gates declared locally within this type (i.e. where gate type is not empty) */
    public Map<String, GateElementEx> getLocalGateDeclarations();

    /** Gate nodes within this type where the "vector size" attribute filled in */
    public Map<String, GateElementEx> getLocalGateSizes();

    /** Inner types declared locally within this type */
    public Map<String, INedTypeElement> getLocalInnerTypes();

    /** Submodules declared locally within this (compound module) type */
    public Map<String, SubmoduleElementEx> getLocalSubmodules();

    /** Module and channel types used locally in this (compound module) type */
    public Set<String> getLocalUsedTypes();

	// same as above, for inherited members as well

    /** XXX ? */
    public Map<String, INEDElement> getMembers();

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
    public Map<String, PropertyElement> getProperties();

    /** Gate declarations (i.e. where gate type is not empty), including inherited ones */
    public Map<String, GateElementEx> getGateDeclarations();

    /** Gate nodes where the "vector size" attribute is filled in, including inherited ones;
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

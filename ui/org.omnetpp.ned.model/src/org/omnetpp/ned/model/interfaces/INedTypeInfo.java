/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.interfaces;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
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
     * Returns the simple name (i.e. unqualified name) of the module/interface/channel/etc.
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
     * Returns true if this type is an inner type, and false otherwise.
     */
    public boolean isInnerType();

    /**
     * Returns the enclosing type if this type is an inner type, and false otherwise.
     */
    public INedTypeElement getEnclosingType();

    /**
     * Returns the underlying INedElement subtree.
     */
    public INedTypeElement getNedElement();

    /**
     * Returns NED file containing the definition. Returns null for built-in types.
     */
    public IFile getNedFile();

    /**
     * Returns project containing the definition. Returns null for built-in types.
     */
    public IProject getProject();

    /**
     * Returns the NED type resolver associated with this type info object; never null.
     */
    public INedTypeResolver getResolver();

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
     * or if this NED type extends itself.
     *
     * For multiple-interface types (module interface, channel interface), this method
     * returns null. To obtain the super types of an interface, use the
     * getLocalInterfaces() method.
     *
     * A few examples:
     * <pre>
     * module m1 {} // returns null
     * module m2 extends m1 {} // returns m1
     * module m3 extends m3 {} // returns null
     * module m4 extends m5 {} // returns null
     * module m5 extends m4 {} // returns null
     * module m6 extends m5 {} // returns m5
     * module m7 extends m6 {} // returns m6
     * </pre>
     *
     * @see getInheritanceChain()
     */
    public INedTypeElement getSuperType();

    /**
     * For single-inheritance types (non-interface types such as modules and channels),
     * this method returns the inheritance chain as a list. The returned list always starts
     * with this NED type (i.e. it is never empty), and ends with the root if no cycle is
     * found. Otherwise the list ends with the first element of the cycle and the rest is
     * skipped. This strategy prevents cycles to be formed even when multiple inheritance
     * chains are used, because cycle edges are never returned. (i.e. there are no two
     * subsequent types present in the list which are part of a cycle).
     *
     * For multiple-interface types (module interface, channel interface), this method
     * returns a list that contains this type only. To build the inheritance tree of
     * an interface, use getInterfaces().
     *
     * A few examples:
     * <pre>
     * module m1 {} // returns [m1]
     * module m2 extends m1 {} // returns [m2, m1]
     * module m3 extends m3 {} // returns [m3]
     * module m4 extends m5 {} // returns [m4]
     * module m5 extends m4 {} // returns [m5]
     * module m6 extends m5 {} // returns [m6, m5]
     * module m7 extends m6 {} // returns [m7, m6, m5]
     * </pre>
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
     *
     * A few examples:
     * <pre>
     * moduleinterface i1 {} // returns [i1]
     * moduleinterface i2 extends i1 {} // returns [i2, i1]
     * moduleinterface i3 extends i3 {} // returns [i3]
     * moduleinterface i4 extends i5 {} // returns [i4, i5]
     * moduleinterface i5 extends i4 {} // returns [i5, i4]
     * moduleinterface i6 extends i5, i3 {} // returns [i6, i5, i4, i3]
     * module m like i6, i2 {} // returns [i6, i5, i4, i3, i2, i1]
     * </pre>
     */
    public Set<INedTypeElement> getInterfaces();

    /** Locally declared inner types, parameters, gates and submodules */
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

    /** Named connections local within this (compound module) type */
    public Map<String, ConnectionElementEx> getLocalNamedConnections();

    /** Types used locally in this (compound module) type as submodules or connections */
    public Set<INedTypeElement> getLocalUsedTypes();

    // same as above, for inherited members as well

    /** Inner types, parameters, gates, submodules and named connections, including inherited ones */
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

    /** All named connections in this (compound module) type, including inherited ones */
    public Map<String, ConnectionElementEx> getNamedConnections();

    /** Types used in this (compound module) type as submodules or connections, including inherited ones */
    public Set<INedTypeElement> getUsedTypes();

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName);
    public List<GateElementEx> getGateInheritanceChain(String gateName);
    public List<PropertyElement> getPropertyInheritanceChain(String propertyName);

}

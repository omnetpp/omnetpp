/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Stack;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElementConstants;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.GateElementEx;
import org.omnetpp.ned.model.ex.NedElementUtilEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.ParamElementEx;
import org.omnetpp.ned.model.ex.PropertyElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IInterfaceTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.NedModelChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.InterfaceNameElement;
import org.omnetpp.ned.model.pojo.NedElementTags;
import org.omnetpp.ned.model.pojo.PropertyElement;

/**
 * Default implementation of INedTypeInfo.
 *
 * @author rhornig, andras
 */
public class NedTypeInfo implements INedTypeInfo, NedElementTags, NedElementConstants {
    private static boolean debug = false;

    protected INedTypeElement componentNode;

    protected int debugId = lastDebugId++;
    protected static int lastDebugId = 0;
    protected static int debugRefreshInheritedCount = 0;
    protected static int debugRefreshLocalCount = 0;

    protected String fullyQualifiedName; // computed on demand

    // local members
    protected boolean needsRefreshLocal;
    protected INedTypeElement extendsType;
    protected Set<INedTypeElement> localInterfaces = new HashSet<INedTypeElement>();
    protected Map<String, Map<String, PropertyElementEx>> localProperties = new LinkedHashMap<String, Map<String, PropertyElementEx>>();
    protected Map<String, ParamElementEx> localParams = new LinkedHashMap<String, ParamElementEx>();
    protected Map<String, ParamElementEx> localParamDecls = new LinkedHashMap<String, ParamElementEx>();
    protected Map<String, ParamElementEx> localParamValues = new LinkedHashMap<String, ParamElementEx>();
    protected Map<String, GateElementEx> localGateDecls = new LinkedHashMap<String, GateElementEx>();
    protected Map<String, GateElementEx> localGateSizes = new LinkedHashMap<String, GateElementEx>();
    protected Map<String, INedTypeElement> localInnerTypes = new LinkedHashMap<String, INedTypeElement>();
    protected Map<String, SubmoduleElementEx> localSubmodules = new LinkedHashMap<String, SubmoduleElementEx>();
    protected Map<String, ConnectionElementEx> localNamedConnections = new LinkedHashMap<String, ConnectionElementEx>();
    protected HashSet<INedTypeElement> localUsedTypes;

    // sum of all "local" stuff
    protected Map<String, INedElement> localMembers = new LinkedHashMap<String, INedElement>();

    // local plus inherited
    protected boolean needsRefreshInherited; //XXX may be replaced with inheritedRefreshSerial, see INedTypeResolver.getLastChangeSerial()
    protected List<INedTypeInfo> extendsChain = null;
    protected Set<INedTypeElement> allInterfaces = new HashSet<INedTypeElement>();
    protected Map<String, Map<String, PropertyElementEx>> allProperties = new LinkedHashMap<String, Map<String, PropertyElementEx>>();
    protected Map<String, ParamElementEx> allParamDecls = new LinkedHashMap<String, ParamElementEx>();
    protected Map<String, ParamElementEx> allParamValues = new LinkedHashMap<String, ParamElementEx>();
    protected Map<String, GateElementEx> allGates = new LinkedHashMap<String, GateElementEx>();
    protected Map<String, GateElementEx> allGateSizes = new LinkedHashMap<String, GateElementEx>();
    protected Map<String, INedTypeElement> allInnerTypes = new LinkedHashMap<String, INedTypeElement>();
    protected Map<String, SubmoduleElementEx> allSubmodules = new LinkedHashMap<String, SubmoduleElementEx>();
    protected Map<String, ConnectionElementEx> allNamedConnections = new LinkedHashMap<String, ConnectionElementEx>();
    protected HashSet<INedTypeElement> allUsedTypes;

    // sum of all local+inherited stuff
    protected Map<String, INedElement> allMembers = new LinkedHashMap<String, INedElement>();


    // for local use
    interface IPredicate {
        public boolean matches(IHasName node);
    }

    /**
     * Constructor
     * @param node INedElement tree to be wrapped
     */
    public NedTypeInfo(INedTypeElement node) {
        componentNode = node;

        // register the created component in the INedElement, so we will have access to it
        // directly from the model. We also want to listen on it, and invalidate localMembers etc
        // if anything changes.
        INedTypeInfo oldTypeInfo = node.getNedTypeInfo();
        if (oldTypeInfo != null)
            node.removeNedChangeListener(oldTypeInfo);
        node.addNedChangeListener(this);

        // the inherited and local members will be collected on demand
        fullyQualifiedName = null;
        needsRefreshLocal = true;
        needsRefreshInherited = true;
    }

    /**
     * Collect elements (gates, params, etc) that match the predicate from the given section
     * (NED_PARAMETERS, NED_GATES, etc) into the map.
     */
    @SuppressWarnings({ "unchecked", "rawtypes" })
    protected void collect(Map<String, ? extends INedElement> map, int sectionTagCode, IPredicate predicate) {
        INedElement section = componentNode.getFirstChildWithTag(sectionTagCode);
        if (section != null)
            for (INedElement node : section)
                if (node instanceof IHasName && predicate.matches((IHasName)node))
                    ((Map)map).put(((IHasName)node).getName(), node);
                else if (node instanceof ConnectionGroupElement)
                    for (INedElement conn : node)
                        if (conn instanceof IHasName && predicate.matches((IHasName)conn))
                            ((Map)map).put(((IHasName)conn).getName(), conn);
    }

    /**
     * Collects all type names that are used in this module (submodule and connection types)
     * @param result storage for the used types
     */
    protected void collectTypesInCompoundModule(Set<INedTypeElement> result) {
        // this is only meaningful for CompoundModules, so skip others
        if (componentNode instanceof CompoundModuleElementEx) {
            // look for submodule types
            INedElement submodules = componentNode.getFirstChildWithTag(NED_SUBMODULES);
            if (submodules != null) {
                for (INedElement node : submodules) {
                    if (node instanceof SubmoduleElementEx) {
                        INedTypeElement usedType = ((SubmoduleElementEx)node).getEffectiveTypeRef();
                        if (usedType != null)
                            result.add(usedType);
                    }
                }
            }

            // look for connection types
            INedElement connections = componentNode.getFirstChildWithTag(NED_CONNECTIONS);
            if (connections != null) {
                for (INedElement node : connections) {
                    if (node instanceof ConnectionElementEx) {
                        INedTypeElement usedType = ((ConnectionElementEx)node).getEffectiveTypeRef();
                        if (usedType != null)
                            result.add(usedType);
                    }
                }
            }
        }
    }

    /**
     * Produce a list that starts with this type, and ends with the root.
     * Cycles in the "extends" chain are handled gracefully. The returned list always starts
     * with this NED type, and ends with the root if no cycle is found. Otherwise the list
     * contains only the first element of the cycle and the rest is skipped. (It is on purpose
     * that we don't include ANY edges from the cycle: this way one cannot get into infinite
     * loop by just following the getSuperType() calls of various types.)
     */
    protected List<INedTypeInfo> resolveExtendsChain() {
        if (getNedElement() instanceof IInterfaceTypeElement)
            return Arrays.asList(new INedTypeInfo[] { this }); // see docu of getExtendsChain()

        List<INedTypeInfo> result = new ArrayList<INedTypeInfo>();
        int thisTagCode = getNedElement().getTagCode();
        INedTypeInfo currentComponent = this;

        while (true) {
            // if cycle detected we remove the cycle members from the tail
            if (result.contains(currentComponent)) {
                int skipPoint = result.indexOf(currentComponent);
                return result.subList(0, skipPoint+1);
            }

            // add current type
            result.add(currentComponent);

            // resolve super type. Finish if there's no super type, it cannot be resolved,
            // or is of different component type (e.g. a channel cannot extend a module)
            String extendsName = currentComponent.getNedElement().getFirstExtends();
            if (StringUtils.isEmpty(extendsName))
                break;
            currentComponent = getResolver().lookupNedType(extendsName, currentComponent.getNedElement().getParentLookupContext());
            if (currentComponent == null || !areInheritanceCompatibleTagCodes(currentComponent.getNedElement().getTagCode(), thisTagCode))
                break;
        }
        return result;
    }

    static boolean areInheritanceCompatibleTagCodes(int tagCode1, int tagCode2) {
        return (tagCode1 == tagCode2) || 
                (tagCode1 == NED_SIMPLE_MODULE && tagCode2 == NED_COMPOUND_MODULE) || 
                (tagCode1 == NED_COMPOUND_MODULE && tagCode2 == NED_SIMPLE_MODULE);
    }

    protected Set<INedTypeElement> resolveInterfaces() {
        Set<INedTypeElement> interfaceElements = new HashSet<INedTypeElement>();
        Stack<INedTypeElement> remainingElements = new Stack<INedTypeElement>();
        for (INedTypeInfo typeInfo : extendsChain)
            remainingElements.add(typeInfo.getNedElement());
        while (!remainingElements.isEmpty()) {
            INedTypeElement currentElement = remainingElements.pop();
            if (currentElement instanceof IInterfaceTypeElement && !interfaceElements.contains(currentElement))
                interfaceElements.add(currentElement);
            for (INedTypeElement localInterfaceElement : currentElement.getNedTypeInfo().getLocalInterfaces()) {
                if (!interfaceElements.contains(localInterfaceElement)) {
                    interfaceElements.add(localInterfaceElement);
                    remainingElements.add(localInterfaceElement);
                }
            }
        }
        return interfaceElements;
    }

    /**
     * Refresh tables of local members
     */
    protected void refreshLocalMembersIfNeeded() {
        if (!needsRefreshLocal)
            return;

        //long startMillis = System.currentTimeMillis();

        ++debugRefreshLocalCount;
        // Debug.println("NedTypeInfo for "+getName()+" localRefresh: " + refreshLocalCount);

        // clear tables before collecting members
        localInterfaces.clear();
        localProperties.clear();
        localParams.clear();
        localParamDecls.clear();
        localParamValues.clear();
        localGateDecls.clear();
        localGateSizes.clear();
        localSubmodules.clear();
        localNamedConnections.clear();
        localInnerTypes.clear();
        localMembers.clear();

        INedTypeLookupContext parentContext = getNedElement().getParentLookupContext();

        // collect local interfaces:
        if (getNedElement() instanceof IInterfaceTypeElement) {
            // interfaces *extend* other interfaces
            for (INedElement child : getNedElement()) {
                if (child instanceof ExtendsElement) {
                    String extendsName = ((ExtendsElement)child).getName();
                    INedTypeInfo extendsTypeInfo = getResolver().lookupNedType(extendsName, parentContext);
                    if (extendsTypeInfo != null)
                        localInterfaces.add(extendsTypeInfo.getNedElement());
                }
            }
        }
        else {
            // modules & channels *implement* interfaces ("like")
            for (INedElement child : getNedElement()) {
                if (child instanceof InterfaceNameElement) {
                    String interfaceName = ((InterfaceNameElement)child).getName();
                    INedTypeInfo interfaceTypeInfo = getResolver().lookupNedType(interfaceName, parentContext);
                    if (interfaceTypeInfo != null)
                        localInterfaces.add(interfaceTypeInfo.getNedElement());
                }
            }
        }

        // collect members from component declaration
        NedElementUtilEx.collectProperties(componentNode, localProperties);
        collect(localParams, NED_PARAMETERS, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_PARAM;
            }});
        collect(localParamDecls, NED_PARAMETERS, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_PARAM && ((ParamElementEx)node).getType() != NED_PARTYPE_NONE;
            }});
        collect(localParamValues, NED_PARAMETERS, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_PARAM && StringUtils.isNotEmpty(((ParamElementEx)node).getValue());
            }});
        collect(localGateDecls, NED_GATES, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_GATE && ((GateElementEx)node).getType() != NED_GATETYPE_NONE;
            }});
        collect(localGateSizes, NED_GATES, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_GATE && StringUtils.isNotEmpty(((GateElementEx)node).getVectorSize());
            }});
        collect(localInnerTypes, NED_TYPES, new IPredicate() {
            public boolean matches(IHasName node) {
                return node instanceof INedTypeElement;
            }});
        collect(localSubmodules, NED_SUBMODULES, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_SUBMODULE;
            }});
        collect(localNamedConnections, NED_CONNECTIONS, new IPredicate() {
            public boolean matches(IHasName node) {
                return node.getTagCode()==NED_CONNECTION && !StringUtils.isEmpty(node.getName());
            }});

        // collect them in one common hash table as well (we assume there's no name clash --
        // that should be checked beforehand by validation!)
        localMembers.putAll(localParamDecls);
        localMembers.putAll(localGateDecls);
        localMembers.putAll(localSubmodules);
        localMembers.putAll(localNamedConnections);
        localMembers.putAll(localInnerTypes);

        needsRefreshLocal = false;

        //long dt = System.currentTimeMillis() - startMillis;
        //Debug.println("typeInfo " + getName() + " refreshLocalMembers(): " + dt + "ms");
    }

    /**
     * Collect all inherited parameters, gates, properties, submodules, named connections, etc.
     */
    protected void refreshInheritedMembersIfNeeded() {
        if (!needsRefreshInherited)
            return;

        long startMillis = System.currentTimeMillis();

        ++debugRefreshInheritedCount;

        if (debug)
            Debug.println("NedTypeInfo for "+getName()+" refreshInherited: " + debugRefreshInheritedCount);

        // first wee need our local members updated
        if (needsRefreshLocal)
            refreshLocalMembersIfNeeded();

        // determine extends chain
        extendsChain = resolveExtendsChain();
        extendsType = extendsChain.size() >= 2 ? extendsChain.get(1).getNedElement() : null;

        allInterfaces = resolveInterfaces();
        if (debug)
            Debug.println("NedTypeInfo for " + getName() + ": has the following interfaces: " + allInterfaces + " and extends chain: " + extendsChain);

        allProperties.clear();
        allParamDecls.clear();
        allParamValues.clear();
        allGates.clear();
        allGateSizes.clear();
        allInnerTypes.clear();
        allSubmodules.clear();
        allNamedConnections.clear();
        allMembers.clear();

        // collect all inherited members (from the extends chain; or for interfaces, from all base interfaces)
        INedTypeInfo[] ancestors;
        if (componentNode instanceof IInterfaceTypeElement) {
            ancestors = new INedTypeInfo[allInterfaces.size()];
            int i = 0;
            for (INedTypeElement element : allInterfaces)
                ancestors[i++] = element.getNedTypeInfo();
        }
        else {
            ancestors = extendsChain.toArray(new INedTypeInfo[]{});
            ArrayUtils.reverse(ancestors);  // we want to start from the root, so for allParamValues and allGateSizes we end up with the *latest* assignments
        }

        for (INedTypeInfo typeInfo : ancestors) {
            Assert.isTrue(typeInfo instanceof NedTypeInfo);
            NedTypeInfo component = (NedTypeInfo)typeInfo;
            allProperties.putAll(component.getLocalProperties());
            allParamDecls.putAll(component.getLocalParamDeclarations());
            allParamValues.putAll(component.getLocalParamAssignments());
            allGates.putAll(component.getLocalGateDeclarations());
            allGateSizes.putAll(component.getLocalGateSizes());
            allInnerTypes.putAll(component.getLocalInnerTypes());
            allSubmodules.putAll(component.getLocalSubmodules());
            allNamedConnections.putAll(component.getLocalNamedConnections());
            allMembers.putAll(component.getLocalMembers());
        }

        if (debug)
            Debug.println("typeInfo " + getName() + " refreshInherited(): " + (System.currentTimeMillis() - startMillis) + "ms");

        needsRefreshInherited = false;
    }

    public void invalidate() {
        if (debug)
            Debug.println(getName() +  ": invalidated *all* members (local+inherited)");

        fullyQualifiedName = null;
        needsRefreshLocal = true;
        needsRefreshInherited = true;
        localUsedTypes = null;
        allUsedTypes = null;
    }

    public void invalidateInherited() {
        if (debug)
            Debug.println(getName() +  ": invalidated inherited members");

        fullyQualifiedName = null;
        needsRefreshInherited = true;
        allUsedTypes = null;
    }

    public String getName() {
        return componentNode.getName();
    }

    public String getFullyQualifiedName() {
        if (fullyQualifiedName == null)
            fullyQualifiedName = getNedElement().getParentLookupContext().getQNameAsPrefix() + getNedElement().getName();
        return fullyQualifiedName;
    }

    public String getNamePrefix() {
        return getNedElement().getParentLookupContext().getQNameAsPrefix();
    }

    public String getPackageName() {
        return componentNode.getContainingNedFileElement().getPackage();
    }

    public boolean isInnerType() {
        return componentNode.getEnclosingTypeElement() != null;
    }

    public INedTypeElement getEnclosingType() {
        return componentNode.getEnclosingTypeElement();
    }

    public INedTypeElement getNedElement() {
        return componentNode;
    }

    public IFile getNedFile() {
        NedFileElementEx nedFileElement = getNedElement().getContainingNedFileElement();
        return nedFileElement==null ? null : getResolver().getNedFile(nedFileElement); // Note: built-in types don't have a NedFileElement parent
    }

    public IProject getProject() {
        IFile file = getNedFile();
        return file != null ? file.getProject() : null;
    }

    public INedTypeResolver getResolver() {
        return componentNode.getResolver();
    }

    public String getFullyQualifiedCppClassName() {
        String className = null;
        List<INedTypeInfo> extendsChain = getInheritanceChain();

        // find the first type in the extends chain that have @class attribute and return that class name
        for (INedTypeInfo typeInfo : extendsChain) {
            PropertyElementEx property = typeInfo.getProperty("class", null);
            if (property != null) {
                className = property.getSimpleValue();
                break;
            }
        }

        // if none of them has, return the last class in the chain
        if (className == null)
            className = extendsChain.get(extendsChain.size()-1).getName();

        NedFileElementEx fileElement = componentNode.getContainingNedFileElement();
        String namespace = getResolver().getSimplePropertyFor(fileElement, INedTypeResolver.NAMESPACE_PROPERTY);

        if (namespace == null)
            return className;
        else
            return namespace + "::" + className;
    }

    public INedTypeElement getSuperType() {
        refreshInheritedMembersIfNeeded();
        return extendsType;
    }

    public List<INedTypeInfo> getInheritanceChain() {
        refreshInheritedMembersIfNeeded();
        return extendsChain;
    }

    public Set<INedTypeElement> getLocalInterfaces() {
        refreshLocalMembersIfNeeded();
        return localInterfaces;
    }

    public Map<String, ParamElementEx> getLocalParamDeclarations() {
        refreshLocalMembersIfNeeded();
        return localParamDecls;
    }

    public Map<String, ParamElementEx> getLocalParamAssignments() {
        refreshLocalMembersIfNeeded();
        return localParamValues;
    }

    public Map<String, ParamElementEx> getLocalParams() {
        refreshLocalMembersIfNeeded();
        return localParams;
    }

    public Map<String, Map<String, PropertyElementEx>> getLocalProperties() {
        refreshLocalMembersIfNeeded();
        return localProperties;
    }

    public PropertyElementEx getLocalProperty(String name, String index) {
        Map<String, PropertyElementEx> propertyMap = getLocalProperties().get(name);
        if (propertyMap == null)
            return null;
        else
            return propertyMap.get(index == null ? PropertyElementEx.DEFAULT_PROPERTY_INDEX : index);
    }

    public Map<String, GateElementEx> getLocalGateDeclarations() {
        refreshLocalMembersIfNeeded();
        return localGateDecls;
    }

    public Map<String, GateElementEx> getLocalGateSizes() {
        refreshLocalMembersIfNeeded();
        return localGateSizes;
    }

    public Map<String,INedTypeElement> getLocalInnerTypes() {
        refreshLocalMembersIfNeeded();
        return localInnerTypes;
    }

    public Map<String, SubmoduleElementEx> getLocalSubmodules() {
        refreshLocalMembersIfNeeded();
        return localSubmodules;
    }

    public Map<String, ConnectionElementEx> getLocalNamedConnections() {
        refreshLocalMembersIfNeeded();
        return localNamedConnections;
    }

    public Map<String,INedElement> getLocalMembers() {
        refreshLocalMembersIfNeeded();
        return localMembers;
    }

    public Set<INedTypeElement> getLocalUsedTypes() {
        if (localUsedTypes == null) {
            localUsedTypes = new HashSet<INedTypeElement>();
            refreshLocalMembersIfNeeded();
            collectTypesInCompoundModule(localUsedTypes);
        }

        return localUsedTypes;
    }

    public Set<INedTypeElement> getInterfaces() {
        refreshInheritedMembersIfNeeded();
        return allInterfaces;
    }

    public Map<String, ParamElementEx> getParamDeclarations() {
        refreshInheritedMembersIfNeeded();
        return allParamDecls;
    }

    public Map<String, ParamElementEx> getParamAssignments() {
        refreshInheritedMembersIfNeeded();
        return allParamValues;
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        refreshInheritedMembersIfNeeded();
        return allProperties;
    }

    public PropertyElementEx getProperty(String name, String index) {
        Map<String, PropertyElementEx> propertyMap = getProperties().get(name);
        if (propertyMap == null)
            return null;
        else
            return propertyMap.get(index == null ? PropertyElementEx.DEFAULT_PROPERTY_INDEX : index);
    }

    public Map<String, GateElementEx> getGateDeclarations() {
        refreshInheritedMembersIfNeeded();
        return allGates;
    }

    public Map<String, GateElementEx> getGateSizes() {
        refreshInheritedMembersIfNeeded();
        return allGateSizes;
    }

    public Map<String, INedTypeElement> getInnerTypes() {
        refreshInheritedMembersIfNeeded();
        return allInnerTypes;
    }

    public Map<String, SubmoduleElementEx> getSubmodules() {
        refreshInheritedMembersIfNeeded();
        return allSubmodules;
    }

    public Map<String, ConnectionElementEx> getNamedConnections() {
        refreshInheritedMembersIfNeeded();
        return allNamedConnections;
    }

    public Map<String, INedElement> getMembers() {
        refreshInheritedMembersIfNeeded();
        return allMembers;
    }

    public Set<INedTypeElement> getUsedTypes() {
        if (allUsedTypes == null) {
            allUsedTypes = new HashSet<INedTypeElement>();
            refreshInheritedMembersIfNeeded();
            INedTypeInfo[] forwardExtendsChain = extendsChain.toArray(new INedTypeInfo[]{});
            ArrayUtils.reverse(forwardExtendsChain);
            for (INedTypeInfo typeInfo : forwardExtendsChain) {
                Assert.isTrue(typeInfo instanceof NedTypeInfo);
                NedTypeInfo component = (NedTypeInfo)typeInfo;
                allUsedTypes.addAll(component.getLocalUsedTypes());
            }
        }

        return allUsedTypes;
    }

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName) {
        List<ParamElementEx> result = new ArrayList<ParamElementEx>();
        for (INedTypeInfo type : getInheritanceChain())
            if (type.getLocalParams().containsKey(parameterName))
                result.add(type.getLocalParams().get(parameterName));
        return result;
    }

    public List<GateElementEx> getGateInheritanceChain(String gateName) {
        List<GateElementEx> result = new ArrayList<GateElementEx>();
        for (INedTypeInfo type : getInheritanceChain())
            if (type.getLocalGateDeclarations().containsKey(gateName))
                result.add(type.getLocalGateDeclarations().get(gateName));
        return result;
    }

    public List<PropertyElement> getPropertyInheritanceChain(String propertyName) {
        List<PropertyElement> result = new ArrayList<PropertyElement>();
        for (INedTypeInfo type : getInheritanceChain())
            if (type.getLocalProperties().containsKey(propertyName))
                result.add(type.getLocalProperty(propertyName, null));
        return result;
    }

    /* (non-Javadoc)
     * @see java.lang.Object#toString()
     * Displays debugging info
     */
    @Override
    public String toString() {
        return "NedTypeInfo for "+getNedElement();
    }

    public void modelChanged(NedModelEvent event) {
        if (event instanceof NedModelChangeEvent)
            invalidate();
    }

    public void debugDump() {
        Debug.println("NedTypeInfo: " + getNedElement().toString() + " debugId=" + debugId);
        if (needsRefreshInherited || needsRefreshLocal)
            Debug.println(" currently invalid (needs refresh)");
        Debug.println("  extends chain: " + StringUtils.join(getInheritanceChain(), ", "));
        Debug.println("  local interfaces: " + StringUtils.join(localInterfaces, ", "));
        Debug.println("  all interfaces: " + StringUtils.join(allInterfaces, ", "));
        Debug.println("  local gates: " + StringUtils.join(localGateDecls.keySet(), ", "));
        Debug.println("  all gates: " + StringUtils.join(allGates.keySet(), ", "));
        Debug.println("  local parameter declarations: " + StringUtils.join(localParamDecls.keySet(), ", "));
        Debug.println("  all parameter declarations: " + StringUtils.join(allParamDecls.keySet(), ", "));
        Debug.println("  local properties: " + StringUtils.join(localProperties.keySet(), ", "));
        Debug.println("  all properties: " + StringUtils.join(allProperties.keySet(), ", "));
        Debug.println("  local submodules: " + StringUtils.join(localSubmodules.keySet(), ", "));
        Debug.println("  all submodules: " + StringUtils.join(allSubmodules.keySet(), ", "));
        Debug.println("  local named connections: " + StringUtils.join(localNamedConnections.keySet(), ", "));
        Debug.println("  all named connections: " + StringUtils.join(allNamedConnections.keySet(), ", "));
    }
}

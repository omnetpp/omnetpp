package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned.model.pojo.ChannelNode;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
import org.omnetpp.ned.model.pojo.SimpleModuleNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Default implementation of INEDComponent.
 *
 * @author rhornig, andras
 */
public class NEDComponent implements INEDTypeInfo, NEDElementTags {

	protected INEDTypeResolver resolver;

	protected INedTypeNode componentNode;
	protected IFile file;
    static int refreshInheritedCount = 0;
    static int refreshOwnCount = 0;

	// own stuff
    boolean needsOwnUpdate;
	protected LinkedHashMap<String,PropertyNode> ownProperties = new LinkedHashMap<String, PropertyNode>();
    protected LinkedHashMap<String,ParamNode> ownParams = new LinkedHashMap<String, ParamNode>();
    protected LinkedHashMap<String,ParamNode> ownParamValues = new LinkedHashMap<String, ParamNode>();
	protected LinkedHashMap<String,GateNode> ownGates = new LinkedHashMap<String, GateNode>();
    protected LinkedHashMap<String,GateNode> ownGateSizes = new LinkedHashMap<String, GateNode>();
	protected LinkedHashMap<String,INedTypeNode> ownInnerTypes = new LinkedHashMap<String, INedTypeNode>();
	protected LinkedHashMap<String,SubmoduleNode> ownSubmodules = new LinkedHashMap<String, SubmoduleNode>();
    protected HashSet<String> ownUsedTypes = new HashSet<String>();

	// sum of all "own" stuff
	protected LinkedHashMap<String, INEDElement> ownMembers = new LinkedHashMap<String, INEDElement>();

	// own plus inherited
	boolean needsUpdate;
	protected LinkedHashMap<String, INEDElement> allProperties = new LinkedHashMap<String, INEDElement>();
    protected LinkedHashMap<String, INEDElement> allParams = new LinkedHashMap<String, INEDElement>();
    protected LinkedHashMap<String, INEDElement> allParamValues = new LinkedHashMap<String, INEDElement>();
    protected LinkedHashMap<String, INEDElement> allGates = new LinkedHashMap<String, INEDElement>();
    protected LinkedHashMap<String, INEDElement> allGateSizes = new LinkedHashMap<String, INEDElement>();
	protected LinkedHashMap<String, INEDElement> allInnerTypes = new LinkedHashMap<String, INEDElement>();
	protected LinkedHashMap<String, INEDElement> allSubmodules = new LinkedHashMap<String, INEDElement>();

	// sum of all own+inherited stuff
	protected LinkedHashMap<String, INEDElement> allMembers = new LinkedHashMap<String, INEDElement>();

    // all types which extends this component
    protected List<INEDTypeInfo> allDerivedTypes = new ArrayList<INEDTypeInfo>();
    
    // all types that contain instances (submodule, connection) of this type
    protected List<INEDTypeInfo> allUsingTypes = new ArrayList<INEDTypeInfo>();

	/**
	 * Constructor
	 * @param node INEDElement tree to be wrapped
	 * @param nedfile file containing the definition
	 * @param res will be used to resolve inheritance (collect gates, params etc from base classes)
	 */
	public NEDComponent(INedTypeNode node, IFile nedfile, INEDTypeResolver res) {
		resolver = res;
		file = nedfile;
		componentNode = node;
        // register the created component in the INEDElement so we will have access to it
        // directly from the model
        node.setNEDTypeInfo(this);
		// the inherited and own members will be collected on demand
        needsOwnUpdate = true;
		needsUpdate = true;
	}

    public INEDTypeResolver getResolver() {
        return resolver;
    }

    /**
	 * Collect parameter declarations, gate declarations, inner types, etc into a hash table
	 * @param map  			stores result
	 * @param tagCode			types of elements to collect (NED_xxx constant)
	 * @param sectionTagCode	tag code of section to search within component node
	 * @param nameAttr			name of "name" attr of given node; this will be the key in the hashmap
	 * @param typeAttr			name of "type" attr, or null if none. If given, the INEDElement must
	 * 							have this attribute non-empty to be collected (this is how we collect
	 * 							parameter declarations, and ignore parameter assignments/refinements)
	 */
	@SuppressWarnings("unchecked")
	protected void collect(Map<String,? extends INEDElement> map, int tagCode,
							int sectionTagCode,	String nameAttr, String typeAttr) {  //FIXME use predicate
		INEDElement section = componentNode.getFirstChildWithTag(sectionTagCode);
		if (section!=null) {
			// search nodes within section ("gates:", "parameters:", etc)
			for (INEDElement node : section) {
				if (tagCode==-1 || node.getTagCode()==tagCode) {
					if (typeAttr==null || !(node.getAttribute(typeAttr)==null || "".equals(node.getAttribute(typeAttr)))) {
						String name = node.getAttribute(nameAttr);
						((Map)map).put(name, node);
					}
				}
			}
		}
	}

    /**
     * Collects all type names that are used in this module (submodule and connection types)
     * @param result storage for the used types
     */
    protected void collectTypesInCompoundModule(Set<String> result) {
        // this is only meaningful for CompoundModules so skip the others
        if (!(componentNode instanceof CompoundModuleNodeEx))
            return;

        // look for submodule types
        INEDElement submodules = componentNode.getFirstChildWithTag(NED_SUBMODULES);
        if (submodules != null) {
            for (INEDElement node : submodules) {
                if (node instanceof SubmoduleNodeEx) {
                    result.add(((SubmoduleNodeEx)node).getEffectiveType());
                }
            }
        }

        // look for connection types
        INEDElement connections = componentNode.getFirstChildWithTag(NED_CONNECTIONS);
        if (connections != null) {
            for (INEDElement node : connections) {
                if (node instanceof ConnectionNodeEx) {
                    result.add(((ConnectionNodeEx)node).getEffectiveType());
                }
            }
        }
    }

	/* (non-Javadoc)
	 * @see org.omnetpp.ned.model.interfaces.INEDTypeInfo#getExtendsChain()
	 */
    public List<INEDTypeInfo> getExtendsChain() {
		return computeExtendsChain(); //XXX result could be cached
	}

	/* (non-Javadoc)
	 * @see org.omnetpp.ned.model.interfaces.INEDTypeInfo#getForwardExtendsChain()
	 */
	public List<INEDTypeInfo> getForwardExtendsChain() {
		List<INEDTypeInfo> extendsChain = computeExtendsChain();
		Collections.reverse(extendsChain);
		return extendsChain;
	}

	/**
	 * Produce a list that starts with this type, and ends with the root.
	 */
	protected List<INEDTypeInfo> computeExtendsChain() {
	    ArrayList<INEDTypeInfo> tmp = new ArrayList<INEDTypeInfo>();
    	tmp.add(this);
	    INEDTypeInfo currentComponent = this;
	    while (true) {
		    INEDElement currentComponentNode = currentComponent.getNEDElement();
	    	INEDElement extendsNode = currentComponentNode.getFirstChildWithTag(NED_EXTENDS);
	    	if (extendsNode==null)
	    		break;
	    	String extendsName = ((ExtendsNode)extendsNode).getName();
	    	if (extendsName==null)
	    		break;
	    	currentComponent = resolver.getComponent(extendsName);
	    	if (currentComponent==null)
	    		break;
	    	tmp.add(currentComponent);
	    }

	    return tmp;
	}

	public List<ParamNode> getParameterInheritanceChain(String parameterName) {
		List<ParamNode> result = new ArrayList<ParamNode>();
		for (INEDTypeInfo type : getExtendsChain())
			if (type.getOwnParams().containsKey(parameterName))
				result.add((ParamNode) type.getOwnParams().get(parameterName));
		return result;
	}

	public List<GateNode> getGateInheritanceChain(String gateName) {
		List<GateNode> result = new ArrayList<GateNode>();
		for (INEDTypeInfo type : getExtendsChain())
			if (type.getOwnGates().containsKey(gateName))
				result.add(type.getOwnGates().get(gateName));
		return result;
	}

	public List<PropertyNode> getPropertyInheritanceChain(String propertyName) {
		List<PropertyNode> result = new ArrayList<PropertyNode>();
		for (INEDTypeInfo type : getExtendsChain())
			if (type.getOwnProperties().containsKey(propertyName))
				result.add((PropertyNode) type.getOwnProperties().get(propertyName));
		return result;
	}

    /**
     * Refreshes the own (local) members
     */
    protected void refreshOwnMembers() {
        // XXX for debugging
        ++refreshOwnCount;
        // System.out.println("NEDComponent for "+getName()+" ownRefresh: " + refreshOwnCount);

        ownProperties.clear();
        ownParams.clear();
        ownParamValues.clear();
        ownGates.clear();
        ownGateSizes.clear();
        ownSubmodules.clear();
        ownInnerTypes.clear();
        ownMembers.clear();
        ownUsedTypes.clear();

        // collect stuff from component declaration
        collect(ownProperties, NED_PROPERTY, NED_PARAMETERS, PropertyNode.ATT_NAME, null);
        collect(ownParams, NED_PARAM, NED_PARAMETERS, ParamNode.ATT_NAME, ParamNode.ATT_TYPE);
        collect(ownParamValues, NED_PARAM, NED_PARAMETERS, ParamNode.ATT_NAME, null);
        collect(ownGates, NED_GATE, NED_GATES, GateNode.ATT_NAME, GateNode.ATT_TYPE);
        collect(ownGateSizes, NED_GATE, NED_GATES, GateNode.ATT_NAME, null); //FIXME and: gatesize attribute is not empty!
        collect(ownSubmodules, NED_SUBMODULE, NED_SUBMODULES, SubmoduleNode.ATT_NAME, null);

        // XXX would be more efficient to collect the following in one pass:
        collect(ownInnerTypes, NED_SIMPLE_MODULE, NED_TYPES, SimpleModuleNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_COMPOUND_MODULE, NED_TYPES, CompoundModuleNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_CHANNEL, NED_TYPES, ChannelNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_MODULE_INTERFACE, NED_TYPES, ModuleInterfaceNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_CHANNEL_INTERFACE, NED_TYPES, ChannelInterfaceNode.ATT_NAME, null);

        // collect them in one common hash table as well (we assume there's no name clash --
        // that should be checked beforehand by validation!)
        ownMembers.putAll(ownProperties);
        ownMembers.putAll(ownParams);
        ownMembers.putAll(ownGates);
        ownMembers.putAll(ownSubmodules);
        ownMembers.putAll(ownInnerTypes);

        // collect the types that were used in this module (meaningful only for compound modules)
        collectTypesInCompoundModule(ownUsedTypes);

        needsOwnUpdate = false;
    }

	/**
	 * Collect all inherited parameters, gates, properties, submodules, etc.
	 */
	protected void refreshInheritedMembers() {
        ++refreshInheritedCount;
        // System.out.println("NEDComponent for "+getName()+" inheritedRefersh: " + refreshInheritedCount);

        // first wee need our own members updated
        if (needsOwnUpdate)
            refreshOwnMembers();

		allProperties.clear();
		allParams.clear();
        allParamValues.clear();
		allGates.clear();
        allGateSizes.clear();
		allInnerTypes.clear();
		allSubmodules.clear();
		allMembers.clear();

        // collect all inherited members
		List<INEDTypeInfo> forwardExtendsChain = getForwardExtendsChain();
		for (INEDTypeInfo icomponent : forwardExtendsChain) {
			Assert.isTrue(icomponent instanceof NEDComponent);
			NEDComponent component = (NEDComponent)icomponent;
			allProperties.putAll(component.getOwnProperties());
			allParams.putAll(component.getOwnParams());
            allParamValues.putAll(component.getOwnParamValues());
			allGates.putAll(component.getOwnGates());
            allGateSizes.putAll(component.getOwnGateSizes());
			allInnerTypes.putAll(component.getOwnInnerTypes());
			allSubmodules.putAll(component.getOwnSubmods());
			allMembers.putAll(component.getOwnMembers());
		}

        // additional tables for derived types and types using this one
		allDerivedTypes.clear();
        // collect all types that are derived from this
        for (INEDTypeInfo currentComp : getResolver().getAllComponents()) {
            // never send notification to ourselves
            if (currentComp == this)
                continue;

            // check for components the are extending us (directly or indirectly)
            INEDElement element = currentComp.getNEDElement();
            for (INEDElement child : element) {
                if (child instanceof ExtendsNode) {
                    String extendsName = ((ExtendsNode)child).getName();
                    if (getName().equals(extendsName)) {
                        allDerivedTypes.add(currentComp);
                    }
                }
            }

            // check for components that contain submodules, connections that use this type
            if (currentComp.getOwnUsedTypes().contains(getName())) {
                allUsingTypes.add(currentComp);
            }
        }
        needsUpdate = false;
	}

	/**
	 * Causes information about inherited members to be discarded, and
	 * later re-built on demand.
	 */
	public void invalidate() {
        // System.out.println("NEDComponent invalidate");
        needsOwnUpdate = true;
		needsUpdate = true;
	}

	public String getName() {
		return componentNode.getAttribute("name");
	}

	public INEDElement getNEDElement() {
		return componentNode;
	}

	public IFile getNEDFile() {
		return file;
	}

	public INEDElement[] getNEDElementsAt(int line, int column) {
		ArrayList<INEDElement> list = new ArrayList<INEDElement>();
		NEDSourceRegion region = componentNode.getSourceRegion();
		if (region!=null && region.contains(line, column)) {
			list.add(componentNode);
			collectNEDElements(componentNode, line, column, list);
			return list.toArray(new INEDElement[list.size()]);
		}
		return null;
	}

	private void collectNEDElements(INEDElement node, int line, int column, ArrayList<INEDElement> list) {
		for (INEDElement child : node) {
			NEDSourceRegion region = child.getSourceRegion();
			if (region!=null && region.contains(line, column)) {
				list.add(child);
				collectNEDElements(child, line, column, list); // children fall inside parent's region
			}
		}
	}

    public Map<String,ParamNode> getOwnParams() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownParams;
    }

    public Map<String,ParamNode> getOwnParamValues() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownParamValues;
    }

    public Map<String,PropertyNode> getOwnProperties() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownProperties;
    }

    public Map<String,GateNode> getOwnGates() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownGates;
    }

    public Map<String,GateNode> getOwnGateSizes() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownGateSizes;
    }

    public Map<String,INedTypeNode> getOwnInnerTypes() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownInnerTypes;
    }

    public Map<String,SubmoduleNode> getOwnSubmods() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownSubmodules;
    }

    public Map<String,INEDElement> getOwnMembers() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownMembers;
    }

    public Set<String> getOwnUsedTypes() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownUsedTypes;
    }

    public Map<String, INEDElement> getParams() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParams;
    }

    public Map<String, INEDElement> getParamValues() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParamValues;
    }

    public Map<String, INEDElement> getProperties() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allProperties;
    }

    public Map<String, INEDElement> getGates() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGates;
    }

    public Map<String, INEDElement> getGateSizes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGateSizes;
    }

    public Map<String, INEDElement> getInnerTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allInnerTypes;
    }

    public Map<String, INEDElement> getSubmods() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allSubmodules;
    }

    public Map<String, INEDElement> getMembers() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allMembers;
    }

    public List<INEDTypeInfo> getAllDerivedTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allDerivedTypes;
    }

    public List<INEDTypeInfo> getAllUsingTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allUsingTypes;
    }

    /* (non-Javadoc)
     * @see java.lang.Object#toString()
     * Displays debugging info
     */
    @Override
    public String toString() {
        return "NEDComponent for "+getNEDElement();
    }

    public void modelChanged(NEDModelEvent event) {
        invalidate();
    }

}

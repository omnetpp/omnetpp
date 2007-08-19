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
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDSourceRegion;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.GateNode;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.PropertyNode;
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
	protected Map<String,PropertyNode> ownProperties = new LinkedHashMap<String, PropertyNode>();
    protected Map<String,ParamNode> ownParams = new LinkedHashMap<String, ParamNode>();
    protected Map<String,ParamNode> ownParamValues = new LinkedHashMap<String, ParamNode>();
	protected Map<String,GateNode> ownGates = new LinkedHashMap<String, GateNode>();
    protected Map<String,GateNode> ownGateSizes = new LinkedHashMap<String, GateNode>();
	protected Map<String,INedTypeNode> ownInnerTypes = new LinkedHashMap<String, INedTypeNode>();
	protected Map<String,SubmoduleNode> ownSubmodules = new LinkedHashMap<String, SubmoduleNode>();
    protected HashSet<String> ownUsedTypes = new HashSet<String>();

	// sum of all "own" stuff
	protected Map<String, INEDElement> ownMembers = new LinkedHashMap<String, INEDElement>();

	// own plus inherited
	boolean needsUpdate;
	protected Map<String, PropertyNode> allProperties = new LinkedHashMap<String, PropertyNode>();
    protected Map<String, ParamNode> allParams = new LinkedHashMap<String, ParamNode>();
    protected Map<String, ParamNode> allParamValues = new LinkedHashMap<String, ParamNode>();
    protected Map<String, GateNode> allGates = new LinkedHashMap<String, GateNode>();
    protected Map<String, GateNode> allGateSizes = new LinkedHashMap<String, GateNode>();
	protected Map<String, INedTypeNode> allInnerTypes = new LinkedHashMap<String, INedTypeNode>();
	protected Map<String, SubmoduleNode> allSubmodules = new LinkedHashMap<String, SubmoduleNode>();

	// sum of all own+inherited stuff
	protected Map<String, INEDElement> allMembers = new LinkedHashMap<String, INEDElement>();

    // all types which extends this component
    protected List<INEDTypeInfo> allDerivedTypes = new ArrayList<INEDTypeInfo>();
    
    // all types that contain instances (submodule, connection) of this type
    protected List<INEDTypeInfo> allUsingTypes = new ArrayList<INEDTypeInfo>();

	// for local use
    interface IPredicate {
		public boolean matches(IHasName node);
	}

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

		// register the created component in the INEDElement, so we will have access to it
        // directly from the model. We also want to listen on it, and invalidate ownMembers etc
		// if anything changes.
		INEDTypeInfo oldTypeInfo = node.getNEDTypeInfo();
		if (oldTypeInfo != null)
			node.removeNEDChangeListener(oldTypeInfo);
		node.addNEDChangeListener(this);
        node.setNEDTypeInfo(this);

        // the inherited and own members will be collected on demand
        needsOwnUpdate = true;
		needsUpdate = true;
	}

    public INEDTypeResolver getResolver() {
        return resolver;
    }

    /**
	 * Collect elements (gates, params, etc) with the given tag code (NED_PARAM, etc) 
	 * from the given section into the map.
	 */
	protected void collect(Map<String,? extends INEDElement> map, int sectionTagCode, final int tagCode) {
		collect(map, sectionTagCode, new IPredicate() {
			public boolean matches(IHasName node) {
				return node.getTagCode() == tagCode;
			}
		});
	}

    /**
	 * Collect elements (gates, params, etc) that match the predicate from the given section 
	 * (NED_PARAMETERS, NED_GATES, etc) into the map.
	 */
	@SuppressWarnings("unchecked")
	protected void collect(Map<String,? extends INEDElement> map, int sectionTagCode, IPredicate predicate) { 
		INEDElement section = componentNode.getFirstChildWithTag(sectionTagCode);
		if (section != null)
			for (INEDElement node : section) 
				if (node instanceof IHasName && predicate.matches((IHasName)node))
					((Map)map).put(((IHasName)node).getName(), node);
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
	    	//FIXME INedTypeNode already contains a getFirstExtendsType() method!!! use that!
	    	INedTypeNode currentComponentNode = currentComponent.getNEDElement();
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
        collect(ownProperties, NED_PARAMETERS, NED_PROPERTY);
        collect(ownParams, NED_PARAMETERS, NED_PARAM);
        collect(ownGates, NED_GATES, NED_GATE);
        collect(ownSubmodules, NED_SUBMODULES, NED_SUBMODULE);

        collect(ownParamValues, NED_PARAMETERS, new IPredicate() {
			public boolean matches(IHasName node) {
				return node.getTagCode()==NED_PARAM && StringUtils.isNotEmpty(((ParamNode)node).getValue());
			}
        });
        collect(ownGateSizes, NED_GATES, new IPredicate() {
			public boolean matches(IHasName node) {
				return node.getTagCode()==NED_GATE && StringUtils.isNotEmpty(((GateNode)node).getVectorSize());
			}
        });
        collect(ownInnerTypes, NED_TYPES, new IPredicate() {
			public boolean matches(IHasName node) {
				return node instanceof INedTypeNode;
			}
        });

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

	public INedTypeNode getNEDElement() {
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

    public Map<String, ParamNode> getParams() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParams;
    }

    public Map<String, ParamNode> getParamValues() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParamValues;
    }

    public Map<String, PropertyNode> getProperties() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allProperties;
    }

    public Map<String, GateNode> getGates() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGates;
    }

    public Map<String, GateNode> getGateSizes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGateSizes;
    }

    public Map<String, INedTypeNode> getInnerTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allInnerTypes;
    }

    public Map<String, SubmoduleNode> getSubmods() {
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

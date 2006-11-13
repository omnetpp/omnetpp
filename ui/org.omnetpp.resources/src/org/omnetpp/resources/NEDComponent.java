package org.omnetpp.resources;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDSourceRegion;
import org.omnetpp.ned2.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned2.model.interfaces.INEDTypeResolver;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
import org.omnetpp.ned2.model.notification.NEDAttributeChangeEvent;
import org.omnetpp.ned2.model.notification.NEDModelEvent;
import org.omnetpp.ned2.model.pojo.ChannelInterfaceNode;
import org.omnetpp.ned2.model.pojo.ChannelNode;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;
import org.omnetpp.ned2.model.pojo.GateNode;
import org.omnetpp.ned2.model.pojo.ModuleInterfaceNode;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.ParamNode;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SimpleModuleNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

/**
 * Default implementation of INEDComponent.
 */
public class NEDComponent implements INEDTypeInfo, NEDElementTags {

	protected INEDTypeResolver resolver;
	
	protected NEDElement componentNode;
	protected IFile file;
    static int refreshInheritedCount = 0;
    static int refreshOwnCount = 0;
	
	// own stuff
    boolean needsOwnUpdate;
	protected HashMap<String, NEDElement> ownProperties = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> ownParams = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> ownParamValues = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownGates = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> ownGateSizes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownInnerTypes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownSubmodules = new HashMap<String, NEDElement>();

	// sum of all "own" stuff
	protected HashMap<String, NEDElement> ownMembers = new HashMap<String, NEDElement>();

	// own plus inherited
	boolean needsUpdate;
	protected HashMap<String, NEDElement> allProperties = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> allParams = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> allParamValues = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> allGates = new HashMap<String, NEDElement>();
    protected HashMap<String, NEDElement> allGateSizes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allInnerTypes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allSubmodules = new HashMap<String, NEDElement>();

	// sum of all own+inherited stuff
	protected HashMap<String, NEDElement> allMembers = new HashMap<String, NEDElement>();
    
    // all types which extends this component
    protected List<INEDTypeInfo> allDerivedTypes = new ArrayList<INEDTypeInfo>();

    private boolean notifyInProgress = false;

	/**
	 * Constructor
	 * @param node NEDElement tree to be wrapped
	 * @param nedfile file containing the definition
	 * @param res will be used to resolve inheritance (collect gates, params etc from base classes)
	 */
	public NEDComponent(NEDElement node, IFile nedfile, INEDTypeResolver res) {
		resolver = res;
		file = nedfile;
		componentNode = node;
        // register the created component in the NEDElement so we will have access to it 
        // directly from the model
        node.setNEDTypeInfo(this);
		// the inherited and own members will be collected on demeand
        needsOwnUpdate = true;
		needsUpdate = true;
	}

    public INEDTypeResolver getResolver() {
        return resolver;
    }

    /**
	 * Collect parameter declarations, gate declarations, inner types, etc into a hash table
	 * @param hashmap  			stores result
	 * @param tagCode			types of elements to collect (NED_xxx constant)
	 * @param sectionTagCode	tag code of section to search within component node
	 * @param nameAttr			name of "name" attr of given node; this will be the key in the hashmap 
	 * @param typeAttr			name of "type" attr, or null if none. If given, the NEDElement must
	 * 							have this attribute non-empty to be collected (this is how we collect
	 * 							parameter declarations, and ignore parameter assignments/refinements)
	 */
	protected void collect(HashMap<String, NEDElement> hashmap, int tagCode, 
							int sectionTagCode,	String nameAttr, String typeAttr) {
		NEDElement section = componentNode.getFirstChildWithTag(sectionTagCode);
		if (section!=null) {
			// search nodes within section ("gates:", "parameters:", etc)
			for (NEDElement node : section) {
				if (tagCode==-1 || node.getTagCode()==tagCode) {
					if (typeAttr==null || !(node.getAttribute(typeAttr)==null || "".equals(node.getAttribute(typeAttr)))) {
						String name = node.getAttribute(nameAttr);
						hashmap.put(name, node);
					}
				}
			}
		}
	}

	/**
	 * Follow inheritance chain, and return the list of super classes 
	 * starting from the base class and ending with the current class
	 */
	public List<INEDTypeInfo> resolveExtendsChain() {
	    ArrayList<INEDTypeInfo> tmp = new ArrayList<INEDTypeInfo>();
    	tmp.add(this);
	    INEDTypeInfo currentComponent = this;
	    while (true) {
		    NEDElement currentComponentNode = currentComponent.getNEDElement();
	    	NEDElement extendsNode = currentComponentNode.getFirstChildWithTag(NED_EXTENDS);
	    	if (extendsNode==null) 
	    		break;
	    	String extendsName = ((ExtendsNode)extendsNode).getName();
	    	if (extendsName==null)
	    		break;
	    	currentComponent = resolver.getComponent(extendsName);
	    	if (currentComponent==null)
	    		break;
	    	tmp.add(0,currentComponent);
	    }
	    
	    return tmp;
	}

    /**
     * Refreshes the own members
     */
    protected void refreshOwnMembers() {
        // XXX for debuging
        ++refreshOwnCount;
        System.out.println("NEDComponent for "+getName()+" ownRefersh: " + refreshOwnCount);

        ownProperties.clear();
        ownParams.clear();
        ownParamValues.clear();
        ownGates.clear();
        ownGateSizes.clear();
        ownSubmodules.clear();
        ownInnerTypes.clear();
        ownMembers.clear();
        // collect stuff from component declaration
        collect(ownProperties, NED_PROPERTY, NED_PARAMETERS, PropertyNode.ATT_NAME, null);
        collect(ownParams, NED_PARAM, NED_PARAMETERS, ParamNode.ATT_NAME, ParamNode.ATT_TYPE);
        collect(ownParamValues, NED_PARAM, NED_PARAMETERS, ParamNode.ATT_NAME, null);
        collect(ownGates, NED_GATE, NED_GATES, GateNode.ATT_NAME, GateNode.ATT_TYPE);
        collect(ownGateSizes, NED_GATE, NED_GATES, GateNode.ATT_NAME, null);
        collect(ownSubmodules, NED_SUBMODULE, NED_SUBMODULES, SubmoduleNode.ATT_NAME, null);
        // XXX would be more efficient to collect the following in one pass:
        collect(ownInnerTypes, NED_SIMPLE_MODULE, NED_TYPES, SimpleModuleNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_COMPOUND_MODULE, NED_TYPES, CompoundModuleNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_CHANNEL, NED_TYPES, ChannelNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_MODULE_INTERFACE, NED_TYPES, ModuleInterfaceNode.ATT_NAME, null);
        collect(ownInnerTypes, NED_CHANNEL_INTERFACE, NED_TYPES, ChannelInterfaceNode.ATT_NAME, null);

        // collect them in one common hashtable as well (we assume there's no name clash -- 
        // that should be checked beforehand by validation!)
        ownMembers.putAll(ownProperties);
        ownMembers.putAll(ownParams);
        ownMembers.putAll(ownGates);
        ownMembers.putAll(ownSubmodules);
        ownMembers.putAll(ownInnerTypes);

        needsOwnUpdate = false;
    }
    
	/**
	 * Collect all inherited parameters, gates, properties, submodules, etc. 
	 */
	protected void refreshInheritedMembers() {
        ++refreshInheritedCount;
        System.out.println("NEDComponent for "+getName()+" inheritedRefersh: " + refreshInheritedCount);

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
        allDerivedTypes.clear();

        // collect all inherited members
		List<INEDTypeInfo> extendsChain = resolveExtendsChain();
		for (INEDTypeInfo icomponent : extendsChain) {
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
        
        // collect all types that are derived from this
        for(INEDTypeInfo currentComp : getResolver().getAllComponents()) {
            NEDElement element = currentComp.getNEDElement();
            for(NEDElement child : element) {
                if (child instanceof ExtendsNode) {
                    String extendsName = ((ExtendsNode)child).getName();
                    if (getName().equals(extendsName)) {
                        allDerivedTypes.add(currentComp);
                        break;
                    }
                }
            }
        }
        
		needsUpdate = false;
	}

	/**
	 * Causes information about inherited members to be discarded, and
	 * later re-built on demand. 
	 */
	public void invalidate() {
        System.out.println("NEDComponent invalidate");
        needsOwnUpdate = true;
		needsUpdate = true;
	}
	
	public String getName() {
		return componentNode.getAttribute("name");
	}
		
	public NEDElement getNEDElement() {
		return componentNode;
	}
	
	public IFile getNEDFile() {
		return file;
	}

	public NEDElement[] getNEDElementsAt(int line, int column) {
		ArrayList<NEDElement> list = new ArrayList<NEDElement>();
		NEDSourceRegion region = componentNode.getSourceRegion();
		if (region!=null && region.contains(line, column)) {
			list.add(componentNode);
			collectNEDElements(componentNode, line, column, list);
			return list.toArray(new NEDElement[list.size()]);
		}
		return null;
	}
	
	private void collectNEDElements(NEDElement node, int line, int column, ArrayList<NEDElement> list) {
		for (NEDElement child : node) {
			NEDSourceRegion region = child.getSourceRegion();
			if (region!=null && region.contains(line, column)) {
				list.add(child);
				collectNEDElements(child, line, column, list); // children fall inside parent's region
			}
		}
	}

    public Map<String, NEDElement> getOwnParams() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownParams;
    }
    
    public Map<String, NEDElement> getOwnParamValues() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownParamValues;
    }
    
    public Map<String, NEDElement> getOwnProperties() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownProperties;
    }

    public Map<String, NEDElement> getOwnGates() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownGates;
    }

    public Map<String, NEDElement> getOwnGateSizes() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownGateSizes;
    }

    public Map<String, NEDElement> getOwnInnerTypes() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownInnerTypes;
    }

    public Map<String, NEDElement> getOwnSubmods() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownSubmodules;
    }

    public Map<String, NEDElement> getOwnMembers() {
        if (needsOwnUpdate)
            refreshOwnMembers();
        return ownMembers;
    }

    public Map<String, NEDElement> getParams() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParams;
    }

    public Map<String, NEDElement> getParamValues() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allParamValues;
    }

    public Map<String, NEDElement> getProperties() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allProperties;
    }

    public Map<String, NEDElement> getGates() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGates;
    }

    public Map<String, NEDElement> getGateSizes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allGateSizes;
    }

    public Map<String, NEDElement> getInnerTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allInnerTypes;
    }

    public Map<String, NEDElement> getSubmods() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allSubmodules;
    }

    public Map<String, NEDElement> getMembers() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allMembers;
    }

    public List<INEDTypeInfo> getAllDerivedTypes() {
        if (needsUpdate)
            refreshInheritedMembers();
        return allDerivedTypes;
    }

//    public void attributeChanged(NEDElement node, String attr) {
//        if (notifyInProgress)
//            return;
//        notifyInProgress = true;
//        // if a name property has changed everything should be rebuilt because inheritence might be changed
//        if ("name".equals(attr) && node instanceof ITopLevelElement) { 
//            getResolver().invalidate();
//            getResolver().rehashIfNeeded();
//        }
//        // TODO test if the name attribute has changed and pass it to NEDResources 
//        // because in that case the whole model (All files) have to be rebuilt
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireAttributeChanged(node, attr);
//        // refresh all ownMemebers
//        refreshOwnMembers();
//        // invalidate and recalculate / refresh all derived and instance lists
//        invalidate();
//        // notify derived types before change
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireAttributeChanged(node, attr);
//        // TODO notify instances (ie. submodules)
//        // send notifcations to all types using us as a type (ie. instances of ourselves)
//        notifyInProgress = false;
//    }
//
//    public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
//        if (notifyInProgress)
//            return;
//        notifyInProgress = true;
//        
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireChildInserted(node, where, child);
//        // refresh all ownMemebers
//        refreshOwnMembers();
//        // invalidate and recalculate / refresh all derived and instance lists
//        invalidate();
//        // notify derived types before change
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireChildInserted(node, where, child);
//        // TODO notify instances (ie. submodules)
//        // send notifcations to all types using us as a type (ie. instances of ourselves)
//        notifyInProgress = false;
//    }
//
//    public void childRemoved(NEDElement node, NEDElement child) {
//        if (notifyInProgress)
//            return;
//        notifyInProgress = true;
//
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireChildRemoved(node, child);
//        // refresh all ownMemebers
//        refreshOwnMembers();
//        // invalidate and recalculate / refresh all derived and instance lists
//        invalidate();
//        // notify derived types before change
//        for(INEDTypeInfo derivedType: getAllDerivedTypes())
//            derivedType.getNEDElement().fireChildRemoved(node, child);
//        // TODO notify instances (ie. submodules)
//        // send notifcations to all types using us as a type (ie. instances of ourselves)
//        notifyInProgress = false;
//    }

    public void modelChanged(NEDModelEvent event) {
        if (notifyInProgress)
            return;
        notifyInProgress = true;
        // if a name property has changed everything should be rebuilt because inheritence might be changed
        if (event instanceof NEDAttributeChangeEvent) {
            NEDAttributeChangeEvent attrEvent = (NEDAttributeChangeEvent)event;
            if (SimpleModuleNode.ATT_NAME.equals(attrEvent.getAttribute()) 
                    && attrEvent.getSource() instanceof ITopLevelElement) { 
                getResolver().invalidate();
                getResolver().rehashIfNeeded();
            }
        }
        
        // TODO test if the name attribute has changed and pass it to NEDResources 
        // because in that case the whole model (All files) have to be rebuilt
        for(INEDTypeInfo derivedType: getAllDerivedTypes())
            derivedType.getNEDElement().fireModelChanged(event);
        // refresh all ownMemebers
        refreshOwnMembers();
        // invalidate and recalculate / refresh all derived and instance lists
        invalidate();
        // notify derived types before change
        for(INEDTypeInfo derivedType: getAllDerivedTypes())
            derivedType.getNEDElement().fireModelChanged(event);
        // TODO notify instances (ie. submodules)
        // send notifcations to all types using us as a type (ie. instances of ourselves)
        notifyInProgress = false;
    }

}

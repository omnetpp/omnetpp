package org.omnetpp.resources;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.NEDSourceRegion;
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
public class NEDComponent implements INEDComponent, NEDElementTags {

	protected INEDComponentResolver resolver;
	
	protected NEDElement componentNode;
	protected IFile file;
	
	// own stuff
	protected HashMap<String, NEDElement> ownProperties = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownParams = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownGates = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownInnerTypes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> ownSubmodules = new HashMap<String, NEDElement>();

	// sum of all "own" stuff
	protected HashMap<String, NEDElement> ownMembers = new HashMap<String, NEDElement>();

	// own plus inherited
	boolean needsUpdate;
	protected HashMap<String, NEDElement> allProperties = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allParams = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allGates = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allInnerTypes = new HashMap<String, NEDElement>();
	protected HashMap<String, NEDElement> allSubmodules = new HashMap<String, NEDElement>();

	// sum of all own+inherited stuff
	protected HashMap<String, NEDElement> allMembers = new HashMap<String, NEDElement>();

	/**
	 * Constructor
	 * @param node NEDElement tree to be wrapped
	 * @param nedfile file containing the definition
	 * @param res will be used to resolve inheritance (collect gates, params etc from base classes)
	 */
	public NEDComponent(NEDElement node, IFile nedfile, INEDComponentResolver res) {
		resolver = res;
		file = nedfile;
		componentNode = node;
		
		// collect stuff from component declaration
		collect(ownProperties, NED_PROPERTY, NED_PARAMETERS, PropertyNode.ATT_NAME, null);
		collect(ownParams, NED_PARAM, NED_PARAMETERS, ParamNode.ATT_NAME, ParamNode.ATT_TYPE);
		collect(ownGates, NED_GATE, NED_GATES, GateNode.ATT_NAME, GateNode.ATT_TYPE);
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
		
		needsUpdate = true;
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
					if (typeAttr==null || node.getAttribute(typeAttr)!=null) {
						String name = node.getAttribute(nameAttr);
						hashmap.put(name, node);
					}
				}
			}
		}
	}

	/**
	 * Follow inheritance chain, and return the list of super classes
	 */
	protected List<INEDComponent> resolveExtendsChain() {
	    ArrayList<INEDComponent> tmp = new ArrayList<INEDComponent>();
    	tmp.add(this);
	    INEDComponent currentComponent = this;
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
	    	tmp.add(currentComponent);
	    }
	    return tmp;
	}

	/**
	 * Collect all inherited parameters, gates, properties, submodules, etc. 
	 */
	protected void refreshInheritedMembers() {
		allProperties.clear();
		allParams.clear();
		allGates.clear();
		allInnerTypes.clear();
		allSubmodules.clear();
		allMembers.clear();

		List<INEDComponent> extendsChain = resolveExtendsChain();
		for (INEDComponent icomponent : extendsChain) {
			Assert.isTrue(icomponent instanceof NEDComponent);
			NEDComponent component = (NEDComponent)icomponent;
			allProperties.putAll(component.ownProperties);
			allParams.putAll(component.ownParams);
			allGates.putAll(component.ownGates);
			allInnerTypes.putAll(component.ownInnerTypes);
			allSubmodules.putAll(component.ownSubmodules);
			allMembers.putAll(component.ownMembers);
		}
		needsUpdate = false;
	}

	/**
	 * Causes information about inherited members to be discarded, and
	 * later re-built on demand. 
	 */
	public void componentsChanged() {
		needsUpdate = true;
	}

	/* INEDComponent method */
	public String getName() {
		return componentNode.getAttribute("name");
	}
	
	/* INEDComponent method */
	public NEDElement getNEDElement() {
		return componentNode;
	}

	/* INEDComponent method */
	public IFile getNEDFile() {
		return file;
	}

	/* INEDComponent method */
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

	/* INEDComponent method */
	public Set<String> getOwnParamNames() {
		return ownParams.keySet();
	}

	/* INEDComponent method */
	public boolean hasOwnParam(String name) {
		return ownParams.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getOwnPropertyNames() {
		return ownProperties.keySet();
	}
	
	/* INEDComponent method */
	public boolean hasOwnProperty(String name) {
		return ownProperties.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getOwnGateNames() {
		return ownGates.keySet();
	}
	
	/* INEDComponent method */
	public boolean hasOwnGate(String name) {
		return ownGates.containsKey(name);
	}
	
	/* INEDComponent method */
	public Set<String> getOwnInnerTypeNames() {
		return ownInnerTypes.keySet();
	}

	/* INEDComponent method */
	public boolean hasOwnInnerType(String name) {
		return ownInnerTypes.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getOwnSubmodNames() {
		return ownSubmodules.keySet();
	}

	/* INEDComponent method */
	public boolean hasOwnSubmod(String name) {
		return ownSubmodules.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getOwnMemberNames() {
		return ownMembers.keySet();
	}

	/* INEDComponent method */
	public boolean hasOwnMember(String name) {
		return ownMembers.containsKey(name);
	}

	/* INEDComponent method */
	public NEDElement getOwnMember(String name) {
		return ownMembers.get(name);
	}

	/* INEDComponent method */
	public Set<String> getParamNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allParams.keySet();
	}

	/* INEDComponent method */
	public boolean hasParam(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allParams.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getPropertyNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allProperties.keySet();
	}

	/* INEDComponent method */
	public boolean hasProperty(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allProperties.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getGateNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allGates.keySet();
	}

	/* INEDComponent method */
	public boolean hasGate(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allGates.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getInnerTypeNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allInnerTypes.keySet();
	}

	/* INEDComponent method */
	public boolean hasInnerType(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allInnerTypes.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getSubmodNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allSubmodules.keySet();
	}

	/* INEDComponent method */
	public boolean hasSubmod(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allSubmodules.containsKey(name);
	}

	/* INEDComponent method */
	public Set<String> getMemberNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.keySet();
	}

	/* INEDComponent method */
	public boolean hasMember(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.containsKey(name);
	}

	/* INEDComponent method */
	public NEDElement getMember(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.get(name);
	}
}

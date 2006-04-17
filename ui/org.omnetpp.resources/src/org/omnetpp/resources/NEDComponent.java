package org.omnetpp.resources;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.ned2.model.NEDElement;
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
 * Wraps a ChannelNode, ChannelInterfaceNode, SimpleModuleNode, CompoundModuleNode or
 * ModuleInterfaceNode NEDElement subtree.
 */
public class NEDComponent implements INEDComponent, NEDElementTags {

	protected INEDComponentResolver resolver;
	
	protected NEDElement componentNode;
	
	protected INEDComponent[] extendsChain = null;
	
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

	public NEDComponent(NEDElement node, INEDComponentResolver res) {
		resolver = res;
		componentNode = node;
		
		// collect stuff from component declaration
		collect(ownProperties, NED_PROPERTY, NED_PARAMETERS, -1, PropertyNode.ATT_NAME, null);
		collect(ownParams, NED_PARAM, NED_PARAMETERS, -1, ParamNode.ATT_NAME, ParamNode.ATT_TYPE);
		collect(ownGates, NED_GATE, NED_GATES, -1, GateNode.ATT_NAME, GateNode.ATT_TYPE);
		collect(ownSubmodules, NED_SUBMODULE, NED_SUBMODULES, -1, SubmoduleNode.ATT_NAME, null);
		// XXX would be more efficient to collect the following in one pass:
		collect(ownInnerTypes, NED_SIMPLE_MODULE, NED_TYPES, -1, SimpleModuleNode.ATT_NAME, null);
		collect(ownInnerTypes, NED_COMPOUND_MODULE, NED_TYPES, -1, CompoundModuleNode.ATT_NAME, null);
		collect(ownInnerTypes, NED_CHANNEL, NED_TYPES, -1, ChannelNode.ATT_NAME, null);
		collect(ownInnerTypes, NED_MODULE_INTERFACE, NED_TYPES, -1, ModuleInterfaceNode.ATT_NAME, null);
		collect(ownInnerTypes, NED_CHANNEL_INTERFACE, NED_TYPES, -1, ChannelInterfaceNode.ATT_NAME, null);

		// collecty  them in one common hashtable as well (we assume there's no name clash -- 
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
	 * @param tagCode			types of elements to collect (NED_xxx constant), or -1 for anything
	 * @param sectionTagCode	tag code of section to search within component node
	 * @param groupTagCode		tag code of group to search within section (e.g. NED_GATE_GROUP); -1 if none
	 * @param nameAttr			name of "name" attr of given node; this will be the key in the hashmap 
	 * @param typeAttr			name of "type" attr; this must be non-null (distinguishes between declaration
	 *  						and assignment/refinement); null if none
	 */
	private void collect(HashMap<String, NEDElement> hashmap, int tagCode, 
							int sectionTagCode,	int groupTagCode, String nameAttr, String typeAttr) {
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
				else if (groupTagCode!=-1 && node.getTagCode()==groupTagCode) {
					// "node" is a group itself, search it
					for (NEDElement node2 : node) {
						if (tagCode==-1 || node2.getTagCode()==tagCode) {
							if (typeAttr==null || node.getAttribute(typeAttr)!=null) {
								String name = node2.getAttribute(nameAttr);
								hashmap.put(name, node2);
							}
						}
					}
				}
			}
		}
	}

	protected void resolveExtendsChain() {
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
	    extendsChain = (INEDComponent[])tmp.toArray();
	}

	protected void refreshInheritedMembers() {
		allProperties.clear();
		allParams.clear();
		allGates.clear();
		allInnerTypes.clear();
		allSubmodules.clear();
		allMembers.clear();

		resolveExtendsChain();
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

	public void componentsChanged() {
		needsUpdate = true;
	}

	public NEDElement getNEDElement() {
		return componentNode;
	}

	public Set<String> getOwnParamNames() {
		return ownParams.keySet();
	}

	public boolean hasOwnParam(String name) {
		return ownParams.containsKey(name);
	}

	public Set<String> getOwnPropertyNames() {
		return ownProperties.keySet();
	}

	public boolean hasOwnProperty(String name) {
		return ownProperties.containsKey(name);
	}

	public Set<String> getOwnGateNames() {
		return ownGates.keySet();
	}

	public boolean hasOwnGate(String name) {
		return ownGates.containsKey(name);
	}

	public Set<String> getOwnInnerTypeNames() {
		return ownInnerTypes.keySet();
	}

	public boolean hasOwnInnerType(String name) {
		return ownInnerTypes.containsKey(name);
	}

	public Set<String> getOwnSubmodNames() {
		return ownSubmodules.keySet();
	}

	public boolean hasOwnSubmod(String name) {
		return ownSubmodules.containsKey(name);
	}

	public Set<String> getOwnMemberNames() {
		return ownMembers.keySet();
	}

	public boolean hasOwnMember(String name) {
		return ownMembers.containsKey(name);
	}

	public NEDElement getOwnMember(String name) {
		return ownMembers.get(name);
	}

	public Set<String> getParamNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allParams.keySet();
	}

	public boolean hasParam(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allParams.containsKey(name);
	}

	public Set<String> getPropertyNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allProperties.keySet();
	}

	public boolean hasProperty(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allProperties.containsKey(name);
	}

	public Set<String> getGateNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allGates.keySet();
	}

	public boolean hasGate(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allGates.containsKey(name);
	}

	public Set<String> getInnerTypeNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allInnerTypes.keySet();
	}

	public boolean hasInnerType(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allInnerTypes.containsKey(name);
	}

	public Set<String> getSubmodNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allSubmodules.keySet();
	}

	public boolean hasSubmod(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allSubmodules.containsKey(name);
	}

	public Set<String> getMemberNames() {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.keySet();
	}

	public boolean hasMember(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.containsKey(name);
	}

	public NEDElement getMember(String name) {
		if (needsUpdate)
			refreshInheritedMembers();
		return allMembers.get(name);
	}
}

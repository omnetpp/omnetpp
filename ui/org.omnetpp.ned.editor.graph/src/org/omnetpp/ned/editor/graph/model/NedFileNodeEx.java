package org.omnetpp.ned.editor.graph.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.NedFileNode;

public class NedFileNodeEx extends NedFileNode implements INedContainer, INedModelElement {
	
	public NedFileNodeEx() {
	}

	public NedFileNodeEx(NEDElement parent) {
		super(parent);
	}

	public List<CompoundModuleNodeEx> getCompoundModules() {
		List<CompoundModuleNodeEx> result = new ArrayList<CompoundModuleNodeEx>();
		for(NEDElement currChild : this) 
			if (currChild instanceof CompoundModuleNodeEx) 
				result.add((CompoundModuleNodeEx)currChild);
				
		return result;
	}
	
	public void addModelChild(INedNode child) {
		appendChild((NEDElement)child);
	}

	public void removeModelChild(INedNode child) {		
		removeChild((NEDElement)child);
	}

	public void insertModelChild(int index, INedNode child) {
		NEDElement insertBefore = getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertModelChild(INedNode insertBefore, INedNode child) {
		insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

	public List<? extends INedNode> getModelChildren() {
		List<INedNode> result = new ArrayList<INedNode>();
		for(NEDElement currChild : this) 
				result.add((INedNode)currChild);
				
		return result;
	}

}

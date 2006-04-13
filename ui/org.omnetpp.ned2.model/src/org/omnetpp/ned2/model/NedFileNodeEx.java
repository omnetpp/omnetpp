package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.pojo.NedFileNode;

public class NedFileNodeEx extends NedFileNode implements INedContainer {
	
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
	
	public void addModelChild(INedModule child) {
		appendChild((NEDElement)child);
	}

	public void removeModelChild(INedModule child) {		
		removeChild((NEDElement)child);
	}

	public void insertModelChild(int index, INedModule child) {
		NEDElement insertBefore = getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertModelChild(INedModule insertBefore, INedModule child) {
		insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

	public List<? extends INedModule> getModelChildren() {
		List<INedModule> result = new ArrayList<INedModule>();
		for(NEDElement currChild : this) 
				result.add((INedModule)currChild);
				
		return result;
	}

}

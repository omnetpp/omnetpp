package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.pojo.NedFileNode;

// TODO ISubmoduleContainer should be changed to some more meaningful interface
// ie we can add top level elements to nedfile but not submodules
public class NedFileNodeEx extends NedFileNode implements ISubmoduleContainer {
	
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
	
	public void addSubmodule(INamedGraphNode child) {
		appendChild((NEDElement)child);
	}

	public void removeSubmodule(INamedGraphNode child) {		
		removeChild((NEDElement)child);
	}

	public void insertSubmodule(int index, INamedGraphNode child) {
		NEDElement insertBefore = getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertSubmodule(INamedGraphNode insertBefore, INamedGraphNode child) {
		insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

	public List<? extends INamedGraphNode> getSubmodules() {
		List<INamedGraphNode> result = new ArrayList<INamedGraphNode>();
		for(NEDElement currChild : this) 
				result.add((INamedGraphNode)currChild);
				
		return result;
	}

}

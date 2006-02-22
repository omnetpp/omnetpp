package org.omnetpp.ned2.model.pojo;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;

public class NedFileNodeEx extends NedFileNode {
	
	public List<CompoundModuleNodeEx> getCompoundModules() {
		List<CompoundModuleNodeEx> result = new ArrayList<CompoundModuleNodeEx>();
		for(NEDElement currChild : this) 
			if (currChild instanceof CompoundModuleNodeEx) 
				result.add((CompoundModuleNodeEx)currChild);
				
		return result;
	}

}

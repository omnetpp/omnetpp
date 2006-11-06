package org.omnetpp.ned2.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.interfaces.ITopLevelElement;
import org.omnetpp.ned2.model.pojo.NedFileNode;

public class NedFileNodeEx extends NedFileNode {
	
	NedFileNodeEx() {
	}

	NedFileNodeEx(NEDElement parent) {
		super(parent);
	}

	public List<ITopLevelElement> getTopLevelElements() {
		List<ITopLevelElement> result = new ArrayList<ITopLevelElement>();
		for(NEDElement currChild : this) 
			if (currChild instanceof ITopLevelElement) 
				result.add((ITopLevelElement)currChild);
				
		return result;
	}
}

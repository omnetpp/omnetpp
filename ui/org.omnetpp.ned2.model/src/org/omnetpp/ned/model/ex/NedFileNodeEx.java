package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.ITopLevelElement;
import org.omnetpp.ned.model.pojo.NedFileNode;

public final class NedFileNodeEx extends NedFileNode {
	
    protected NedFileNodeEx() {
	}

    protected NedFileNodeEx(NEDElement parent) {
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

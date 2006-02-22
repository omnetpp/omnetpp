package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.PropertyNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode {
	// FIXME move the constants out
	public static final String DISPLAY_PROPERTY = "display";
	
	public String getDisplayString() {
		// check all parameters
		try {
			for(NEDElement currElement : getFirstParametersChild())
				if(currElement instanceof PropertyNode) {
					PropertyNode currProp = (PropertyNode)currElement;
					if(DISPLAY_PROPERTY.equals(currProp.getName()))
						return currProp.getFirstPropertyKeyChild()
											.getFirstLiteralChild().getValue();
				}
		} catch (NullPointerException e) {
			// display string is in illegal format, missing value etc (e.g @display )
		}
		return null;
	}

}

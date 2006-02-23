package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.NEDElementTags;
import org.omnetpp.ned2.model.pojo.PropertyNode;

public class NedElementExUtil implements NEDElementTags {
	public static final String DISPLAY_PROPERTY = "display";
	
	public static String getDisplayString(NEDElement node) {
		// check all parameters
		try {
			for(NEDElement currElement : node.getFirstChildWithTag(NED_PARAMETERS))
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
	
	public static void setDisplayString(NEDElement node, String dspString) {
		// TODO implement setter for display property 
	}
	

}

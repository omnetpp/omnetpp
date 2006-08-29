package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned2.model.NEDElementFactoryEx;
import org.omnetpp.ned2.model.SubmoduleNodeEx;


/**
 * @author rhornig
 * A factory used to create new submodules with a given type
 */
public class SubmoduleFactory implements CreationFactory {
	protected String submoduleType;

	public SubmoduleFactory(String type) {
		submoduleType = type;
	}
	
	public Object getNewObject() {
		SubmoduleNodeEx subMod = (SubmoduleNodeEx)NEDElementFactoryEx.getInstance()
									.createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULE);
		subMod.setType(submoduleType);
		return subMod;
	}

	public Object getObjectType() {
		return SubmoduleNodeEx.class;
	}

}


package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.requests.CreationFactory;
import org.omnetpp.ned2.model.NEDElementFactoryEx;

/**
 * @author rhornig
 * Factory to create top level elements
 */
public class ToplevelComponentFactory implements CreationFactory {

	protected String type;

	public ToplevelComponentFactory(String type) {
		this.type = type;
	}

	public Object getNewObject() {
		Object namedToplevelComp = NEDElementFactoryEx.getInstance().createNodeWithTag(type);
		return namedToplevelComp;
	}

	public Object getObjectType() {
		return type;
	}

}

package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.NEDChangeListener;

public interface INedModelElement {

	public void addListener(NEDChangeListener l);
	public void removeListener(NEDChangeListener l);

}

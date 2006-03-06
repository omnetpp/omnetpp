package org.omnetpp.ned.editor.graph.model;

import org.omnetpp.ned2.model.INotifiableNEDElement;


/**
 * FIXME.... 
 */
public interface INedModelElement extends INotifiableNEDElement {

	/**
	 * Add listener
	 */
	public void addListener(INEDChangeListener l);

	/**
	 * Remove listener
	 */
	public void removeListener(INEDChangeListener l);
}

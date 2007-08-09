package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * Objects that have a name property
 * @author rhornig
 */
public interface IHasName extends INEDElement {

	static String DEFAULT_TYPE_NAME = "Unnamed";
	/**
	 * Returns name attribute
	 */
	public String getName();

	/**
	 * Sets name attribute
	 */
	public void setName(String name);

}
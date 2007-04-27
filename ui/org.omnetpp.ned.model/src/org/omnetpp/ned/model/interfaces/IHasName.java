package org.omnetpp.ned.model.interfaces;

/**
 * Objects that have a name property
 * @author rhornig
 */
public interface IHasName {

	static String INITIAL_NAME = "unnamed";
	/**
	 * Returns name attribute
	 */
	public String getName();

	/**
	 * Sets name attribute
	 */
	public void setName(String name);

}
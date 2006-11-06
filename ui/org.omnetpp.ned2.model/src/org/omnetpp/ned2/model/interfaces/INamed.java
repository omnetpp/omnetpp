package org.omnetpp.ned2.model.interfaces;

/**
 * @author rhornig
 * Objects that have a name property
 */
public interface INamed {

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
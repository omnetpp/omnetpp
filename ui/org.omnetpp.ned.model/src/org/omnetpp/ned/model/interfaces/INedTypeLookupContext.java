package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;


/**
 * Marker interface for elements in whose context NED type names
 * need to be resolved (compound module and NED file)
 *
 * @author andras
 */
public interface INedTypeLookupContext extends INEDElement {
	/**
	 * Returns an empty string or a string ending in a dot. For a NED file 
	 * this string contains the package name, and for a container type 
	 * (compound module) it contains the fully qualified name of the module.
	 */
	public String getQNameAsPrefix();
}
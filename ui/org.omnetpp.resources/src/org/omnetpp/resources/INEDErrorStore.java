package org.omnetpp.resources;

import org.omnetpp.ned2.model.NEDElement;

/**
 * Interface for issuing NED error messages
 * @author andras
 */
public interface INEDErrorStore {
	  public void add(NEDElement context, String message);
}

package org.omnetpp.ned.core;

import org.omnetpp.ned.model.INEDElement;

/**
 * Interface for issuing NED error messages
 * @author andras
 */
public interface INEDErrorStore {
	  public void add(INEDElement context, String message);
}

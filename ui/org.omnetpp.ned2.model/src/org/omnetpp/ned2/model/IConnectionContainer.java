package org.omnetpp.ned2.model;

import java.util.List;

public interface IConnectionContainer {

	/**
	 * Add this connection to the model (connections section)
	 * @param conn
	 */
	void addConnection(ConnectionNodeEx conn);

	/**
	 * Add this connection to the model (connections section)
	 * @param insertBefore The sibling connection before we want to insert our conn 
	 * @param conn
	 */
	void insertConnection(ConnectionNodeEx insertBefore, ConnectionNodeEx conn);
	
	/**
	 * Removes the connecion from the model
	 * @param conn
	 */
	void removeConnection(ConnectionNodeEx conn);

	/**
	 * Returns all connections containedin this module
	 * @return
	 */
	List<ConnectionNodeEx> getConnections();

}
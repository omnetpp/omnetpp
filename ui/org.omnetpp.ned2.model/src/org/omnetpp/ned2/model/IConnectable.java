package org.omnetpp.ned2.model;


/**
 * @author rhornig
 * Generic interface implemented by all nodes that can be connected via gates (Compound and submodule)
 */
public interface IConnectable extends INamed {

	public final String ATT_DEST_CONNECTION = "destConnection";
	public final String ATT_SRC_CONNECTION = "srcConnection";

//	/**
//	 * Returns connections whose "src" side is this component 
//	 * (ie this compound module or submodule; for others it returns
//	 * null.) 
//	 */
//	List<ConnectionNodeEx> getSrcConnections();
//
//	/**
//	 * Returns connections whose "dest" side is this component 
//	 * (ie this compound module or submodule; for others it returns
//	 * null.) 
//	 */
//	List<ConnectionNodeEx> getDestConnections();
//
//	/**
//	 * add a connection to the node. this should be called only from the
//	 * ConnectionNodeEx calss. DO NOT call this from the application directly
//	 * use ConnectionNodeEx.setSrcModuleRef instead
//	 */
//	void attachSrcConnection(ConnectionNodeEx conn);
//
//	/**
//	 * remove a connection to the node. this should be called only from the
//	 * ConnectionNodeEx calss. DO NOT call this from the application directly
//	 * use ConnectionNodeEx.setSrcModuleRef instead
//	 */
//	void detachSrcConnection(ConnectionNodeEx conn);
//
//	/**
//	 * add a connection to the node. this should be called only from the
//	 * ConnectionNodeEx calss. DO NOT call this from the application directly
//	 * use ConnectionNodeEx.setDestModuleRef instead
//	 */
//	void attachDestConnection(ConnectionNodeEx conn);
//
//	/**
//	 * remove a connection to the node. this should be called only from the
//	 * ConnectionNodeEx calss. DO NOT call this from the application directly
//	 * use ConnectionNodeEx.setDestModuleRef instead
//	 */
//	void detachDestConnection(ConnectionNodeEx conn);

}
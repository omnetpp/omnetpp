package org.omnetpp.ned2.model;

import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

/**
 * CompoundModule, Submodule model interface
 * @author rhornig
 */
public interface INedModule extends IDisplayStringProvider {

    public final String ATT_SRC_CONNECTION = "srcConnection";
    public final String ATT_DEST_CONNECTION = "destConnection";

    /**
	 * Returns name attribute 
	 */
	public String getName();

	/**
	 * Sets name attribute 
	 */
    public void setName(String name);

	/**
     * Returns connections whose "src" side is this component 
     * (ie this compound module or submodule; for others it returns
     * null.) 
     */
    public List<ConnectionNodeEx> getSrcConnections();

    /**
     * Returns connections whose "dest" side is this component 
     * (ie this compound module or submodule; for others it returns
     * null.) 
     */
	public List<ConnectionNodeEx> getDestConnections();
	

	/**
	 * add a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
     * use ConnectionNodeEx.setSrcModuleRef instead
	 */
	public void addSrcConnection(ConnectionNodeEx conn);

	/**
	 * remove a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
     * use ConnectionNodeEx.setSrcModuleRef instead
	 */
	public void removeSrcConnection(ConnectionNodeEx conn);

	/**
	 * add a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
     * use ConnectionNodeEx.setDestModuleRef instead
	 */
	public void addDestConnection(ConnectionNodeEx conn);

	/**
	 * remove a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
     * use ConnectionNodeEx.setDestModuleRef instead
	 */
	public void removeDestConnection(ConnectionNodeEx conn);
	
	/**
	 * Add this connection to the model (connections section)
	 * @param conn
	 */
	public void addConnection(ConnectionNodeEx conn);

	/**
	 * Removes the connecion from the model
	 * @param conn
	 */
	public void removeConnection(ConnectionNodeEx conn);
	
    /**
     * Removes this node from the parent
     */
    public void removeFromParent();

}

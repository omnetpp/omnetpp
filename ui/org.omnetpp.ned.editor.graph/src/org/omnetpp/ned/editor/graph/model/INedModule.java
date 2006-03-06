package org.omnetpp.ned.editor.graph.model;

import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;

/**
 * CompoundModule, Submodule model interface
 * @author rhornig
 */
public interface INedModule {

	/**
	 * Returns name attribute 
	 */
	public String getName();

	/**
	 * Sets name attribute 
	 */
    public void setName(String name);

	/**
	 * Returns display string property, or null 
	 */
    public String getDisplayString();

	/**
	 * Sets the display string property 
	 */
    public void setDisplayString(String dspString);
    
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
	 */
	public void addSrcConnection(ConnectionNodeEx conn);

	/**
	 * remove a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
	 */
	public void removeSrcConnection(ConnectionNodeEx conn);

	/**
	 * add a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
	 */
	public void addDestConnection(ConnectionNodeEx conn);

	/**
	 * remove a connection to the node. this should be called only from the
	 * ConnectionNodeEx calss. DO NOT call this from the application directly
	 */
	public void removeDestConnection(ConnectionNodeEx conn);

	/**
	 * Helper function to set the location of the node
	 * @param location
	 */
	public void setLocation(Point location);

	/**
	 * Helper function to get the location of the node
	 * @param location
	 */
	public Point getLocation();
	
	/**
	 * Helper function to get the size of the node
	 * @return
	 */
	public Dimension getSize();
	
	/**
	 * Helper function to set the size of the node
	 * @param size
	 */
	public void setSize(Dimension size);

}

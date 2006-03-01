package org.omnetpp.ned.editor.graph.model;

import java.util.List;

import org.eclipse.draw2d.geometry.Dimension;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.ned2.model.DisplayString;

/**
 * Parameters, gates, inheritence, name, display string
 * @author rhornig
 */
public interface INedNode extends INedModelElement {

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
	 * Set the TEMPORARY location of the node
	 * @param location
	 */
	public void setTransientLocation(Point location);

	/**
	 * Get the TEMPORARY location of the node
	 * @param location
	 */
	public Point getTransientLocation();
	
	public Dimension getSize();
	public void setSize(Dimension size);

}

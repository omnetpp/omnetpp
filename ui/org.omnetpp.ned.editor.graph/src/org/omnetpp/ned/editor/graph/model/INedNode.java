package org.omnetpp.ned.editor.graph.model;

import java.util.List;

import org.eclipse.draw2d.geometry.Point;

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
	 * Sets display string property 
	 */
    public void setDisplayString(String dspString);
    
    public Point getLocation();
    public void setLocation(Point loc);
    

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
}

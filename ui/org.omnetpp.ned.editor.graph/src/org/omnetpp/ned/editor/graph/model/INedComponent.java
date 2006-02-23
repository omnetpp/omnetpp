package org.omnetpp.ned.editor.graph.model;

import java.util.List;

/**
 * Parameters, gates, inheritence, name, display string
 * @author rhornig
 */
public interface INedComponent extends INedModelElement {

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

    /**
     * Returns connections whose "src" side is this component 
     * (ie this compound module or submodule; for others it returns
     * null.) 
     */
    public List<ConnectionNodeEx> getSourceConnections();

    /**
     * Returns connections whose "dest" side is this component 
     * (ie this compound module or submodule; for others it returns
     * null.) 
     */
	public List<ConnectionNodeEx> getTargetConnections();
}

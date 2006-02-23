package org.omnetpp.ned.editor.graph.model;

import java.util.List;

import org.omnetpp.ned2.model.pojo.ConnectionsNode;



/**
 * Parameters, gates, inheritence, name, display string
 * @author rhornig
 *
 */
public interface INedComponent {
	public String getName();
	public void setName(String name);
	public String getDisplayString();
	public void setDisplayString(String dspString);
	public List<ConnectionNodeEx> getSourceConnections(ConnectionsNode connsNode, String moduleName);
	public List<ConnectionNodeEx> getTargetConnections(ConnectionsNode connsNode, String moduleName);
}

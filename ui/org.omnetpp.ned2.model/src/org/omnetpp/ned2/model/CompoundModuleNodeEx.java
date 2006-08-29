package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;

public class CompoundModuleNodeEx extends CompoundModuleNode 
								  implements ISubmoduleContainer, IConnectionContainer,
								  			 INamedGraphNode {
    
	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();

	protected DisplayString displayString = null;
	
	public CompoundModuleNodeEx() {
		init();
	}

	public CompoundModuleNodeEx(NEDElement parent) {
		super(parent);
		init();
	}

	
    private void init() {
        // TODO correctly handle the initial naming for new nodes (name mUst be unique)
        setName("unnamed");
    }
	
	public DisplayString getDisplayString() {
		if (displayString == null)
			displayString = new DisplayString(this, NedElementExUtil.getDisplayString(this));
		return displayString;
	}
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NedElementExUtil.setDisplayString(this, displayString.toString());
	}

	public List<SubmoduleNodeEx> getSubmodules() {
		List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submodulesNode == null)
			return result;
		for(NEDElement currChild : submodulesNode) 
			if (currChild instanceof SubmoduleNodeEx) 
				result.add((SubmoduleNodeEx)currChild);
				
		return result;
	}

	public SubmoduleNodeEx getSubmoduleByName(String submoduleName) {
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submoduleName == null)
			return null;
		return (SubmoduleNodeEx)submodulesNode
					.getFirstChildWithAttribute(NED_SUBMODULE, SubmoduleNode.ATT_NAME, submoduleName);
	}

	public List<ConnectionNodeEx> getSrcConnections() {
		return srcConns;
	}
	
	public List<ConnectionNodeEx> getDestConnections() {
		return destConns;
	}
	
    public void attachSrcConnection(ConnectionNodeEx conn) {
        assert(!srcConns.contains(conn));
        srcConns.add(conn);
        addConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void detachSrcConnection(ConnectionNodeEx conn) {
        assert(srcConns.contains(conn));
        srcConns.remove(conn);
        removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void attachDestConnection(ConnectionNodeEx conn) {
        assert(!destConns.contains(conn));
        destConns.add(conn);
        addConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    public void detachDestConnection(ConnectionNodeEx conn) {
        assert(destConns.contains(conn));
        destConns.remove(conn);
        removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

	public void addSubmodule(INamedGraphNode child) {
        SubmodulesNode snode = getFirstSubmodulesChild();
        if (snode == null) 
            snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild((NEDElement)child);
	}

	public void removeSubmodule(INamedGraphNode child) {		
        child.removeFromParent();
        SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode != null && !snode.hasChildren()) 
			snode.removeFromParent();
	}

	public void insertSubmodule(int index, INamedGraphNode child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null) 
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);
		
		NEDElement insertBefore = snode.getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i) 
			insertBefore = insertBefore.getNextSibling();
		
		snode.insertChildBefore(insertBefore, (NEDElement)child);
	}

	public void insertSubmodule(INamedGraphNode insertBefore, INamedGraphNode child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null) 
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);
		
		snode.insertChildBefore((NEDElement)insertBefore, (NEDElement)child);
	}

	public void addConnection(ConnectionNodeEx conn) {
		insertConnection(null, conn);
	}

	public void insertConnection(ConnectionNodeEx insertBefore, ConnectionNodeEx conn) {
		// do nothing if it's already in the model
		if (conn.getParent() != null)
			return;
		// check wheter Submodules node exists and create one if doesn't
		ConnectionsNode snode = getFirstConnectionsChild();
		if (snode == null) 
			snode = (ConnectionsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_CONNECTIONS, this);
		
		// add it to the connections subnode
		snode.insertChildBefore(insertBefore, (NEDElement)conn);
	}

	public void removeConnection(ConnectionNodeEx conn) {
		conn.removeFromParent();
		
		ConnectionsNode snode = getFirstConnectionsChild();
		if (snode != null && !snode.hasChildren()) 
			snode.removeFromParent();
	}

	public List<ConnectionNodeEx> getConnections() {
		List<ConnectionNodeEx> result = new ArrayList<ConnectionNodeEx>();
		ConnectionsNode connectionsNode = getFirstConnectionsChild();
		if (connectionsNode == null)
			return result;
		for(NEDElement currChild : connectionsNode) 
			if (currChild instanceof ConnectionNodeEx) 
				result.add((ConnectionNodeEx)currChild);
				
		return result;
	}

}

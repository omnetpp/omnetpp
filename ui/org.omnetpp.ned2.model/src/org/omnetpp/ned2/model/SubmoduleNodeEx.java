package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.ned2.model.DisplayString.Prop;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;

public class SubmoduleNodeEx extends SubmoduleNode implements INamedGraphNode {
    // srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();
	protected SubmoduleDisplayString displayString = null;
	
	public SubmoduleNodeEx() {
        init();
	}

	public SubmoduleNodeEx(NEDElement parent) {
		super(parent);
        init();
	}

    private void init() {
        // TODO correctly handle the initial naming for new nodes (name most be unique)
        setName("unnamed");
        setType("node");
    }
    
	public DisplayString getDisplayString() {
		// TODO set the ancestor module correctly
		if (displayString == null)
			displayString = new SubmoduleDisplayString(this, null, 
												NedElementExUtil.getDisplayString(this));
		return displayString;
	}
	
	public void propertyChanged(Prop changedProp) {
		// syncronize it to the underlying model 
		NedElementExUtil.setDisplayString(this, displayString.toString());
	}

	public CompoundModuleNodeEx getCompoundModule() {
		if (getParent() == null) return null;
		return (CompoundModuleNodeEx)(getParent().getParent());
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
        getCompoundModule().addConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void detachSrcConnection(ConnectionNodeEx conn) {
        assert(srcConns.contains(conn));
        srcConns.remove(conn);
        getCompoundModule().removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void attachDestConnection(ConnectionNodeEx conn) {
        assert(!destConns.contains(conn));
        destConns.add(conn);
        getCompoundModule().addConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    public void detachDestConnection(ConnectionNodeEx conn) {
        assert(destConns.contains(conn));
        destConns.remove(conn);
        getCompoundModule().removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

//	public void addConnection(ConnectionNodeEx conn) {
//		// do nothing if it's already in the modell
//		if (conn.getParent() != null)
//			return;
//		// get the submodule's container
//		CompoundModuleNodeEx parentModule = getCompoundModule();
//		// check wheter Submodules node exists and create one if doesn't
//		ConnectionsNode snode = parentModule.getFirstConnectionsChild();
//		if (snode == null) 
//			snode = (ConnectionsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_CONNECTIONS, parentModule);
//		
//		// add it to the connections subnode
//		snode.insertChildBefore(null, (NEDElement)conn);
//	}
//
//	public void removeConnection(ConnectionNodeEx conn) {
//		// do nothing if the connectin is not in the modell
//		if(conn.getParent() == null)
//			return;
//
//		conn.removeFromParent();
//	}

	@Override
    public String debugString() {
        return "srcConnSize="+srcConns.size()+" destConnSize="+destConns.size();
    }

}

package org.omnetpp.ned2.model;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.displaymodel.IDisplayString.Prop;
import org.omnetpp.ned2.model.pojo.CompoundModuleNode;
import org.omnetpp.ned2.model.pojo.ConnectionsNode;
import org.omnetpp.ned2.model.pojo.ExtendsNode;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.ned2.model.pojo.SubmodulesNode;

public class CompoundModuleNodeEx extends CompoundModuleNode
								  implements INamedGraphNode, IDerived, ITopLevelElement {

	// srcConns contains all connections where the sourcemodule is this module
	protected List<ConnectionNodeEx> srcConns = new ArrayList<ConnectionNodeEx>();
	// destConns contains all connections where the destmodule is this module
	protected List<ConnectionNodeEx> destConns = new ArrayList<ConnectionNodeEx>();

	protected DisplayString displayString = null;

	CompoundModuleNodeEx() {
		init();
	}

	CompoundModuleNodeEx(NEDElement parent) {
		super(parent);
		init();
	}


    private void init() {
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
        fireAttributeChangedToAncestors(IDisplayString.ATT_DISPLAYSTRING+"."+changedProp);
	}

    /**
     * Returns all submodule containd in the module.
     * @param child
     * @return
     */
	public List<SubmoduleNodeEx> getOwnSubmodules() {
		List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submodulesNode == null)
			return result;
		for(NEDElement currChild : submodulesNode)
			if (currChild instanceof SubmoduleNodeEx)
				result.add((SubmoduleNodeEx)currChild);

		return result;
	}
    
    /**
     * @return All submodules direct and inherited submodules 
     */
    public List<SubmoduleNodeEx> getAllSubmodules() {
        ITypeInfo it = getTypeInfo();
        // if no typeinfo present fall back to the current module list
        if (it == null)
            return getOwnSubmodules();
        
        List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
        for(NEDElement currChild : it.getSubmods())
            if (currChild instanceof SubmoduleNodeEx)
                result.add((SubmoduleNodeEx)currChild);
        
        return result;
    }

	/**
	 * @param submoduleName
	 * @return The submodule with the provided name
	 */
	public SubmoduleNodeEx getSubmoduleByName(String submoduleName) {
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submoduleName == null)
			return null;
		return (SubmoduleNodeEx)submodulesNode
					.getFirstChildWithAttribute(NED_SUBMODULE, SubmoduleNode.ATT_NAME, submoduleName);
	}

	/**
     * Add a 
	 * @param child
	 */
	public void addSubmodule(SubmoduleNodeEx child) {
        SubmodulesNode snode = getFirstSubmodulesChild();
        if (snode == null)
            snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild(child);
	}

    /**
     * Remove a specfic child from the parent.
     * @param child
     */
	public void removeSubmodule(SubmoduleNodeEx child) {
        child.removeFromParent();
        SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode != null && !snode.hasChildren())
			snode.removeFromParent();
	}

    /**
     * Insert the child at the give position.
     * @param child Child to be inserted
     * @param insertBefore Sibling element where the child will be inserted
     */
	public void insertSubmodule(int index, SubmoduleNodeEx child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null)
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

		NEDElement insertBefore = snode.getFirstChild();
		for(int i=0; (i<index) && (insertBefore!=null); ++i)
			insertBefore = insertBefore.getNextSibling();

		snode.insertChildBefore(insertBefore, child);
	}

    /**
     * Insert the child at the give position.
     * @param insertBefore
     * @param child
     */
	public void insertSubmodule(SubmoduleNodeEx insertBefore, SubmoduleNodeEx child) {
		// check wheter Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null)
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

		snode.insertChildBefore(insertBefore, child);
	}

    ////////////////////////////////////////////////////////////////////////////////////////
    // connection related methods
    
    public List<ConnectionNodeEx> getSrcConnections() {
        return srcConns;
    }

    public List<ConnectionNodeEx> getDestConnections() {
        return destConns;
    }

    public void attachSrcConnection(ConnectionNodeEx conn) {
        Assert.isTrue(!srcConns.contains(conn));
        srcConns.add(conn);
        addConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void detachSrcConnection(ConnectionNodeEx conn) {
        Assert.isTrue(srcConns.contains(conn));
        srcConns.remove(conn);
        removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_SRC_CONNECTION);
    }

    public void attachDestConnection(ConnectionNodeEx conn) {
        Assert.isTrue(!destConns.contains(conn));
        destConns.add(conn);
        addConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    public void detachDestConnection(ConnectionNodeEx conn) {
        Assert.isTrue(destConns.contains(conn));
        destConns.remove(conn);
        removeConnection(conn);
        fireAttributeChangedToAncestors(ATT_DEST_CONNECTION);
    }

    /**
     * Add this connection to the model (connections section)
     * @param conn
     */
	public void addConnection(ConnectionNodeEx conn) {
		insertConnection(null, conn);
	}

    /**
     * Add this connection to the model (connections section)
     * @param insertBefore The sibling connection before we want to insert our conn 
     * @param conn
     */
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

    /**
     * Removes the connecion from the model
     * @param conn
     */
	public void removeConnection(ConnectionNodeEx conn) {
		conn.removeFromParent();

		ConnectionsNode snode = getFirstConnectionsChild();
		if (snode != null && !snode.hasChildren())
			snode.removeFromParent();
	}

    /**
     * Returns all connections containedin this module
     * @return
     */
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
    
    ///////////////////////////////////////////////////////////////////////
    // extend support
    public String getExtends() {
        ExtendsNode extendsNode = (ExtendsNode)getFirstExtendsChild();
        if(extendsNode == null)
            return null;

        return extendsNode.getName();
    }

    public void setExtends(String ext) {
        ExtendsNode extendsNode = getFirstExtendsChild();
            if (extendsNode == null) {
                extendsNode = (ExtendsNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NED_EXTENDS);
                appendChild(extendsNode);
            }
            extendsNode.setName(ext);
    }

}

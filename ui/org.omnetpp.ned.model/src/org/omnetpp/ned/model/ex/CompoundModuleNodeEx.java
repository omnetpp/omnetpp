package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeNode;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INamedGraphNode;
import org.omnetpp.ned.model.interfaces.INedTypeNode;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleNode;
import org.omnetpp.ned.model.pojo.ConnectionGroupNode;
import org.omnetpp.ned.model.pojo.ConnectionsNode;
import org.omnetpp.ned.model.pojo.ExtendsNode;
import org.omnetpp.ned.model.pojo.InterfaceNameNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;
import org.omnetpp.ned.model.pojo.SubmodulesNode;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public final class CompoundModuleNodeEx extends CompoundModuleNode
				implements IModuleTypeNode, INamedGraphNode {

	protected DisplayString displayString = null;

    protected CompoundModuleNodeEx() {
		init();
	}

    protected CompoundModuleNodeEx(INEDElement parent) {
		super(parent);
		init();
	}

    private void init() {
        setName("Unnamed");
    }

    @Override
    public void fireModelChanged(NEDModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NEDElementUtilEx.isDisplayStringUpToDate(this))
    		displayString = null;
    	super.fireModelChanged(event);
    }

    public DisplayString getDisplayString() {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayString(this));
    	displayString.setFallbackDisplayString(NEDElementUtilEx.displayStringOf(getFirstExtendsRef()));
    	return displayString;
    }

    // submodule related methods
    /**
     * Returns all submodules contained in THIS module.
     */
	protected List<SubmoduleNodeEx> getOwnSubmodules() {
		List<SubmoduleNodeEx> result = new ArrayList<SubmoduleNodeEx>();
		SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		if (submodulesNode == null)
			return result;
		for (INEDElement currChild : submodulesNode)
			if (currChild instanceof SubmoduleNodeEx)
				result.add((SubmoduleNodeEx)currChild);

		return result;
	}

    /**
     * Returns the list of all direct and inherited submodules
     */
    public List<SubmoduleNodeEx> getSubmodules() {
        List<SubmoduleNodeEx> result = getOwnSubmodules();

        // FIXME beware this can lead to an infite recursion if we have a circle in the extend chanin
        INEDElement extendsElem = getFirstExtendsRef();
        if (extendsElem != null && extendsElem instanceof CompoundModuleNodeEx)
            result.addAll(((CompoundModuleNodeEx)extendsElem).getSubmodules());

        return result;
    }

	/**
	 * Returns the submodule with the provided name, excluding inherited submodules.
	 */
	protected SubmoduleNodeEx getOwnSubmoduleByName(String submoduleName) {
	    if (submoduleName == null)
	        return null;

        SubmodulesNode submodulesNode = getFirstSubmodulesChild();
		return (SubmoduleNodeEx)submodulesNode
					.getFirstChildWithAttribute(NED_SUBMODULE, SubmoduleNode.ATT_NAME, submoduleName);
	}

    /**
     * Returns the submodule (including inherited ones) with the provided name,
     * or null if not found.
     */
    public SubmoduleNodeEx getSubmoduleByName(String submoduleName) {
        // first look in the current module
        SubmoduleNodeEx submoduleNode = getOwnSubmoduleByName(submoduleName);
        if (submoduleNode != null)
            return submoduleNode;
        // then look in ancestors
        // FIXME beware this can lead to an infinite recursion if we have a circle in the extend chanin
        INEDElement extendsElem = getFirstExtendsRef();
        if (extendsElem != null && extendsElem instanceof CompoundModuleNodeEx)
            return ((CompoundModuleNodeEx)extendsElem).getSubmoduleByName(submoduleName);

        // not found
        return null;
    }

	/**
     * Add the given submodule to this module
	 */
	public void addSubmodule(SubmoduleNodeEx child) {
        SubmodulesNode snode = getFirstSubmodulesChild();
        if (snode == null)
            snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild(child);
	}

    /**
     * Remove a specfic child from this module.
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

		INEDElement insertBefore = snode.getFirstChild();
		for (int i=0; (i<index) && (insertBefore!=null); ++i)
			insertBefore = insertBefore.getNextSibling();

		snode.insertChildBefore(insertBefore, child);
	}

    /**
     * Insert the submodule child at the given position. (Internally, a
     * "submodules:" node will be created if not yet present).
     */
	public void insertSubmodule(SubmoduleNodeEx insertBefore, SubmoduleNodeEx child) {
		// check whether Submodules node exists and create one if doesn't
		SubmodulesNode snode = getFirstSubmodulesChild();
		if (snode == null)
			snode = (SubmodulesNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementFactoryEx.NED_SUBMODULES, this);

		snode.insertChildBefore(insertBefore, child);
	}

    ////////////////////////////////////////////////////////////////////////////////////////
    // connection related methods

    /**
     *
     * @param srcName srcModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param srcGate source gate name to filter or NULL if no filtering needed
     * @param destName destModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param destGate destination gate name to filter or NULL if no filtering needed
     * @return ALL VALID!!! connections contained in this module with matching src and dest module name
     */
    private List<ConnectionNodeEx> getOwnConnections(String srcName, String srcGate, String destName, String destGate) {
        List<ConnectionNodeEx> result = new ArrayList<ConnectionNodeEx>();
        INEDElement connectionsNode = getFirstConnectionsChild();
        if (connectionsNode != null)
            gatherConnections(connectionsNode, srcName, srcGate, destName, destGate, result);
        return result;
    }

    /**
     * TODO add docu
     */
    private void gatherConnections(INEDElement parent, String srcName, String srcGate, String destName, String destGate, List<ConnectionNodeEx> result) {
        for (INEDElement currChild : parent) {
            if (currChild instanceof ConnectionNodeEx) {
                ConnectionNodeEx connChild = (ConnectionNodeEx)currChild;
                // by default add the connection
                if (srcName != null && !srcName.equals(connChild.getSrcModule()))
                    continue;

                if (srcGate != null && !srcGate.equals(connChild.getSrcGate()))
                    continue;

                if (destName != null && !destName.equals(connChild.getDestModule()))
                    continue;

                if (destGate != null && !destGate.equals(connChild.getDestGate()))
                    continue;

                // skip invalid connections (those that has unknown modules at either side)
                if (!connChild.isValid())
                    continue;

                // if all was ok, add it to the list
                result.add(connChild);
            }
            else if (currChild instanceof ConnectionGroupNode) {
                // FIXME remove the comment is the layouter works without infite loops
                gatherConnections(currChild, srcName, srcGate, destName, destGate, result);
            }
        }
    }



    /**
     * @param srcName srcModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param srcGate source gate name to filter or NULL if no filtering needed
     * @param destName destModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param destGate destination gate name to filter or NULL if no filtering needed
     * @return ALL VALID!!! connections contained in / and inherited by this module
     */
    public List<ConnectionNodeEx> getConnections(String srcName, String srcGate, String destName, String destGate) {
        List<ConnectionNodeEx> result = getOwnConnections(srcName, srcGate, destName, destGate);

        // FIXME beware this can lead to an infite recursion if we have a circle in the extend chanin
        INEDElement extendsElem = getFirstExtendsRef();
        if (extendsElem != null && extendsElem instanceof CompoundModuleNodeEx)
            result.addAll(((CompoundModuleNodeEx)extendsElem).getConnections(srcName, srcGate, destName, destGate));

        return result;
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by this module where this module is the source
     */
    public List<ConnectionNodeEx> getSrcConnections() {
        return getConnections("", null, null, null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by this module where this module is the destination
     */
    public List<ConnectionNodeEx> getDestConnections() {
        return getConnections(null, null, "", null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by the provided module
     * where this module is the source
     */
    public List<ConnectionNodeEx> getSrcConnectionsFor(String submoduleName) {
        return getConnections(submoduleName,null, null, null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by the provided module
     * where this module is the destinaion
     */
    public List<ConnectionNodeEx> getDestConnectionsFor(String submoduleName) {
        return getConnections(null, null, submoduleName, null);
    }

    /**
     * Add this connection to the model (to the "connections" section; it will
     * be created if not yet exists)
     */
	public void addConnection(ConnectionNodeEx conn) {
		insertConnection(null, conn);
	}

    /**
     * Add this connection to the model (connections section)
     * @param insertBefore The sibling connection before we want to insert our conn
     * FIXME what if insertBefore==null ??? --Andras
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
		snode.insertChildBefore(insertBefore, conn);
	}

    /**
     * Removes the connecion from the model
     */
	public void removeConnection(ConnectionNodeEx conn) {
		conn.removeFromParent();

		ConnectionsNode snode = getFirstConnectionsChild();
		if (snode != null && !snode.hasChildren() && !snode.getAllowUnconnected())
			snode.removeFromParent();
	}

    ///////////////////////////////////////////////////////////////////////
    // extends support
    public String getFirstExtends() {
        return NEDElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NEDElementUtilEx.setFirstExtends(this, ext);
    }

    public INEDTypeInfo getFirstExtendsNEDTypeInfo() {
        String extendsName = getFirstExtends();
        INEDTypeInfo typeInfo = getContainerNEDTypeInfo();
        if ( extendsName == null || "".equals(extendsName) || typeInfo == null)
            return null;

        return typeInfo.getResolver().getComponent(extendsName);
    }

    public INedTypeNode getFirstExtendsRef() {
        INEDTypeInfo it = getFirstExtendsNEDTypeInfo();
        return it == null ? null : (INedTypeNode) it.getNEDElement();
    }

    public List<ExtendsNode> getAllExtends() {
        List<ExtendsNode> result = new ArrayList<ExtendsNode>();
        ExtendsNode extendsNode = getFirstExtendsChild();
        if (extendsNode == null)
            return result;

        for (INEDElement currChild : extendsNode)
            if (currChild instanceof ExtendsNode)
                result.add(extendsNode);

        return result;
    }
    // parameter query support
    public Map<String, INEDElement> getParamValues() {
        return getContainerNEDTypeInfo().getParamValues();
    }

    public Map<String, INEDElement> getParams() {
        return getContainerNEDTypeInfo().getParams();
    }

    // gate support
    public Map<String, INEDElement> getGateSizes() {
        return getContainerNEDTypeInfo().getGateSizes();
    }

    public Map<String, INEDElement> getGates() {
        return getContainerNEDTypeInfo().getGates();
    }

    // interface implementation support
    public List<InterfaceNameNode> getAllInterfaces() {
        List<InterfaceNameNode> result = new ArrayList<InterfaceNameNode>();

        for (INEDElement currChild : this)
            if (currChild instanceof InterfaceNameNode)
                result.add((InterfaceNameNode)currChild);

        return result;
    }

}

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;

import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.notification.NEDModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleElementEx extends CompoundModuleElement implements IModuleTypeElement, IConnectableElement {

	private INEDTypeInfo typeInfo;
	protected DisplayString displayString = null;

    protected CompoundModuleElementEx() {
		init();
	}

    protected CompoundModuleElementEx(INEDElement parent) {
		super(parent);
		init();
	}

    private void init() {
        setName("Unnamed");
    }

    public void setNEDTypeInfo(INEDTypeInfo typeInfo) {
    	this.typeInfo = typeInfo;
    }

    public INEDTypeInfo getNEDTypeInfo() {
    	return typeInfo;
    }

    @Override
    public void fireModelChanged(NEDModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NEDElementUtilEx.isDisplayStringUpToDate(displayString, this))
    		displayString = null;
    	super.fireModelChanged(event);
    }

    public DisplayString getDisplayString() {
    	if (displayString == null)
    		displayString = new DisplayString(this, NEDElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(NEDElementUtilEx.displayStringOf(getFirstExtendsRef()));
    	return displayString;
    }

    // submodule related methods

    /**
     * Returns all submodules contained in THIS module.
     */
	protected List<SubmoduleElementEx> getOwnSubmodules() {
		List<SubmoduleElementEx> result = new ArrayList<SubmoduleElementEx>();

		SubmodulesElement submodulesElement = getFirstSubmodulesChild();
		if (submodulesElement != null)
			for (INEDElement currChild : submodulesElement)
				if (currChild instanceof SubmoduleElementEx)
					result.add((SubmoduleElementEx)currChild);

		return result;
	}

    /**
     * Returns the list of all direct and inherited submodules
     *
     * "Best-Effort": This method never returns null, but the returned list
     * may be incomplete if some NED type is incorrect, missing, or duplicate.
     */
    public List<SubmoduleElementEx> getSubmodules() {
    	return Arrays.asList(getNEDTypeInfo().getSubmodules().values().toArray(new SubmoduleElementEx[]{}));
    }

	/**
	 * Returns the submodule with the provided name, excluding inherited submodules.
	 * Returns null if not found.
	 */
	protected SubmoduleElementEx getOwnSubmoduleByName(String submoduleName) {
		return getNEDTypeInfo().getLocalSubmodules().get(submoduleName);
	}

    /**
     * Returns the submodule (including inherited ones) with the provided name,
     * or null if not found.
     */
    public SubmoduleElementEx getSubmoduleByName(String submoduleName) {
		return getNEDTypeInfo().getSubmodules().get(submoduleName);
    }

	/**
     * Add the given submodule to this module
	 */
	public void addSubmodule(SubmoduleElementEx child) {
        SubmodulesElement snode = getFirstSubmodulesChild();
        if (snode == null)
            snode = (SubmodulesElement)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild(child);
	}

    /**
     * Remove a specific child from this module.
     */
	public void removeSubmodule(SubmoduleElementEx child) {
        child.removeFromParent();
        SubmodulesElement snode = getFirstSubmodulesChild();
		if (snode != null && !snode.hasChildren())
			snode.removeFromParent();
	}

    /**
     * Insert the child at the give position.
     * @param child Child to be inserted
     * @param insertBefore Sibling element where the child will be inserted
     */
	public void insertSubmodule(int index, SubmoduleElementEx child) {
		// check whether Submodules node exists and create one if doesn't
		SubmodulesElement snode = getFirstSubmodulesChild();
		if (snode == null)
			snode = (SubmodulesElement)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_SUBMODULES, this);

		INEDElement insertBefore = snode.getFirstChild();
		for (int i=0; (i<index) && (insertBefore!=null); ++i)
			insertBefore = insertBefore.getNextSibling();

		snode.insertChildBefore(insertBefore, child);
	}

    /**
     * Insert the submodule child at the given position. (Internally, a
     * "submodules:" node will be created if not yet present).
     */
	public void insertSubmodule(SubmoduleElementEx insertBefore, SubmoduleElementEx child) {
		// check whether Submodules node exists and create one if doesn't
		SubmodulesElement snode = getFirstSubmodulesChild();
		if (snode == null)
			snode = (SubmodulesElement)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_SUBMODULES, this);

		snode.insertChildBefore(insertBefore, child);
	}

    // connection related methods

    /**
     *
     * @param srcName srcModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param srcGate source gate name to filter or NULL if no filtering needed
     * @param destName destModule to filter for ("" for compound module and NULL if not filtering is required)
     * @param destGate destination gate name to filter or NULL if no filtering needed
     * @return ALL VALID!!! connections contained in this module with matching src and dest module name
     */
    private List<ConnectionElementEx> getOwnConnections(String srcName, String srcGate, String destName, String destGate) {
        List<ConnectionElementEx> result = new ArrayList<ConnectionElementEx>();
        INEDElement connectionsNode = getFirstConnectionsChild();
        if (connectionsNode != null)
            gatherConnections(connectionsNode, srcName, srcGate, destName, destGate, result);
        return result;
    }

    /**
     * TODO add docu
     */
    private void gatherConnections(INEDElement parent, String srcName, String srcGate, String destName, String destGate, List<ConnectionElementEx> result) {
        for (INEDElement currChild : parent) {
            if (currChild instanceof ConnectionElementEx) {
                ConnectionElementEx connChild = (ConnectionElementEx)currChild;
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
            else if (currChild instanceof ConnectionGroupElement) {
                // FIXME remove the comment is the layouter works without infinite loops
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
    public List<ConnectionElementEx> getConnections(String srcName, String srcGate, String destName, String destGate) {
    	List<ConnectionElementEx> result = new ArrayList<ConnectionElementEx>();
    	for (INEDTypeInfo typeInfo : getNEDTypeInfo().getExtendsChain())
    		if (typeInfo.getNEDElement() instanceof CompoundModuleElementEx)
    			result.addAll(((CompoundModuleElementEx)typeInfo.getNEDElement()).getOwnConnections(srcName, srcGate, destName, destGate));
        return result;
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by this module where this module is the source
     */
    public List<ConnectionElementEx> getSrcConnections() {
        return getConnections("", null, null, null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by this module where this module is the destination
     */
    public List<ConnectionElementEx> getDestConnections() {
        return getConnections(null, null, "", null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by the provided module
     * where this module is the source
     */
    public List<ConnectionElementEx> getSrcConnectionsFor(String submoduleName) {
        return getConnections(submoduleName, null, null, null);
    }

    /**
     * Returns ALL VALID connections contained in / and inherited by the provided module
     * where this module is the destination
     */
    public List<ConnectionElementEx> getDestConnectionsFor(String submoduleName) {
        return getConnections(null, null, submoduleName, null);
    }

    /**
     * Add this connection to the model (to the "connections" section; it will
     * be created if not yet exists)
     */
	public void addConnection(ConnectionElementEx conn) {
		insertConnection(null, conn);
	}

    /**
     * Add this connection to the model (connections section)
     * @param insertBefore The sibling connection we want to insert our connection
     *                     before, or null for append
     */
	public void insertConnection(ConnectionElementEx insertBefore, ConnectionElementEx conn) {
		// do nothing if it's already in the model
		if (conn.getParent() != null)
			return;
		// check whether Submodules node exists and create one if doesn't
		ConnectionsElement snode = getFirstConnectionsChild();
		if (snode == null)
			snode = (ConnectionsElement)NEDElementFactoryEx.getInstance().createElement(NEDElementFactoryEx.NED_CONNECTIONS, this);

		// add it to the connections node
		snode.insertChildBefore(insertBefore, conn);
	}

    /**
     * Removes the connection from the model
     */
	public void removeConnection(ConnectionElementEx conn) {
		conn.removeFromParent();

		ConnectionsElement snode = getFirstConnectionsChild();
		if (snode != null && !snode.hasChildren() && !snode.getAllowUnconnected())
			snode.removeFromParent();
	}

    // "extends" support
    public String getFirstExtends() {
        return NEDElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NEDElementUtilEx.setFirstExtends(this, ext);
    }

    public INedTypeElement getFirstExtendsRef() {
        return getNEDTypeInfo().getFirstExtendsRef();
    }

    public List<ExtendsElement> getAllExtends() {
        List<ExtendsElement> result = new ArrayList<ExtendsElement>();
        ExtendsElement extendsElement = getFirstExtendsChild();
        if (extendsElement == null)
            return result;

        for (INEDElement currChild : extendsElement)
            if (currChild instanceof ExtendsElement)
                result.add(extendsElement);

        return result;
    }

    // parameter query support
    public Map<String, ParamElementEx> getParamAssignments() {
        return getNEDTypeInfo().getParamAssignments();
    }

    public Map<String, ParamElementEx> getParamDeclarations() {
        return getNEDTypeInfo().getParamDeclarations();
    }

    // gate support
    public Map<String, GateElementEx> getGateSizes() {
        return getNEDTypeInfo().getGateSizes();
    }

    public Map<String, GateElementEx> getGateDeclarations() {
        return getNEDTypeInfo().getGateDeclarations();
    }

}

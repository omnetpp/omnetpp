/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModuleElementEx extends CompoundModuleElement implements IModuleTypeElement, IConnectableElement, INedTypeLookupContext {

	private INedTypeInfo typeInfo;
	protected DisplayString displayString = null;

    protected CompoundModuleElementEx() {
		init();
	}

    protected CompoundModuleElementEx(INedElement parent) {
		super(parent);
		init();
	}

    private void init() {
        setName("Unnamed");
		typeInfo = getDefaultNedTypeResolver().createTypeInfoFor(this);
    }

    @Override
    public String getReadableTagName() {
        if (isNetwork())
            return "network";
        else
            return super.getReadableTagName();
    }

    public INedTypeInfo getNedTypeInfo() {
    	return typeInfo;
    }

	public String getQNameAsPrefix() {
		return getNedTypeInfo().getFullyQualifiedName() + ".";
	}

	public INedTypeLookupContext getParentLookupContext() {
		return getParentLookupContextFor(this);
	}

    @Override
    public void fireModelEvent(NedModelEvent event) {
    	// invalidate cached display string because NED tree may have changed outside the DisplayString class
    	if (!NedElementUtilEx.isDisplayStringUpToDate(displayString, this))
    		displayString = null;
    	super.fireModelEvent(event);
    }

    public boolean isNetwork() {
    	// this isNetwork property should not be inherited so we look only among the local properties
    	PropertyElementEx networkPropertyElementEx = getNedTypeInfo().getLocalProperty(IS_NETWORK_PROPERTY, null);
    	if (networkPropertyElementEx == null)
    		return false;
    	String propValue = NedElementUtilEx.getPropertyValue(networkPropertyElementEx);
        return !StringUtils.equalsIgnoreCase("false", propValue);
    }

    public void setIsNetwork(boolean value) {
        NedElementUtilEx.setBooleanProperty(this, IModuleTypeElement.IS_NETWORK_PROPERTY, value);
    }

    public DisplayString getDisplayString() {
    	if (displayString == null)
    		displayString = new DisplayString(this, NedElementUtilEx.getDisplayStringLiteral(this));
    	displayString.setFallbackDisplayString(NedElement.displayStringOf(getFirstExtendsRef()));
    	return displayString;
    }

    /**
     * Returns all inner types contained in THIS module.
     */
    public List<INedTypeElement> getOwnInnerTypes() {
        List<INedTypeElement> result = new ArrayList<INedTypeElement>();

        TypesElement typesElement = getFirstTypesChild();
        if (typesElement != null)
            for (INedElement currChild : typesElement)
                if (currChild instanceof INedTypeElement)
                    result.add((INedTypeElement)currChild);

        return result;
    }

    // submodule related methods

    /**
     * Returns all submodules contained in THIS module.
     */
	protected List<SubmoduleElementEx> getOwnSubmodules() {
		List<SubmoduleElementEx> result = new ArrayList<SubmoduleElementEx>();

		SubmodulesElement submodulesElement = getFirstSubmodulesChild();
		if (submodulesElement != null)
			for (INedElement currChild : submodulesElement)
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
    	return Arrays.asList(getNedTypeInfo().getSubmodules().values().toArray(new SubmoduleElementEx[]{}));
    }

	/**
	 * Returns the submodule with the provided name, excluding inherited submodules.
	 * Returns null if not found.
	 */
	protected SubmoduleElementEx getOwnSubmoduleByName(String submoduleName) {
		return getNedTypeInfo().getLocalSubmodules().get(submoduleName);
	}

    /**
     * Returns the submodule (including inherited ones) with the provided name,
     * or null if not found.
     */
    public SubmoduleElementEx getSubmoduleByName(String submoduleName) {
		return getNedTypeInfo().getSubmodules().get(submoduleName);
    }

	/**
     * Add the given submodule to this module
	 */
	public void addSubmodule(SubmoduleElementEx child) {
        SubmodulesElement snode = getFirstSubmodulesChild();
        if (snode == null)
            snode = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild(child);
	}

    /**
     * Remove a specific submodule child from this module.
     * Submodules node will be removed if iw was the last child
     */
	public void removeSubmodule(SubmoduleElementEx child) {
        child.removeFromParent();
        SubmodulesElement snode = getFirstSubmodulesChild();
		if (snode != null && !snode.hasChildren())
			snode.removeFromParent();
	}

    /**
     * Insert the submodule child at the given position. (Internally, a
     * "submodules:" node will be created if not yet present).
     */
	public void insertSubmodule(int index, SubmoduleElementEx child) {
		// check whether Submodules node exists and create one if doesn't
		SubmodulesElement submodulesElement = getFirstSubmodulesChild();
		if (submodulesElement == null)
			submodulesElement = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_SUBMODULES, this);

		INedElement insertBefore = submodulesElement.getFirstChild();
		for (int i=0; i<index && insertBefore!=null; ++i)
			insertBefore = insertBefore.getNextSibling();

		submodulesElement.insertChildBefore(insertBefore, child);
	}

    /**
     * Insert the submodule child at the given position. (Internally, a
     * "submodules:" node will be created if not yet present).
     */
	public void insertSubmodule(SubmoduleElementEx insertBefore, SubmoduleElementEx child) {
		// check whether Submodules node exists and create one if doesn't
		SubmodulesElement submodulesElement = getFirstSubmodulesChild();
		if (submodulesElement == null)
			submodulesElement = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_SUBMODULES, this);

		submodulesElement.insertChildBefore(insertBefore, child);
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
        INedElement connectionsNode = getFirstConnectionsChild();
        if (connectionsNode != null)
            gatherConnections(connectionsNode, srcName, srcGate, destName, destGate, result);
        return result;
    }

    /**
     * TODO add docu
     */
    private void gatherConnections(INedElement parent, String srcName, String srcGate, String destName, String destGate, List<ConnectionElementEx> result) {
        for (INedElement currChild : parent) {
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
    	for (INedTypeInfo typeInfo : getNedTypeInfo().getExtendsChain())
    		if (typeInfo.getNedElement() instanceof CompoundModuleElementEx)
    			result.addAll(((CompoundModuleElementEx)typeInfo.getNedElement()).getOwnConnections(srcName, srcGate, destName, destGate));
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
			snode = (ConnectionsElement)NedElementFactoryEx.getInstance().createElement(NedElementFactoryEx.NED_CONNECTIONS, this);

		// add it to the connections node
		snode.insertChildBefore(insertBefore, conn);
	}

    // "extends" support
    public String getFirstExtends() {
        return NedElementUtilEx.getFirstExtends(this);
    }

    public void setFirstExtends(String ext) {
        NedElementUtilEx.setFirstExtends(this, ext);
    }

    public INedTypeElement getFirstExtendsRef() {
        return getNedTypeInfo().getFirstExtendsRef();
    }

    public List<ExtendsElement> getAllExtends() {
    	return getAllExtendsFrom(this);
    }

    // parameter query support
    public Map<String, ParamElementEx> getParamAssignments() {
        return getNedTypeInfo().getParamAssignments();
    }

    public Map<String, ParamElementEx> getParamDeclarations() {
        return getNedTypeInfo().getParamDeclarations();
    }

    public List<ParamElementEx> getParameterInheritanceChain(String parameterName) {
        return getNedTypeInfo().getParameterInheritanceChain(parameterName);
    }

    public Map<String, Map<String, PropertyElementEx>> getProperties() {
        return getNedTypeInfo().getProperties();
    }

    // gate support
    public Map<String, GateElementEx> getGateSizes() {
        return getNedTypeInfo().getGateSizes();
    }

    public Map<String, GateElementEx> getGateDeclarations() {
        return getNedTypeInfo().getGateDeclarations();
    }

    public Set<INedTypeElement> getLocalUsedTypes() {
        return getNedTypeInfo().getLocalUsedTypes();
    }
}

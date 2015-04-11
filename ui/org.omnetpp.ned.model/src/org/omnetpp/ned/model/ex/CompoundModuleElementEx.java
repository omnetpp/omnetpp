/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.ex;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.commons.collections.ListUtils;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.interfaces.IConnectableElement;
import org.omnetpp.ned.model.interfaces.IModuleTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeInfo;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.pojo.CompoundModuleElement;
import org.omnetpp.ned.model.pojo.ConnectionGroupElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.ExtendsElement;
import org.omnetpp.ned.model.pojo.SubmodulesElement;
import org.omnetpp.ned.model.pojo.TypesElement;

/**
 * Represents a compound module type
 *
 * @author rhornig, andras (cleanup, performance)
 */
public class CompoundModuleElementEx extends CompoundModuleElement implements IModuleTypeElement, IConnectableElement, INedTypeLookupContext {

    private INedTypeResolver resolver;
    private INedTypeInfo typeInfo;
    protected DisplayString displayString = null;

    private long cacheUpdateSerial = -1; // resolver.getLastChangeSerial() when connection cache was filled in
    private Map<String,List<ConnectionElementEx>> localSrcConnCache = null;  // list of non-inherited, valid connections by source module name
    private Map<String,List<ConnectionElementEx>> localDestConnCache = null; // list of non-inherited, valid connections by destination module name

    @SuppressWarnings("unchecked")
    private static final List<ConnectionElementEx> EMPTY_CONN_LIST = ListUtils.unmodifiableList(new ArrayList<ConnectionElementEx>());

    protected CompoundModuleElementEx(INedTypeResolver resolver) {
        init(resolver);
    }

    protected CompoundModuleElementEx(INedTypeResolver resolver, INedElement parent) {
        super(parent);
        init(resolver);
    }

    private void init(INedTypeResolver resolver) {
        Assert.isNotNull(resolver, "This NED element type needs a resolver");
        this.resolver = resolver;
        this.typeInfo = getResolver().createTypeInfoFor(this);
        setName("Unnamed");
    }

    public INedTypeResolver getResolver() {
        return resolver;
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
        displayString.setFallbackDisplayString(NedElement.displayStringOf(getSuperType()));
        return displayString;
    }

    /**
     * Returns all inner types contained in THIS module.
     */
    public List<INedTypeElement> getOwnInnerTypes() {
        // TODO cache!
        // Note: cannot use INedTypeInfo's methods, because it loses types with duplicate names
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
    //XXX currently unused (why?)
    protected List<SubmoduleElementEx> getOwnSubmodules() {
        // TODO cache!
        // Note: cannot use INedTypeInfo's methods, because it loses submodules with duplicate names
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
            snode = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementFactoryEx.NED_SUBMODULES, this);

        snode.appendChild(child);
    }

    /**
     * Remove a specific submodule child from this module.
     * Submodules node will be removed if it was the last child
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
            submodulesElement = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementFactoryEx.NED_SUBMODULES, this);

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
            submodulesElement = (SubmodulesElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementFactoryEx.NED_SUBMODULES, this);

        submodulesElement.insertChildBefore(insertBefore, child);
    }

    // connection related methods

    @SuppressWarnings("unchecked")
    private void ensureConnectionCache() {
        if (localSrcConnCache == null || cacheUpdateSerial != getResolver().getLastChangeSerial()) {
            localSrcConnCache = new HashMap<String, List<ConnectionElementEx>>();
            localDestConnCache = new HashMap<String, List<ConnectionElementEx>>();
            INedElement connectionsNode = getFirstConnectionsChild();
            if (connectionsNode != null) {
                // fill in the maps
                gatherConnectionsForCache(connectionsNode);

                // make the lists unmodifiable, because getters return them to clients
                for (Entry<String, List<ConnectionElementEx>> entry : localSrcConnCache.entrySet())
                    entry.setValue(ListUtils.unmodifiableList(entry.getValue()));
                for (Entry<String, List<ConnectionElementEx>> entry : localDestConnCache.entrySet())
                    entry.setValue(ListUtils.unmodifiableList(entry.getValue()));
            }
            cacheUpdateSerial = getResolver().getLastChangeSerial();
        }
    }

    private void gatherConnectionsForCache(INedElement parent) {
        for (INedElement child : parent) {
            if (child instanceof ConnectionElementEx) {
                ConnectionElementEx conn = (ConnectionElementEx) child;
                if (conn.isValid()) {
                    String src = conn.getSrcModule();
                    List<ConnectionElementEx> srcList = localSrcConnCache.get(src);
                    if (srcList == null)
                        localSrcConnCache.put(src, srcList = new ArrayList<ConnectionElementEx>());
                    srcList.add(conn);

                    String dest = conn.getDestModule();
                    List<ConnectionElementEx> destList = localDestConnCache.get(dest);
                    if (destList == null)
                        localDestConnCache.put(dest, destList = new ArrayList<ConnectionElementEx>());
                    destList.add(conn);
                }
            }
            else if (child instanceof ConnectionGroupElement) {
                gatherConnectionsForCache(child);
            }
        }
    }

    /**
     * Returns a filtered list of local plus inherited valid (see ConnectionElementEx.isValid())
     * connections. The four arguments correspond to connection attributes; any can be null
     * to mean "no filtering".
     *
     * Note: this method uses linear search, so it should not be used in performance-critical code.
     *
     * @param srcName source module name to filter for; "" for compound module or null for no filtering
     * @param srcGate source gate name to filter for, or null for no filtering
     * @param destName destination module name to filter for; "" for compound module or null for no filtering
     * @param destGate destination gate name to filter for, or null for no filtering
     * @return list of matching connections
     */
    public List<ConnectionElementEx> getConnections(String srcName, String srcGate, String destName, String destGate) {
        List<ConnectionElementEx> result = new ArrayList<ConnectionElementEx>();
        for (INedTypeInfo typeInfo : getNedTypeInfo().getInheritanceChain())
            if (typeInfo.getNedElement() instanceof CompoundModuleElementEx)
                result.addAll(((CompoundModuleElementEx)typeInfo.getNedElement()).getOwnConnections(srcName, srcGate, destName, destGate));
        return result;
    }

    private List<ConnectionElementEx> getOwnConnections(String srcName, String srcGate, String destName, String destGate) {
        List<ConnectionElementEx> result = new ArrayList<ConnectionElementEx>();
        INedElement connectionsNode = getFirstConnectionsChild();
        if (connectionsNode != null)
            gatherConnections(connectionsNode, srcName, srcGate, destName, destGate, result);
        return result;
    }

    private void gatherConnections(INedElement parent, String srcName, String srcGate, String destName, String destGate, List<ConnectionElementEx> result) {
        for (INedElement child : parent) {
            if (child instanceof ConnectionElementEx) {
                ConnectionElementEx conn = (ConnectionElementEx)child;
                if (srcName != null && !srcName.equals(conn.getSrcModule()))
                    continue;

                if (srcGate != null && !srcGate.equals(conn.getSrcGate()))
                    continue;

                if (destName != null && !destName.equals(conn.getDestModule()))
                    continue;

                if (destGate != null && !destGate.equals(conn.getDestGate()))
                    continue;

                // skip invalid connections (those that have unknown modules at either side)
                if (!conn.isValid())
                    continue;

                // if all was OK, add it to the list
                result.add(conn);
            }
            else if (child instanceof ConnectionGroupElement) {
                gatherConnections(child, srcName, srcGate, destName, destGate, result);
            }
        }
    }

    /**
     * Returns the list of local plus inherited valid (see ConnectionElementEx.isValid()) connections,
     * where this compound module is the source module of the connection.
     */
    public List<ConnectionElementEx> getSrcConnections() {
        return getSrcConnectionsFor("");
    }

    /**
     * Returns the list of local plus inherited valid (see ConnectionElementEx.isValid()) connections,
     * where this compound module is the destination module of the connection.
     */
    public List<ConnectionElementEx> getDestConnections() {
        return getDestConnectionsFor("");
    }

    /**
     * Returns the list of local plus inherited valid (see ConnectionElementEx.isValid()) connections,
     * where the given submodule is the destination module of the connection. Returns an empty list
     * if such submodule does not exist.
     */
    public List<ConnectionElementEx> getSrcConnectionsFor(String submoduleName) {
        ensureConnectionCache();
        List<ConnectionElementEx> result = localSrcConnCache.get(submoduleName); // immutable list
        if (result == null)
            result = EMPTY_CONN_LIST;

        // add inherited connections too, if we have any (uncommon)
        INedTypeElement superType = getNedTypeInfo().getSuperType();
        if (superType instanceof CompoundModuleElementEx) {
            List<ConnectionElementEx> inherited = ((CompoundModuleElementEx) superType).getSrcConnectionsFor(submoduleName);
            if (!inherited.isEmpty()) {
                result = new ArrayList<ConnectionElementEx>(result); // make a modifiable copy
                result.addAll(inherited);
            }
        }
        return result;
    }

    /**
     * Returns the list of local plus inherited valid (see ConnectionElementEx.isValid()) connections,
     * where the given submodule is the destination module of the connection. Returns an empty list
     * if such submodule does not exist.
     */
    public List<ConnectionElementEx> getDestConnectionsFor(String submoduleName) {
        ensureConnectionCache();
        List<ConnectionElementEx> result = localDestConnCache.get(submoduleName); // immutable list
        if (result == null)
            result = EMPTY_CONN_LIST;

        // add inherited connections too, if we have any (uncommon)
        INedTypeElement superType = getNedTypeInfo().getSuperType();
        if (superType instanceof CompoundModuleElementEx) {
            List<ConnectionElementEx> inherited = ((CompoundModuleElementEx) superType).getDestConnectionsFor(submoduleName);
            if (!inherited.isEmpty()) {
                result = new ArrayList<ConnectionElementEx>(result); // make a modifiable copy
                result.addAll(inherited);
            }
        }
        return result;
    }

    /**
     * Returns the named connection with the provided name, excluding inherited connections.
     * Returns null if not found.
     */
    protected ConnectionElementEx getOwnConnectionByName(String connectionName) {
        return getNedTypeInfo().getLocalNamedConnections().get(connectionName);
    }

    /**
     * Returns the named connection (including inherited ones) with the provided name,
     * or null if not found.
     */
    public ConnectionElementEx getConnectionByName(String connectionName) {
        return getNedTypeInfo().getNamedConnections().get(connectionName);
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
            snode = (ConnectionsElement)NedElementFactoryEx.getInstance().createElement(getResolver(), NedElementFactoryEx.NED_CONNECTIONS, this);

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

    public INedTypeElement getSuperType() {
        return getNedTypeInfo().getSuperType();
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

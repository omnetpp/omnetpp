package org.omnetpp.ned.model.interfaces;

import java.util.List;

import org.omnetpp.ned.model.ex.ConnectionElementEx;

/**
 * Marker interface for elements that can be connected to each other (compound module and submodules)
 *
 * @author rhornig
 */
public interface IConnectableElement extends IHasName, IHasGates, IHasDisplayString {
    /**
     * Returns the typeinfo for the effective type.
     *
     * Returns null if the effective type is not filled in, or is not a valid NED type.
     */
    public INEDTypeInfo getNEDTypeInfo();

    /**
     * Returns ALL VALID connections contained in / and inherited by 
     * this module where this module is the source. 
     * 
     * IMPORTANT: cannot use this method to get the connections in a compound module
     * where this module is an inherited submodule! The returned list won't contain
     * connections added in the derived compound module.
     */
    public List<ConnectionElementEx> getSrcConnections();

    /**
     * Returns ALL VALID connections contained in / and inherited by 
     * this module where this module is the destination.
     * 
     * IMPORTANT: cannot use this method to get the connections in a compound module
     * where this module is an inherited submodule! The returned list won't contain
     * connections added in the derived compound module.
     */
    public List<ConnectionElementEx> getDestConnections();

}
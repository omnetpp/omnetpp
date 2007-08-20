package org.omnetpp.ned.model.interfaces;

/**
 * Marker interface for elements that can be connected to each other (compound module and submodules)
 *
 * @author rhornig
 */
//FIXME this is the same as INamedGraphNode -- eliminate one of them?
public interface IHasConnections extends IHasName {
    /**
     * Returns the typeinfo for the effective type.
     * 
     * Returns null if the effective type is not filled in, or is not a valid NED type. 
     */
    public INEDTypeInfo getNEDTypeInfo();

}
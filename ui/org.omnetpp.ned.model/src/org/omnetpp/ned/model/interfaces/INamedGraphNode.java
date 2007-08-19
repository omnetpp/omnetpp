package org.omnetpp.ned.model.interfaces;


/**
 * An element that has name, display string, can be connected (submodule and compound module)
 *
 * @author rhornig
 */
//XXX how about inventing an IHasTypeOrIsType with would be types+submodules+connections ? to hold getNEDTypeInfo()? 
public interface INamedGraphNode extends IHasDisplayString, IHasConnections, IHasName  {
    /**
     * Returns the typeinfo for the effective type.
     * 
     * Returns null if the effective type is not filled in, or is not a valid NED type. 
     */
    public INEDTypeInfo getNEDTypeInfo();

}

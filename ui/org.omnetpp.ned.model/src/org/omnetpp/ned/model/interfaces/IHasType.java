package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * Elements that are instance of a NED type: submodule and connection.
 *
 * @author rhornig
 */
public interface IHasType extends INEDElement {
    /**
     * Returns the type of the object
     */
    public String getType();

    /**
     * Sets the type attribute
     */
    public void setType(String type);

    /**
     * Returns the type info after the "like" keyword
     */
    public String getLikeType();

    /**
     * Sets the like-type
     */
    public void setLikeType(String type);

    /**
     * Returns the like parameter
     */
    public String getLikeParam();

    /**
     * Sets the like parameter
     */
    public void setLikeParam(String type);
    
    /**
     * Returns the type, or the likeType if type was not specified
     */
    public String getEffectiveType();

    /**
     * Returns the typeinfo for the effective type.
     * 
     * Returns null if the effective type is not filled in, or is not a valid NED type. 
     */
    public INEDTypeInfo getNEDTypeInfo();

    /**
     * Returns the model element for the effective type. Equivalent to 
     * getNEDTypeInfo().getNEDElement(), but handles nulls.
     * 
     * Returns null if the effective type is not filled in, or is not a valid NED type. 
     */
    public INedTypeNode getEffectiveTypeRef();
}

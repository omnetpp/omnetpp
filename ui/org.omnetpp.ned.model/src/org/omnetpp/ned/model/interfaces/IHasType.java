package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * Elements that have type-like property
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
     * Returns the typeinfo object of the TYPE of this component. That is, 
     * this method checks the type name of this element, and looks up the 
     * typeinfo object from that.
     * 
     * Returns null if the type is not filled in, or is not a valid NED type. 
     */
    public INEDTypeInfo getTypeNEDTypeInfo();

    /**
     * Returns the model element that represents the type of this element.
     * 
     * Returns null if the type is not filled in, or is not a valid NED type. 
     */
    public INedTypeNode getEffectiveTypeRef();
}

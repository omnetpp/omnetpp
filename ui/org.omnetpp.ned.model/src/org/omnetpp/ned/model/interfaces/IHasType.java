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
     * Returns the type info after the like keyword
     */
    public String getLikeType();

    /**
     * Sets the like type
     * @param type
     */
    public void setLikeType(String type);

    /**
     * Returns the like-param of the implemented interface
     */
    public String getLikeParam();

    /**
     * Sets the like parameter
     * @param type
     */
    public void setLikeParam(String type);
    /**
     * Returns the type or the likeType if type was not specified
     */
    public String getEffectiveType();

    /**
     * @param type
     */
    public void setType(String type);

    /**
     * Returns the typeinfo object of the TYPE of this component. That is, 
     * this method checks the typename of this element, and looks up the 
     * typeinfo object from that.
     */
    public INEDTypeInfo getTypeNEDTypeInfo();

    /**
     * Returns the model element that represents the type of this element
     */
    public INedTypeNode getEffectiveTypeRef();
}

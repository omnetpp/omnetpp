package org.omnetpp.ned.model.interfaces;

import org.omnetpp.ned.model.INEDElement;

/**
 * @author rhornig
 * Elements that have type-like property
 */
public interface IHasType {
    /**
     * @return The type of the object
     */
    public String getType();

    /**
     * @return The type info after the like keyword
     */
    public String getLikeType();

    /**
     * Setes the like type
     * @param type
     */
    public void setLikeType(String type);

    /**
     * @return The like-param of the implemented if
     */
    public String getLikeParam();

    /**
     * Sets the like parameter
     * @param type
     */
    public void setLikeParam(String type);
    /**
     * @return The type or the likeType if type was not specified
     */
    public String getEffectiveType();

    /**
     * @param type
     */
    public void setType(String type);

    /**
     * @return The TRypeInfo object of the TYPE of this component. ie this method checks the typename
     *         of this element and looks up the typeinfo object to that
     */
    public INEDTypeInfo getTypeNEDTypeInfo();

    /**
     * @return The model element that represents the type of this element
     */
    public INEDElement getEffectiveTypeRef();
}

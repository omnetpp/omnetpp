package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Elements that have type-like property
 */
public interface IStringTyped {
    /**
     * @return The type of the object
     */
    public String getType();

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
    public NEDElement getTypeRef();
}

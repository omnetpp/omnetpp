package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Objects that can extend other objects, ie they are derived objects.
 */
public interface IDerived {
    
    /**
     * 
     * @return The base object's name
     */
    public String getExtends();
    
    /**
     * @param ext The object's name that is extended
     */
    public void setExtends(String ext);

    /**
     * @return The TTypeInfo object of the base object of this component. ie this method checks the base type
     *         of this element and looks up the typeinfo object to that  
     */
    public ITypeInfo getExtendsTypeInfo();
    
    /**
     * @return The model element that represents the base object of this element
     */
    public NEDElement getExtendsRef();
}

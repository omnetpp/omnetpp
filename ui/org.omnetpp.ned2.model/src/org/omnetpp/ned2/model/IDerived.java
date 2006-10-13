package org.omnetpp.ned2.model;

/**
 * @author rhornig
 * Objects that can extend other objects, ie they are derived objects.
 */
public interface IDerived {
    
    /**
     * 
     * @return The base object's name (ONLY the first extends node name returned)
     */
    public String getFirstExtends();
    
    /**
     * @param ext The object's name that is extended
     */
    public void setFirstExtends(String ext);

    /**
     * @return The TTypeInfo object of the base object of this component. ie this method checks the base type
     *         of this element and looks up the typeinfo object to that. NOTE that this check only the
     *         FIRST extends node, so it returns the first extends child for ModuleInterface and ChannelInterface
     *         (Special handling needed for these types)  
     */
    public INEDTypeInfo getFirstExtendsNEDTypeInfo();
    
    /**
     * @return The model element that represents the base object of this element
     *         NOTE that this check only the FIRST extends node, so it returns the first 
     *         extends child for ModuleInterface and ChannelInterface
     *         (Special handling needed for these types)  
     */
    public NEDElement getFirstExtendsRef();
}

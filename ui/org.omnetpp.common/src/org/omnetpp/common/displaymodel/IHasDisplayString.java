package org.omnetpp.common.displaymodel;




/**
 * @author rhornig
 * Interface for model objects that provide a display string (ie has a visual appearence)
 */
public interface IHasDisplayString extends IDisplayStringChangeListener {

    /**
     * Returns a display string object, which can be used to manipulate the properties
     * any change in the returned DisplayString object (is calling the set(...) method)
     * causes displayStringChanged() notification. 
     */
    public DisplayString getDisplayString();
    
    /**
     * @return The displaystring object, but sets it's default handling in a way that the
     * base object's display properties are used as defaults. ie. the display properties are inherited.
     * either from the base object (extends) or the type (for submodules and connections)
     */
    public DisplayString getEffectiveDisplayString();

}
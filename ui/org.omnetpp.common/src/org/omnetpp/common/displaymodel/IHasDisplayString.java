package org.omnetpp.common.displaymodel;


/**
 * Interface for NED model objects that provide a display string (i.e. have a visual appearance).
 * @author rhornig
 */
public interface IHasDisplayString extends IDisplayStringChangeListener {

    /**
	 * Returns a display string object, which can be used to manipulate the
	 * properties. Any change in the returned DisplayString object (e.g. calling
	 * a set(...) method) causes displayStringChanged() notification.
	 */
    public DisplayString getDisplayString();
    
    /**
	 * Returns the display string object, but sets its default handling in a way
	 * that the base object's display properties are used as defaults. That is,
	 * display properties are inherited, either from the base object (extends)
	 * or the type (for submodules and connections).
	 */
    public DisplayString getEffectiveDisplayString();

}
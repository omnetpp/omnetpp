package org.omnetpp.ned.model.interfaces;


/**
 * Common interface for SimpleModuleElementEx and CompoundModuleElementEx.
 *
 * @author Andras
 */
public interface IModuleTypeElement extends IModuleKindTypeElement {
	public static String IS_NETWORK_PROPERTY = "isNetwork";
    /**
     * Maps to the @isNetwork property. True if the given element has an isNetwork property with default value ie. @isNetwork or @isNetwork(true)
     * false if the property is missing or @isNetwork(false). Otherwise the value is unspecified  
     */
    boolean isNetwork();

    /**
     * Adds/removes the @isNetwork property.
     */
    void setIsNetwork(boolean value);
}

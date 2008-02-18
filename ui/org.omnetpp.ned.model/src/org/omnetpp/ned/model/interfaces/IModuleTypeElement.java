package org.omnetpp.ned.model.interfaces;


/**
 * Common interface for SimpleModuleElementEx and CompoundModuleElementEx.
 *
 * @author Andras
 */
public interface IModuleTypeElement extends IModuleKindTypeElement {
    /**
     * Maps to the @isNetwork property.
     */
    boolean isNetwork();

    /**
     * Adds/removes the @isNetwork property.
     */
    void setIsNetwork(boolean value);
}

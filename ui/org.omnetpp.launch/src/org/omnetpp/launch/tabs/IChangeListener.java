package org.omnetpp.launch.tabs;


/**
 * Allows a notification of content change from a tab block.
 */
interface IChangeListener  
{
    /**
     * Should be called whenever anything has changed in the block
     */
    public void widgetChanged(); 
}

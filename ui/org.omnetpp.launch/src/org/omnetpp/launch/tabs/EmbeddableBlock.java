package org.omnetpp.launch.tabs;


/**
 * A block of controls that can be embedded in a parent tab. If widget listeners added to the notification
 * will be redirected to the parent tab (if this block has a parent). Also error messages are redirected to parent
 *
 * @author rhornig
 */
interface EmbeddableBlock  
{
    /**
     * Should be called whenever anything has changed on the block
     */
    public void widgetChanged(); 
}

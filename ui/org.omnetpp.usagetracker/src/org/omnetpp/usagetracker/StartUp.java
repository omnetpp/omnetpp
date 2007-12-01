package org.omnetpp.usagetracker;
import org.eclipse.ui.IStartup;

/**
 * Forces activation of the plugin on platform startup.
 * @author Andras
 */
public class StartUp implements IStartup {
    
    public void earlyStartup() {
        // nothing to do -- all work is done in Activator.start()
    }
}

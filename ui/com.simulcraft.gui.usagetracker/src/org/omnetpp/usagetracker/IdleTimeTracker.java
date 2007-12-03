package org.omnetpp.usagetracker;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.util.DisplayUtils;

/**
 * Records user idle periods into the log.
 *   
 * @author Andras
 */
public class IdleTimeTracker implements Listener {
    private static final int[] eventTypes = { SWT.KeyDown, SWT.MouseDown };
    // state
    protected long startActivityTimeMillis;
    protected long lastActivityTimeMillis;
    protected long idleThresholdSeconds = 60;
    
    public IdleTimeTracker() {
    }
    
    public void hookListeners() {
        // We must ensure our listener runs before the key binding service. Otherwise we'll miss all hotkeys.
        for (int eventType : eventTypes)
            DisplayUtils.addAsFirstFilter(eventType, this);
        startActivityTimeMillis = lastActivityTimeMillis = System.currentTimeMillis();
    }

    public void unhookListeners() {
        for (int eventType : eventTypes)
            Display.getCurrent().removeFilter(eventType, this);
    }

    public void handleEvent(final Event e) {
        long currentTimeMillis = System.currentTimeMillis();
        if (currentTimeMillis - lastActivityTimeMillis > 1000*idleThresholdSeconds) {
            //Activator.logMessage("User activity for " + (lastActivityTimeMillis-startActivityTimeMillis)/1000 + "+ seconds");
            Activator.logMessage("Active again after idling for about " + (currentTimeMillis - lastActivityTimeMillis)/1000 + " seconds");
            startActivityTimeMillis = currentTimeMillis;
        }
        lastActivityTimeMillis = currentTimeMillis;
    }
}


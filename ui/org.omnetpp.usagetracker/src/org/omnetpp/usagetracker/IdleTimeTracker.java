package org.omnetpp.usagetracker;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.omnetpp.common.util.ReflectionUtils;

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
            addAsFirstFilter(Display.getCurrent(), eventType, this);
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

    //FIXME copy of GUIRecorder's similar function -- make it common
    private static void addAsFirstFilter(Display display, int eventType, Listener listener) {
        // first, make room in the table (this adds the listener at the end of the table)
        display.addFilter(eventType, listener);
        
        // shift everything up, then add our listener to slot 0. Sorry about the reflection, 
        // there seem to be no other way 
        Object filterTable = ReflectionUtils.getFieldValue(display, "filterTable");
        int[] types = (int[]) ReflectionUtils.getFieldValue(filterTable, "types");
        Listener[] listeners = (Listener[]) ReflectionUtils.getFieldValue(filterTable, "listeners");
        System.arraycopy(types, 0, types, 1, types.length-1);
        System.arraycopy(listeners, 0, listeners, 1, listeners.length-1);
        types[0] = eventType;
        listeners[0] = listener;
        for (int i = 1; i < types.length; i++)
            if (types[i] == eventType && listeners[i] == listener) {
                types[i] = 0; listeners[i] = null; break; // remove instance added first
            }
    }

}


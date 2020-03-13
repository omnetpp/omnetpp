/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;


/**
 * Supports a listener list used for change and structural notification in a INedElement tree
 *
 * @author rhornig
 */
public class NedChangeListenerList extends ListenerList<INedChangeListener> {
    protected boolean enabled = true;

    /**
     * Returns whether change notifications are enabled
     */
    public boolean isEnabled() {
        return enabled;
    }

    /**
     * Enable or disable change notifications; returns the original notification
     * state before the call.
     */
    public boolean setEnabled(boolean notifyEnabled) {
        boolean oldState = this.enabled;
        this.enabled = notifyEnabled;
        return oldState;
    }

    /**
     * If change notifications are enabled, fires a model change event
     * to all listeners; otherwise it does nothing.
     */
    public void fireModelChanged(final NedModelEvent event) {
        if (enabled) {
            for (INedChangeListener listener : this) {
                SafeRunner.run(new ISafeRunnable() {
                    public void handleException(Throwable e) {
                        // exception logged in SafeRunner#run
                    }
                    public void run() throws Exception {
                        listener.modelChanged(event);
                    }
                });
            }
        }
    }
}

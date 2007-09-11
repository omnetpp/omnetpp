package org.omnetpp.ned.model.notification;

import org.eclipse.core.runtime.ISafeRunnable;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.SafeRunner;


/**
 * Supports a listener list used for change and structural notification in a INEDElement tree
 *
 * @author rhornig
 */
public class NEDChangeListenerList extends ListenerList {
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
    public void fireModelChanged(final NEDModelEvent event) {
        if (enabled) {
            for (final Object listener : getListeners()) {
				SafeRunner.run(new ISafeRunnable() {
					public void handleException(Throwable e) {
						// exception logged in SafeRunner#run
					}
					public void run() throws Exception {
						((INEDChangeListener)listener).modelChanged(event);
					}
				});
            }
        }
    }
}

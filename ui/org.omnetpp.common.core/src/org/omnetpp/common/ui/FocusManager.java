package org.omnetpp.common.ui;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;

/**
 * Keeps track of the last focused control inside a Composite (or among an arbitrary
 * set of controls), so that the old focus can be restored when this Composite loses
 * and then re-gains focus.
 *
 * The default behavior of Composite's setFocus() is to give the focus to the first
 * child control that accepts it, which is often not good enough from a usability perspective.
 *
 * If you use a FocusManager for a Composite, it is recommended that you override
 * the setFocus() method of the Composite to call FocusManager's setFocus(), like this:
 *
 * <pre>
 * public boolean setFocus() {
 *     if (focusManager.setFocus())
 *         return true;
 *     return super.setFocus();
 * }
 * </pre>
 *
 * @author Andras
 */
public class FocusManager {

    private Control lastFocusControl = null;

    private Listener listener = new Listener() {
        public void handleEvent(Event e) {
            // note: no "instanceOf" checks because we only hook this listener on Controls
            if (e.type == SWT.FocusOut)
                controlLostFocus((Control)e.widget);
            else if (e.type == SWT.Dispose)
                controlDisposed((Control)e.widget);
        }
    };

    /**
     * Creates an empty FocusManager
     */
    public FocusManager() {
    }

    /**
     * Create a FocusManager that covers all Controls under this Composite.
     * See addAllControls().
     */
    public FocusManager(Composite composite) {
        addAllControls(composite);
    }

    /**
     * Adds all Controls under the given Composite. Can be called multiple times
     * to cover controls created since the last call.
     *
     * To help with nested FocusManagers, a Composite might implement IHasFocusManager,
     * and then this focus manager will not add listeners to the controls in it.
     * It is then expected that that composite has setFocus() overridden to use its
     * own focus manager.
     */
    public void addAllControls(Composite composite) {
        for (Control control : composite.getChildren()) {
            add(control); // even if it's a Composite! many widgets subclass from Composite (Combo, Table, Tree...)
            if (control instanceof Composite && !(control instanceof IHasFocusManager))
                addAllControls((Composite)control);
        }
    }

    public void add(Control control) {
        // note: FocusIn listener has been tried and it is NOT good for our purposes!
        // (because TabFolder widgets etc also contain setFocus(), fixFocus(), setFixedfocus()
        // and similar calls that interfere with us)
        addListenerIfNotAlreadyAdded(control, SWT.FocusOut, listener);
        addListenerIfNotAlreadyAdded(control, SWT.Dispose, listener);
    }

    public void forgetAllControls(Composite composite) {
        for (Control control : composite.getChildren()) {
            forget(control);
            if (control instanceof Composite)
                forgetAllControls((Composite)control);
        }
    }

    public void forget(Control control) {
        control.removeListener(SWT.FocusOut, listener);
        control.removeListener(SWT.Dispose, listener);
        if (lastFocusControl == control)
            lastFocusControl = null;
    }

    /**
     * Restores the focus to the last focused control, using its setFocus()
     * method. Return value is that of setFocus(), i.e. true if successful.
     */
    public boolean setFocus() {
        if (lastFocusControl != null) {
            //Debug.println("FocusManager: restoring focus to " + lastFocusControl);
            Assert.isTrue(!lastFocusControl.isDisposed()); // we have a disposeListener
            return lastFocusControl.setFocus();
        }
        else {
            return false; // sorry
        }
    }

    /**
     * Note: this method is not as useful as you think, because value will be
     * overwritten when the currently focused widget *loses* focus.
     */
    public void setLastFocusControl(Control lastFocusControl) {
        this.lastFocusControl = lastFocusControl;
    }

    /**
     * Note: this is usually not the currently focused widget but the one focused
     * before that, as value is updated when the currently focused widget *loses* focus.
     */
    public Control getLastFocusControl() {
        return lastFocusControl;
    }

    protected void controlLostFocus(Control control) {
        //Debug.println("FocusManager: storing last focused widget: " + control);
        lastFocusControl = control;
    }

    protected void controlDisposed(Control control) {
        forget(control);
    }

    protected void addListenerIfNotAlreadyAdded(Control control, int eventType, Listener listener) {
        Listener[] listeners = control.getListeners(eventType);
        if (!ArrayUtils.contains(listeners, listener))
            control.addListener(eventType, listener);
    }
}

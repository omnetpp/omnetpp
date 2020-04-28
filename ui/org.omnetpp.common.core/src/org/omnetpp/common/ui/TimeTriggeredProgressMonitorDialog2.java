/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.ui;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonCorePlugin;
import org.omnetpp.scave.engine.InterruptedFlag;

/**
 * TimeTriggeredProgressMonitorDialog2 is a progress monitor dialog that only
 * opens if the runnable provided exceeds the specified long operation time.
 *
 * Functionality is similar to org.eclipse.ui.internal.operations.TimeTriggeredProgressMonitorDialog
 * (and its clones in other packages), with the change that this one ACTUALLY WORKS.
 *
 * With the original TimeTriggeredProgressMonitorDialog, the dialog often comes up
 * too late (if ever), no matter the time limit given. Namely, if the task does not report
 * progress via its progress monitor, the dialog NEVER comes up. That's because checking
 * the time is implemented in the methods of a wrapped IProgressMonitor.
 *
 * This implementation uses a different approach, Display.timerExec(). This not only
 * works better but it is also significantly less code.
 *
 * In addition, the class provides utility methods (runWithDialog() and variants)
 * for convenience.
 *
 * In addition to IProgressMonitor.isCanceled(), cancellation is also signaled via
 * an InterruptedFlag instance. Use the following expression to access the relevant
 * InterruptedFlag instance:
 *
 * <pre>
 * TimeTriggeredProgressMonitorDialog2.getActiveInstance().getInterruptedFlag()
 * </pre>
 *
 * @author andras
 */
public class TimeTriggeredProgressMonitorDialog2 extends ProgressMonitorDialog {
    public static final int DEFAULT_DELAY_MILLIS = 3000;

    /**
     * The time considered to be the long operation time.
     */
    private int delayMillis;

    /**
     * This flag will also be set when the Cancel button is clicked.
     */
    private InterruptedFlag interruptedFlag = new InterruptedFlag();

    /**
     * The currently running instance
     */
    private static TimeTriggeredProgressMonitorDialog2 activeInstance;

    /**
     * Create a new instance of the receiver.
     *
     * @param parent
     *            the parent of the dialog
     * @param delayMillis
     *            the time (in milliseconds) considered to be a long enough
     *            execution time to warrant opening a dialog.
     */
    public TimeTriggeredProgressMonitorDialog2(Shell parent, int delayMillis) {
        super(parent);
        setOpenOnRun(false);
        this.delayMillis = delayMillis;
    }

    @Override
    protected void cancelPressed() {
        super.cancelPressed();
        interruptedFlag.setFlag(true);
    }

    /**
     * Allows the runnable to access the InterruptedFlag of the active dialog.
     */
    public static TimeTriggeredProgressMonitorDialog2 getActiveInstance() {
        return activeInstance;
    }

    public InterruptedFlag getInterruptedFlag() {
        return interruptedFlag;
    }

    @Override
    public void run(boolean fork, final boolean cancelable, final IRunnableWithProgress runnable) throws InvocationTargetException, InterruptedException {
        Assert.isTrue(activeInstance == null); // no nesting
        try {
            activeInstance = this;

            Display display = Display.getCurrent();
            if (display == null) {
                runnable.run(new NullProgressMonitor());
                return;
            }

            Runnable openDialogRunnable = () -> open();
            display.timerExec(delayMillis, openDialogRunnable);

            InvocationTargetException[] invokes = new InvocationTargetException[1];
            InterruptedException[] interrupt = new InterruptedException[1];
            BusyIndicator.showWhile(display, () -> {
                try {
                    TimeTriggeredProgressMonitorDialog2.super.run(fork, cancelable, runnable);
                } catch (InvocationTargetException e) {
                    invokes[0] = e;
                } catch (InterruptedException e) {
                    interrupt[0]= e;
                }
            });

            if (!display.isDisposed())
                display.timerExec(-1, openDialogRunnable);

            if (invokes[0] != null)
                throw invokes[0];
            if (interrupt[0] != null)
                throw interrupt[0];

        }
        finally {
            activeInstance = null;
        }
     }

    /**
     * Utility function to run UI code in the given runnable, with a delayed
     * progress dialog. Returns true on success, and false on error and
     * cancellation.
     *
     * For the dialog to paint and stay responsive, the runnable needs to
     * periodically do the following:
     *
     * <code>while (Display.getCurrent().readAndDispatch());</code>
     */
    public static boolean runWithDialogInUIThread(String what, IRunnableWithProgress runnable) {
        return runWithDialog(what, Display.getCurrent().getActiveShell(), DEFAULT_DELAY_MILLIS, false, runnable);
    }

    /**
     * Utility function to run the given runnable in a background thread, with a
     * delayed progress dialog. Returns true on success, and false on error and
     * cancellation.
     *
     * This method cannot be used with UI code which needs to run in the UI thread;
     * use runWithDialogInUIThread() for that.
     */
    public static boolean runWithDialog(String what, IRunnableWithProgress runnable) {
        return runWithDialog(what, Display.getCurrent().getActiveShell(), DEFAULT_DELAY_MILLIS, true, runnable);
    }

    /**
     * Utility function to run the given runnable with a delayed progress dialog.
     * Returns true on success, and false on error and cancellation.
     */
    public static boolean runWithDialog(String what, Shell parent, int delayMillis, boolean fork, IRunnableWithProgress runnable) {
        try {
            TimeTriggeredProgressMonitorDialog2 dialog = new TimeTriggeredProgressMonitorDialog2(parent, delayMillis);
            dialog.run(fork, true, (monitor) -> {
                runnable.run(monitor);
            });
            return true;
        }
        catch (InterruptedException e) {
            return false;
        }
        catch (InvocationTargetException e) {
            Throwable ee = e.getCause();
            if (ee.getCause() instanceof InterruptedException)
                return false;
            CommonCorePlugin.logError(ee);
            MessageDialog.openError(parent, "Error", what + " failed: " + ee.getMessage());
            return false;
        }
    }
}

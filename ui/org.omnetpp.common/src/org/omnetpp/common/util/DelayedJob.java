package org.omnetpp.common.util;

import org.eclipse.swt.widgets.Display;

/**
 * Run the given job after x milliseconds, but every restart() call 
 * will re-start the countdown, the net effect being that the job
 * gets run after the first "idle" period of x milliseconds (idle 
 * meaning free of restart() calls). Can be used to implement
 * reconciler-style jobs, like lazy updating of views.
 * 
 * @author Andras
 */
abstract public class DelayedJob implements Runnable {
	private int delayMillis;
	
	private boolean scheduled;

	/**
	 * Creates the object, but does not start the timer yet.
	 */
	public DelayedJob(int delayMillis) {
		this.delayMillis = delayMillis;
	}
	
	public boolean isScheduled() {
        return scheduled;
    }

	/**
	 * Starts or re-starts the timer.
	 */
	public void restartTimer() {
		// Alas, timerExec() may only be invoked from the UI thread 
		if (Display.getCurrent() != null) {
			Display.getDefault().timerExec(-1, this);
			Display.getDefault().timerExec(delayMillis, this);
			scheduled = true;
		}
		else {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					restartTimer(); // try again from the UI thread
				}
			});
		}
	}

	/**
	 * Cancels the currently running timer.
	 */
	public void cancel() {
		// Alas, timerExec() may only be invoked from the UI thread 
		if (Display.getCurrent() != null) {
			Display.getDefault().timerExec(-1, this);
			scheduled = false;
		}
		else {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					cancel(); // try again from the UI thread
				}
			});
		}
	}
    
    /**
     * Runs the job immediately
     */
    public void runNow() {
        cancel();
        run();
    }
}

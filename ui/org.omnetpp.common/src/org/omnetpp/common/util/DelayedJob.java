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
public class DelayedJob {
	private Runnable runnable;
	private int delayMillis;

	/**
	 * Creates the object, but does not start the timer yet.
	 */
	public DelayedJob(int delayMillis, Runnable runnable) {
		this.delayMillis = delayMillis;
		this.runnable = runnable;
	}

	/**
	 * Starts or re-starts the timer.
	 */
	public void restartTimer() {
		// Alas, timerExec() may only be invoked from the UI thread 
		if (Display.getCurrent() != null) {
			Display.getDefault().timerExec(-1, runnable);
			Display.getDefault().timerExec(delayMillis, runnable);
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
			Display.getDefault().timerExec(-1, runnable);
		}
		else {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					cancel(); // try again from the UI thread
				}
			});
		}
	}
}

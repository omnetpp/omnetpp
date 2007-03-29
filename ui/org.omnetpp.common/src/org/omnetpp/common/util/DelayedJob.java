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
		Display.getDefault().timerExec(-1, runnable);
		Display.getDefault().timerExec(delayMillis, runnable);
	}

	/**
	 * Cancels currently running timer.
	 */
	public void cancel() {
		Display.getDefault().timerExec(-1, runnable);
	}
}

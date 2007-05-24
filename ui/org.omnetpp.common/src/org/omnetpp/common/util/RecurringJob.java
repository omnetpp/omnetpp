package org.omnetpp.common.util;

import org.eclipse.swt.widgets.Display;

/**
 * Run the given job every x milliseconds.
 */
abstract public class RecurringJob implements Runnable {
	private int delayMillis;

	/**
	 * Creates the object, but does not start the timer yet.
	 */
	public RecurringJob(int delayMillis) {
		this.delayMillis = delayMillis;
	}

	/**
	 * Starts the timer.
	 */
	public void start() {
		// Alas, timerExec() may only be invoked from the UI thread 
		if (Display.getCurrent() != null) {
			Display.getDefault().timerExec(-1, this);
			Display.getDefault().timerExec(delayMillis, new Runnable() {
				public void run() {
					start();
					RecurringJob.this.run();
				}
			});
		}
		else {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					start(); // try again from the UI thread
				}
			});
		}
	}

	/**
	 * Stops the currently running timer.
	 */
	public void stop() {
		// Alas, timerExec() may only be invoked from the UI thread 
		if (Display.getCurrent() != null) {
			Display.getDefault().timerExec(-1, this);
		}
		else {
			Display.getDefault().asyncExec(new Runnable() {
				public void run() {
					stop(); // try again from the UI thread
				}
			});
		}
	}
}

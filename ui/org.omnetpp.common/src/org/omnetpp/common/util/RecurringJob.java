/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import org.eclipse.swt.widgets.Display;

/**
 * Run the given job every x milliseconds.
 */
abstract public class RecurringJob implements Runnable {
    private int delayMillis;

    private Runnable internalRunnable;

    /**
     * Creates the object, but does not start the timer yet.
     */
    public RecurringJob(int delayMillis) {
        this.delayMillis = delayMillis;
        this.internalRunnable = new Runnable() {
            public void run() {
                start();
                RecurringJob.this.run();
            }
        };
    }

    /**
     * Starts the timer.
     */
    public void start() {
        // Alas, timerExec() may only be invoked from the UI thread
        if (Display.getCurrent() != null) {
            Display.getDefault().timerExec(-1, internalRunnable);
            Display.getDefault().timerExec(delayMillis, internalRunnable);
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
            Display.getDefault().timerExec(-1, internalRunnable);
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

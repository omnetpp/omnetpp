/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.util.ArrayList;

import org.eclipse.swt.widgets.Display;

/**
 * Manages a list of timers and executes them using timerExec.
 * This class is not thread safe and meant to be called from the GUI thread only.
 */
public class TimerQueue implements Runnable {
    // TODO: use log4j or something similar
    private static boolean debug = false;

    /**
     * Indicates whether this timer queue is active and executing its timers.
     */
    private boolean running;

    /**
     * A list of timers to execute. Timers are ordered based on their next execution time.
     */
    private ArrayList<Timer> timers = new ArrayList<Timer>();

    /**
     * The last timer scheduled to execute.
     */
    private Timer lastScheduledTimer;

    /**
     * The execution time when the last scheduled timer will be executed.
     */
    private long lastScheduledNextExecutionTime;

    public Timer[] getTimers() {
        return timers.toArray(new Timer[0]);
    }

    public void addTimer(Timer timer) {
        if (debug)
            System.out.println("Adding timer: " + timer);

        timers.add(timer);

        if (running)
            scheduleTimer();
    }

    public void removeTimer(Timer timer) {
        if (debug)
            System.out.println("Removing timer: " + timer);

        Timer firstTimer = getFirstTimer();
        timers.remove(timer);

        if (running && timer == firstTimer)
            scheduleTimer();
    }

    public boolean hasTimer(Timer timer) {
        return timers.contains(timer);
    }

    public void resetTimer(Timer timer) {
        if (debug)
            System.out.println("Resetting timer: " + timer);

        timer.reset();

        if (running)
            scheduleTimer();
    }

    public void start() {
        if (debug)
            System.out.println("Starting timer queue");

        running = true;
        scheduleTimer();
    }

    public void stop() {
        if (debug)
            System.out.println("Stopping timer queue");

        running = false;
        Display.getDefault().timerExec(-1, this);
    }

    public boolean isRunning() {
        return running;
    }

    public void run() {
        if (running) {
            lastScheduledTimer = null;
            lastScheduledNextExecutionTime = -1;

            runFirstTimer();
            scheduleTimer();
        }
    }

    private void runFirstTimer() {
        Timer firstTimer = getFirstTimer();

        if (firstTimer != null) {
            if (debug)
                System.out.println("Running timer: " + firstTimer);

            firstTimer.execute();

            if (!firstTimer.isRecurring())
                timers.remove(firstTimer);
        }
    }

    private void scheduleTimer() {
        if (timers.size() != 0) {
            sortTimers();
            Timer firstTimer = getFirstTimer();
            long currentTime = System.currentTimeMillis();
            long nextExecutionTime = firstTimer.getNextExecutionTime();
            long nextExecutionSleepTime = nextExecutionTime - currentTime;

            if (firstTimer != lastScheduledTimer ||
                firstTimer.getNextExecutionTime() != lastScheduledNextExecutionTime ||
                nextExecutionSleepTime < 0)
            {
                if (debug)
                    System.out.println("Scheduling timer: " + firstTimer + " at: " + currentTime + " to: " + firstTimer.getNextExecutionTime());

                if (nextExecutionSleepTime < 0) {
                    if (firstTimer.isAllowSkipping())
                        firstTimer.calculateNextExecution();
                    else
                        runFirstTimer();

                    scheduleTimer();
                }
                else {
                    Display.getDefault().timerExec((int)nextExecutionSleepTime, this);

                    lastScheduledTimer = firstTimer;
                    lastScheduledNextExecutionTime = nextExecutionTime;
                }
            }
        }
    }

    private Timer getFirstTimer() {
        return timers.size() != 0 ? timers.get(0) : null;
    }

    private void sortTimers() {
        java.util.Collections.sort(timers);
    }
}

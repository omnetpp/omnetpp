/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.util.PriorityQueue;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;

/**
 * Manages a list of timers and executes them using timerExec. This class is not
 * thread safe and meant to be called from the GUI thread only.
 */
public class TimerQueue implements Runnable {
    private static boolean debug = false;

    /**
     * Specifies whether this timer queue is currently active and as such it is
     * executing its timers.
     */
    private boolean isRunning;

    /**
     * Specifies whether a timer is currently executing.
     */
    private boolean isExecuting;

    /**
     * A list of timers to execute. Timers are ordered based on their next
     * execution time.
     */
    private PriorityQueue<Timer> timers;

    /**
     * The last timer that was scheduled to execute.
     */
    private Timer lastScheduledTimer;

    /**
     * The execution time when the last scheduled timer will be executed.
     */
    private long lastScheduledNextExecutionTime;

    public TimerQueue() {
        this.timers = new PriorityQueue<Timer>();
    }

    public Iterable<Timer> getTimers() {
        return timers;
    }

    public int getTimerCount() {
        return timers.size();
    }

    public void addTimer(Timer timer) {
        if (debug)
            Debug.println("Adding timer: " + timer);
        timers.add(timer);
        if (isRunning && !isExecuting)
            scheduleNextTimer();
    }

    public void removeTimer(Timer timer) {
        if (debug)
            Debug.println("Removing timer: " + timer);
        Timer firstTimer = timers.peek();
        timers.remove(timer);
        if (isRunning && timer == firstTimer && !isExecuting)
            scheduleNextTimer();
    }

    public void removeAllTimers() {
        if (debug)
            System.out.println("Removing all timers");
        timers.clear();
        Display.getDefault().timerExec(-1, this);
    }

    public boolean hasTimer(Timer timer) {
        return timers.contains(timer);
    }

    public void resetTimer(Timer timer) {
        if (debug)
            Debug.println("Resetting timer: " + timer);
        timer.reset();
        if (isRunning && !isExecuting)
            scheduleNextTimer();
    }

    public void start() {
        Assert.isTrue(!isRunning);
        if (debug)
            Debug.println("Starting timer execution");

        isRunning = true;
        scheduleNextTimer();
    }

    public void stop() {
        Assert.isTrue(isRunning);
        if (debug)
            Debug.println("Stopping timer execution");
        isRunning = false;
        Display.getDefault().timerExec(-1, this);
    }

    public boolean isRunning() {
        return isRunning;
    }

    /**
     * Display's timerExec will call this function when the first timer is to be
     * executed.
     */
    public void run() {
        if (isRunning) {
            lastScheduledTimer = null;
            lastScheduledNextExecutionTime = -1;
            Timer firstTimer = timers.peek();
            if (firstTimer != null)
                executeTimer(firstTimer);
            scheduleNextTimer();
        }
    }

    private Timer getFirstTimer() {
        return timers.size() != 0 ? timers.peek() : null;
    }

    private void runFirstTimer() {
        Timer firstTimer = getFirstTimer();

        if (firstTimer != null) {
            if (debug)
                Debug.println("Running timer: " + firstTimer);

            firstTimer.execute();

            if (!firstTimer.isRecurring())
                timers.remove(firstTimer);
        }
    }

/**
 * Schedules the next timer with Display's timerExec. All timers that are
 * already expired and don't allow skipping execution will be executed
 * first.
 */
    private void scheduleNextTimer() {
        if (debug)
            System.out.println("Searching for next timer at: " + System.currentTimeMillis() + " from " + timers.size() + " timers");
        while (timers.size() != 0) {
            Timer firstTimer = getFirstTimer();
            long currentTime = System.currentTimeMillis();
            long nextExecutionTime = firstTimer.getNextExecutionTime();
            long nextExecutionSleepTime = nextExecutionTime - currentTime;
            Assert.isTrue(nextExecutionTime != -1);
            if (firstTimer != lastScheduledTimer ||
                firstTimer.getNextExecutionTime() != lastScheduledNextExecutionTime ||
                nextExecutionSleepTime < 0)
            {
                if (nextExecutionSleepTime < 0) {
                    if (debug)
                        System.out.println("Timer: " + firstTimer + " already expired at: " + currentTime + " was scheduled to: " + firstTimer.getNextExecutionTime());
                    if (firstTimer.isAllowSkipping())
                        recurTimer(firstTimer);
                    else
                        executeTimer(firstTimer);
                }
                else {
                    if (debug)
                        System.out.println("Scheduling timer: " + firstTimer + " at: " + currentTime + " to: " + firstTimer.getNextExecutionTime());
                    Display.getDefault().timerExec((int)nextExecutionSleepTime, this);
                    lastScheduledTimer = firstTimer;
                    lastScheduledNextExecutionTime = nextExecutionTime;
                    break;
                }
            }
        }
    }

    /**
     * Executes the given timer immediately.
     */
    private void executeTimer(Timer timer) {
        try {
            if (debug)
                System.out.println("Executing timer: " + timer);
            isExecuting = true;
            timer.execute();
            recurTimer(timer);
        }
        finally {
            isExecuting = false;
        }
    }

    /**
     * Either removes the timer or calculates the next execution time.
     */
    private void recurTimer(Timer timer) {
        // NOTE: may have been removed and must be properly repositioned
        if (timers.contains(timer)) {
            timers.remove(timer);
            if (timer.isRecurring()) {
                timer.calculateNextExecution();
                timers.add(timer);
            }
        }
    }
}

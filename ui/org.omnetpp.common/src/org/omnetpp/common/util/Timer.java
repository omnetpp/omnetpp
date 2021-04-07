/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

/**
 * Represents a one-time or recurring task that is to be executed in the future.
 */
public abstract class Timer implements Comparable<Timer>, Runnable {
    /**
     * Specifies if the timer should be executed periodically or only once.
     */
    protected boolean isRecurring;

    /**
     * Specifies if it is allowed to skip timer executions. This happens when
     * the execution time has already passed due to system load.
     */
    protected boolean isAllowSkipping;

    /**
     * For a recurring timer it is the time between subsequent executions
     * starting from the time when the timer was created. For other timers it is
     * the time between the timer is created and the only one execution.
     */
    protected long sleepTime;

    /**
     * The time when this timer was created.
     */
    protected long creationTime;

    /**
     * The first time this timer should be executed.
     */
    protected long firstExecutionTime;

    /**
     * The time when this timer was first actually executed.
     */
    protected long firstActualExecutionTime = -1;

    /**
     * The next time when this timer should be executed.
     */
    protected long nextExecutionTime = -1;

    /**
     * The last time when this timer was actually executed.
     */
    protected long lastActualExecutionTime = -1;

    /**
     * The number of times this timer was actually executed.
     */
    protected int numberOfActualExecutions;

    public Timer(long sleepTime) {
        this(sleepTime, false, false);
    }

    public Timer(long sleepTime, boolean isRecurring, boolean isAllowSkipping) {
        this.sleepTime = sleepTime;
        this.isRecurring = isRecurring;
        this.isAllowSkipping = isAllowSkipping;
        reset();
    }

    public void reset() {
        this.creationTime = System.currentTimeMillis();
        this.firstExecutionTime = creationTime + sleepTime;
        this.nextExecutionTime = firstExecutionTime;
        this.numberOfActualExecutions = 0;
    }

    public double getSleepTime() {
        return sleepTime;
    }

    public boolean isRecurring() {
        return isRecurring;
    }

    public boolean isAllowSkipping() {
        return isAllowSkipping;
    }

    public long getNextExecutionTime() {
        return nextExecutionTime;
    }

    public long getFirstActualExecutionTime() {
        return firstActualExecutionTime;
    }

    public long getLastActualExecutionTime() {
        return lastActualExecutionTime;
    }

    public long getNumberOfActualExecutions() {
        return numberOfActualExecutions;
    }

    public long getNumberOfExecutions() {
        long currentTime = System.currentTimeMillis();
        long numberOfExecutions = (currentTime - creationTime) / sleepTime;
        return isRecurring ? numberOfExecutions : Math.min(1, numberOfExecutions);
    }

    public int compareTo(Timer other) {
        if (nextExecutionTime < other.nextExecutionTime)
            return -1;
        else if (nextExecutionTime > other.nextExecutionTime)
            return +1;
        else
            return 0;
    }

    public void execute() {
        lastActualExecutionTime = System.currentTimeMillis();
        if (firstActualExecutionTime == -1)
            firstActualExecutionTime = lastActualExecutionTime;
        run();
        numberOfActualExecutions++;
    }

    public void calculateNextExecution() {
        long currentTime = System.currentTimeMillis();
        if (isRecurring)
            nextExecutionTime = currentTime + sleepTime - (currentTime - firstExecutionTime) % sleepTime;
        else if (currentTime > firstExecutionTime && isAllowSkipping)
            nextExecutionTime = -1;
    }

    public abstract void run();
}

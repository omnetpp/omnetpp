/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

public abstract class Timer implements Comparable<Timer>, Runnable {
    /**
     * Specifies if the timer should be executed periodically or only once.
     */
    protected boolean recurring;

    /**
     * True means it is allowed to skip timer executions if time passed due to system load.
     */
    protected boolean allowSkipping;

    /**
     * For a recurring timer it is the time between subsequent executions starting from the time when the timer was created.
     * For other timers it is the time between the timer is created and the only one execution.
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
    protected long numberOfActualExecutions;

    public Timer(long sleepTime) {
        this.sleepTime = sleepTime;
        reset();
    }

    public Timer(long sleepTime, boolean recurring, boolean allowSkipping) {
        this(sleepTime);
        this.recurring = recurring;
        this.allowSkipping = allowSkipping;
    }

    public void reset() {
        this.creationTime = System.currentTimeMillis();
        this.firstExecutionTime = creationTime + sleepTime;
        this.nextExecutionTime = firstExecutionTime;
    }

    public double getSleepTime() {
        return sleepTime;
    }

    public boolean isRecurring() {
        return recurring;
    }

    public boolean isAllowSkipping() {
        return allowSkipping;
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

        return recurring ? numberOfExecutions : Math.min(1, numberOfExecutions);
    }

    public int compareTo(Timer other) {
        return ((Long)nextExecutionTime).compareTo(other.nextExecutionTime);
    }

    public void execute() {
        lastActualExecutionTime = System.currentTimeMillis();

        if (firstActualExecutionTime == -1)
            firstActualExecutionTime = lastActualExecutionTime;

        run();

        numberOfActualExecutions++;
        calculateNextExecution();
    }

    public void calculateNextExecution() {
        long currentTime = System.currentTimeMillis();

        if (recurring)
            nextExecutionTime = currentTime + sleepTime - (currentTime - firstExecutionTime) % sleepTime;
        else if (currentTime > firstExecutionTime && allowSkipping)
            nextExecutionTime = -1;
    }

    public abstract void run();
}

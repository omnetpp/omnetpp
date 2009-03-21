package org.omnetpp.scave.writers.example;

public abstract class Event implements Comparable<Event> {
    public double time;

    public int compareTo(Event e) {
        return time==e.time ? 0 : time<e.time ? -1 : 1;
    }

    abstract void execute();
}

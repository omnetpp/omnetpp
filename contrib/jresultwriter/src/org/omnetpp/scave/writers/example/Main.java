package org.omnetpp.scave.writers.example;

public class Main {
    public static void main(String[] args) {
        SimulationManager sim = new SimulationManager();

        Component top = new Component("top", sim, null);
        new FooComponent("alice", sim, top);
        new FooComponent("bob", sim, top);
        new FooComponent("cecil", sim, top);

        sim.simulate(50.0);
    }
}

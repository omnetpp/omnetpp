package org.omnetpp.simulation.liveanimation;

public class DummyAnimationPrimitive extends AbstractAnimationPrimitive {
    private int remaining;

    public DummyAnimationPrimitive(int count) {
        remaining = count;
    }

    public boolean updateFor(double time) {
        System.out.println("DummyAnimationPrimitive: remaining=" + remaining);
        remaining--;
        return remaining > 0;
    }
}

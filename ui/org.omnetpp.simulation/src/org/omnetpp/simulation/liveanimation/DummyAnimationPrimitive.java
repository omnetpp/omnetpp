package org.omnetpp.simulation.liveanimation;

public class DummyAnimationPrimitive extends AnimationPrimitive {
    private int remaining;
    
    public DummyAnimationPrimitive(int count) {
        remaining = count;
    }
    
    public boolean update() {
        System.out.println("DummyAnimationPrimitive: remaining=" + remaining);
        remaining--;
        return remaining > 0;
    }
}

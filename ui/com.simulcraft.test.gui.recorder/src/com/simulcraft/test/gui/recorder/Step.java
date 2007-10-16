package com.simulcraft.test.gui.recorder;

public class Step {
    private String javaCode;
    private double quality;

    public Step(String javaCode, double quality) {
        this.javaCode = javaCode;
        this.quality = quality;
    }

    public String getJavaCode() {
        return javaCode;
    }

    /**
     * Quality of this guess, 0.0=worst, 1.0=best.
     */
    public double getQuality() {
        return quality;
    }
}

package org.omnetpp.samples.animation.controller;

import java.util.ArrayList;

import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.animation.controller.IAnimationCoordinateSystem;
import org.omnetpp.common.engine.BigDecimal;

public class CustomAnimationCoordinateSystem implements IAnimationCoordinateSystem {
    @Override
    public void setAnimationPositions(ArrayList<AnimationPosition> animationPositions) {
    }

    @Override
    public long getFirstEventNumber() {
        return 0;
    }

    @Override
    public long getLastEventNumber() {
        return 0;
    }

    @Override
    public BigDecimal getFirstSimulationTime() {
        return BigDecimal.getZero();
    }

    @Override
    public BigDecimal getLastSimulationTime() {
        return BigDecimal.getZero();
    }

    @Override
    public BigDecimal getSimulationTime(long eventNumber) {
        return BigDecimal.getZero();
    }

    @Override
    public double getAnimationTime(long eventNumber) {
        return 0;
    }

    @Override
    public double getAnimationTimeForSimulationTime(BigDecimal simulationTime) {
        return 0;
    }

    @Override
    public BigDecimal getSimulationTimeForAnimationTime(double animationTime) {
        return BigDecimal.getZero();
    }

    @Override
    public long getLastEventNumberNotAfterAnimationTime(double animationTime) {
        return 0;
    }

    @Override
    public long getLastEventNumberNotAfterSimulationTime(BigDecimal simulationTime) {
        return 0;
    }

    @Override
    public double getAnimationTimeDelta(double simulationTimeDelta) {
        return 0;
    }
}

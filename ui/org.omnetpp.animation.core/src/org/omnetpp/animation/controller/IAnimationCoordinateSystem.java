/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.controller;

import java.util.ArrayList;

import org.omnetpp.common.engine.BigDecimal;

/**
 * Interface to map between event numbers, simulation times and animation times.
 *
 * @author levy
 */
public interface IAnimationCoordinateSystem {
    public void setAnimationPositions(ArrayList<AnimationPosition> animationPositions);

    public long getFirstEventNumber();

    public long getLastEventNumber();

    public BigDecimal getFirstSimulationTime();

    public BigDecimal getLastSimulationTime();

    public BigDecimal getSimulationTime(long eventNumber);

    public double getAnimationTime(long eventNumber);

    public double getAnimationTimeForSimulationTime(BigDecimal simulationTime);

    public BigDecimal getSimulationTimeForAnimationTime(double animationTime);

    public long getLastEventNumberNotAfterAnimationTime(double animationTime);

    public long getLastEventNumberNotAfterSimulationTime(BigDecimal simulationTime);

    public double getAnimationTimeDelta(double simulationTimeDelta);
}

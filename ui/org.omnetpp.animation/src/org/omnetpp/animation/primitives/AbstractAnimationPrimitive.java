/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.


  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.animation.widgets.AnimationController;
import org.omnetpp.animation.widgets.AnimationPosition;
import org.omnetpp.animation.widgets.AnimationSimulation;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogGate;
import org.omnetpp.common.eventlog.EventLogModule;
import org.omnetpp.common.util.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.anchors.NoncentralChopboxAnchor;

/**
 * Base class for animation primitives. The default behavior is that the animation primitive does not draw anything.
 */
public abstract class AbstractAnimationPrimitive implements IAnimationPrimitive {
    protected boolean isActive;

	protected AnimationController animationController;

	protected long sourceEventNumber;

	// NOTE: there is a strict relationship between the begin and the end animation positions and the durations (both in terms of simulation and animation time)
	protected AnimationPosition beginAnimationPosition;

    protected AnimationPosition endAnimationPosition;

    protected BigDecimal simulationTimeDuration;

    protected double animationTimeDuration;

    public AbstractAnimationPrimitive(AnimationController animationController) {
        this.animationController = animationController;
        this.sourceEventNumber = -1;
        this.beginAnimationPosition = new AnimationPosition();
        this.endAnimationPosition = new AnimationPosition();
        this.simulationTimeDuration = null;
        this.animationTimeDuration = -1;

    }

    public long getSourceEventNumber() {
        return sourceEventNumber;
    }

    public void setSourceEventNumber(long sourceEventNumber) {
        this.sourceEventNumber = sourceEventNumber;
    }

    /*************************************************************************************
     * IAnimationPrimitive API
     */

	public AnimationPosition getBeginAnimationPosition() {
	    return beginAnimationPosition;
	}

    public AnimationPosition getEndAnimationPosition() {
        return endAnimationPosition;
    }

    public long getBeginEventNumber() {
        return beginAnimationPosition.getEventNumber();
    }

    public void setBeginEventNumber(long eventNumber) {
        beginAnimationPosition.setEventNumber(eventNumber);
    }

    public long getEndEventNumber() {
        return endAnimationPosition.getEventNumber();
    }

    public void setEndEventNumber(long eventNumber) {
        endAnimationPosition.setEventNumber(eventNumber);
    }

    public BigDecimal getBeginSimulationTime() {
		return beginAnimationPosition.getSimulationTime();
	}

    public void setBeginSimulationTime(BigDecimal beginSimulationTime) {
        Assert.isTrue(beginAnimationPosition.getSimulationTime() == null);
        beginAnimationPosition.setSimulationTime(beginSimulationTime);
        if (simulationTimeDuration != null)
            endAnimationPosition.setSimulationTime(beginSimulationTime.add(simulationTimeDuration));
        else if (endAnimationPosition.getSimulationTime() != null)
            simulationTimeDuration = endAnimationPosition.getSimulationTime().subtract(beginSimulationTime);
    }

	public BigDecimal getEndSimulationTime() {
		return endAnimationPosition.getSimulationTime();
	}

    public void setEndSimulationTime(BigDecimal endSimulationTime) {
        Assert.isTrue(endAnimationPosition.getSimulationTime() == null);
        endAnimationPosition.setSimulationTime(endSimulationTime);
        if (simulationTimeDuration != null)
            beginAnimationPosition.setSimulationTime(endSimulationTime.subtract(simulationTimeDuration));
        else if (beginAnimationPosition.getSimulationTime() != null)
            simulationTimeDuration = endSimulationTime.subtract(beginAnimationPosition.getSimulationTime());
    }

	public double getBeginAnimationTime() {
		return beginAnimationPosition.getAnimationTime();
	}

    public void setBeginAnimationTime(double beginAnimationTime) {
        Assert.isTrue(beginAnimationPosition.getAnimationTime() == -1);
        beginAnimationPosition.setAnimationTime(beginAnimationTime);
        if (animationTimeDuration != -1)
            endAnimationPosition.setAnimationTime(beginAnimationTime + animationTimeDuration);
        else if (endAnimationPosition.getAnimationTime() != -1)
            animationTimeDuration = endAnimationPosition.getAnimationTime() - beginAnimationTime;
    }

	public double getEndAnimationTime() {
		return endAnimationPosition.getAnimationTime();
	}

    public void setEndAnimationTime(double endAnimationTime) {
        Assert.isTrue(endAnimationPosition.getAnimationTime() == -1);
        endAnimationPosition.setAnimationTime(endAnimationTime);
        if (animationTimeDuration != -1)
            beginAnimationPosition.setAnimationTime(endAnimationTime - animationTimeDuration);
        else if (beginAnimationPosition.getAnimationTime() != -1)
            animationTimeDuration = endAnimationTime - beginAnimationPosition.getAnimationTime();
    }

    public double getRelativeBeginAnimationTime() {
	    return beginAnimationPosition.getRelativeAnimationTime();
	}

    public void setRelativeBeginAnimationTime(double relativeBeginAnimationTime) {
        beginAnimationPosition.setRelativeAnimationTime(relativeBeginAnimationTime);
    }

    public double getRelativeEndAnimationTime() {
        return endAnimationPosition.getRelativeAnimationTime();
    }

    public void setRelativeEndAnimationTime(double relativeEndAnimationTime) {
        endAnimationPosition.setRelativeAnimationTime(relativeEndAnimationTime);
    }

    public BigDecimal getSimulationTimeDuration() {
        return simulationTimeDuration;
    }

    public void setSimulationTimeDuration(BigDecimal simulationTimeDuration) {
        Assert.isTrue(this.simulationTimeDuration == null && simulationTimeDuration.greaterOrEqual(BigDecimal.getZero()));
        this.simulationTimeDuration = simulationTimeDuration;
        if (beginAnimationPosition.getSimulationTime() != null)
            endAnimationPosition.setSimulationTime(beginAnimationPosition.getSimulationTime().add(simulationTimeDuration));
        else if (endAnimationPosition.getSimulationTime() != null)
            beginAnimationPosition.setSimulationTime(endAnimationPosition.getSimulationTime().subtract(simulationTimeDuration));
    }

    public Double getAnimationTimeDuration() {
        return animationTimeDuration;
    }

    public void setAnimationTimeDuration(Double animationTimeDuration) {
        // TODO: Assert.isTrue(this.animationTimeDuration == -1 && animationTimeDuration >= 0);
        this.animationTimeDuration = animationTimeDuration;
        if (beginAnimationPosition.getAnimationTime() != -1)
            endAnimationPosition.setAnimationTime(beginAnimationPosition.getAnimationTime() + animationTimeDuration);
        else if (endAnimationPosition.getAnimationTime() != -1)
            beginAnimationPosition.setAnimationTime(endAnimationPosition.getAnimationTime() - animationTimeDuration);
    }

	public void activate() {
 	    Assert.isTrue(beginAnimationPosition != null && endAnimationPosition != null && getSimulationTimeDuration() != null && getAnimationTimeDuration() != null);
	    // TODO: Assert.isTrue(!endAnimationPosition.isValid() || beginAnimationPosition.compareTo(endAnimationPosition) <= 0);
	    // TODO: Assert.isTrue(getSimulationTimeDuration().greaterOrEqual(BigDecimal.getZero()) && getAnimationTimeDuration() >= 0);
	    Assert.isTrue(!isActive);
	    isActive = true;
	}

	public void deactivate() {
        Assert.isTrue(isActive);
        isActive = false;
	}

	public boolean isActive() {
	    return isActive;
	}

	public void refreshAnimation(AnimationPosition animationPosition) {
	    // void
	}

    @Override
    public String toString() {
        return getClass().getSimpleName() + " : begin" + beginAnimationPosition + " end" + endAnimationPosition + " duration[" + getSimulationTimeDuration() + "s, " + getAnimationTimeDuration() + "]";
    }

    /*************************************************************************************
     * UTILITIES
     */

    protected AnimationSimulation getSimulation() {
		return animationController.getAnimationSimulation();
	}

	protected TimerQueue getTimerQueue() {
		return animationController.getTimerQueue();
	}

	protected IFigure getRootFigure() {
		return animationController.getRootFigure();
	}

	protected LayoutManager getLayoutManager() {
		return getRootFigure().getLayoutManager();
	}

	protected CompoundModuleFigure getCompoundModuleFigure(int moduleId) {
        EventLogModule module = getSimulation().getModuleById(moduleId);
        if (module != null)
            return getCompoundModuleFigure(module);
        else
            return null;
    }

	protected CompoundModuleFigure getCompoundModuleFigure(EventLogModule module) {
		return (CompoundModuleFigure)animationController.getFigure(module, CompoundModuleFigure.class);
	}

	protected void setConstraint(IFigure figure, Rectangle constraint) {
		getLayoutManager().setConstraint(figure, constraint);
	}

	protected CompoundModuleFigure getEnclosingCompoundModuleFigure(int submoduleId) {
		EventLogModule enclosingModule = getSimulation().getModuleById(submoduleId).getParentModule();
		return getCompoundModuleFigure(enclosingModule);
	}

    protected CompoundModuleFigure getEnclosingCompoundModuleFigure(EventLogGate sourceGate) {
        // KLUDGE: remove this hack
        ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(sourceGate, ConnectionFigure.class);
        if (connectionFigure == null)
            return null;
        else
            // KLUDGE: find a more reliable way
            return (CompoundModuleFigure)connectionFigure.getParent().getParent();
    }

	protected SubmoduleFigure getSubmoduleFigure(int moduleId) {
		EventLogModule module = getSimulation().getModuleById(moduleId);
		if (module != null)
			return (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
		else
			return null;
	}

	protected SubmoduleFigure getSubmoduleFigure(EventLogModule module) {
        return (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
	}

	protected int getAncestorUnderParentModule(int enclosedModuleId) {
		EventLogModule rootModule = getSimulation().getRootModule();
		EventLogModule module = getSimulation().getModuleById(enclosedModuleId);
		while (module != null && module.getParentModule() != rootModule)
			module = module.getParentModule();
		if (module != null)
			return module.getId();
		else
			return -1;
	}

	protected Point getSubmoduleFigureCenter(int moduleId) {
		return getSubmoduleFigure(moduleId).getBounds().getCenter();
	}


    protected GateAnchor getGateAnchor(EventLogGate gate, Class<?> clazz) {
        IFigure figure = animationController.getFigure(animationController.getAnimationSimulation().getModuleById(gate.getOwnerModuleId()), clazz);
        // TODO: use gate name
        if (clazz.equals(CompoundModuleFigure.class))
            return new CompoundModuleGateAnchor(figure);
        else
            return new NoncentralChopboxAnchor(figure);
    }

	protected Point getConvexCombination(Point p1, Point p2, double alpha) {
		return new Point(Math.round((1 - alpha) * p1.x + alpha * p2.x), Math.round((1 - alpha) * p1.y + alpha * p2.y));
	}
}

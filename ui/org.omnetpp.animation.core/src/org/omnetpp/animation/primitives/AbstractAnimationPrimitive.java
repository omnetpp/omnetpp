/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.


  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.primitives;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.LayoutManager;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.animation.controller.AnimationController;
import org.omnetpp.animation.controller.AnimationPosition;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.GateModel;
import org.omnetpp.common.simulation.ModuleModel;
import org.omnetpp.common.simulation.SimulationModel;
import org.omnetpp.common.util.TimerQueue;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.figures.SubmoduleFigure;
import org.omnetpp.figures.anchors.CompoundModuleGateAnchor;
import org.omnetpp.figures.anchors.GateAnchor;
import org.omnetpp.figures.anchors.NoncentralChopboxAnchor;

/**
 * <p>
 * Abstract base class for animation primitives. The default behavior is that
 * the animation primitive does not change the model state and does not draw
 * anything.
 * </p>
 * <p>
 * This class automatically keeps the invariant between the begin and end
 * animation positions and the corresponding durations (both in terms of
 * simulation and animation times).
 * </p>
 *
 * @author levy
 */
public class AbstractAnimationPrimitive implements IAnimationPrimitive {
    /**
     * The owner animation controller that is responsible to manage this
     * animation primitive.
     */
    protected final AnimationController animationController;

    /**
     * Specifies that whether this animation primitive is active or not.
     */
    private boolean isActive;

    /**
     * The event number of the event that provided this animation primitive.
     * Note that this might be different from the event numbers present in the
     * begin and end animation positions.
     */
	private long sourceEventNumber;

    /**
     * The animation position when this animation primitive begins. Note that
     * there is a strict relationship between the begin and end animation
     * positions and the durations (both in terms of simulation and animation
     * time). This invariant is automatically kept by this class.
     */
	private AnimationPosition beginAnimationPosition;

    /**
     * The animation position when this animation primitive end. Note that
     * there is a strict relationship between the begin and end animation
     * positions and the durations (both in terms of simulation and animation
     * time). This invariant is automatically kept by this class.
     */
	private AnimationPosition endAnimationPosition;

	/**
     * Returns the total duration of this animation primitive in terms of
     * simulation time. The value <code>null</code> means that it is not yet
     * set. Note that the time duration is automatically updated when both the
     * begin and end simulation times are set.
	 */
	private BigDecimal simulationTimeDuration;

    /**
     * Returns the total duration of this animation primitive in terms of
     * animation time. The value <code>-1</code> means that it is not yet set.
     * Note that the time duration is automatically updated when both the begin
     * and end animation times are set.
     */
	private double animationTimeDuration;

    /*************************************************************************************
     * CONSTRUCTOR
     */

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

	public double getOriginRelativeBeginAnimationTime() {
		return beginAnimationPosition.getOriginRelativeAnimationTime();
	}

    public void setOriginRelativeBeginAnimationTime(double beginAnimationTime) {
        // Assert.isTrue(beginAnimationPosition.getOriginRelativeAnimationTime() == null);
        beginAnimationPosition.setOriginRelativeAnimationTime(beginAnimationTime);
        if (animationTimeDuration != -1)
            endAnimationPosition.setOriginRelativeAnimationTime(beginAnimationTime + animationTimeDuration);
        else if (endAnimationPosition.getOriginRelativeAnimationTime() != null)
            animationTimeDuration = endAnimationPosition.getOriginRelativeAnimationTime() - beginAnimationTime;
    }

	public double getOriginRelativeEndAnimationTime() {
		return endAnimationPosition.getOriginRelativeAnimationTime();
	}

    public void setOriginRelativeEndAnimationTime(double endAnimationTime) {
        // Assert.isTrue(endAnimationPosition.getOriginRelativeAnimationTime() == null);
        endAnimationPosition.setOriginRelativeAnimationTime(endAnimationTime);
        if (animationTimeDuration != -1)
            beginAnimationPosition.setOriginRelativeAnimationTime(endAnimationTime - animationTimeDuration);
        else if (beginAnimationPosition.getOriginRelativeAnimationTime() != null)
            animationTimeDuration = endAnimationTime - beginAnimationPosition.getOriginRelativeAnimationTime();
    }

    public double getFrameRelativeBeginAnimationTime() {
	    return beginAnimationPosition.getFrameRelativeAnimationTime();
	}

    public void setFrameRelativeBeginAnimationTime(double frameRelativeBeginAnimationTime) {
        beginAnimationPosition.setFrameRelativeAnimationTime(frameRelativeBeginAnimationTime);
    }

    public double getFrameRelativeEndAnimationTime() {
        return endAnimationPosition.getFrameRelativeAnimationTime();
    }

    public void setFrameRelativeEndAnimationTime(double frameRelativeEndAnimationTime) {
        endAnimationPosition.setFrameRelativeAnimationTime(frameRelativeEndAnimationTime);
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
        Assert.isTrue(animationTimeDuration >= 0);
        this.animationTimeDuration = animationTimeDuration;
        if (beginAnimationPosition.getOriginRelativeAnimationTime() != null)
            endAnimationPosition.setOriginRelativeAnimationTime(beginAnimationPosition.getOriginRelativeAnimationTime() + animationTimeDuration);
        else if (endAnimationPosition.getOriginRelativeAnimationTime() != null)
            beginAnimationPosition.setOriginRelativeAnimationTime(endAnimationPosition.getOriginRelativeAnimationTime() - animationTimeDuration);
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

    protected SimulationModel getSimulation() {
		return animationController.getSimulation();
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
        ModuleModel module = getSimulation().getModuleById(moduleId);
        if (module != null)
            return getCompoundModuleFigure(module);
        else
            return null;
    }

	protected CompoundModuleFigure getCompoundModuleFigure(ModuleModel module) {
		return (CompoundModuleFigure)animationController.getFigure(module, CompoundModuleFigure.class);
	}

	protected CompoundModuleFigure getEnclosingCompoundModuleFigure(int submoduleId) {
		ModuleModel enclosingModule = getSimulation().getModuleById(submoduleId).getParentModule();
		return getCompoundModuleFigure(enclosingModule);
	}

    protected CompoundModuleFigure getEnclosingCompoundModuleFigure(GateModel sourceGate) {
        // KLUDGE: remove this hack
        ConnectionFigure connectionFigure = (ConnectionFigure)animationController.getFigure(sourceGate, ConnectionFigure.class);
        if (connectionFigure == null)
            return null;
        else
            // KLUDGE: find a more reliable way
            return (CompoundModuleFigure)connectionFigure.getParent().getParent();
    }

	protected SubmoduleFigure getSubmoduleFigure(int moduleId) {
		ModuleModel module = getSimulation().getModuleById(moduleId);
		if (module != null)
			return (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
		else
			return null;
	}

	protected SubmoduleFigure getSubmoduleFigure(ModuleModel module) {
        return (SubmoduleFigure)animationController.getFigure(module, SubmoduleFigure.class);
	}

	protected int getAncestorUnderParentModule(int enclosedModuleId) {
		ModuleModel rootModule = getSimulation().getRootModule();
		ModuleModel module = getSimulation().getModuleById(enclosedModuleId);
		while (module != null && module.getParentModule() != rootModule)
			module = module.getParentModule();
		if (module != null)
			return module.getId();
		else
			return -1;
	}

	protected Point getSubmoduleFigureCenter(int moduleId) {
		return getSubmoduleFigure(moduleId).getCenterLocation();
	}

    protected GateAnchor getGateAnchor(GateModel gate, Class<?> clazz) {
        IFigure figure = animationController.getFigure(animationController.getSimulation().getModuleById(gate.getOwnerModuleId()), clazz);
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

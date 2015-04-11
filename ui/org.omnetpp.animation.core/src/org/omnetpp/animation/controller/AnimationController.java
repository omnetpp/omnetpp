/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.controller;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.collections.map.MultiValueMap;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.animation.primitives.AbstractAnimationPrimitive;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.providers.IAnimationPrimitiveProvider;
import org.omnetpp.animation.widgets.AnimationCanvas;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.simulation.SimulationModel;
import org.omnetpp.common.util.BinarySearchUtils;
import org.omnetpp.common.util.BinarySearchUtils.BoundKind;
import org.omnetpp.common.util.BinarySearchUtils.KeyComparator;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.Timer;
import org.omnetpp.common.util.TimerQueue;

/**
 * <p>
 * The animation controller is responsible for managing the animation primitives
 * while it is playing the animation. The animation can be played in both
 * forward and backward direction. The controller keeps track of the active
 * animation primitives as real time goes by and calls the appropriate
 * activate/deactivate functions as needed. It is also possible to immediately
 * go to animation positions, event numbers or simulation times.
 * </p>
 *
 * @author levy
 */
public class AnimationController {
	private final static boolean debug = false;

    /**
     * The widget used to display the animation figures.
     */
    private AnimationCanvas animationCanvas;

    /**
     * Provides animation primitives for the current animation position.
     */
    private IAnimationPrimitiveProvider animationPrimitiveProvider;

    /**
     * The coordinate system that maps between animation times, simulation times
     * and event numbers.
     */
    private IAnimationCoordinateSystem animationCoordinateSystem;

    /**
     * A list of timers used during the animation. The queue contains the
     * animationTimer and animation primitives may add their own timers here. As
     * real time goes by the timer queue will call its timers based on their
     * settings, so that they can update their figures.
     */
    private TimerQueue timerQueue;

    /**
     * The main timer that is responsible for updating the animation position
     * during the animation.
     */
    private AnimationTimer animationTimer;

    /**
     * The list of loaded animation primitives. This may contain more animation
     * primitives than that is rendered to the canvas. This list is ordered by
     * begin animation time.
     */
    private ArrayList<IAnimationPrimitive> beginAnimationTimeOrderedPrimitives;

    /**
     * The list of loaded animation primitives. This may contain more animation
     * primitives than that is rendered to the canvas. This list is ordered by
     * end animation time.
     *
     * In fact the list is ordered by end simulation time and as soon as any
     * item is required at a given simulation time, then all the primitives at
     * the very same end simulation time are ordered in place by their end
     * animation time.
     */
    private ArrayList<IAnimationPrimitive> endAnimationTimeOrderedPrimitives;

    /**
     * The list of currently active animation primitives. These will be called
     * to refresh their animation at the current animation position. At any
     * given time it should contain an animation primitive iff the current
     * animation position is between the begin and end animation time of that
     * primitive.
     */
    private ArrayList<IAnimationPrimitive> activeAnimationPrimitives;

    /**
     * The simulation environment is responsible for managing modules,
     * connections, gates, messages.
     */
    private SimulationModel simulation;

    /**
     * Specifies whether the animation is currently running or not.
     */
    private boolean isRunning;

    /**
     * Current direction of the animation.
     */
    private boolean isPlayingForward;

    /**
     * The animation position when the animation was last started or continued.
     * The current animation position will be calculated by a linear
     * approximation using the elapsed real time and the last start animation
     * position.
     */
    private AnimationPosition lastStartAnimationPosition;

    /**
     * The current animation position. It is updated periodically from a timer
     * callback during animation. This is where the animation is right now.
     */
    private AnimationPosition currentAnimationPosition;

    /**
     * The animation position which is reflected in the model state. This may be
     * different from the current animation position temporarily. It is only
     * updated when the model gets updated.
     */
    private AnimationPosition modelAnimationPosition;

    /**
     * The animation position when the animation will be next stopped if there's
     * no such limit, then this position is unspecified.
     */
    private AnimationPosition stopAnimationPosition;

    /**
     * The animation position for the very beginning of the whole animation.
     */
    private AnimationPosition beginAnimationPosition;

    /**
     * The animation position for the first event in the animation.
     */
    private AnimationPosition firstEventAnimationPosition;

    /**
     * The animation position for the very end of the whole animation.
     */
    private AnimationPosition endAnimationPosition;

    /**
     * The real time when the animation was last started or continued.
     */
    private double lastStartRealTime;

    /**
     * This multiplier controls the speed of the animation, it defaults to 1.
     */
    private double realTimeToAnimationTimeScale;

    /**
     * A map for figures and animation related objects. Animation primitives may
     * store information in this map and may communicate with each other.
     */
    private Map<Object, Object> figureMap;

    /**
     * Animation listeners are notified for various events such as changes in
     * the current animation position.
     */
    private ArrayList<IAnimationListener> animationListeners = new ArrayList<IAnimationListener>();

    /**
     * Last time the animation position changed notification was sent (in
     * milliseconds). This helps to avoid sending notifications too often.
     */
    private long lastTimeAnimationPositionChangedNotificationWasSent;

    /**
     * An event number specifying the current origin of the animation time
     * coordinate system. The value <code>null</code> means the coordinate
     * system is not yet specified. The value must be greater than or equal to
     * <code>0</code>.
     */
    private Long animationTimeOriginEventNumber;

    /**
     * This map is filled as animation primitives are loaded.
     */
    private Map<Long, Double> eventNumberToFrameRelativeEndAnimationTime;

    /**
     * This is a special purpose comparator for animation positions. It uses the
     * event numbers when the simulation times are equal. Otherwise it uses the
     * relative animation times.
     */
    // TODO: KLUDGE: isn't this obsolete now that we have the new animation frame concept?
    private Comparator<AnimationPosition> animationPositionComparator = new Comparator<AnimationPosition>() {
        public int compare(AnimationPosition animationPosition1, AnimationPosition animationPosition2) {
            BigDecimal simulationTime1 = animationPosition1.getSimulationTime();
            BigDecimal simulationTime2 = animationPosition2.getSimulationTime();
            if (simulationTime1.less(simulationTime2))
                return -1;
            else if (simulationTime1.greater(simulationTime2))
                return 1;
            else {
                long eventNumber1 = animationPosition1.getEventNumber();
                long eventNumber2 = animationPosition2.getEventNumber();
                long eventNumber = animationCoordinateSystem.getLastEventNumberNotAfterSimulationTime(simulationTime1);
                BigDecimal simulationTime = animationCoordinateSystem.getSimulationTime(eventNumber);
                boolean sameSimulationTime = simulationTime.equals(simulationTime1);
                if (sameSimulationTime && eventNumber1 < eventNumber && eventNumber2 >= eventNumber)
                    return -1;
                else if (sameSimulationTime && eventNumber2 < eventNumber && eventNumber1 >= eventNumber)
                    return 1;
                else if (sameSimulationTime && eventNumber1 >= eventNumber && eventNumber2 >= eventNumber && eventNumber1 != eventNumber2)
                    return (int)Math.signum(eventNumber1 - eventNumber2);
                else {
                    double relativeAnimationTime1 = animationPosition1.getFrameRelativeAnimationTime();
                    double relativeAnimationTime2 = animationPosition2.getFrameRelativeAnimationTime();
                    return (int)Math.signum(relativeAnimationTime1 - relativeAnimationTime2);
                }
            }
        }
    };

    /*************************************************************************************
     * CONSTRUCTION
     */

	public AnimationController(IAnimationCoordinateSystem animationCoordinateSystem, IAnimationPrimitiveProvider animationPrimitiveProvider) {
	    this.animationCoordinateSystem = animationCoordinateSystem;
        this.animationPrimitiveProvider = animationPrimitiveProvider;
        this.isPlayingForward = true;
        this.isRunning = false;
        this.realTimeToAnimationTimeScale = 1;
        this.timerQueue = new TimerQueue();
        this.timerQueue.start();
        this.animationTimer = new AnimationTimer();
        this.timerQueue.addTimer(animationTimer);
        this.simulation = null;
	}

    /**
     * Reloads the animation primitives.
     */
    public void clearInternalState() {
        figureMap = new HashMap<Object, Object>();
        currentAnimationPosition = new AnimationPosition();
        modelAnimationPosition = new AnimationPosition();
        stopAnimationPosition = new AnimationPosition();
        lastStartAnimationPosition = new AnimationPosition();
        beginAnimationPosition = new AnimationPosition();
        firstEventAnimationPosition = new AnimationPosition();
        endAnimationPosition = new AnimationPosition();
        beginAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        endAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        activeAnimationPrimitives = new ArrayList<IAnimationPrimitive>();
        eventNumberToFrameRelativeEndAnimationTime = new HashMap<Long, Double>();
        for (Timer timer : timerQueue.getTimers())
            if (timer != animationTimer)
                timerQueue.removeTimer(timer);
        if (animationCanvas != null)
            animationCanvas.removeDecorationFigures();
    }

    /**
     * Shuts down the controller and releases all resources.
     */
    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        timerQueue.stop();
    }

    /**
     * Returns the canvas on which the animation figures will be drawn.
     */
    public AnimationCanvas getAnimationCanvas() {
        return animationCanvas;
    }

    /**
     * Sets the canvas on which the animation figures will be drawn.
     */
    public void setAnimationCanvas(AnimationCanvas animationCanvas) {
        this.animationCanvas = animationCanvas;
    }

    /**
     * Returns the animation provider which creates the animation primitives.
     */
    public IAnimationPrimitiveProvider getAnimationPrimitiveProvider() {
        return animationPrimitiveProvider;
    }

    /**
     * Returns the animation coordinate system that maps between animation times,
     * simulation times and event numbers.
     */
    public IAnimationCoordinateSystem getAnimationCoordinateSystem() {
        return animationCoordinateSystem;
    }

	/**
	 * Returns the canvas'es root figure.
	 */
	public IFigure getRootFigure() {
		return animationCanvas.getRootFigure();
	}

    /**
     * Returns the current simulation attached to this controller.
     */
	public SimulationModel getSimulation() {
		return simulation;
	}

    /**
     * Sets the current simulation of this controller.
     */
    public void setSimulation(SimulationModel simulation) {
        Assert.isTrue(this.simulation == null);
        this.simulation = simulation;
    }

    public Double getFrameRelativeEndAnimationTimeForEventNumber(long eventNumber) {
        return eventNumberToFrameRelativeEndAnimationTime.get(eventNumber);
    }

    public void setFrameRelativeEndAnimationTimeForEventNumber(long eventNumber, double endAnimationTime) {
        eventNumberToFrameRelativeEndAnimationTime.put(eventNumber, endAnimationTime);
    }

    /**
     * Returns the timer queue that is used to schedule timer events during the
     * animation.
     */
	public TimerQueue getTimerQueue() {
		return timerQueue;
	}

	/**
	 * Specifies the direction of the animation.
	 */
	public boolean isPlayingForward() {
		return isPlayingForward;
	}

    /**
     * Specifies the direction of the animation.
     */
    public boolean isPlayingBackward() {
        return !isPlayingForward;
    }

	/**
	 * Specifies whether the animation is currently running.
	 */
	public boolean isRunning() {
		return isRunning;
	}

	/**
	 * Changes the state of the animation.
	 */
	private void setRunning(boolean isRunning) {
	    if (this.isRunning != isRunning) {
    		this.isRunning = isRunning;
    		runningStateChanged();
	    }
	}

    /**
     * Increase animation speed.
     */
    public void increaseAnimationSpeed() {
        setRealTimeToAnimationTimeScale(realTimeToAnimationTimeScale * 1.5);
    }

    /**
     * Decrease animation speed.
     */
    public void decreaseAnimationSpeed() {
        setRealTimeToAnimationTimeScale(realTimeToAnimationTimeScale / 1.5);
    }

    /**
     * Relocates the animation coordinate system origin to the animation begin
     * of the event with the provided event number. Calling this function also
     * clears all dependent internal state such as stored animation positions
     * and animation primitives.
     */
    private void relocateAnimationTimeOrigin(long eventNumber) {
        animationTimeOriginEventNumber = eventNumber;
        clearInternalState();
    }

    /**
     * Returns the current animation position.
     */
    public AnimationPosition getCurrentAnimationPosition() {
        return currentAnimationPosition;
    }

    /**
     * Sets the current animation position.
     */
    private void setCurrentAnimationPosition(AnimationPosition animationPosition) {
        currentAnimationPosition = animationPosition;
        loadAnimationPrimitivesForAnimationPosition(animationPosition);
        if (currentAnimationPosition.getFrameRelativeAnimationTime() == null) {
            // TODO: KLUDGE: really here? really this simple?
            long eventNumber = currentAnimationPosition.getEventNumber();
            double animationTime = animationCoordinateSystem.getAnimationTime(eventNumber);
            currentAnimationPosition.setFrameRelativeAnimationTime(currentAnimationPosition.getOriginRelativeAnimationTime() - animationTime);
        }
        refreshAnimation();
        animationPositionChanged();
    }

    /**
     * Returns the animation position for the very beginning of the whole animation.
     */
    public AnimationPosition getBeginAnimationPosition() {
        if (!beginAnimationPosition.isCompletelySpecified())
            beginAnimationPosition = getAnimationPositionForEventNumber(0);
        return beginAnimationPosition;
    }

    /**
     * Returns the animation position for the first event in the animation.
     */
    public AnimationPosition getFirstEventAnimationPosition() {
        if (!firstEventAnimationPosition.isCompletelySpecified())
            firstEventAnimationPosition = getAnimationPositionForEventNumber(1);
        return firstEventAnimationPosition;
    }

    /**
     * Returns the animation position for the very end of the whole animation.
     */
    public AnimationPosition getEndAnimationPosition() {
        if (!endAnimationPosition.isCompletelySpecified()) {
            BigDecimal simulationTime = animationCoordinateSystem.getLastSimulationTime();
            loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(animationCoordinateSystem.getLastEventNumber(), simulationTime, null, null));
            for (int i = endAnimationTimeOrderedPrimitives.size() - 1; i >= 0; i--) {
                IAnimationPrimitive animationPrimitive = endAnimationTimeOrderedPrimitives.get(i);
                AnimationPosition endAnimationPosition = animationPrimitive.getEndAnimationPosition();
                if (endAnimationPosition.getSimulationTime().lessOrEqual(simulationTime) && !endAnimationPosition.getFrameRelativeAnimationTime().isInfinite()) {
                    this.endAnimationPosition = endAnimationPosition;
                    break;
                }
            }
        }
        return endAnimationPosition;
    }

    public boolean isAtAnimationPosition(AnimationPosition animationPosition) {
        loadAnimationPrimitivesForAnimationPosition(animationPosition);
        return beginAnimationTimeOrderedPrimitives.size() == 0 || currentAnimationPosition.equals(animationPosition);
    }

	/**
	 * Checks if the current animation position is at the very beginning.
	 */
	public boolean isAtBeginAnimationPosition() {
	    return isAtAnimationPosition(getBeginAnimationPosition());
	}

    /**
     * Checks if the current animation position is at the very beginning.
     */
    public boolean isAtFirstEventAnimationPosition() {
        return isAtAnimationPosition(getFirstEventAnimationPosition());
    }

	/**
	 * Checks if the current animation position is at the very end.
	 */
	public boolean isAtEndAnimationPosition() {
	    return isAtAnimationPosition(getEndAnimationPosition());
	}

	/**
     * Retrieves a figure or an animation related object.
	 */
	public IFigure getFigure(Object key, Class<?> subkey) {
		return (IFigure)figureMap.get(new Pair<Object, Class<?>>(key, subkey));
	}

    /**
     * Stores a figure or an animation related object.
     */
    public void setFigure(Object key, IFigure figure) {
        setFigure(key, figure.getClass(), figure);
    }

	/**
     * Stores a figure or an animation related object.
	 */
	public void setFigure(Object key, Class<?> subkey, IFigure figure) {
		figureMap.put(new Pair<Object, Class<?>>(key, subkey), figure);
	}

	/**
	 * Returns the current event number.
	 */
	public long getCurrentEventNumber() {
		return currentAnimationPosition.getEventNumber();
	}

    /**
     * Returns an animation position representing the processing of the given event number.
     */
    private AnimationPosition getAnimationPositionForEventNumber(long eventNumber) {
        BigDecimal simulationTime = animationCoordinateSystem.getSimulationTime(eventNumber);
        return new AnimationPosition(eventNumber, simulationTime, 0.0, eventNumber == animationTimeOriginEventNumber ? 0.0 : animationCoordinateSystem.getAnimationTime(eventNumber));
    }

    /**
	 * Returns the current simulation time.
	 */
	public BigDecimal getCurrentSimulationTime() {
		return currentAnimationPosition.getSimulationTime();
	}

    /**
     * Returns an animation position representing the simulation time.
     */
    private AnimationPosition getAnimationPositionForSimulationTime(BigDecimal simulationTime) {
        double animationTime = animationCoordinateSystem.getAnimationTimeForSimulationTime(simulationTime);
        long eventNumber = animationCoordinateSystem.getLastEventNumberNotAfterAnimationTime(animationTime);
        double frameRelativeAnimationTime = animationTime - animationCoordinateSystem.getAnimationTime(eventNumber);
        AnimationPosition animationPosition = new AnimationPosition(eventNumber, simulationTime, frameRelativeAnimationTime, animationTime);
        return animationPosition;
    }

	/**
	 * Returns the current animation time relative to the origin.
	 */
	public double getCurrentAnimationTime() {
		return currentAnimationPosition.getOriginRelativeAnimationTime();
	}

	/**
	 * Returns an animation position representing the given origin relative animation time.
	 */
    private AnimationPosition getAnimationPositionForAnimationTime(double animationTime) {
        long eventNumber = animationCoordinateSystem.getLastEventNumberNotAfterAnimationTime(animationTime);
        BigDecimal simulationTime = animationCoordinateSystem.getSimulationTimeForAnimationTime(animationTime);
        return new AnimationPosition(eventNumber, simulationTime, null, animationTime);
    }

	/**
	 * Returns the current real time in seconds.
	 */
	public double getCurrentRealTime() {
		return System.currentTimeMillis() / 1000.0;
	}

	/**
	 * Changes the scaling factor between animation time and real time while keeping the current animation position.
	 */
	private void setRealTimeToAnimationTimeScale(double realTimeToAnimationTimeScale) {
		this.realTimeToAnimationTimeScale = realTimeToAnimationTimeScale;
        lastStartRealTime = getCurrentRealTime();
        lastStartAnimationPosition = currentAnimationPosition;
        timerQueue.resetTimer(animationTimer);
	}

	/**
	 * Returns the animation time for the given real time.
	 */
	private double getAnimationTimeForRealTime(double realTime) {
		return lastStartAnimationPosition.getOriginRelativeAnimationTime() + (isPlayingForward ? 1 : -1) * (realTime - lastStartRealTime) * realTimeToAnimationTimeScale;
	}

	/**
	 * Adds a new animation listener to the list of listeners.
	 */
	public void addAnimationListener(IAnimationListener listener) {
		animationListeners.add(listener);
	}

    /**
     * Removes an animation listener from the list of listeners.
     */
    public void removeAnimationListener(IAnimationListener listener) {
        animationListeners.remove(listener);
    }

	/**
	 * Starts animation backward from the current animation position.
	 * Asynchronous operation.
	 */
	public void playAnimationBackward() {
		startAnimation(false);
	}

	/**
	 * Starts animation backward from the current animation position and stops at the previous event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationBackwardToPreviousEvent() {
		long eventNumber = getCurrentEventNumber();
        AnimationPosition animationPosition = getAnimationPositionForEventNumber(eventNumber);
		animationPosition = currentAnimationPosition.equals(animationPosition) ? getAnimationPositionForEventNumber(eventNumber - 1) : animationPosition;
		startAnimation(false, animationPosition);
	}

    private void stepAnimationTowardToSiblingAnimationChange(boolean forward) {
        final double currentOriginRelativeAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
        int beginOrderedIndex = (int)BinarySearchUtils.binarySearch(new KeyComparator() {
            public int compareTo(long index) {
                return (int)Math.signum(currentOriginRelativeAnimationTime - beginAnimationTimeOrderedPrimitives.get((int)index).getBeginAnimationPosition().getOriginRelativeAnimationTime());
            }
        }, beginAnimationTimeOrderedPrimitives.size(), forward ? BoundKind.LOWER_BOUND : BoundKind.UPPER_BOUND);
        int endOrderedIndex = (int)BinarySearchUtils.binarySearch(new KeyComparator() {
            public int compareTo(long index) {
                return (int)Math.signum(currentOriginRelativeAnimationTime - endAnimationTimeOrderedPrimitives.get((int)index).getEndAnimationPosition().getOriginRelativeAnimationTime());
            }
        }, endAnimationTimeOrderedPrimitives.size(), forward ? BoundKind.LOWER_BOUND : BoundKind.UPPER_BOUND);
        if (!forward) {
            beginOrderedIndex--;
            endOrderedIndex--;
        }
        // find an animation primitive that strictly begins or ends in the past and has the highest begin or end animation position respectively
        while (true) {
            boolean beginOrderedIndexValid = beginOrderedIndex >= 0 && beginOrderedIndex < beginAnimationTimeOrderedPrimitives.size();
            boolean endOrderedIndexValid = endOrderedIndex >= 0 && endOrderedIndex < endAnimationTimeOrderedPrimitives.size();
            if (!beginOrderedIndexValid && !endOrderedIndexValid)
                break;
            AnimationPosition beginAnimationPosition = beginOrderedIndexValid ? beginAnimationTimeOrderedPrimitives.get(beginOrderedIndex).getBeginAnimationPosition() : null;
            AnimationPosition endAnimationPosition = endOrderedIndexValid ? endAnimationTimeOrderedPrimitives.get(endOrderedIndex).getEndAnimationPosition() : null;
            if ((beginOrderedIndexValid && !endOrderedIndexValid) ||
                (beginOrderedIndexValid && endOrderedIndexValid && (forward ? beginAnimationPosition.compareTo(endAnimationPosition) <= 0 : beginAnimationPosition.compareTo(endAnimationPosition) >= 0)))
            {
                if (forward ? currentOriginRelativeAnimationTime < beginAnimationPosition.getOriginRelativeAnimationTime() : beginAnimationPosition.getOriginRelativeAnimationTime() < currentOriginRelativeAnimationTime) {
                    startAnimation(forward, beginAnimationPosition);
                    return;
                }
                if (forward)
                    beginOrderedIndex++;
                else
                    beginOrderedIndex--;
            }
            if ((!beginOrderedIndexValid && endOrderedIndexValid) ||
                (beginOrderedIndexValid && endOrderedIndexValid && (forward ? beginAnimationPosition.compareTo(endAnimationPosition) >= 0 : beginAnimationPosition.compareTo(endAnimationPosition) <= 0)))
            {
                if (forward ? currentOriginRelativeAnimationTime < endAnimationPosition.getOriginRelativeAnimationTime() : endAnimationPosition.getOriginRelativeAnimationTime() < currentOriginRelativeAnimationTime) {
                    startAnimation(forward, endAnimationPosition);
                    return;
                }
                if (forward)
                    endOrderedIndex++;
                else
                    endOrderedIndex--;
            }
        }
    }

    /**
     * Starts animation backward from the current animation position and stops at the previous visible animation change.
     * Asynchronous operation.
     */
    public void stepAnimationBackwardToPreviousAnimationChange() {
        stepAnimationTowardToSiblingAnimationChange(false);
    }

    /**
	 * Starts animation forward from the current animation position.
	 * Asynchronous operation.
	 */
	public void playAnimationForward() {
		startAnimation(true);
	}

	/**
	 * Starts animation forward from the current animation position and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationForwardToNextEvent() {
		startAnimation(true, getAnimationPositionForEventNumber(getCurrentEventNumber() + 1));
	}

    /**
     * Starts animation forward from the current animation position and stops at the next visible animation change.
     * Asynchronous operation.
     */
	public void stepAnimationForwardToNextAnimationChange() {
	    stepAnimationTowardToSiblingAnimationChange(true);
	}

    /**
     * Starts a temporarily stopped animation.
     */
    public void startAnimation() {
        startAnimation(isPlayingForward, stopAnimationPosition);
    }

    /**
     * Starts animation in the given direction.
     */
    public void startAnimation(boolean isPlayingForward) {
        startAnimation(isPlayingForward, new AnimationPosition());
    }

    /**
     * Starts animation in the given direction and stops at the given animation position.
     */
    public void startAnimation(boolean isPlayingForward, AnimationPosition stopAnimationPosition) {
        this.isPlayingForward = isPlayingForward;
        this.stopAnimationPosition = stopAnimationPosition;
        startAnimationInternal();
    }

	/**
	 * Temporarily stops animation.
	 */
	public void stopAnimation() {
		stopAnimationInternal();
	}

	/**
	 * Stops animation and goes to the given animation position.
	 */
	public void gotoAnimationPosition(AnimationPosition animationPosition) {
        stopAnimationInternal();
		setCurrentAnimationPosition(animationPosition);
	}

    /**
     * Stops animation and goes to the initially shown animation position.
     */
    public void gotoInitialAnimationPosition() {
        long firstEventNumber = animationCoordinateSystem.getFirstEventNumber();
        long lastEventNumber = animationCoordinateSystem.getLastEventNumber();
        if (firstEventNumber != -1 && lastEventNumber != -1 && firstEventNumber != lastEventNumber)
            gotoFirstEventAnimationPosition();
        else if (firstEventNumber != -1)
            gotoBeginAnimationPosition();
    }

	/**
     * Stops animation and jumps to the very begin.
	 */
	public void gotoBeginAnimationPosition() {
        gotoEventNumber(0);
	}

    /**
     * Stops animation and jumps to the first event.
     */
    public void gotoFirstEventAnimationPosition() {
        gotoEventNumber(1);
    }

	/**
     * Stops animation and jumps to the very end.
	 */
	public void gotoEndAnimationPosition() {
		gotoAnimationPosition(getEndAnimationPosition());
	}

    /**
     * Goes to the given event number without animating.
     */
    public void gotoEventNumber(long eventNumber) {
        stopAnimationInternal();
        if (eventNumberToFrameRelativeEndAnimationTime.get(eventNumber) == null)
            relocateAnimationTimeOrigin(eventNumber);
        setCurrentAnimationPosition(getAnimationPositionForEventNumber(eventNumber));
    }

	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(BigDecimal simulationTime) {
        stopAnimationInternal();
        setCurrentAnimationPosition(getAnimationPositionForSimulationTime(simulationTime));
	}

	/**
	 * Goes to the given origin relative animation time without animating.
	 */
	public void gotoAnimationTime(double animationTime) {
        stopAnimationInternal();
        setCurrentAnimationPosition(getAnimationPositionForAnimationTime(animationTime));
	}

	/**
	 * Refreshes the animation according to the current animation position.
	 */
	public void refreshAnimation() {
		if (!animationCanvas.isDisposed() && currentAnimationPosition.isCompletelySpecified()) {
            updateAnimationModel();
            if (debug)
                System.out.println("Displaying " + activeAnimationPrimitives.size() + " primitives at -> " + currentAnimationPosition);
            getRootFigure().getLayoutManager().layout(getRootFigure());
            getRootFigure().validate();
            for (IAnimationPrimitive animationPrimitive : activeAnimationPrimitives)
                animationPrimitive.refreshAnimation(currentAnimationPosition);
		}
	}

	/**
	 * The animation timer is responsible to change the animation position as real time elapses.
	 */
	private class AnimationTimer extends Timer {
		public AnimationTimer() {
			super(100, true, true);
		}

		/**
		 * This method is called every 100 milliseconds when the animation is running.
		 * It refreshes animation model and graphics state according to the new position.
		 */
		@Override
        public void run() {
			try {
			    if (animationCanvas.isDisposed())
			        timerQueue.removeTimer(animationTimer);
			    if (isRunning) {
                    double animationTime = getAnimationTimeForRealTime(getCurrentRealTime());
                    loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(null, null, null, animationTime));
                    if (isPlayingForward && getEndAnimationPosition().getOriginRelativeAnimationTime() <= animationTime)
                        gotoEndAnimationPosition();
                    else if (!isPlayingForward && animationTime <= getBeginAnimationPosition().getOriginRelativeAnimationTime())
                        gotoBeginAnimationPosition();
                    else if (stopAnimationPosition.isCompletelySpecified() && (isPlayingForward ? stopAnimationPosition.getOriginRelativeAnimationTime() <= animationTime : stopAnimationPosition.getOriginRelativeAnimationTime() >= animationTime))
                        gotoAnimationPosition(stopAnimationPosition);
                    else
                        setCurrentAnimationPosition(getAnimationPositionForAnimationTime(animationTime));
			    }
			    else if (animationCanvas != null)
			        refreshAnimation();
			}
			catch (RuntimeException e) {
				MessageDialog.openError(null, "Internal error", e.toString());
				throw e;
			}
		}
	};

    /**
     * Immediately starts animation from the current animation position in the current direction.
     */
	private void startAnimationInternal() {
		if ((isPlayingForward && !isAtEndAnimationPosition()) || (!isPlayingForward && !isAtBeginAnimationPosition())) {
	        setRunning(true);
			timerQueue.resetTimer(animationTimer);
			lastStartRealTime = getCurrentRealTime();
			lastStartAnimationPosition = currentAnimationPosition;
		}
	}

    /**
	 * Immediately stops animation at the current animation position.
	 */
	private void stopAnimationInternal() {
		setRunning(false);
		stopAnimationPosition = new AnimationPosition();
	}

    /**
     * Updates the animation model according to the current animation position.
     * This will result in a bunch of activate/deactivate calls on the
     * appropriate animation primitives. The idea is to go through the animation
     * primitives from the old animation time to the new animation time in both
     * ordered animation primitive arrays at the same time and call
     * activate/deactivate in order.
     */
	private void updateAnimationModel() {
		final double oldAnimationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getOriginRelativeAnimationTime() : Double.NEGATIVE_INFINITY;
		double newAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
		boolean forward = newAnimationTime >= oldAnimationTime;
		int beginOrderedIndex = (int)BinarySearchUtils.binarySearch(new KeyComparator() {
            public int compareTo(long index) {
                return (int)Math.signum(oldAnimationTime - beginAnimationTimeOrderedPrimitives.get((int)index).getBeginAnimationPosition().getOriginRelativeAnimationTime());
            }
		}, beginAnimationTimeOrderedPrimitives.size(), forward ? BoundKind.LOWER_BOUND : BoundKind.UPPER_BOUND);
		int endOrderedIndex = (int)BinarySearchUtils.binarySearch(new KeyComparator() {
            public int compareTo(long index) {
                return (int)Math.signum(oldAnimationTime - endAnimationTimeOrderedPrimitives.get((int)index).getEndAnimationPosition().getOriginRelativeAnimationTime());
            }
		}, endAnimationTimeOrderedPrimitives.size(), forward ? BoundKind.LOWER_BOUND : BoundKind.UPPER_BOUND);
		if (!forward) {
			beginOrderedIndex--;
			endOrderedIndex--;
		}
		double previousBeginAnimationTime = forward ? Double.NEGATIVE_INFINITY : Double.POSITIVE_INFINITY;
		double previousEndAnimationTime = previousBeginAnimationTime;
		while (true) {
			boolean beginOrderedIndexValid = beginOrderedIndex >= 0 && beginOrderedIndex < beginAnimationTimeOrderedPrimitives.size();
			boolean endOrderedIndexValid = endOrderedIndex >= 0 && endOrderedIndex < endAnimationTimeOrderedPrimitives.size();
			if (!beginOrderedIndexValid && !endOrderedIndexValid)
				break;
			IAnimationPrimitive beginOrderedAnimationPrimitive = beginOrderedIndexValid ? beginAnimationTimeOrderedPrimitives.get(beginOrderedIndex) : null;
			IAnimationPrimitive endOrderedAnimationPrimitive = endOrderedIndexValid ? endAnimationTimeOrderedPrimitives.get(endOrderedIndex) : null;
			double beginAnimationTime = beginOrderedIndexValid ? beginOrderedAnimationPrimitive.getBeginAnimationPosition().getOriginRelativeAnimationTime() : -1;
			double endAnimationTime = endOrderedIndexValid ? endOrderedAnimationPrimitive.getEndAnimationPosition().getOriginRelativeAnimationTime() : -1;
			// check correct ordering
			if (debug) {
				if (beginOrderedIndexValid && (forward ? beginAnimationTime < previousBeginAnimationTime : beginAnimationTime > previousBeginAnimationTime))
					throw new RuntimeException("Begin ordered animation primitives are not correctly ordered");

				if (endOrderedIndexValid && (forward ? endAnimationTime < previousEndAnimationTime : endAnimationTime > previousEndAnimationTime))
					throw new RuntimeException("End ordered animation primitives are not correctly ordered");
			}
			// NOTE: begin animation position is inclusive with respect to being activated
			if (forward ? beginAnimationTime > newAnimationTime : beginAnimationTime <= newAnimationTime) {
				beginOrderedIndex = -1;
				beginOrderedIndexValid = false;
				beginOrderedAnimationPrimitive = null;
			}
            // NOTE: end animation position is exclusive with respect to being activated
			if (forward ? endAnimationTime > newAnimationTime : endAnimationTime <= newAnimationTime) {
				endOrderedIndex = -1;
				endOrderedIndexValid = false;
				endOrderedAnimationPrimitive = null;
			}
			if (forward) {
				if (beginOrderedIndexValid && (!endOrderedIndexValid || beginAnimationTime <= endAnimationTime)) {
					if (!activeAnimationPrimitives.contains(beginOrderedAnimationPrimitive)) {
					    if (debug)
					        System.out.println("Forward animation activate: " + beginOrderedAnimationPrimitive);
						beginOrderedAnimationPrimitive.activate();
						activeAnimationPrimitives.add(beginOrderedAnimationPrimitive);
					}
					beginOrderedIndex++;
				}
				// compare using > to allow activations to happen before deactivations having the same time
				if (endOrderedIndexValid  && (!beginOrderedIndexValid || endAnimationTime < beginAnimationTime)) {
					if (activeAnimationPrimitives.contains(endOrderedAnimationPrimitive)) {
					    if (debug)
					        System.out.println("Forward animation deactivate: " + endOrderedAnimationPrimitive);
						endOrderedAnimationPrimitive.deactivate();
						activeAnimationPrimitives.remove(endOrderedAnimationPrimitive);
					}
					endOrderedIndex++;
				}
			}
			else {
				if (endOrderedIndexValid && (!beginOrderedIndexValid || endAnimationTime >= beginAnimationTime)) {
					if (!activeAnimationPrimitives.contains(endOrderedAnimationPrimitive)) {
					    if (debug)
					        System.out.println("Backward animation activate: " + endOrderedAnimationPrimitive);
						endOrderedAnimationPrimitive.activate();
						activeAnimationPrimitives.add(endOrderedAnimationPrimitive);
					}
					endOrderedIndex--;
				}
				// compare using > to allow activations to happen before deactivations having the same time
				if (beginOrderedIndexValid && (!endOrderedIndexValid || beginAnimationTime > endAnimationTime)) {
					if (activeAnimationPrimitives.contains(beginOrderedAnimationPrimitive)) {
					    if (debug)
					        System.out.println("Backward animation deactivate: " + beginOrderedAnimationPrimitive);
						beginOrderedAnimationPrimitive.deactivate();
						activeAnimationPrimitives.remove(beginOrderedAnimationPrimitive);
					}
					beginOrderedIndex--;
				}
			}
			previousBeginAnimationTime = beginAnimationTime;
			previousEndAnimationTime = endAnimationTime;
		}
		// check active animation primitives
//		if (debug) {
//			for (IAnimationPrimitive animationPrimitive : beginAnimationTimeOrderedPrimitives) {
//				if ((animationPrimitive.getBeginAnimationPosition().getOriginRelativeAnimationTime() <= getCurrentAnimationTime() &&
//					 getCurrentAnimationTime() <= animationPrimitive.getEndAnimationPosition().getOriginRelativeAnimationTime()) !=
//					 activeAnimationPrimitives.contains(animationPrimitive))
//				{
//					Assert.isTrue(false, "Animation primitive should be active iff its animation time range includes the current animation position");
//				}
//			}
//		}
		modelAnimationPosition = currentAnimationPosition;
	}

	/**
	 * Notifies listeners about a new animation position.
	 */
	private void animationPositionChanged() {
	    long currentTime = System.currentTimeMillis();
	    if (currentTime - lastTimeAnimationPositionChangedNotificationWasSent > 100 || !isRunning) {
	        lastTimeAnimationPositionChangedNotificationWasSent = currentTime;
    		for (IAnimationListener listener : animationListeners)
    			listener.animationPositionChanged(currentAnimationPosition);
	    }
	}

	/**
     * Notifies the listeners when the animation has been stopped or started.
	 */
	private void runningStateChanged() {
		for (IAnimationListener listener : animationListeners)
		    if (isRunning)
		        listener.animationStarted();
		    else
		        listener.animationStopped();
	}

	private void loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
	    ArrayList<IAnimationPrimitive> animationPrimitives = animationPrimitiveProvider.loadAnimationPrimitivesForAnimationPosition(animationPosition);
        animationPrimitives.addAll(beginAnimationTimeOrderedPrimitives);
        animationCoordinateSystem.setAnimationPositions(calculateAnimationTimes(animationPrimitives));
        calculateBeginAnimationTimeOrderedPrimitives(animationPrimitives);
        calculateEndAnimationTimeOrderedPrimitives(animationPrimitives);
	}

    /**
     * Calculates the animation times for each IAnimationPrimitive's begin and end AnimationPositions.
     *
     * It first collects the partially filled AnimationPositions from the IAnimationPrimitives.
     * Then sorts these AnimationPositions using their simulation times, event numbers and relative animation times.
     * Applies the non-linear animation time delta transformation to subsequent AnimationPositions and sets
     * the animation times by summing them up. Finally sorts the internal IAnimationPrimitive lists that will be
     * used during the animation using the begin and end animation times.
     */
    private ArrayList<AnimationPosition> calculateAnimationTimes(ArrayList<IAnimationPrimitive> animationPrimitives) {
        ArrayList<AnimationPosition> animationPositions = calculateSortedAnimationPositions(animationPrimitives);
        if (debug) {
            System.out.println("*** ANIMATION PRIMITIVES BEFORE ASSIGNING ANIMATION TIMES ***");
            for (IAnimationPrimitive animationPrimitive : animationPrimitives)
                System.out.println(animationPrimitive);
        }
        calculateAnimationTimesForSortedAnimationPositions(animationPositions);
        if (debug)
            System.out.println("*** ANIMATION PRIMITIVES AFTER ASSIGNING ANIMATION TIMES ***");
        for (IAnimationPrimitive animationPrimitive : animationPrimitives) {
            Assert.isTrue(animationPrimitive.getBeginAnimationPosition().getSimulationTime() != null);
            Assert.isTrue(animationPrimitive.getBeginAnimationPosition().getOriginRelativeAnimationTime() != null);
            Assert.isTrue(animationPrimitive.getEndAnimationPosition().getSimulationTime() != null);
            Assert.isTrue(animationPrimitive.getEndAnimationPosition().getOriginRelativeAnimationTime() != null);
            // TODO: should not need to set the animation time duration?!
            // TODO: maybe, sometimes it is calculated this late? (e.g. duration is specified in terms of event numbers, simulation times and frame relative animation times)
            if (animationPrimitive.getAnimationTimeDuration() == -1) {
                AbstractAnimationPrimitive abstractAnimationPrimitive = (AbstractAnimationPrimitive)animationPrimitive;
                abstractAnimationPrimitive.setAnimationTimeDuration(abstractAnimationPrimitive.getOriginRelativeEndAnimationTime() - abstractAnimationPrimitive.getOriginRelativeBeginAnimationTime());
            }
            if (debug)
                System.out.println(animationPrimitive);
        }
        return animationPositions;
    }

    @SuppressWarnings("unchecked")
    private ArrayList<AnimationPosition> calculateSortedAnimationPositions(ArrayList<IAnimationPrimitive> animationPrimitives) {
        MultiValueMap animationPositionsMap = new MultiValueMap();
        AnimationPosition zeroAnimationPosition = new AnimationPosition(null, BigDecimal.getZero(), 0.0, null);
        animationPositionsMap.put(zeroAnimationPosition, zeroAnimationPosition);
        ArrayList<AnimationPosition> uniqueAnimationPositions = new ArrayList<AnimationPosition>();
        for (IAnimationPrimitive animationPrimitive : animationPrimitives) {
            addAnimationPositionToBag(animationPositionsMap, uniqueAnimationPositions, animationPrimitive.getBeginAnimationPosition());
            addAnimationPositionToBag(animationPositionsMap, uniqueAnimationPositions, animationPrimitive.getEndAnimationPosition());
        }
        Collections.sort(uniqueAnimationPositions, animationPositionComparator);
        ArrayList<AnimationPosition> animationPositions = new ArrayList<AnimationPosition>();
        for (AnimationPosition uniqueAnimationPosition : uniqueAnimationPositions)
            animationPositions.addAll(animationPositionsMap.getCollection(uniqueAnimationPosition));
        if (debug) {
            System.out.println("*** SORTED ANIMATION POSITIONS ***");
            for (int i = 0; i < animationPositions.size(); i++) {
                AnimationPosition animationPosition = animationPositions.get(i);
                System.out.println(animationPosition);
                if (i < animationPositions.size() - 1)
                    Assert.isTrue(animationPositionComparator.compare(animationPosition, animationPositions.get(i + 1)) <= 0);
            }
        }
        return animationPositions;
    }

    @SuppressWarnings("unchecked")
    private void addAnimationPositionToBag(MultiValueMap animationPositionBag, ArrayList<AnimationPosition> uniqueAnimationPositions, AnimationPosition animationPosition) {
        if (!animationPositionBag.containsKey(animationPosition)) {
            animationPositionBag.put(animationPosition, animationPosition);
            uniqueAnimationPositions.add(animationPosition);
        }
        else
            animationPositionBag.getCollection(animationPosition).add(animationPosition);
    }

    private void calculateAnimationTimesForSortedAnimationPositions(ArrayList<AnimationPosition> animationPositions) {
        double animationTime = 0;
        double animationTimeDelta = 0;
        AnimationPosition previousAnimationPosition = null;
        for (AnimationPosition animationPosition : animationPositions) {
            if (previousAnimationPosition != null) {
                animationTimeDelta = getAnimationTimeDelta(previousAnimationPosition, animationPosition);
                Assert.isTrue(animationTimeDelta >= 0);
                if (!Double.isInfinite(animationTimeDelta))
                    animationTime += animationTimeDelta;
            }
            Assert.isTrue(!Double.isNaN(animationTime) && !Double.isInfinite(animationTime));
            previousAnimationPosition = animationPosition;
            if (animationPosition.getEventNumber() >= animationTimeOriginEventNumber)
                break;
        }
        animationTime *= -1;
        if (debug)
            System.out.println("Animation time of first animation position: " + animationTime);
        previousAnimationPosition = null;
        for (AnimationPosition animationPosition : animationPositions) {
            if (previousAnimationPosition != null) {
                Assert.isTrue(previousAnimationPosition.getEventNumber() < animationPosition.getEventNumber() || previousAnimationPosition.getSimulationTime().less(animationPosition.getSimulationTime()) || previousAnimationPosition.getFrameRelativeAnimationTime() <= animationPosition.getFrameRelativeAnimationTime() || previousAnimationPosition.equals(animationPosition));
                animationTimeDelta = getAnimationTimeDelta(previousAnimationPosition, animationPosition);
                Assert.isTrue(animationTimeDelta >= 0);
                animationTime += animationTimeDelta;
            }
            Assert.isTrue(!Double.isNaN(animationTime));
            animationPosition.setOriginRelativeAnimationTime(animationTime);
            Assert.isTrue(animationPosition.isCompletelySpecified());
            previousAnimationPosition = animationPosition;
        }
        if (debug) {
            System.out.println("*** ANIMATION POSITIONS AFTER ASSIGNING ANIMATION TIMES ***");
            for (AnimationPosition animationPosition : animationPositions)
                System.out.println(animationPosition);
        }
    }

    private void calculateBeginAnimationTimeOrderedPrimitives(ArrayList<IAnimationPrimitive> animationPrimitives) {
        beginAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>(animationPrimitives);
        Collections.sort(beginAnimationTimeOrderedPrimitives, new Comparator<IAnimationPrimitive>() {
            public int compare(IAnimationPrimitive animationPrimitive1, IAnimationPrimitive animationPrimitive2) {
                double animationTime1 = animationPrimitive1.getBeginAnimationPosition().getOriginRelativeAnimationTime();
                double animationTime2 = animationPrimitive2.getBeginAnimationPosition().getOriginRelativeAnimationTime();
                return (int)Math.signum(animationTime1 - animationTime2);
            }
        });
        if (debug) {
            System.out.println("*** BEGIN ANIMATION TIME ORDERED ANIMATION PRIMITIVES ***");
            for (IAnimationPrimitive animationPrimitive : beginAnimationTimeOrderedPrimitives)
                System.out.println(animationPrimitive);
        }
    }

    private double getAnimationTimeDelta(AnimationPosition previousAnimationPosition, AnimationPosition animationPosition) {
        BigDecimal simulationTime = animationPosition.getSimulationTime();
        BigDecimal previousSimulationTime = previousAnimationPosition.getSimulationTime();
        Double frameRelativeAnimationTime = animationPosition.getFrameRelativeAnimationTime();
        Double previousFrameRelativeAnimationTime = previousAnimationPosition.getFrameRelativeAnimationTime();
        Long eventNumber = animationPosition.getEventNumber();
        Long previousEventNumber = previousAnimationPosition.getEventNumber();
        if (previousEventNumber.equals(eventNumber) && previousSimulationTime.equals(simulationTime)) {
            if (Double.isInfinite(frameRelativeAnimationTime) && Double.isInfinite(previousFrameRelativeAnimationTime))
                return 0;
            else
                return frameRelativeAnimationTime - previousFrameRelativeAnimationTime;
        }
        else {
            double animationTimeDelta = animationCoordinateSystem.getAnimationTimeDelta(simulationTime.subtract(previousSimulationTime).doubleValue());
            if (frameRelativeAnimationTime < 0)
                return animationTimeDelta;
            else
                return frameRelativeAnimationTime + animationTimeDelta;
        }
    }

    private void calculateEndAnimationTimeOrderedPrimitives(ArrayList<IAnimationPrimitive> animationPrimitives) {
        endAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>(animationPrimitives);
        Collections.sort(endAnimationTimeOrderedPrimitives, new Comparator<IAnimationPrimitive>() {
            public int compare(IAnimationPrimitive animationPrimitive1, IAnimationPrimitive animationPrimitive2) {
                double animationTime1 = animationPrimitive1.getEndAnimationPosition().getOriginRelativeAnimationTime();
                double animationTime2 = animationPrimitive2.getEndAnimationPosition().getOriginRelativeAnimationTime();
                return (int)Math.signum(animationTime1 - animationTime2);
            }
        });
        if (debug) {
            System.out.println("*** END ANIMATION TIME ORDERED ANIMATION PRIMITIVES ***");
            for (IAnimationPrimitive animationPrimitive : endAnimationTimeOrderedPrimitives)
                System.out.println(animationPrimitive);
        }
    }
}

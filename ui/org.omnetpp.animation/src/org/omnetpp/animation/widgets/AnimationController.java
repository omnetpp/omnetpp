/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.collections.map.MultiValueMap;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.animation.primitives.AbstractAnimationPrimitive;
import org.omnetpp.animation.primitives.BubbleAnimation;
import org.omnetpp.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.animation.primitives.CreateGateAnimation;
import org.omnetpp.animation.primitives.CreateModuleAnimation;
import org.omnetpp.animation.primitives.HandleMessageAnimation;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.primitives.ModuleMethodCallAnimation;
import org.omnetpp.animation.primitives.ScheduleMessageAnimation;
import org.omnetpp.animation.primitives.ScheduleSelfMessageAnimation;
import org.omnetpp.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.animation.primitives.SendDirectAnimation;
import org.omnetpp.animation.primitives.SendMessageAnimation;
import org.omnetpp.animation.primitives.SendSelfMessageAnimation;
import org.omnetpp.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.common.engine.BigDecimal;
import org.omnetpp.common.eventlog.EventLogConnection;
import org.omnetpp.common.eventlog.EventLogGate;
import org.omnetpp.common.eventlog.EventLogInput;
import org.omnetpp.common.eventlog.EventLogInput.TimelineMode;
import org.omnetpp.common.eventlog.EventLogMessage;
import org.omnetpp.common.eventlog.EventLogModule;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.Timer;
import org.omnetpp.common.util.TimerQueue;
import org.omnetpp.eventlog.engine.BeginSendEntry;
import org.omnetpp.eventlog.engine.BubbleEntry;
import org.omnetpp.eventlog.engine.ConnectionCreatedEntry;
import org.omnetpp.eventlog.engine.ConnectionDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.EndSendEntry;
import org.omnetpp.eventlog.engine.EventEntry;
import org.omnetpp.eventlog.engine.EventLogEntry;
import org.omnetpp.eventlog.engine.GateCreatedEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.KeyframeEntry;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.ned.model.DisplayString;

/**
 * <p>
 * The animation controller is responsible for managing the animation primitives
 * while it is playing the animation. The animation can be played in both
 * forward and backward direction. The controller keeps track of the active
 * animation primitives as real time goes by and calls the appropriate
 * activate/deactivate functions as needed. It is also possible to immediately
 * go to animation positions, event numbers or simulation times.
 * </p>
 * <p>
 * Animation primitives are loaded lazily based on the current animation
 * position. At any given moment animation primitives are loaded for a
 * continuous range of events plus the extra events marked in the corresponding
 * eventlog keyframes.
 * </p>
 * <p>
 * The animation controller ensures that when the eventlog file is appended, the
 * animation that is already done by the previous version of the file will not
 * change. In other words it is not allowed to animate into the not completely
 * known future.
 * </p>
 *
 * @author levy
 */
public class AnimationController {
	private final static boolean debug = false;

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
    private AnimationSimulation simulation;

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
     * callback during animation. This is where the animation is right now. It
     * may be different from the live simulation position if it is also running
     * and the handling of the last message has not yet finished or the eventlog
     * file is not yet flushed.
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
     * The real time when the animation was last started or continued.
     */
    private double lastStartRealTime;

    /**
     * This multiplier controls the speed of the animation, it defaults to 1.
     */
    private double realTimeToAnimationTimeScale;

    /**
     * The widget used to display the animation figures.
     */
    private AnimationCanvas animationCanvas;

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
     * The eventlog input that is used to construct the animation primitives.
     */
    private EventLogInput eventLogInput;

    /**
     * The coordinate system that maps between animation times, simulation times
     * and event numbers.
     */
    private AnimationCoordinateSystem coordinateSystem;

    /**
     * Last time the animation position changed notification was sent (in
     * milliseconds). This helps to avoid sending notifications too often.
     */
    private long lastTimeAnimationPositionChangedNotificationWasSent;

    /**
     * The parameters that specifies what is animated and how.
     */
    private AnimationParameters animationParameters;

    /**
     * An event number specifying the current origin of the animation time
     * coordinate system. The value <code>null</code> means the coordinate
     * system is not yet specified. The value must be greater than or equal to
     * <code>0</code>.
     */
    private Long animationTimeOriginEventNumber;

    /**
     * TODO:
     */
    private Map<Long, Double> eventNumberToFrameRelativeAnimationTime;

    /**
     * This is a special purpose comparator for animation positions. It uses the
     * event numbers when the simulation times are equal. Otherwise it uses the
     * relative animation times.
     */
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
                IEvent event = eventLogInput.getEventLog().getLastEventNotAfterSimulationTime(simulationTime1);
                long eventNumber = event.getEventNumber();
                BigDecimal simulationTime = event.getSimulationTime();
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

	public AnimationController(AnimationCanvas animationCanvas, EventLogInput eventLogInput) {
		this.animationCanvas = animationCanvas;
        this.eventLogInput = eventLogInput;
        this.isPlayingForward = true;
        this.isRunning = false;
        this.realTimeToAnimationTimeScale = 1;
        this.timerQueue = new TimerQueue();
        this.timerQueue.start();
        this.animationTimer = new AnimationTimer();
        this.simulation = null;
        this.animationParameters = new AnimationParameters();
        clearInternalState();
        IEvent firstEvent = eventLogInput.getEventLog().getFirstEvent();
        if (firstEvent != null && firstEvent.getNextEvent() != null)
            gotoFirstEventAnimationPosition();
        else if (firstEvent != null)
            gotoBeginAnimationPosition();
	}

    public AnimationCoordinateSystem getAnimationCoordinateSystem() {
        return coordinateSystem;
    }

    public AnimationParameters getAnimationParameters() {
        return animationParameters;
    }

    public void setAnimationParameters(AnimationParameters animationParameters) {
        this.animationParameters = animationParameters;
    }

	/**
	 * Reloads the animation primitives.
	 */
	public void clearInternalState() {
        figureMap = new HashMap<Object, Object>();
        coordinateSystem = new AnimationCoordinateSystem(eventLogInput, new ArrayList<AnimationPosition>());
        currentAnimationPosition = new AnimationPosition();
        modelAnimationPosition = new AnimationPosition();
        stopAnimationPosition = new AnimationPosition();
        lastStartAnimationPosition = new AnimationPosition();
        beginAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        endAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        activeAnimationPrimitives = new ArrayList<IAnimationPrimitive>();
        eventNumberToFrameRelativeAnimationTime = new HashMap<Long, Double>();
        for (Timer timer : timerQueue.getTimers())
            if (timer != animationTimer)
                timerQueue.removeTimer(timer);
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
	 * Returns the C++ wrapper object.
	 */
    public EventLogInput getEventLogInput() {
        return eventLogInput;
    }


    /**
     * Returns the canvas on which the animation figures will be drawn.
     */
	public AnimationCanvas getAnimationCanvas() {
		return animationCanvas;
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
	public AnimationSimulation getAnimationSimulation() {
		return simulation;
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
		this.isRunning = isRunning;
		runningStateChanged();
	}

	/**
	 * Returns the current animation mode.
	 */
    public TimelineMode getAnimationMode() {
        return eventLogInput.getTimelineMode();
    }

    /**
     * Changes the current animation mode.
     */
    public void setAnimationMode(TimelineMode timelineMode) {
        eventLogInput.setTimelineMode(timelineMode);
        long eventNumber = getCurrentEventNumber();
        lastStartRealTime = getCurrentRealTime();
        gotoEventNumber(eventNumber);
        lastStartAnimationPosition = currentAnimationPosition;
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
            // TODO: plus some extra defined by origin relative?
            currentAnimationPosition.setFrameRelativeAnimationTime(eventNumberToFrameRelativeAnimationTime.get(currentAnimationPosition.getEventNumber()));
        }
        refreshAnimation();
        animationPositionChanged();
    }

	/**
	 * Checks if the current animation position is at the very beginning.
	 */
	public boolean isAtBeginAnimationPosition() {
		return animationTimeOriginEventNumber == 0 && currentAnimationPosition.getOriginRelativeAnimationTime() == 0;
	}

    /**
     * Checks if the current animation position is at the very beginning.
     */
    public boolean isAtFirstEventAnimationPosition() {
        return animationTimeOriginEventNumber == 1 && currentAnimationPosition.getOriginRelativeAnimationTime() == 0;
    }

	/**
	 * Checks if the current animation position is at the very end.
	 */
	public boolean isAtEndAnimationPosition() {
	    // TODO: something like this
//      IEvent lastEvent = eventLogInput.getEventLog().getLastEvent();
//      loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(lastEvent.getEventNumber(), lastEvent.getSimulationTime(), null, null));
//      IAnimationPrimitive lastAnimationPrimitive = endAnimationTimeOrderedPrimitives.get(endAnimationTimeOrderedPrimitives.size() - 1);
//    return currentAnimationPosition.equals(lastAnimationPrimitive.getEndAnimationPosition());
	    return false;
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
        IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
        BigDecimal simulationTime = event.getSimulationTime();
        if (eventNumber == animationTimeOriginEventNumber)
            return new AnimationPosition(eventNumber, simulationTime, eventNumberToFrameRelativeAnimationTime.get(eventNumber), 0.0);
        else {
            double animationTime = coordinateSystem.getAnimationTime(event);
            return new AnimationPosition(eventNumber, simulationTime, 0.0, animationTime);
        }
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
        double animationTime = coordinateSystem.getAnimationTimeForSimulationTime(simulationTime);
        long eventNumber = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime).getEventNumber();
        AnimationPosition animationPosition = new AnimationPosition(eventNumber, simulationTime, 0.0, animationTime);
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
        long eventNumber = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime).getEventNumber();
        BigDecimal simulationTime = coordinateSystem.getSimulationTimeForAnimationTime(animationTime);
        return new AnimationPosition(eventNumber, simulationTime, null, animationTime);
    }

	/**
	 * Returns the current real time in seconds.
	 */
	private double getCurrentRealTime() {
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
		startAnimationInternal(false, new AnimationPosition());
	}

	/**
	 * Starts animation backward from the current animation position and stops at the previous event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationBackwardToPreviousEvent() {
		long eventNumber = getCurrentEventNumber();
        AnimationPosition animationPosition = getAnimationPositionForEventNumber(eventNumber);
		animationPosition = currentAnimationPosition.equals(animationPosition) ? getAnimationPositionForEventNumber(eventNumber - 1) : animationPosition;
		startAnimationInternal(false, animationPosition);
	}

    /**
     * Starts animation backward from the current animation position and stops at the previous visible animation change.
     * Asynchronous operation.
     */
    public void stepAnimationBackwardToPreviousAnimationChange() {
        AnimationPosition stopAnimationPosition = null;
        int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
            public double getValue(int index) {
                return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getOriginRelativeAnimationTime();
            }
        }, endAnimationTimeOrderedPrimitives.size(), currentAnimationPosition.getOriginRelativeAnimationTime(), false);
        // find last inactive animation primitive that ends in the past
        if (endOrderedIndex == endAnimationTimeOrderedPrimitives.size())
            endOrderedIndex--;
        while (endOrderedIndex >= 0) {
            AnimationPosition endAnimationPosition = endAnimationTimeOrderedPrimitives.get(endOrderedIndex).getEndAnimationPosition();
            endOrderedIndex--;
            if (currentAnimationPosition.getOriginRelativeAnimationTime() > endAnimationPosition.getOriginRelativeAnimationTime()) {
                stopAnimationPosition = endAnimationPosition;
                break;
            }
        }
        // find last active animation primitive that starts in the past
        for (IAnimationPrimitive animationPrimitive : activeAnimationPrimitives) {
            AnimationPosition beginAnimationPosition = animationPrimitive.getBeginAnimationPosition();
            if (currentAnimationPosition.getOriginRelativeAnimationTime() > beginAnimationPosition.getOriginRelativeAnimationTime() && beginAnimationPosition.getOriginRelativeAnimationTime() > stopAnimationPosition.getOriginRelativeAnimationTime())
                stopAnimationPosition = beginAnimationPosition;
        }
        startAnimationInternal(false, stopAnimationPosition);
    }

    /**
	 * Starts animation forward from the current animation position.
	 * Asynchronous operation.
	 */
	public void playAnimationForward() {
		startAnimationInternal(true, new AnimationPosition());
	}

	/**
	 * Starts animation forward from the current animation position and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationForwardToNextEvent() {
		startAnimationInternal(true, getAnimationPositionForEventNumber(getCurrentEventNumber() + 1));
	}

    /**
     * Starts animation forward from the current animation position and stops at the next visible animation change.
     * Asynchronous operation.
     */
	public void stepAnimationForwardToNextAnimationChange() {
        AnimationPosition stopAnimationPosition = null;
        int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
            public double getValue(int index) {
                return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
            }
        }, beginAnimationTimeOrderedPrimitives.size(), currentAnimationPosition.getOriginRelativeAnimationTime(), true);
        // find first inactive animation primitive that starts in the future
        while (beginOrderedIndex < beginAnimationTimeOrderedPrimitives.size()) {
            AnimationPosition beginAnimationPosition = beginAnimationTimeOrderedPrimitives.get(beginOrderedIndex).getBeginAnimationPosition();
            beginOrderedIndex++;
            if (currentAnimationPosition.getOriginRelativeAnimationTime() < beginAnimationPosition.getOriginRelativeAnimationTime()) {
                stopAnimationPosition = beginAnimationPosition;
                break;
            }
        }
        // find first active animation primitive that ends in the future
	    for (IAnimationPrimitive animationPrimitive : activeAnimationPrimitives) {
	        AnimationPosition endAnimationPosition = animationPrimitive.getEndAnimationPosition();
            if (currentAnimationPosition.getOriginRelativeAnimationTime() < endAnimationPosition.getOriginRelativeAnimationTime() && endAnimationPosition.getOriginRelativeAnimationTime() < stopAnimationPosition.getOriginRelativeAnimationTime())
	            stopAnimationPosition = endAnimationPosition;
	    }
	    startAnimationInternal(true, stopAnimationPosition);
	}

	/**
	 * Starts animation forward from the current animation position.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void runAnimationToSimulationTime(BigDecimal simulationTime) {
		startAnimationInternal(simulationTime.greaterOrEqual(getCurrentSimulationTime()), new AnimationPosition(null, simulationTime, null, null));
	}

    /**
     * Starts a temporarily stopped animation.
     */
    public void startAnimation() {
        startAnimationInternal(isPlayingForward, new AnimationPosition());
    }

	/**
	 * Temporarily stops animation.
	 */
	public void stopAnimation() {
		stopAnimationInternal();
		refreshAnimation();
	}

	/**
	 * Stops animation and goes to the given animation position.
	 */
	public void gotoAnimationPosition(AnimationPosition animationPosition) {
		stopAnimationInternal();
		timerQueue.resetTimer(animationTimer);
		setCurrentAnimationPosition(animationPosition);
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
	    IEvent lastEvent = eventLogInput.getEventLog().getLastEvent();
	    loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(lastEvent.getEventNumber(), lastEvent.getSimulationTime(), null, null));
	    IAnimationPrimitive lastAnimationPrimitive = endAnimationTimeOrderedPrimitives.get(endAnimationTimeOrderedPrimitives.size() - 1);
		gotoAnimationPosition(lastAnimationPrimitive.getEndAnimationPosition());
	}

    /**
     * Goes to the given event number without animating.
     */
    public void gotoEventNumber(long eventNumber) {
        timerQueue.resetTimer(animationTimer);
        // TODO: do not relocate if already loaded!!!!
        if (eventNumberToFrameRelativeAnimationTime.get(eventNumber) == null)
            relocateAnimationTimeOrigin(eventNumber);
        setCurrentAnimationPosition(getAnimationPositionForEventNumber(eventNumber));
    }

	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(BigDecimal simulationTime) {
		timerQueue.resetTimer(animationTimer);
        setCurrentAnimationPosition(getAnimationPositionForSimulationTime(simulationTime));
	}

	/**
	 * Goes to the given origin relative animation time without animating.
	 */
	public void gotoAnimationTime(double animationTime) {
		timerQueue.resetTimer(animationTimer);
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
			super(20, true, true);
		}

		/**
		 * This method is called every 20 milliseconds when the animation is running.
		 * It refreshes animation model and graphics state according to the new position.
		 */
		@Override
        public void run() {
			try {
			    double animationTime = getAnimationTimeForRealTime(getCurrentRealTime());
			    loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(null, null, null, animationTime));
		        // first check for stop at end
			    // TODO:
//		        if (isPlayingForward && getEndAnimationPosition().isCompletelySpecified() && endAnimationPosition.getOriginRelativeAnimationTime() <= animationTime) {
//		            setCurrentAnimationPosition(endAnimationPosition);
//		            stopAnimationInternal();
//		        }
//		        // stop at begin
//		        else if (!isPlayingForward && getBeginAnimationPosition().isCompletelySpecified() && animationTime <= beginAnimationPosition.getOriginRelativeAnimationTime()) {
//		            setCurrentAnimationPosition(beginAnimationPosition);
//		            stopAnimationInternal();
//		        }
//		        // stop at requested stop position
//		        else if (stopAnimationPosition.isCompletelySpecified() && (isPlayingForward ? stopAnimationPosition.getOriginRelativeAnimationTime() <= animationTime : stopAnimationPosition.getOriginRelativeAnimationTime() >= animationTime)) {
//		            setCurrentAnimationPosition(stopAnimationPosition);
//		            stopAnimationInternal();
//		        }
//		        else
		            setCurrentAnimationPosition(getAnimationPositionForAnimationTime(animationTime));
			}
			catch (RuntimeException e) {
				MessageDialog.openError(null, "Internal error", e.toString());

				throw e;
			}
		}
	};

	/**
	 * Asynchronously starts animation in the given direction.
	 */
	private void startAnimationInternal(boolean forward, AnimationPosition stopAnimationPosition) {
		this.isPlayingForward = forward;
		this.stopAnimationPosition = stopAnimationPosition;
		setRunning(true);
		// TODO:
//		if ((forward && (!endAnimationPosition.isCompletelySpecified() || currentAnimationPosition.compareTo(endAnimationPosition) < 0)) ||
//			(!forward && (!beginAnimationPosition.isCompletelySpecified() || currentAnimationPosition.compareTo(beginAnimationPosition) > 0)))
		{
			timerQueue.resetTimer(animationTimer);
			lastStartRealTime = getCurrentRealTime();
			lastStartAnimationPosition = currentAnimationPosition;
			if (!timerQueue.hasTimer(animationTimer))
				timerQueue.addTimer(animationTimer);
		}
	}

    /**
	 * Immediately stops animation.
	 */
	private void stopAnimationInternal() {
		setRunning(false);
		isPlayingForward = true;
		stopAnimationPosition = new AnimationPosition();
		if (timerQueue.hasTimer(animationTimer))
			timerQueue.removeTimer(animationTimer);
		animationPositionChanged();
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
//	    BigDecimal oldSimulationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getSimulationTime() : BigDecimal.getZero();
		double oldAnimationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getOriginRelativeAnimationTime() : Double.NEGATIVE_INFINITY;
//		BigDecimal newSimulationTime = currentAnimationPosition.getSimulationTime();
		double newAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
		boolean forward = newAnimationTime >= oldAnimationTime;

		int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
		        public double getValue(int index) {
		            return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, beginAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward ? true : false);

//		if (forward)
//			sortEndOrderedAnimationPrimitivesFragment(oldSimulationTime, newSimulationTime);
		int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, endAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward ? true : false);

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

			if (forward ? beginAnimationTime > newAnimationTime : beginAnimationTime <= newAnimationTime) {
				beginOrderedIndex = -1;
				beginOrderedIndexValid = false;
				beginOrderedAnimationPrimitive = null;
			}

			if (forward ? endAnimationTime >= newAnimationTime : endAnimationTime < newAnimationTime) {
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
// TODO: revive
//		if (debug)
//			for (IAnimationPrimitive animationPrimitive : beginOrderedAnimationPrimitives)
//				if ((animationPrimitive.getBeginAnimationPosition().getAnimationTime() <= getAnimationTime() &&
//					 getAnimationTime() <= animationPrimitive.getEndAnimationPosition().getAnimationTime()) !=
//					 activeAnimationPrimitives.contains(animationPrimitive))
//						throw new RuntimeException("Animation primitive should be active iff its animation time range includes the current animation position");

		modelAnimationPosition = currentAnimationPosition;
	}

	/**
	 * Sorts animation primitives having the given end simulation time based on their end animation time.
	 */
	private void sortEndOrderedAnimationPrimitivesFragment(BigDecimal beginSimulationTime, BigDecimal endSimulationTime) {
		int beginSortIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
				public double getValue(int index) {
					return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getSimulationTime().doubleValue();
				}
			}, endAnimationTimeOrderedPrimitives.size(), beginSimulationTime.doubleValue(), true);

		int endSortIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getSimulationTime().doubleValue();
			    }
		    }, endAnimationTimeOrderedPrimitives.size(), endSimulationTime.doubleValue(), false);

		// FIXME: hack to sort the whole thing for now
		beginSortIndex = 0;
		endSortIndex = endAnimationTimeOrderedPrimitives.size();

		if (endSortIndex - beginSortIndex > 1) {
			// copy to temporary array
			IAnimationPrimitive[] endOrderedAnimationPrimitivesFragment = new IAnimationPrimitive[endSortIndex - beginSortIndex];
			for (int i = beginSortIndex; i < endSortIndex; i++)
				endOrderedAnimationPrimitivesFragment[i - beginSortIndex] = endAnimationTimeOrderedPrimitives.get(i);

			// sort based on end animation time
			Arrays.sort(endOrderedAnimationPrimitivesFragment, new Comparator<IAnimationPrimitive>() {
				public int compare(IAnimationPrimitive p1, IAnimationPrimitive p2) {
					double time1 = p1.getEndAnimationPosition().getOriginRelativeAnimationTime();
					double time2 = p2.getEndAnimationPosition().getOriginRelativeAnimationTime();

					if (time1 == time2)
						return 0;
					else if (time1 < time2)
						return -1;
					else
						return 1;
				}
			});

			// copy back to original place
			for (int i = beginSortIndex; i < endSortIndex; i++)
				endAnimationTimeOrderedPrimitives.set(i, endOrderedAnimationPrimitivesFragment[i - beginSortIndex]);
		}
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
			listener.runningStateChanged(isRunning);
	}

	/**
	 * Helper to provide the value for an index on which the binary search is working.
	 */
	private interface IValueProvider {
		public double getValue(int index);
	}

	/**
	 * Binary search among animation primitives. Search key is provided by valueProvider.
	 * If exactly one match is found, then (if first==true) returns that one, otherwise returns the next one.
	 * If more than one equal values are found, returns the first one
	 * (if first==true) or the one after the last one (if first==false) of them.
	 * If none are found, returns the next one.
	 *
	 * The returned index may be used to insert into the list, so valid values are [0, size].
	 * The flag specifies in what order the same values are to be inserted.
	 *
	 * @param valueProvider  provides search key
	 * @param value  search for this value
	 * @param first  return first or last among equal values
	 */
	private int getAnimationPrimitiveIndexForValue(IValueProvider valueProvider, int size, double value, boolean first) {
		int index = -1;
		int left = 0;
		int right = size - 1;

		while (left <= right) {
	        int mid = (int)Math.floor((right + left) / 2);
	        double midValue = valueProvider.getValue(mid);

	        if (midValue == value) {
	        	do {
	        		index = mid;

	        		if (first)
	        			mid--;
	        		else
	        			mid++;
	        	}
	        	while (mid >= 0 && mid < size && valueProvider.getValue(mid) == value);

	        	if (!first)
	        		index++;
	        	break;
	        }
            else if (value < midValue)
	            right = mid - 1;
	        else
	            left = mid + 1;
		}

		if (left > right)
			index = left;

		if (index < 0 || index > size)
			return -1;
		else {
//            TODO: revive Assert.isTrue(size == index || (first && valueProvider.getValue(index) < value) || (!first && valueProvider.getValue(index) > value));
			return index;
		}
	}

	/**
	 * Creates and stores all animation primitives for the given animation position.
     *
	 * The first step is to generate the primitives and fill in the begin/end simulation times and simulation/animation
	 * time durations that are known just by looking at the eventlog.
	 *
	 * In the second step the begin/end animation times are calculated by doing a topological sort on all the animation
	 * positions of the collected animation primitives.
	 */
    private void loadAnimationPrimitivesForAnimationPosition(AnimationPosition animationPosition) {
        long begin = System.currentTimeMillis();
        IEventLog eventLog = animationCanvas.getEventLog();
        if (eventLog != null && !eventLog.isEmpty()) {
            if (animationPosition.getEventNumber() != null) {
                int keyframeBlockSize = eventLog.getKeyframeBlockSize();
                int keyframeBlockIndex = (int)(animationPosition.getEventNumber() / keyframeBlockSize);
                long firstEventNumber = keyframeBlockIndex * keyframeBlockSize;
                long lastEventNumber = firstEventNumber + keyframeBlockSize - 1;
                IEvent firstEvent = eventLog.getEventForEventNumber(firstEventNumber);
                IEvent lastEvent = eventLog.getEventForEventNumber(lastEventNumber);
                if (lastEvent == null)
                    lastEvent = eventLog.getLastEvent();
                ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
                animationPrimitives.addAll(collectAnimationPrimitivesForKeyframeSimulationState(firstEvent));
                animationPrimitives.addAll(collectAnimationPrimitivesForEvents(firstEvent, lastEvent));
                if (!animationPrimitives.isEmpty()) {
                    animationPrimitives.addAll(beginAnimationTimeOrderedPrimitives);
                    calculateAnimationTimes(animationPrimitives);
                    calculateBeginAnimationTimeOrderedPrimitives(animationPrimitives);
                    calculateEndAnimationTimeOrderedPrimitives(animationPrimitives);
                }
            }
            else {
                // TODO: we don't seem to get here, because even during playing
                // the animation event numbers are always present in the
                // animation position
            }
        }
        if (debug)
            System.out.println("loadAnimationPrimitivesForAnimationPosition:" + (System.currentTimeMillis() - begin) + "ms");
    }

    private ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForKeyframeSimulationState(IEvent keyframeEvent) {
        KeyframeEntry keyframeEntry = null;
        for (int i = 0; i < keyframeEvent.getNumEventLogEntries(); i++) {
            EventLogEntry eventLogEntry = keyframeEvent.getEventLogEntry(i);
            if (eventLogEntry instanceof KeyframeEntry)
                keyframeEntry = (KeyframeEntry)eventLogEntry;
        }
        String[] simulationStateEntries = keyframeEntry.getSimulationStateEntries().split(",");
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        for (String simulationStateEntry : simulationStateEntries) {
            if (!StringUtils.isEmpty(simulationStateEntry)) {
                long eventNumber = Long.parseLong(simulationStateEntry.split(":")[0]);
                if (eventNumber != -1) {
                    IEvent event = keyframeEvent.getEventLog().getEventForEventNumber(eventNumber);
                    animationPrimitives.addAll(collectAnimationPrimitivesForEvents(event, event));
                }
            }
        }
        return animationPrimitives;
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
    private void calculateAnimationTimes(ArrayList<IAnimationPrimitive> animationPrimitives) {
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
            // TODO: should not need to set the animation time duration
            if (animationPrimitive.getAnimationTimeDuration() == -1) {
                AbstractAnimationPrimitive abstractAnimationPrimitive = (AbstractAnimationPrimitive)animationPrimitive;
                abstractAnimationPrimitive.setAnimationTimeDuration(abstractAnimationPrimitive.getOriginRelativeEndAnimationTime() - abstractAnimationPrimitive.getOriginRelativeBeginAnimationTime());
            }
            if (debug)
                System.out.println(animationPrimitive);
        }
        // TODO: fill in the interpolation lists in the animation coordinate system for each event
        coordinateSystem = new AnimationCoordinateSystem(eventLogInput, animationPositions);
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
            for (int i = 0; i < animationPositions.size() - 1; i++) {
                AnimationPosition animationPosition = animationPositions.get(i);
                System.out.println(animationPosition);
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
        AnimationPosition previousAnimationPosition = null;
        for (AnimationPosition animationPosition : animationPositions) {
            if (previousAnimationPosition != null)
                animationTime += getAnimationTimeDelta(previousAnimationPosition, animationPosition);
            previousAnimationPosition = animationPosition;
            if (animationPosition.getEventNumber().equals(animationTimeOriginEventNumber))
                break;
        }
        animationTime *= -1;
        if (debug)
            System.out.println("Animation time of first animation position: " + animationTime);
        previousAnimationPosition = null;
        for (AnimationPosition animationPosition : animationPositions) {
            if (previousAnimationPosition != null)
                animationTime += getAnimationTimeDelta(previousAnimationPosition, animationPosition);
            Assert.isTrue(!Double.isNaN(animationTime));
            animationPosition.setOriginRelativeAnimationTime(animationTime);
            previousAnimationPosition = animationPosition;
        }
    }

    private double getAnimationTimeDelta(AnimationPosition previousAnimationPosition, AnimationPosition animationPosition) {
        BigDecimal simulationTimeDelta = animationPosition.getSimulationTime().subtract(previousAnimationPosition.getSimulationTime());
        if (simulationTimeDelta.equals(BigDecimal.getZero()))
            return animationPosition.getFrameRelativeAnimationTime() - previousAnimationPosition.getFrameRelativeAnimationTime();
        else
            return coordinateSystem.getAnimationTimeDelta(simulationTimeDelta.doubleValue());
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

    /**
     * Collects animation primitives for all events between the two specified events.
     * The range is extended in both directions with the events having the same simulation time.
     */
    private ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForEvents(IEvent beginEvent, IEvent endEvent) {
        AnimationPrimitiveContext animationPrimitiveContext = new AnimationPrimitiveContext();
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        // we have to begin collecting after an event having a fresh simulation time to find out the proper relativeAnimationTime
        BigDecimal beginSimulationTime = beginEvent.getSimulationTime();
        while (true) {
            IEvent previousEvent = beginEvent.getPreviousEvent();
            if (previousEvent == null || !previousEvent.getSimulationTime().equals(beginSimulationTime))
                break;
            else
                beginEvent = previousEvent;
        }
        // we have to end collecting before an event having a fresh simulation time
        BigDecimal endSimulationTime = endEvent.getSimulationTime();
        while (true) {
            IEvent nextEvent = endEvent.getNextEvent();
            if (nextEvent == null || !nextEvent.getSimulationTime().equals(endSimulationTime))
                break;
            else
                endEvent = nextEvent;
        }
        IEvent event = beginEvent;
        while (true) {
            long eventNumber = event.getEventNumber();
            if (eventNumberToFrameRelativeAnimationTime.get(eventNumber) == null) {
                animationPrimitiveContext.handleMessageAnimation = null;
                animationPrimitiveContext.isCollectingHandleMessageAnimations = true;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                animationPrimitiveContext.isCollectingHandleMessageAnimations = false;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                eventNumberToFrameRelativeAnimationTime.put(eventNumber, animationPrimitiveContext.frameRelativeAnimationTime);
            }
            if (eventNumber == endEvent.getEventNumber())
                break;
            else {
                IEvent nextEvent = event.getNextEvent();
                if (nextEvent != null && !event.getSimulationTime().equals(nextEvent.getSimulationTime()))
                    animationPrimitiveContext.frameRelativeAnimationTime = 0;
                event = nextEvent;
            }
        }
        if (/*debug && */animationPrimitives.size() != 0)
            System.out.println("Loaded " + animationPrimitives.size() + " animation primitives for events " + beginEvent.getEventNumber() + " - " + endEvent.getEventNumber());
        return animationPrimitives;
    }

    /**
     * Adds new IAnimationPrimitives to the collection based on the EventLogEntry.
     */
    private void collectAnimationPrimitivesForEventLogEntry(AnimationPrimitiveContext animationPrimitiveContext, EventLogEntry eventLogEntry, ArrayList<IAnimationPrimitive> animationPrimitives) {
        IEvent event = eventLogEntry.getEvent();
        long eventNumber = event == null ? -1 : event.getEventNumber();
        BigDecimal simulationTime = event == null ? BigDecimal.getZero() : event.getSimulationTime();
        if (eventLogEntry instanceof EventEntry) {
            EventEntry eventEntry = (EventEntry)eventLogEntry;
            if (eventEntry.getModuleId() == 1 || isSubmoduleVisible(eventEntry.getModuleId())) {
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (animationParameters.enableHandleMessageAnimations) {
                        HandleMessageAnimation animationPrimitive = new HandleMessageAnimation(this, event.getEventNumber(), event.getModuleId());
                        animationPrimitive.setBeginEventNumber(eventNumber);
                        animationPrimitive.setEndEventNumber(eventNumber);
                        animationPrimitive.setBeginSimulationTime(simulationTime);
                        animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                        animationPrimitive.setSourceEventNumber(eventNumber);
                        animationPrimitives.add(animationPrimitive);
                        animationPrimitiveContext.handleMessageAnimation = animationPrimitive;
                        animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.handleMessageAnimationShift;
                    }
                }
                else {
                    if (animationPrimitiveContext.handleMessageAnimation != null) {
                        animationPrimitiveContext.handleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        double relativeBeginAnimationTime = animationPrimitiveContext.handleMessageAnimation.getFrameRelativeBeginAnimationTime();
                        animationPrimitiveContext.handleMessageAnimation.setAnimationTimeDuration(animationPrimitiveContext.frameRelativeAnimationTime - relativeBeginAnimationTime);
                    }
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.handleMessageAnimationShift;
                }
            }
        }
        else if (eventLogEntry instanceof ModuleCreatedEntry) {
            if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                int parentModuleId = moduleCreatedEntry.getParentModuleId();
                EventLogModule parentModule = parentModuleId == -1 ? null : simulation.getModuleById(parentModuleId);
                EventLogModule module = simulation == null ? null : simulation.getModuleById(moduleCreatedEntry.getModuleId());
                if (module == null) {
                    module = new EventLogModule(parentModule);
                    module.setId(moduleCreatedEntry.getModuleId());
                    module.setName(moduleCreatedEntry.getFullName());
                    module.setTypeName(moduleCreatedEntry.getNedTypeName());
                    module.setClassName(moduleCreatedEntry.getModuleClassName());
                    if (simulation == null) {
                        simulation = new AnimationSimulation(module);
                        animationCanvas.addShownCompoundModule(moduleCreatedEntry.getModuleId());
                    }
                    else
                        simulation.addModule(module);
                }
                CreateModuleAnimation animationPrimitive = new CreateModuleAnimation(this, module, parentModuleId);
                animationPrimitive.setBeginEventNumber(eventNumber);
                animationPrimitive.setBeginSimulationTime(simulationTime);
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.createModuleAnimationShift;
                animationPrimitive.setSourceEventNumber(eventNumber);
                animationPrimitives.add(animationPrimitive);
            }
        }
        else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
            if (animationParameters.enableSetModuleDisplayStringAnimations && !animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleDisplayStringChangedEntry.getModuleId())) {
                    DisplayString displayString = new DisplayString(moduleDisplayStringChangedEntry.getDisplayString());
                    SetModuleDisplayStringAnimation animationPrimitive = new SetModuleDisplayStringAnimation(this, moduleDisplayStringChangedEntry.getModuleId(), displayString);
                    animationPrimitive.setBeginEventNumber(eventNumber);
                    animationPrimitive.setBeginSimulationTime(simulationTime);
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.setModuleDisplayStringAnimationShift;
                    animationPrimitive.setSourceEventNumber(eventNumber);
                    animationPrimitives.add(animationPrimitive);
                }
            }
        }
        else if (eventLogEntry instanceof GateCreatedEntry) {
            if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                EventLogModule module = simulation.getModuleById(gateCreatedEntry.getModuleId());
                EventLogGate gate = new EventLogGate(module, gateCreatedEntry.getGateId());
                gate.setName(gateCreatedEntry.getName());
                gate.setIndex(gateCreatedEntry.getIndex());
                module.addGate(gate);
                CreateGateAnimation animationPrimitive = new CreateGateAnimation(this, gate);
                animationPrimitive.setBeginEventNumber(eventNumber);
                animationPrimitive.setBeginSimulationTime(simulationTime);
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                animationPrimitive.setSourceEventNumber(eventNumber);
                animationPrimitives.add(animationPrimitive);
            }
        }
        else if (eventLogEntry instanceof ConnectionCreatedEntry) {
            if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                EventLogModule sourceModule = simulation.getModuleById(connectionCreatedEntry.getSourceModuleId());
                EventLogGate sourceGate = sourceModule.getGateById(connectionCreatedEntry.getSourceGateId());
                EventLogModule destinationModule = simulation.getModuleById(connectionCreatedEntry.getDestModuleId());
                EventLogGate destinationGate = destinationModule.getGateById(connectionCreatedEntry.getDestGateId());
                CreateConnectionAnimation animationPrimitive = new CreateConnectionAnimation(this, new EventLogConnection(sourceModule, sourceGate, destinationModule, destinationGate));
                animationPrimitive.setBeginEventNumber(eventNumber);
                animationPrimitive.setBeginSimulationTime(simulationTime);
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.createConnectionAnimationShift;
                animationPrimitive.setSourceEventNumber(eventNumber);
                animationPrimitives.add(animationPrimitive);
            }
        }
        else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
            if (animationParameters.enableSetConnectionDisplayStringAnimations && !animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
                if (isSubmoduleVisible(connectionDisplayStringChangedEntry.getSourceModuleId())) {
                    DisplayString displayString = new DisplayString(connectionDisplayStringChangedEntry.getDisplayString());
                    int sourceGateId = connectionDisplayStringChangedEntry.getSourceGateId();
                    EventLogModule ownerModule = simulation.getModuleById(connectionDisplayStringChangedEntry.getSourceModuleId());
                    EventLogGate sourceGate = ownerModule.getGateById(sourceGateId);
                    if (sourceGate == null) {
                        sourceGate = new EventLogGate(ownerModule, sourceGateId);
                        ownerModule.addGate(sourceGate);
                    }
                    SetConnectionDisplayStringAnimation animationPrimitive = new SetConnectionDisplayStringAnimation(this, sourceGate, displayString);
                    animationPrimitive.setBeginEventNumber(eventNumber);
                    animationPrimitive.setBeginSimulationTime(simulationTime);
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.setConnectionDisplayStringAnimationShift;
                    animationPrimitive.setSourceEventNumber(eventNumber);
                    animationPrimitives.add(animationPrimitive);
                }
            }
        }
        else if (eventLogEntry instanceof BubbleEntry) {
            if (animationParameters.enableBubbleAnimations && !animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
                if (isSubmoduleVisible(bubbleEntry.getModuleId())) {
                    BubbleAnimation animationPrimitive = new BubbleAnimation(this, bubbleEntry.getText(), bubbleEntry.getModuleId());
                    animationPrimitive.setBeginEventNumber(eventNumber);
                    animationPrimitive.setEndEventNumber(eventNumber);
                    animationPrimitive.setBeginSimulationTime(simulationTime);
                    animationPrimitive.setAnimationTimeDuration(0.0);
                    animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.bubbleAnimationShift;
                    animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitive.setSourceEventNumber(eventNumber);
                    animationPrimitives.add(animationPrimitive);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
            if (animationParameters.enableModuleMethodCallAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleMethodBeginEntry.getFromModuleId()) && isSubmoduleVisible(moduleMethodBeginEntry.getToModuleId())) {
                    double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                    ModuleMethodCallAnimation animationPrimitive = new ModuleMethodCallAnimation(this, moduleMethodBeginEntry.getMethod(), moduleMethodBeginEntry.getFromModuleId(), moduleMethodBeginEntry.getToModuleId());
                    animationPrimitive.setBeginEventNumber(eventNumber);
                    animationPrimitive.setEndEventNumber(eventNumber);
                    animationPrimitive.setBeginSimulationTime(simulationTime);
                    animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                    animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.moduleMethodCallAnimationShift;
                    animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitive.setSourceEventNumber(eventNumber);
                    animationPrimitives.add(animationPrimitive);
                }
            }
        }
        else if (eventLogEntry instanceof BeginSendEntry) {
            BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
            EndSendEntry endSendEntry = event.getEndSendEntry(beginSendEntry);
            EventLogMessage message = createMessage(beginSendEntry, endSendEntry);
            int beginSendEntryIndex = beginSendEntry.getEntryIndex();
            int endSendEntryIndex = endSendEntry.getEntryIndex();
            if (beginSendEntryIndex == endSendEntryIndex - 1) {
                // self message send
                EventLogModule module = simulation.getModuleById(event.getModuleId());
                message.setSenderModule(module);
                message.setArrivalModule(module);
                if (isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (animationParameters.enableScheduleSelfMessageAnimations) {
                            double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                            ScheduleSelfMessageAnimation animationPrimitive = new ScheduleSelfMessageAnimation(this, beginSendEntry.getContextModuleId(), message);
                            animationPrimitive.setBeginEventNumber(eventNumber);
                            animationPrimitive.setEndEventNumber(eventNumber);
                            animationPrimitive.setBeginSimulationTime(simulationTime);
                            animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                            animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleSelfMessageAnimationShift;
                            animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitive.setSourceEventNumber(eventNumber);
                            animationPrimitives.add(animationPrimitive);
                        }
                    }
                    else {
                        if (animationParameters.enableSendSelfMessageAnimations) {
                            SendSelfMessageAnimation animationPrimitive = new SendSelfMessageAnimation(this, beginSendEntry.getContextModuleId(), message);
                            animationPrimitive.setBeginEventNumber(eventNumber);
                            animationPrimitive.setEndEventNumber(eventNumber); // FIXME: should be calculated based on simulation time
                            animationPrimitive.setBeginSimulationTime(simulationTime);
                            BigDecimal simulationTimeDuration = endSendEntry.getArrivalTime().subtract(simulationTime);
                            animationPrimitive.setSimulationTimeDuration(simulationTimeDuration);
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            if (simulationTimeDuration.equals(BigDecimal.getZero())) {
                                double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                                animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                                animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            }
                            else
                                animationPrimitive.setFrameRelativeEndAnimationTime(0.0);
                            animationPrimitive.setSourceEventNumber(eventNumber);
                            animationPrimitives.add(animationPrimitive);
                        }
                    }
                }
            }
            else if (beginSendEntryIndex <= endSendEntryIndex - 2 && event.getEventLogEntry(beginSendEntryIndex + 1) instanceof SendDirectEntry) {
                // direct message send
                SendDirectEntry sendDirectEntry = (SendDirectEntry)event.getEventLogEntry(beginSendEntryIndex + 1);
                int senderModuleId = sendDirectEntry.getSenderModuleId();
                int destModuleId = sendDirectEntry.getDestModuleId();
                EventLogModule senderModule = simulation.getModuleById(senderModuleId);
                EventLogModule destinationModule = simulation.getModuleById(destModuleId);
                message.setSenderModule(senderModule);
                message.setArrivalModule(destinationModule);
                message.setArrivalGate(destinationModule.getGateById(sendDirectEntry.getDestGateId()));
                if (isCompoundModuleVisible(senderModule.getCommonAncestorModule(destinationModule).getId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (animationParameters.enableScheduleDirectAnimations) {
                            double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                            ScheduleMessageAnimation animationPrimitive = new ScheduleMessageAnimation(this, beginSendEntry.getContextModuleId(), message);
                            animationPrimitive.setBeginEventNumber(eventNumber);
                            animationPrimitive.setEndEventNumber(eventNumber);
                            animationPrimitive.setBeginSimulationTime(simulationTime);
                            animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                            animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleDirectAnimationShift;
                            animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitive.setSourceEventNumber(eventNumber);
                            animationPrimitives.add(animationPrimitive);
                        }
                    }
                    else {
                        if (animationParameters.enableSendDirectAnimations) {
                            BigDecimal transmissionDelay = sendDirectEntry.getTransmissionDelay();
                            BigDecimal propagationDelay = sendDirectEntry.getPropagationDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            SendDirectAnimation sendDirectAnimation = new SendDirectAnimation(this, propagationDelay, transmissionDelay, senderModuleId, destModuleId, message);
                            sendDirectAnimation.setBeginEventNumber(eventNumber);
                            sendDirectAnimation.setEndEventNumber(eventNumber); // FIXME: should be calculated based on simulation time
                            sendDirectAnimation.setBeginSimulationTime(simulationTime);
                            sendDirectAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendDirectAnimation.setFrameRelativeEndAnimationTime(0.0);
                            sendDirectAnimation.setSimulationTimeDuration(simulationTimeDuration);
                            if (simulationTimeDuration.equals(BigDecimal.getZero()))
                                sendDirectAnimation.setAnimationTimeDuration(coordinateSystem.getAnimationTimeDelta(0));
                            sendDirectAnimation.setSourceEventNumber(eventNumber);
                            animationPrimitives.add(sendDirectAnimation);
                            // TODO: this should not be the default here
                            SendBroadcastAnimation sendBroadcastAnimation = new SendBroadcastAnimation(this, propagationDelay, transmissionDelay, senderModuleId, destModuleId, message);
                            sendBroadcastAnimation.setBeginEventNumber(eventNumber);
                            sendBroadcastAnimation.setEndEventNumber(eventNumber); // FIXME: should be calculated based on simulation time
                            sendBroadcastAnimation.setBeginSimulationTime(simulationTime);
                            sendBroadcastAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendBroadcastAnimation.setFrameRelativeEndAnimationTime(0.0);
                            sendBroadcastAnimation.setSimulationTimeDuration(simulationTimeDuration);
                            if (simulationTimeDuration.equals(BigDecimal.getZero()))
                                sendBroadcastAnimation.setAnimationTimeDuration(coordinateSystem.getAnimationTimeDelta(0));
                            sendBroadcastAnimation.setSourceEventNumber(eventNumber);
                            animationPrimitives.add(sendBroadcastAnimation);
                        }
                    }
                }
            }
            else {
                // normal message send
                SendHopEntry firstSendHopEntry = (SendHopEntry)event.getEventLogEntry(eventLogEntry.getEntryIndex() + 1);
                EventLogModule senderModule = simulation.getModuleById(firstSendHopEntry.getSenderModuleId());
                message.setSenderModule(senderModule);
                message.setSenderGate(senderModule.getGateById(firstSendHopEntry.getSenderGateId()));
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (animationParameters.enableScheduleMessageAnimations && isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                        double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                        ScheduleMessageAnimation animationPrimitive = new ScheduleMessageAnimation(this, beginSendEntry.getContextModuleId(), message);
                        animationPrimitive.setBeginEventNumber(eventNumber);
                        animationPrimitive.setEndEventNumber(eventNumber);
                        animationPrimitive.setBeginSimulationTime(simulationTime);
                        animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                        animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                        animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleMessageAnimationShift;
                        animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitive.setSourceEventNumber(eventNumber);
                        animationPrimitives.add(animationPrimitive);
                    }
                }
                else {
                    if (animationParameters.enableSendMessageAnimations) {
                        BigDecimal simulationTimeDelta = BigDecimal.getZero();
                        double localAnimationTime = animationPrimitiveContext.frameRelativeAnimationTime;
                        for (int i = beginSendEntryIndex + 1; i < endSendEntryIndex; i++) {
                            SendHopEntry sendHopEntry = (SendHopEntry)event.getEventLogEntry(i);
                            BigDecimal transmissionDelay = sendHopEntry.getTransmissionDelay();
                            BigDecimal propagationDelay = sendHopEntry.getPropagationDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            // TODO: also check for arrival module
                            if (isSubmoduleVisible(sendHopEntry.getSenderModuleId())) {
                                EventLogGate gate = new EventLogGate(simulation.getModuleById(sendHopEntry.getSenderModuleId()), sendHopEntry.getSenderGateId());
                                SendMessageAnimation animationPrimitive = new SendMessageAnimation(this, propagationDelay, transmissionDelay, gate, message);
                                animationPrimitive.setBeginEventNumber(eventNumber); // FIXME: should be calculated based on simulation time
                                animationPrimitive.setEndEventNumber(eventNumber); // FIXME: should be calculated based on simulation time
                                animationPrimitive.setBeginSimulationTime(simulationTime.add(simulationTimeDelta));
                                animationPrimitive.setFrameRelativeBeginAnimationTime(localAnimationTime);
                                animationPrimitive.setSourceEventNumber(eventNumber);
                                animationPrimitives.add(animationPrimitive);
                                animationPrimitive.setSimulationTimeDuration(simulationTimeDuration);
                                if (simulationTimeDuration.equals(BigDecimal.getZero())) {
                                    double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                                    animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                                    localAnimationTime += animationTimeDuration;
                                    animationPrimitive.setFrameRelativeEndAnimationTime(localAnimationTime);
                                }
                                else {
                                    localAnimationTime = 0;
                                    animationPrimitive.setFrameRelativeEndAnimationTime(0.0);
                                }
                            }
                            simulationTimeDelta = simulationTimeDelta.add(simulationTimeDuration);
                        }
                    }
                }
            }
        }
    }

    private boolean isSubmoduleVisible(int moduleId) {
        EventLogModule module = simulation.getModuleById(moduleId);
        EventLogModule parentModule = module.getParentModule();
        return module != null && parentModule != null && isCompoundModuleVisible(parentModule.getId());
    }

    private boolean isCompoundModuleVisible(int moduleId) {
        return animationCanvas.showsCompoundModule(moduleId);
    }

    private EventLogMessage createMessage(BeginSendEntry beginSendEntry, EndSendEntry endSendEntry) {
        EventLogMessage message = new EventLogMessage();
        message.setName(beginSendEntry.getMessageName());
        message.setClassName(beginSendEntry.getMessageClassName());
        message.setId(beginSendEntry.getMessageId());
        message.setTreeId(beginSendEntry.getMessageTreeId());
        message.setEncapsulationId(beginSendEntry.getMessageEncapsulationId());
        message.setEncapsulationTreeId(beginSendEntry.getMessageEncapsulationTreeId());
        message.setSendingTime(beginSendEntry.getEvent().getSimulationTime());
        message.setArrivalTime(endSendEntry.getArrivalTime());
        return message;
    }

    private static class AnimationPrimitiveContext {
        public HandleMessageAnimation handleMessageAnimation;
        public double frameRelativeAnimationTime;
        public boolean isCollectingHandleMessageAnimations;
    }
}

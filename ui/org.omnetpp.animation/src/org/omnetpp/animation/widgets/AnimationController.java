/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.animation.widgets;

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
import org.omnetpp.animation.primitives.BubbleAnimation;
import org.omnetpp.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.animation.primitives.CreateGateAnimation;
import org.omnetpp.animation.primitives.CreateModuleAnimation;
import org.omnetpp.animation.primitives.HandleMessageAnimation;
import org.omnetpp.animation.primitives.IAnimationPrimitive;
import org.omnetpp.animation.primitives.ModuleMethodCallAnimation;
import org.omnetpp.animation.primitives.ScheduleDirectMessageAnimation;
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
import org.omnetpp.eventlog.engine.EventNumberKind;
import org.omnetpp.eventlog.engine.GateCreatedEntry;
import org.omnetpp.eventlog.engine.IEvent;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.KeyframeEntry;
import org.omnetpp.eventlog.engine.MessageSendDependency;
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
     * This map is filled when animation primitives are loaded from the eventlog
     * file.
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
        timerQueue.addTimer(animationTimer);
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
	    if (this.isRunning != isRunning) {
    		this.isRunning = isRunning;
    		runningStateChanged();
	    }
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
            // TODO: KLUDGE: really here? really this simple?
            long eventNumber = currentAnimationPosition.getEventNumber();
            IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
            double animationTime = coordinateSystem.getAnimationTime(event);
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
     * If the simulation is continued and the eventlog file is appended, it is
     * guaranteed that the animation will not change up to this position.
     * Therefore this animation position cannot be beyond the last event's
     * simulation time, because that part is not yet final.
     */
    public AnimationPosition getEndAnimationPosition() {
        if (!endAnimationPosition.isCompletelySpecified()) {
            IEvent lastEvent = eventLogInput.getEventLog().getLastEvent();
            BigDecimal simulationTime = lastEvent.getSimulationTime();
            loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(lastEvent.getEventNumber(), simulationTime, null, null));
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
        IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
        BigDecimal simulationTime = event.getSimulationTime();
        return new AnimationPosition(eventNumber, simulationTime, 0.0, eventNumber == animationTimeOriginEventNumber ? 0.0 : coordinateSystem.getAnimationTime(event));
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
        IEvent event = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime);
        long eventNumber = event.getEventNumber();
        double frameRelativeAnimationTime = animationTime - coordinateSystem.getAnimationTime(event);
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
        long eventNumber = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime).getEventNumber();
        BigDecimal simulationTime = coordinateSystem.getSimulationTimeForAnimationTime(animationTime);
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
        double currentOriginRelativeAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
        int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
            public double getValue(int index) {
                return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
            }
        }, beginAnimationTimeOrderedPrimitives.size(), currentOriginRelativeAnimationTime, forward);
        int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
            public double getValue(int index) {
                return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getOriginRelativeAnimationTime();
            }
        }, endAnimationTimeOrderedPrimitives.size(), currentOriginRelativeAnimationTime, forward);
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
			    else
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
		double oldAnimationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getOriginRelativeAnimationTime() : Double.NEGATIVE_INFINITY;
		double newAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
		boolean forward = newAnimationTime >= oldAnimationTime;
		int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
		        public double getValue(int index) {
		            return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, beginAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward);
		int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, endAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward);
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
	 * If exactly one match is found, then (if first == true) returns that one, otherwise returns the next one.
	 * If more than one equal values are found, returns the first one
	 * (if first == true) or the one after the last one (if first == false) of them.
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
            Long eventNumber = animationPosition.getEventNumber();
            if (eventNumber != null) {
                if (eventNumberToFrameRelativeEndAnimationTime.get(eventNumber) == null) {
                    int keyframeBlockSize = eventLog.getKeyframeBlockSize();
                    int keyframeBlockIndex = (int)(eventNumber / keyframeBlockSize);
                    long firstEventNumber = keyframeBlockIndex * keyframeBlockSize;
                    long lastEventNumber = firstEventNumber + keyframeBlockSize - 1;
                    IEvent firstEvent = eventLog.getEventForEventNumber(firstEventNumber);
                    IEvent lastEvent = eventLog.getEventForEventNumber(lastEventNumber);
                    if (lastEvent == null)
                        lastEvent = eventLog.getLastEvent();
                    ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
                    // TODO: KLUDGE: what about events that generate animation positions that fall into another event's animation frame???
                    // TODO: KLUDGE: how can we be still lazy by knowing this??? is it enough that we load those events too? does the order matter?
                    animationPrimitives.addAll(collectAnimationPrimitivesForKeyframeSimulationState(firstEvent));
                    animationPrimitives.addAll(collectAnimationPrimitivesForEvents(firstEvent, lastEvent));
                    if (!animationPrimitives.isEmpty()) {
                        animationPrimitives.addAll(beginAnimationTimeOrderedPrimitives);
                        calculateAnimationTimes(animationPrimitives);
                        calculateBeginAnimationTimeOrderedPrimitives(animationPrimitives);
                        calculateEndAnimationTimeOrderedPrimitives(animationPrimitives);
                    }
                }
            }
            else if (animationPosition.getOriginRelativeAnimationTime() != null) {
                double currentAnimationTime = getCurrentAnimationTime();
                double newAnimationTime = animationPosition.getOriginRelativeAnimationTime();
                boolean forward = currentAnimationTime < newAnimationTime;
                eventNumber = getCurrentEventNumber();
                long lastEventNumber = eventLog.getLastEvent().getEventNumber();
                while (eventNumber >= 0 && eventNumber <= lastEventNumber) {
                    double eventAnimationTime = coordinateSystem.getAnimationTime(eventLog.getEventForEventNumber(eventNumber));
                    if (forward ? eventAnimationTime >= newAnimationTime : eventAnimationTime <= newAnimationTime)
                        break;
                    loadAnimationPrimitivesForAnimationPosition(new AnimationPosition(eventNumber, null, null, null));
                    eventNumber += forward ? 1 : -1;
                }
            }
            else
                throw new RuntimeException("Animation posisiton is unspecified");
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
            // TODO: should not need to set the animation time duration?!
            // TODO: maybe, sometimes it is calculated this late? (e.g. duration is specified in terms of event numbers, simulation times and frame relative animation times)
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
        AnimationPosition previousAnimationPosition = null;
        for (AnimationPosition animationPosition : animationPositions) {
            if (previousAnimationPosition != null)
                animationTime += getAnimationTimeDelta(previousAnimationPosition, animationPosition);
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
                animationTime += getAnimationTimeDelta(previousAnimationPosition, animationPosition);
            }
            Assert.isTrue(!Double.isNaN(animationTime));
            animationPosition.setOriginRelativeAnimationTime(animationTime);
            Assert.isTrue(animationPosition.isCompletelySpecified());
            previousAnimationPosition = animationPosition;
        }
    }

    private double getAnimationTimeDelta(AnimationPosition previousAnimationPosition, AnimationPosition animationPosition) {
        BigDecimal simulationTime = animationPosition.getSimulationTime();
        BigDecimal previousSimulationTime = previousAnimationPosition.getSimulationTime();
        Double frameRelativeAnimationTime = animationPosition.getFrameRelativeAnimationTime();
        Double previousFrameRelativeAnimationTime = previousAnimationPosition.getFrameRelativeAnimationTime();
        Long eventNumber = animationPosition.getEventNumber();
        Long previousEventNumber = previousAnimationPosition.getEventNumber();
        boolean sameSimulationTimes = previousSimulationTime.equals(simulationTime);
        boolean sameEventNumbers = previousEventNumber.equals(eventNumber);
        if (sameEventNumbers && sameSimulationTimes) {
            if (Double.isInfinite(frameRelativeAnimationTime) && Double.isInfinite(previousFrameRelativeAnimationTime))
                return 0;
            else
                return frameRelativeAnimationTime - previousFrameRelativeAnimationTime;
        }
        else if (!sameEventNumbers && sameSimulationTimes)
            // NOTE: between these two events there's zero simulation time, but that is a non-zero animation time
            return frameRelativeAnimationTime + coordinateSystem.getAnimationTimeDelta(0);
        else
            return coordinateSystem.getAnimationTimeDelta(simulationTime.subtract(previousSimulationTime).doubleValue());
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
    // TODO: KLUDGE: do we still need to extend the range now that we have the new animation frame concept?
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
            if (eventNumberToFrameRelativeEndAnimationTime.get(eventNumber) == null) {
                animationPrimitiveContext.handleMessageAnimation = null;
                animationPrimitiveContext.frameRelativeAnimationTime = 0;
                animationPrimitiveContext.isCollectingHandleMessageAnimations = true;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                animationPrimitiveContext.isCollectingHandleMessageAnimations = false;
                for (int i = 0; i < event.getNumEventLogEntries(); i++)
                    collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
                eventNumberToFrameRelativeEndAnimationTime.put(eventNumber, animationPrimitiveContext.frameRelativeAnimationTime);
            }
            if (eventNumber == endEvent.getEventNumber())
                break;
            else
                event = event.getNextEvent();
        }
        if (debug) // avoids dead code warning
            if (animationPrimitives.size() != 0)
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
                        HandleMessageAnimation handleMessageAnimation = new HandleMessageAnimation(this, eventNumber, simulationTime, event.getModuleId());
                        handleMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitives.add(handleMessageAnimation);
                        animationPrimitiveContext.handleMessageAnimation = handleMessageAnimation;
                        animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.handleMessageAnimationShift;
                    }
                }
                else {
                    if (animationPrimitiveContext.handleMessageAnimation != null)
                        animationPrimitiveContext.handleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleCreatedEntry) {
            if (animationParameters.enableCreateModuleAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleCreatedEntry moduleCreatedEntry = (ModuleCreatedEntry)eventLogEntry;
                int moduleId = moduleCreatedEntry.getModuleId();
                int parentModuleId = moduleCreatedEntry.getParentModuleId();
                EventLogModule parentModule = parentModuleId == -1 ? null : simulation.getModuleById(parentModuleId);
                EventLogModule module = simulation == null ? null : simulation.getModuleById(moduleId);
                if (module == null) {
                    module = new EventLogModule(parentModule);
                    module.setId(moduleId);
                    module.setName(moduleCreatedEntry.getFullName());
                    module.setTypeName(moduleCreatedEntry.getNedTypeName());
                    module.setClassName(moduleCreatedEntry.getModuleClassName());
                    if (simulation == null) {
                        simulation = new AnimationSimulation(module);
                        animationCanvas.addShownCompoundModule(moduleId);
                    }
                    else
                        simulation.addModule(module);
                }
                if (isSubmoduleVisible(moduleId) || isCompoundModuleVisible(moduleId)) {
                    CreateModuleAnimation createModuleAnimation = new CreateModuleAnimation(this, eventNumber, simulationTime, module, parentModuleId);
                    createModuleAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.createModuleAnimationShift;
                    animationPrimitives.add(createModuleAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleDisplayStringChangedEntry) {
            if (animationParameters.enableSetModuleDisplayStringAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleDisplayStringChangedEntry moduleDisplayStringChangedEntry = (ModuleDisplayStringChangedEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleDisplayStringChangedEntry.getModuleId())) {
                    DisplayString displayString = new DisplayString(moduleDisplayStringChangedEntry.getDisplayString());
                    SetModuleDisplayStringAnimation setModuleDisplayStringAnimation = new SetModuleDisplayStringAnimation(this, eventNumber, simulationTime, moduleDisplayStringChangedEntry.getModuleId(), displayString);
                    setModuleDisplayStringAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.setModuleDisplayStringAnimationShift;
                    animationPrimitives.add(setModuleDisplayStringAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof GateCreatedEntry) {
            if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                int moduleId = gateCreatedEntry.getModuleId();
                if (isSubmoduleVisible(moduleId) || isCompoundModuleVisible(moduleId)) {
                    EventLogModule module = simulation.getModuleById(moduleId);
                    EventLogGate gate = new EventLogGate(module, gateCreatedEntry.getGateId());
                    gate.setName(gateCreatedEntry.getName());
                    gate.setIndex(gateCreatedEntry.getIndex());
                    module.addGate(gate);
                    CreateGateAnimation createGateAnimation = new CreateGateAnimation(this, eventNumber, simulationTime, gate);
                    createGateAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(createGateAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ConnectionCreatedEntry) {
            if (animationParameters.enableCreateConnectionAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                int sourceModuleId = connectionCreatedEntry.getSourceModuleId();
                int destinationModuleId = connectionCreatedEntry.getDestModuleId();
                if ((isSubmoduleVisible(sourceModuleId) || isCompoundModuleVisible(sourceModuleId)) && (isSubmoduleVisible(destinationModuleId) || isCompoundModuleVisible(destinationModuleId))) {
                    EventLogModule sourceModule = simulation.getModuleById(sourceModuleId);
                    EventLogGate sourceGate = sourceModule.getGateById(connectionCreatedEntry.getSourceGateId());
                    EventLogModule destinationModule = simulation.getModuleById(destinationModuleId);
                    EventLogGate destinationGate = destinationModule.getGateById(connectionCreatedEntry.getDestGateId());
                    CreateConnectionAnimation createConnectionAnimation = new CreateConnectionAnimation(this, eventNumber, simulationTime, new EventLogConnection(sourceModule, sourceGate, destinationModule, destinationGate));
                    createConnectionAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.createConnectionAnimationShift;
                    animationPrimitives.add(createConnectionAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ConnectionDisplayStringChangedEntry) {
            if (animationParameters.enableSetConnectionDisplayStringAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionDisplayStringChangedEntry connectionDisplayStringChangedEntry = (ConnectionDisplayStringChangedEntry)eventLogEntry;
                int sourceModuleId = connectionDisplayStringChangedEntry.getSourceModuleId();
                if (isSubmoduleVisible(sourceModuleId) || isCompoundModuleVisible(sourceModuleId)) {
                    DisplayString displayString = new DisplayString(connectionDisplayStringChangedEntry.getDisplayString());
                    int sourceGateId = connectionDisplayStringChangedEntry.getSourceGateId();
                    EventLogModule ownerModule = simulation.getModuleById(sourceModuleId);
                    EventLogGate sourceGate = ownerModule.getGateById(sourceGateId);
                    if (sourceGate == null) {
                        sourceGate = new EventLogGate(ownerModule, sourceGateId);
                        ownerModule.addGate(sourceGate);
                    }
                    SetConnectionDisplayStringAnimation setConnectionDisplayStringAnimation = new SetConnectionDisplayStringAnimation(this, eventNumber, simulationTime, sourceGate, displayString);
                    setConnectionDisplayStringAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.setConnectionDisplayStringAnimationShift;
                    animationPrimitives.add(setConnectionDisplayStringAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof BubbleEntry) {
            if (animationParameters.enableBubbleAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                BubbleEntry bubbleEntry = (BubbleEntry)eventLogEntry;
                if (isSubmoduleVisible(bubbleEntry.getModuleId())) {
                    BubbleAnimation bubbleAnimation = new BubbleAnimation(this, eventNumber, simulationTime, bubbleEntry.getText(), bubbleEntry.getModuleId());
                    bubbleAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.bubbleAnimationDuration;
                    bubbleAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(bubbleAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof ModuleMethodBeginEntry) {
            if (animationParameters.enableModuleMethodCallAnimations && animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ModuleMethodBeginEntry moduleMethodBeginEntry = (ModuleMethodBeginEntry)eventLogEntry;
                if (isSubmoduleVisible(moduleMethodBeginEntry.getFromModuleId()) && isSubmoduleVisible(moduleMethodBeginEntry.getToModuleId())) {
                    ModuleMethodCallAnimation moduleMethodCallAnimation = new ModuleMethodCallAnimation(this, eventNumber, simulationTime, moduleMethodBeginEntry.getMethod(), moduleMethodBeginEntry.getFromModuleId(), moduleMethodBeginEntry.getToModuleId());
                    moduleMethodCallAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.moduleMethodCallAnimationDuration;
                    moduleMethodCallAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                    animationPrimitives.add(moduleMethodCallAnimation);
                }
            }
        }
        else if (eventLogEntry instanceof BeginSendEntry) {
            IEventLog eventLog = eventLogInput.getEventLog();
            IEvent lastEvent = eventLog.getLastEvent();
            BeginSendEntry beginSendEntry = (BeginSendEntry)eventLogEntry;
            // TODO: handle EndSendEntry's isReceptionStart flag
            EndSendEntry endSendEntry = event.getEndSendEntry(beginSendEntry);
            EventLogMessage message = createMessage(beginSendEntry, endSendEntry);
            int beginSendEntryIndex = beginSendEntry.getEntryIndex();
            int endSendEntryIndex = endSendEntry.getEntryIndex();
            BigDecimal arrivalTime = endSendEntry.getArrivalTime();
            long arrivalEventNumber = new MessageSendDependency(eventLog, eventNumber, beginSendEntryIndex).getConsequenceEventNumber();
            if (beginSendEntryIndex == endSendEntryIndex - 1) {
                // self message send
                EventLogModule module = simulation.getModuleById(event.getModuleId());
                message.setSenderModule(module);
                message.setArrivalModule(module);
                if (isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (animationParameters.enableScheduleSelfMessageAnimations) {
                            ScheduleSelfMessageAnimation scheduleSelfMessageAnimation = new ScheduleSelfMessageAnimation(this, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                            scheduleSelfMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleSelfMessageAnimationDuration;
                            scheduleSelfMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitives.add(scheduleSelfMessageAnimation);
                        }
                    }
                    else {
                        if (animationParameters.enableSendSelfMessageAnimations && arrivalEventNumber != EventNumberKind.NO_SUCH_EVENT) {
                            SendSelfMessageAnimation sendSelfMessageAnimation = new SendSelfMessageAnimation(this, beginSendEntry.getContextModuleId(), message, animationParameters.handleMessageAnimationShift);
                            sendSelfMessageAnimation.setSourceEventNumber(eventNumber);
                            sendSelfMessageAnimation.setBeginEventNumber(eventNumber);
                            sendSelfMessageAnimation.setBeginSimulationTime(simulationTime);
                            sendSelfMessageAnimation.setEndSimulationTime(arrivalTime);
                            sendSelfMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            if (arrivalEventNumber < 0 || lastEvent.getSimulationTime().less(arrivalTime)) {
                                sendSelfMessageAnimation.setEndEventNumber(lastEvent.getEventNumber());
                                sendSelfMessageAnimation.setFrameRelativeEndAnimationTime(Double.POSITIVE_INFINITY);
                            }
                            else {
                                sendSelfMessageAnimation.setEndEventNumber(arrivalEventNumber);
                                sendSelfMessageAnimation.setFrameRelativeEndAnimationTime(animationParameters.handleMessageAnimationShift);
                            }
                            animationPrimitives.add(sendSelfMessageAnimation);
                        }
                    }
                }
            }
            else if (beginSendEntryIndex <= endSendEntryIndex - 2 && event.getEventLogEntry(beginSendEntryIndex + 1) instanceof SendDirectEntry) {
                // direct message send
                SendDirectEntry sendDirectEntry = (SendDirectEntry)event.getEventLogEntry(beginSendEntryIndex + 1);
                int senderModuleId = sendDirectEntry.getSenderModuleId();
                int destinationModuleId = sendDirectEntry.getDestModuleId();
                EventLogModule senderModule = simulation.getModuleById(senderModuleId);
                EventLogModule destinationModule = simulation.getModuleById(destinationModuleId);
                message.setSenderModule(senderModule);
                message.setArrivalModule(destinationModule);
                message.setArrivalGate(destinationModule.getGateById(sendDirectEntry.getDestGateId()));
                if (isCompoundModuleVisible(senderModule.getCommonAncestorModule(destinationModule).getId())) {
                    if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                        if (animationParameters.enableScheduleDirectAnimations) {
                            ScheduleDirectMessageAnimation scheduleDirectMessageAnimation = new ScheduleDirectMessageAnimation(this, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                            scheduleDirectMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleDirectAnimationDuration;
                            scheduleDirectMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            animationPrimitives.add(scheduleDirectMessageAnimation);
                        }
                    }
                    else {
                        if (animationParameters.enableSendDirectAnimations) {
                            BigDecimal transmissionDelay = sendDirectEntry.getTransmissionDelay();
                            BigDecimal propagationDelay = sendDirectEntry.getPropagationDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            BigDecimal endSimulationTime = simulationTime.add(simulationTimeDuration);
                            long endEventNumber = arrivalEventNumber;
                            double frameRelativeEndAnimationTime = animationParameters.handleMessageAnimationShift;
                            if (endEventNumber < 0 || lastEvent.getSimulationTime().less(endSimulationTime)) {
                                endEventNumber = lastEvent.getEventNumber();
                                frameRelativeEndAnimationTime = Double.POSITIVE_INFINITY;
                            }
                            SendDirectAnimation sendDirectAnimation = new SendDirectAnimation(this, propagationDelay, transmissionDelay, senderModuleId, destinationModuleId, message, animationParameters.handleMessageAnimationShift);
                            sendDirectAnimation.setSourceEventNumber(eventNumber);
                            sendDirectAnimation.setBeginEventNumber(eventNumber);
                            sendDirectAnimation.setEndEventNumber(endEventNumber);
                            sendDirectAnimation.setBeginSimulationTime(simulationTime);
                            sendDirectAnimation.setEndSimulationTime(endSimulationTime);
                            sendDirectAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendDirectAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                            animationPrimitives.add(sendDirectAnimation);
                            // TODO: SendBroadcastAnimation should not be used by default
                            SendBroadcastAnimation sendBroadcastAnimation = new SendBroadcastAnimation(this, propagationDelay, transmissionDelay, senderModuleId, destinationModuleId, message, animationParameters.handleMessageAnimationShift);
                            sendBroadcastAnimation.setSourceEventNumber(eventNumber);
                            sendBroadcastAnimation.setBeginEventNumber(eventNumber);
                            sendBroadcastAnimation.setEndEventNumber(endEventNumber);
                            sendBroadcastAnimation.setBeginSimulationTime(simulationTime);
                            sendBroadcastAnimation.setEndSimulationTime(endSimulationTime);
                            sendBroadcastAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                            sendBroadcastAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                            animationPrimitives.add(sendBroadcastAnimation);
                        }
                    }
                }
            }
            else {
                // normal message send
                SendHopEntry firstSendHopEntry = (SendHopEntry)event.getEventLogEntry(eventLogEntry.getEntryIndex() + 1);
                EventLogModule senderModule = simulation.getModuleById(firstSendHopEntry.getSenderModuleId());
                int destinationModuleId = arrivalEventNumber < 0 ? -1 : eventLog.getEventForEventNumber(arrivalEventNumber).getModuleId();
                EventLogModule destinationModule = destinationModuleId == -1 ? null : simulation.getModuleById(destinationModuleId);
                message.setSenderModule(senderModule);
                message.setSenderGate(senderModule.getGateById(firstSendHopEntry.getSenderGateId()));
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (animationParameters.enableScheduleMessageAnimations && isSubmoduleVisible(beginSendEntry.getContextModuleId())) {
                        ScheduleMessageAnimation scheduleMessageAnimation = new ScheduleMessageAnimation(this, eventNumber, simulationTime, beginSendEntry.getContextModuleId(), message);
                        scheduleMessageAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitiveContext.frameRelativeAnimationTime += animationParameters.scheduleMessageAnimationDuration;
                        scheduleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.frameRelativeAnimationTime);
                        animationPrimitives.add(scheduleMessageAnimation);
                    }
                }
                else {
                    // a send message animation is a list of alternating constant and interpolation frames
                    if (animationParameters.enableSendMessageAnimations) {
                        AnimationPosition beginAnimationPosition = new AnimationPosition(eventNumber, simulationTime, animationPrimitiveContext.frameRelativeAnimationTime, null);
                        for (int i = beginSendEntryIndex + 1; i < endSendEntryIndex; i++) {
                            boolean isLastHop = i == endSendEntryIndex - 1;
                            SendHopEntry sendHopEntry = (SendHopEntry)event.getEventLogEntry(i);
                            BigDecimal propagationDelay = sendHopEntry.getPropagationDelay();
                            BigDecimal transmissionDelay = sendHopEntry.getTransmissionDelay();
                            BigDecimal simulationTimeDuration = transmissionDelay.add(propagationDelay);
                            long beginEventNumber = beginAnimationPosition.getEventNumber();
                            BigDecimal beginSimulationTime = beginAnimationPosition.getSimulationTime();
                            double frameRelativeBeginAnimationTime = beginAnimationPosition.getFrameRelativeAnimationTime();
                            // TODO: arrival of what? first or last bit or what?
                            BigDecimal endSimulationTime = isLastHop ? arrivalTime : beginSimulationTime.add(simulationTimeDuration);
                            long endEventNumber = isLastHop ? arrivalEventNumber : Math.max(beginEventNumber, coordinateSystem.getLastEventNotAfterSimulationTime(endSimulationTime).getEventNumber());
                            double frameRelativeEndAnimationTime;
                            if (simulationTimeDuration.equals(BigDecimal.getZero())) {
                                if (isLastHop)
                                    frameRelativeEndAnimationTime = 0;
                                else
                                    frameRelativeEndAnimationTime = frameRelativeBeginAnimationTime + coordinateSystem.getAnimationTimeDelta(0);
                            }
                            else {
                                // to avoid overlapping with the end event's beginning
                                // we have to count the number of send hops with zero simulation time
                                double remainingAnimationTimeDuration = 0;
                                for (int j = i + 1; j < endSendEntryIndex; j++) {
                                    SendHopEntry remainingSendHopEntry = (SendHopEntry)event.getEventLogEntry(j);
                                    if (remainingSendHopEntry.getPropagationDelay().add(remainingSendHopEntry.getTransmissionDelay()).equals(BigDecimal.getZero()))
                                        remainingAnimationTimeDuration += coordinateSystem.getAnimationTimeDelta(0);
                                }
                                frameRelativeEndAnimationTime = -remainingAnimationTimeDuration;
                            }
                            if (endEventNumber < 0 || lastEvent.getSimulationTime().less(endSimulationTime)) {
                                endEventNumber = lastEvent.getEventNumber();
                                frameRelativeEndAnimationTime = Double.POSITIVE_INFINITY;
                            }
                            frameRelativeEndAnimationTime += animationParameters.handleMessageAnimationShift;
                            EventLogModule sendHopSenderModule = simulation.getModuleById(sendHopEntry.getSenderModuleId());
                            EventLogModule sendHopDestinationModule = isLastHop ? destinationModule : simulation.getModuleById(((SendHopEntry)event.getEventLogEntry(i + 1)).getSenderModuleId());
                            boolean visible = sendHopSenderModule != null && sendHopDestinationModule != null && isCompoundModuleVisible(sendHopSenderModule.getCommonAncestorModule(sendHopDestinationModule).getId());
                            if (visible) {
                                EventLogGate gate = new EventLogGate(sendHopSenderModule, sendHopEntry.getSenderGateId());
                                SendMessageAnimation sendMessageAnimation = new SendMessageAnimation(this, propagationDelay, transmissionDelay, gate, message, animationParameters.handleMessageAnimationShift);
                                sendMessageAnimation.setSourceEventNumber(eventNumber);
                                sendMessageAnimation.setBeginEventNumber(beginEventNumber);
                                sendMessageAnimation.setEndEventNumber(endEventNumber);
                                sendMessageAnimation.setBeginSimulationTime(beginSimulationTime);
                                sendMessageAnimation.setEndSimulationTime(endSimulationTime);
                                sendMessageAnimation.setFrameRelativeBeginAnimationTime(frameRelativeBeginAnimationTime);
                                sendMessageAnimation.setFrameRelativeEndAnimationTime(frameRelativeEndAnimationTime);
                                animationPrimitives.add(sendMessageAnimation);
                            }
                            // NOTE: avoid adding animation time periods where nothing happens
                            beginAnimationPosition = visible || beginEventNumber != endEventNumber ?
                                new AnimationPosition(endEventNumber, endSimulationTime, frameRelativeEndAnimationTime, null) :
                                new AnimationPosition(endEventNumber, endSimulationTime, frameRelativeBeginAnimationTime, null);
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
        message.setBitLength(beginSendEntry.getMessageLength());
        return message;
    }

    private static class AnimationPrimitiveContext {
        public HandleMessageAnimation handleMessageAnimation;
        public double frameRelativeAnimationTime;
        public boolean isCollectingHandleMessageAnimations;
    }
}

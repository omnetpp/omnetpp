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
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleDisplayStringChangedEntry;
import org.omnetpp.eventlog.engine.ModuleMethodBeginEntry;
import org.omnetpp.eventlog.engine.SendDirectEntry;
import org.omnetpp.eventlog.engine.SendHopEntry;
import org.omnetpp.ned.model.DisplayString;

/**
 * The animation controller is responsible for managing the animation primitives while it is playing the animation backward or forward.
 *
 * @author levy
 */
public class AnimationController {
	private final static boolean debug = false;

	/**
	 * A list of timers used during the animation. The queue contains the animationTimer and
	 * animation primitives may add their own timers here. As real time goes by the timer queue
	 * will call its timers based on their settings, so that they can update their figures.
	 */
	private TimerQueue timerQueue;

	/**
	 * The main timer that is responsible for updating the animation position during the animation.
	 */
	private AnimationTimer animationTimer;

	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas. This list is ordered by begin animation time.
	 */
	private ArrayList<IAnimationPrimitive> beginAnimationTimeOrderedPrimitives;

	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas. This list is ordered by end animation time.
	 *
	 * In fact the list is ordered by end simulation time and as soon as any item is required at a given simulation time,
	 * then all the primitives at the very same end simulation time are ordered in place by their end animation time.
	 */
	private ArrayList<IAnimationPrimitive> endAnimationTimeOrderedPrimitives;

	/**
	 * The list of currently active animation primitives. These will be called to refresh their animation at the
	 * current animation position. At any given time it should contain an animation primitive iff the current animation
	 * position is between the begin and end animation time of that primitive.
	 */
	private ArrayList<IAnimationPrimitive> activeAnimationPrimitives;

	/**
	 * The simulation environment is responsible for managing modules, connections, gates, messages.
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
	 * The animation position when the animation was last started or continued. The current animation position
	 * will be calculated by a linear approximation using the elapsed real time and the last start animation position.
	 */
	private AnimationPosition lastStartAnimationPosition;

	/**
	 * The current animation position. It is updated periodically from a timer callback during animation.
	 * This is where the animation is right now. It may be different from the live simulation position
	 * if it is also running and the handling of the last message has not yet finished or the eventlog file
	 * is not yet flushed.
	 */
	private AnimationPosition currentAnimationPosition;

	/**
	 * The animation position which is reflected in the model state. This may be different from the current
	 * animation position temporarily. It is only updated when the model gets updated.
	 */
	private AnimationPosition modelAnimationPosition;

	/**
	 * The begin of the whole animation, may be unspecified.
	 */
	private AnimationPosition beginAnimationPosition;

    /**
     * The start of initialize, may be unspecified.
     */
    private AnimationPosition initializeAnimationPosition;

    /**
     * The start of the first event, may be unspecified.
     */
    private AnimationPosition firstEventAnimationPosition;

	/**
	 * The end of the whole animation, may be unspecified.
	 */
	private AnimationPosition endAnimationPosition;

	/**
	 * The animation position when the animation will be next stopped if there's no such limit, then this position is unspecified.
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
	 * A map for figures and animation related objects. Animation primitives may store information in this map and
	 * may communicate with each other.
	 */
	private Map<Object, Object> figureMap;

	/**
	 * Animation listeners are notified for various events such as changes in the current animation position.
	 */
	private ArrayList<IAnimationListener> animationListeners = new ArrayList<IAnimationListener>();

	/**
	 * The eventlog input that is used to construct the animation primitives.
	 */
	private EventLogInput eventLogInput;

	/**
	 * The coordinate system that maps between animation times, simulation times and event numbers.
	 */
    private AnimationCoordinateSystem coordinateSystem;

    /**
     * Last time in milliseconds.
     */
    private long lastTimeAnimationPositionChangeNotificationWasSent;

    /**
     * The parameters that specifies what is animated and how.
     */
    private AnimationParameters animationParameters;

    /**
     * This is a special purpose comparator for animation positions.
     * It uses the event numbers when the simulation times are equal.
     * Otherwise it uses the relative animation times.
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
		reloadAnimationPrimitives();
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
	public void reloadAnimationPrimitives() {
        figureMap = new HashMap<Object, Object>();
        coordinateSystem = new AnimationCoordinateSystem(eventLogInput, new ArrayList<AnimationPosition>());
        currentAnimationPosition = new AnimationPosition();
        modelAnimationPosition = new AnimationPosition();
        beginAnimationPosition = new AnimationPosition();
        initializeAnimationPosition = new AnimationPosition();
        firstEventAnimationPosition = new AnimationPosition();
        endAnimationPosition = new AnimationPosition();
        stopAnimationPosition = new AnimationPosition();
        lastStartAnimationPosition = new AnimationPosition();
        beginAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        endAnimationTimeOrderedPrimitives = new ArrayList<IAnimationPrimitive>();
        activeAnimationPrimitives = new ArrayList<IAnimationPrimitive>();
        addAnimationPrimitivesForPosition(currentAnimationPosition);
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
	 * Returns the timer queue that is used to schedule timer events during the animation.
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
        BigDecimal simulationTime = getCurrentSimulationTime();
        lastStartRealTime = getCurrentRealTime();
        setCurrentSimulationTime(simulationTime);
        lastStartAnimationPosition = currentAnimationPosition;
        timerQueue.resetTimer(animationTimer);
        refreshAnimation();
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
        animationPositionChanged();
    }

	/**
	 * Checks if the current animation position is at the very beginning.
	 */
	public boolean isAtBeginAnimationPosition() {
		return currentAnimationPosition.equals(getBeginAnimationPosition());
	}

    /**
     * Checks if the current animation position is at the very beginning.
     */
    public boolean isAtInitializeAnimationPosition() {
        return currentAnimationPosition.equals(getInitializeAnimationPosition());
    }

    /**
     * Checks if the current animation position is at the very beginning.
     */
    public boolean isAtFirstEventAnimationPosition() {
        return currentAnimationPosition.equals(getFirstEventAnimationPosition());
    }

	/**
	 * Checks if the current animation position is at the very end.
	 */
	public boolean isAtEndAnimationPosition() {
		return currentAnimationPosition.equals(getEndAnimationPosition());
	}

	/**
	 * Returns the animation position for the very begin of the animation.
	 */
	public AnimationPosition getBeginAnimationPosition() {
        if (!beginAnimationPosition.isCompletelySpecified()) {
            // TODO: check
            long firstEventNumber = eventLogInput.getEventLog().getFirstEvent().getEventNumber();
            beginAnimationPosition = getAnimationPositionForEventNumber(firstEventNumber);
        }

        return beginAnimationPosition;
	}

    /**
     * Returns the animation position for the first moment of the initialize animation.
     */
	public AnimationPosition getInitializeAnimationPosition() {
        if (!initializeAnimationPosition.isCompletelySpecified()) {
            // TODO: check
            long firstEventNumber = eventLogInput.getEventLog().getFirstEvent().getEventNumber();
            initializeAnimationPosition = getAnimationPositionForEventNumber(firstEventNumber);
        }

        return initializeAnimationPosition;
	}

    /**
     * Returns the animation position for the first moment of the first event animation.
     */
    public AnimationPosition getFirstEventAnimationPosition() {
        if (!firstEventAnimationPosition.isCompletelySpecified()) {
            // TODO: check
            long firstEventNumber = eventLogInput.getEventLog().getFirstEvent().getEventNumber();
            firstEventAnimationPosition = getAnimationPositionForEventNumber(firstEventNumber);
        }

        return firstEventAnimationPosition;
    }

    /**
     * Returns the animation position for the very end of the animation.
     */
    public AnimationPosition getEndAnimationPosition() {
        if (!endAnimationPosition.isCompletelySpecified()) {
            long lastEventNumber = eventLogInput.getEventLog().getLastEvent().getEventNumber();
            endAnimationPosition = getAnimationPositionForEventNumber(lastEventNumber);
        }

        return endAnimationPosition;
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
	 * Changes the current event number and notifies listeners.
	 */
	private void setCurrentEventNumber(long eventNumber) {
		setCurrentAnimationPosition(getAnimationPositionForEventNumber(eventNumber));
	}

    /**
     * Returns an animation position representing the processing of the given event number.
     */
    private AnimationPosition getAnimationPositionForEventNumber(long eventNumber) {
        IEvent event = eventLogInput.getEventLog().getEventForEventNumber(eventNumber);
        BigDecimal simulationTime = event.getSimulationTime();
        double animationTime = coordinateSystem.getAnimationTime(event);
        return new AnimationPosition(eventNumber, simulationTime, 0, animationTime);
    }

    /**
	 * Returns the current simulation time.
	 */
	public BigDecimal getCurrentSimulationTime() {
		return currentAnimationPosition.getSimulationTime();
	}

	/**
	 * Changes the current simulation time and notifies listeners.
	 */
	private void setCurrentSimulationTime(BigDecimal simulationTime) {
        setCurrentAnimationPosition(getAnimationPositionForSimulationTime(simulationTime));
	}

    /**
     * Returns an animation position representing the simulation time.
     */
    private AnimationPosition getAnimationPositionForSimulationTime(BigDecimal simulationTime) {
        double animationTime = coordinateSystem.getAnimationTimeForSimulationTime(simulationTime);
        long eventNumber = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime).getEventNumber();
        AnimationPosition animationPosition = new AnimationPosition(eventNumber, simulationTime, 0, animationTime);
        return animationPosition;
    }

	/**
	 * Returns the current animation time.
	 */
	public double getCurrentAnimationTime() {
		return currentAnimationPosition.getOriginRelativeAnimationTime();
	}

	/**
	 * Changes the current animation position according to the given animation time.
	 */
	private void setCurrentAnimationTime(double animationTime) {
		setCurrentAnimationPosition(getAnimationPositionForAnimationTime(animationTime));
	}

	/**
	 * Returns an animation position representing the given animation time.
	 */
    private AnimationPosition getAnimationPositionForAnimationTime(double animationTime) {
        long eventNumber = coordinateSystem.getLastEventNotAfterAnimationTime(animationTime).getEventNumber();
        BigDecimal simulationTime = coordinateSystem.getSimulationTimeForAnimationTime(animationTime);
        return new AnimationPosition(eventNumber, simulationTime, 0, animationTime);
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
        AnimationPosition stopAnimationPosition = getBeginAnimationPosition();
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
        AnimationPosition stopAnimationPosition = getEndAnimationPosition();
        int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
            public double getValue(int index) {
                return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
            }
        }, beginAnimationTimeOrderedPrimitives.size(), currentAnimationPosition.getOriginRelativeAnimationTime(), true);
        // find first incactive animation primitive that starts in the future
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
		startAnimationInternal(simulationTime.greaterOrEqual(getCurrentSimulationTime()), new AnimationPosition(-1, simulationTime, -1, -1));
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
		refreshAnimation();
	}

	/**
     * Stops animation and jumps to the very begin.
	 */
	public void gotoBeginAnimationPosition() {
		gotoAnimationPosition(getBeginAnimationPosition());
	}

    /**
     * Stops animation and jumps to initialize.
     */
	public void gotoInitializeAnimationPosition() {
        gotoAnimationPosition(getInitializeAnimationPosition());
	}

    /**
     * Stops animation and jumps to the first event.
     */
    public void gotoFirstEventAnimationPosition() {
        gotoAnimationPosition(getFirstEventAnimationPosition());
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
        timerQueue.resetTimer(animationTimer);
        setCurrentEventNumber(eventNumber);
        refreshAnimation();
    }

	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(BigDecimal simulationTime) {
		timerQueue.resetTimer(animationTimer);
		setCurrentSimulationTime(simulationTime);
		refreshAnimation();
	}

	/**
	 * Goes to the given animation time without animating.
	 */
	public void gotoAnimationTime(double animationTime) {
		timerQueue.resetTimer(animationTimer);
		setCurrentAnimationTime(animationTime);
		refreshAnimation();
	}

	/**
	 * Refreshes the animation according to the current animation position.
	 */
	public void refreshAnimation() {
		if (!animationCanvas.isDisposed()) {
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
		        // first check for stop at end
		        if (isPlayingForward && getEndAnimationPosition().isCompletelySpecified() && endAnimationPosition.getOriginRelativeAnimationTime() <= animationTime) {
		            setCurrentAnimationPosition(endAnimationPosition);
		            stopAnimationInternal();
		        }
		        // stop at begin
		        else if (!isPlayingForward && getBeginAnimationPosition().isCompletelySpecified() && animationTime <= beginAnimationPosition.getOriginRelativeAnimationTime()) {
		            setCurrentAnimationPosition(beginAnimationPosition);
		            stopAnimationInternal();
		        }
		        // stop at requested stop position
		        else if (stopAnimationPosition.isCompletelySpecified() && (isPlayingForward ? stopAnimationPosition.getOriginRelativeAnimationTime() <= animationTime : stopAnimationPosition.getOriginRelativeAnimationTime() >= animationTime)) {
		            setCurrentAnimationPosition(stopAnimationPosition);
		            stopAnimationInternal();
		        }
		        else
		            setCurrentAnimationPosition(getAnimationPositionForAnimationTime(animationTime));
				refreshAnimation();
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

		// start from beginning if no position
		if (!currentAnimationPosition.isCompletelySpecified())
			setCurrentAnimationPosition(getBeginAnimationPosition());

		if ((forward && (!endAnimationPosition.isCompletelySpecified() || currentAnimationPosition.compareTo(endAnimationPosition) < 0)) ||
			(!forward && (!beginAnimationPosition.isCompletelySpecified() || currentAnimationPosition.compareTo(beginAnimationPosition) > 0)))
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
	 * Updates the animation model according to the current animation position. This will result in a bunch of (de)activate
	 * calls on the appropriate animation primitives. The idea is to go through the animation primitives from
	 * the old animation time to the new animation time in both ordered animation primitive arrays at the same time
	 * and call (de)activate in order.
	 */
	private void updateAnimationModel() {
	    BigDecimal oldSimulationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getSimulationTime() : BigDecimal.getZero();
		double oldAnimationTime = modelAnimationPosition.isCompletelySpecified() ? modelAnimationPosition.getOriginRelativeAnimationTime() : 0;
		BigDecimal newSimulationTime = currentAnimationPosition.getSimulationTime();
		double newAnimationTime = currentAnimationPosition.getOriginRelativeAnimationTime();
		boolean forward = newAnimationTime >= oldAnimationTime;

		int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
		        public double getValue(int index) {
		            return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, beginAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward ? true : false);

		if (forward)
			sortEndOrderedAnimationPrimitivesFragment(oldSimulationTime, newSimulationTime);
		int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getOriginRelativeAnimationTime();
		        }
		    }, endAnimationTimeOrderedPrimitives.size(), oldAnimationTime, forward ? true : false);

		if (!forward) {
			beginOrderedIndex--;
			endOrderedIndex--;
		}

		double previousBeginAnimationTime = forward ? 0 : Double.MAX_VALUE;
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
	    if (currentTime - lastTimeAnimationPositionChangeNotificationWasSent > 100 || !isRunning) {
	        lastTimeAnimationPositionChangeNotificationWasSent = currentTime;
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
	 * Stores a loaded animation primitive.
	 */
	private void addAnimationPrimitive(IAnimationPrimitive animationPrimitive) {
	    Assert.isTrue(animationPrimitive.getBeginAnimationPosition().getSimulationTime() != null && animationPrimitive.getEndAnimationPosition().getSimulationTime() != null);
		// keeps list ordered by begin animation time
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return beginAnimationTimeOrderedPrimitives.get(index).getBeginAnimationPosition().getOriginRelativeAnimationTime();
			    }
		    }, beginAnimationTimeOrderedPrimitives.size(), animationPrimitive.getBeginAnimationPosition().getOriginRelativeAnimationTime(), false);
		beginAnimationTimeOrderedPrimitives.add(index, animationPrimitive);
		// keeps list ordered by end simulation time
		// later it will be sorted in small fragments by end animation time on demand
		index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			    public double getValue(int index) {
			        return endAnimationTimeOrderedPrimitives.get(index).getEndAnimationPosition().getSimulationTime().doubleValue();
			    }
		    }, endAnimationTimeOrderedPrimitives.size(), animationPrimitive.getEndAnimationPosition().getSimulationTime().doubleValue(), false);
		endAnimationTimeOrderedPrimitives.add(index, animationPrimitive);
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
    private void addAnimationPrimitivesForPosition(AnimationPosition animationPosition) {
        if (beginAnimationTimeOrderedPrimitives.size() == 0 && animationCanvas.getEventLog() != null) {
            long begin = System.currentTimeMillis();
            IEventLog eventLog = animationCanvas.getEventLog();
            ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
            animationPrimitives.addAll(collectAnimationPrimitivesForInitializationEventLogEntries(eventLog));
            //animationPrimitives.addAll(collectAnimationPrimitivesForEvents(eventLog.getFirstEvent(), eventLog.getLastEvent()));
            animationPrimitives.addAll(collectAnimationPrimitivesForEvents(eventLog.getFirstEvent(), eventLog.getEventForEventNumber(1000)));
            calculateAnimationTimes(animationPrimitives);
            if (debug)
                System.out.println("addAnimationPrimitivesForPosition:" + (System.currentTimeMillis() - begin) + "ms");
        }
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
            System.out.println("*** SORTED ANIMATION POSITIONS ***");
            for (AnimationPosition animationPosition : animationPositions)
                System.out.println(animationPosition);
        }
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
            Assert.isTrue(animationPrimitive.getBeginAnimationPosition().getOriginRelativeAnimationTime() != -1);
            Assert.isTrue(animationPrimitive.getEndAnimationPosition().getSimulationTime() != null);
            Assert.isTrue(animationPrimitive.getEndAnimationPosition().getOriginRelativeAnimationTime() != -1);
            // TODO: should not need to set the animation time duration
            if (animationPrimitive.getAnimationTimeDuration() == -1) {
                AbstractAnimationPrimitive abstractAnimationPrimitive = (AbstractAnimationPrimitive)animationPrimitive;
                abstractAnimationPrimitive.setAnimationTimeDuration(abstractAnimationPrimitive.getOriginRelativeEndAnimationTime() - abstractAnimationPrimitive.getOriginRelativeBeginAnimationTime());
            }
            if (debug)
                System.out.println(animationPrimitive);
        }
        calculateBeginAnimationTimeOrderedPrimitives(animationPrimitives);
        if (debug) {
            System.out.println("*** BEGIN ANIMATION TIME ORDERED ANIMATION PRIMITIVES ***");
            for (IAnimationPrimitive animationPrimitive : beginAnimationTimeOrderedPrimitives)
                System.out.println(animationPrimitive);
        }
        calculateEndAnimationTimeOrderedPrimitives(animationPrimitives);
        if (debug) {
            System.out.println("*** END ANIMATION TIME ORDERED ANIMATION PRIMITIVES ***");
            for (IAnimationPrimitive animationPrimitive : endAnimationTimeOrderedPrimitives)
                System.out.println(animationPrimitive);
        }
        // TODO: fill in the interpolation lists in the animation coordinate system for each event
        coordinateSystem = new AnimationCoordinateSystem(eventLogInput, animationPositions);
    }

    @SuppressWarnings("unchecked")
    private ArrayList<AnimationPosition> calculateSortedAnimationPositions(ArrayList<IAnimationPrimitive> animationPrimitives) {
        MultiValueMap animationPositionsMap = new MultiValueMap();
        AnimationPosition zeroAnimationPosition = new AnimationPosition(-1, BigDecimal.getZero(), 0, 0);
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
        if (debug)
            for (int i = 0; i < animationPositions.size() - 1; i++)
                Assert.isTrue(animationPositionComparator.compare(animationPositions.get(i), animationPositions.get(i + 1)) <= 0);
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
            if (previousAnimationPosition != null) {
                BigDecimal simulationTimeDelta = animationPosition.getSimulationTime().subtract(previousAnimationPosition.getSimulationTime());
                if (simulationTimeDelta.equals(BigDecimal.getZero())) {
                    double animationTimeDelta = animationPosition.getFrameRelativeAnimationTime() - previousAnimationPosition.getFrameRelativeAnimationTime();
                    if (animationTimeDelta > 0)
                        animationTime += animationTimeDelta;
                }
                else
                    animationTime += coordinateSystem.getAnimationTimeDelta(simulationTimeDelta.doubleValue());
            }
            Assert.isTrue(!Double.isNaN(animationTime));
            animationPosition.setOriginRelativeAnimationTime(animationTime);
            previousAnimationPosition = animationPosition;
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
    }

    private ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForInitializationEventLogEntries(IEventLog eventLog) {
        AnimationPrimitiveContext eventAnimationContext = new AnimationPrimitiveContext();
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        eventAnimationContext.isCollectingHandleMessageAnimations = false;
        for (int i = 0; i < eventLog.getNumInitializationLogEntries(); i++)
            collectAnimationPrimitivesForEventLogEntry(eventAnimationContext, eventLog.getInitializationLogEntry(i), animationPrimitives);
        return animationPrimitives;
    }

    /**
     * Collects animation primitives for all events between the two specified events.
     * The range is extended in both directions with the events having the same simulation time.
     */
    private ArrayList<IAnimationPrimitive> collectAnimationPrimitivesForEvents(IEvent beginEvent, IEvent endEvent) {
        AnimationPrimitiveContext animationPrimitiveContext = new AnimationPrimitiveContext();
        ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();
        // NOTE: we have to begin after an event having a fresh simulation time to find out the proper relativeAnimationTime
        BigDecimal beginSimulationTime = beginEvent.getSimulationTime();
        while (true) {
            IEvent previousEvent = beginEvent.getPreviousEvent();
            if (previousEvent == null || !previousEvent.getSimulationTime().equals(beginSimulationTime))
                break;
            else
                beginEvent = previousEvent;
        }
        // NOTE: we have to end before an event having a fresh simulation time
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
            animationPrimitiveContext.handleMessageAnimation = null;
            animationPrimitiveContext.isCollectingHandleMessageAnimations = true;
            for (int i = 0; i < event.getNumEventLogEntries(); i++)
                collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
            animationPrimitiveContext.isCollectingHandleMessageAnimations = false;
            for (int i = 0; i < event.getNumEventLogEntries(); i++)
                collectAnimationPrimitivesForEventLogEntry(animationPrimitiveContext, event.getEventLogEntry(i), animationPrimitives);
            if (eventNumber == endEvent.getEventNumber())
                break;
            else {
                IEvent nextEvent = event.getNextEvent();
                if (nextEvent != null && !event.getSimulationTime().equals(nextEvent.getSimulationTime()))
                    animationPrimitiveContext.relativeAnimationTime = 0;
                event = nextEvent;
            }
        }
        return animationPrimitives;
    }

    /**
     * Adds new IAnimationPrimitives to the collection based on the EventLogEntry.
     */
    private void collectAnimationPrimitivesForEventLogEntry(AnimationPrimitiveContext animationPrimitiveContext, EventLogEntry eventLogEntry, ArrayList<IAnimationPrimitive> animationPrimitives) {
        IEvent event = eventLogEntry.getEvent();
        long eventNumber = event == null ? -1 : event.getEventNumber();
        BigDecimal simulationTime = event == null ? BigDecimal.getZero() : event.getSimulationTime();
        if (eventLogEntry instanceof EventEntry && event.getModuleId() != 1) {
            EventEntry eventEntry = (EventEntry)eventLogEntry;
            if (isSubmoduleVisible(eventEntry.getModuleId())) {
                if (animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                    if (animationParameters.enableHandleMessageAnimations) {
                        double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                        HandleMessageAnimation animationPrimitive = new HandleMessageAnimation(this, event.getEventNumber(), event.getModuleId());
                        animationPrimitive.setBeginEventNumber(eventNumber);
                        animationPrimitive.setEndEventNumber(eventNumber);
                        animationPrimitive.setBeginSimulationTime(simulationTime);
                        animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                        animationPrimitive.setSimulationTimeDuration(BigDecimal.getZero());
                        animationPrimitive.setSourceEventNumber(eventNumber);
                        animationPrimitives.add(animationPrimitive);
                        animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                        animationPrimitiveContext.handleMessageAnimation = animationPrimitive;
                    }
                }
                else {
                    if (animationPrimitiveContext.handleMessageAnimation != null) {
                        animationPrimitiveContext.handleMessageAnimation.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                        double relativeBeginAnimationTime = animationPrimitiveContext.handleMessageAnimation.getFrameRelativeBeginAnimationTime();
                        animationPrimitiveContext.handleMessageAnimation.setAnimationTimeDuration(animationPrimitiveContext.relativeAnimationTime - relativeBeginAnimationTime);
                    }
                    double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                    animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                }
            }
        }
        else if (eventLogEntry instanceof ModuleCreatedEntry) {
            if (!animationPrimitiveContext.isCollectingHandleMessageAnimations) {
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
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                    animationPrimitive.setSourceEventNumber(eventNumber);
                    animationPrimitives.add(animationPrimitive);
                }
            }
        }
        else if (eventLogEntry instanceof GateCreatedEntry) {
            if (!animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                GateCreatedEntry gateCreatedEntry = (GateCreatedEntry)eventLogEntry;
                EventLogModule module = simulation.getModuleById(gateCreatedEntry.getModuleId());
                EventLogGate gate = new EventLogGate(module, gateCreatedEntry.getGateId());
                gate.setName(gateCreatedEntry.getName());
                gate.setIndex(gateCreatedEntry.getIndex());
                module.addGate(gate);
                CreateGateAnimation animationPrimitive = new CreateGateAnimation(this, gate);
                animationPrimitive.setBeginEventNumber(eventNumber);
                animationPrimitive.setBeginSimulationTime(simulationTime);
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                animationPrimitive.setSourceEventNumber(eventNumber);
                animationPrimitives.add(animationPrimitive);
            }
        }
        else if (eventLogEntry instanceof ConnectionCreatedEntry) {
            if (!animationPrimitiveContext.isCollectingHandleMessageAnimations) {
                ConnectionCreatedEntry connectionCreatedEntry = (ConnectionCreatedEntry)eventLogEntry;
                EventLogModule sourceModule = simulation.getModuleById(connectionCreatedEntry.getSourceModuleId());
                EventLogGate sourceGate = sourceModule.getGateById(connectionCreatedEntry.getSourceGateId());
                EventLogModule destinationModule = simulation.getModuleById(connectionCreatedEntry.getDestModuleId());
                EventLogGate destinationGate = destinationModule.getGateById(connectionCreatedEntry.getDestGateId());
                CreateConnectionAnimation animationPrimitive = new CreateConnectionAnimation(this, new EventLogConnection(sourceModule, sourceGate, destinationModule, destinationGate));
                animationPrimitive.setBeginEventNumber(eventNumber);
                animationPrimitive.setBeginSimulationTime(simulationTime);
                animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                    animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                    animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                    animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                    animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                            animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                            animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                            if (simulationTimeDuration.equals(BigDecimal.getZero())) {
                                double animationTimeDuration = coordinateSystem.getAnimationTimeDelta(0);
                                animationPrimitive.setAnimationTimeDuration(animationTimeDuration);
                                animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                                animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                            animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                            animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                            animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                            sendDirectAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                            sendBroadcastAnimation.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
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
                        animationPrimitive.setFrameRelativeBeginAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                        animationPrimitiveContext.relativeAnimationTime += animationTimeDuration;
                        animationPrimitive.setFrameRelativeEndAnimationTime(animationPrimitiveContext.relativeAnimationTime);
                        animationPrimitive.setSourceEventNumber(eventNumber);
                        animationPrimitives.add(animationPrimitive);
                    }
                }
                else {
                    if (animationParameters.enableSendMessageAnimations) {
                        BigDecimal simulationTimeDelta = BigDecimal.getZero();
                        double localAnimationTime = animationPrimitiveContext.relativeAnimationTime;
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
        public double relativeAnimationTime;
        public boolean isCollectingHandleMessageAnimations;
    }
}

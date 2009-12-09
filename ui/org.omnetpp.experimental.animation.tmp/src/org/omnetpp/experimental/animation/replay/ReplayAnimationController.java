/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.animation.replay;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.eclipse.jface.dialogs.MessageDialog;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.experimental.animation.controller.AnimationPosition;
import org.omnetpp.experimental.animation.controller.IReplayAnimationListener;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.primitives.BubbleAnimation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.DeleteModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationEnvironment;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.ScheduleSelfMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SendBroadcastAnimation;
import org.omnetpp.experimental.animation.primitives.SendDirectAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.experimental.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayMessage;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.ned.model.DisplayString;

/**
 * The animation controller is responsible for managing the animation primitives, navigating back and forth and
 * updating the display accordingly.
 */
public class ReplayAnimationController implements IAnimationEnvironment {
	protected final static double NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.1;
	protected final static double FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE = 1;
	protected final static boolean debug = true;

	/**
	 * A list of timers used during the animation. The queue contains the simulationTimer and
	 * animation primitives may add their own timers here. As real time goes by the timer queue
	 * will call its timers based on their settings, so that they can update their figures.
	 */
	protected TimerQueue timerQueue;

	/**
	 * The main timer that is responsible for updating the animation position during the animation.
	 */
	protected AnimationTimer animationTimer;

	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas. This list is ordered by begin animation time.
	 */
	protected ArrayList<IAnimationPrimitive> beginOrderedAnimationPrimitives = new ArrayList<IAnimationPrimitive>();

	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas. This list is ordered by end animation time.
	 *
	 * In fact the list is ordered by end simulation time and as soon as any item is required at a given simulation time,
	 * then all the primitives at the very same end simulation time are ordered in place by their end animation time.
	 */
	protected ArrayList<IAnimationPrimitive> endOrderedAnimationPrimitives = new ArrayList<IAnimationPrimitive>();

	/**
	 * The list of currently active animation primitives. These will be called to animate at the current position.
	 * At any given time it should contain an animation primitive iff the current animation position is between the
	 * begin and end animation time of the primitive.
	 */
	protected ArrayList<IAnimationPrimitive> activeAnimationPrimitives = new ArrayList<IAnimationPrimitive>();

	/**
	 * The list of HandleMessageAnimations in the order of event numbers.
	 */
	protected ArrayList<HandleMessageAnimation> handleMessageAnimationPrimitives = new ArrayList<HandleMessageAnimation>();

	/**
	 * The simulation environment is responsible for managing modules, connections, gates, messages.
	 */
	protected ReplaySimulation simulation;

	/**
	 * Direction of the simulation in time.
	 */
	protected boolean forward;

	/**
	 * Specifies whether the animation is currently running or not.
	 */
	protected boolean isRunning;

	/**
	 * The animation position when the animation was last started or continued. The current animation position
	 * will be calculated by a linear interpolation from the real time and the last start animation position.
	 */
	protected AnimationPosition lastStartAnimationPosition;

	/**
	 * The current animation position. It is updated periodically from a timer callback during animation.
	 * This is where the animation is right now which may be different from the live simulation position.
	 */
	protected AnimationPosition currentAnimationPosition;

	/**
	 * The animation position which is reflected in the model state. This may differ from the current position
	 * temporaryly, because it is only updated when the model gets updated.
	 */
	protected AnimationPosition modelAnimationPosition;

	/**
	 * The begin of the whole animation if known, otherwise null.
	 */
	protected AnimationPosition beginAnimationPosition;

	/**
	 * The end of the whole animation if known, otherwise null.
	 */
	protected AnimationPosition endAnimationPosition;

	/**
	 * The animation position when the animation will be next stopped if there's no such limit, then this position is invalid.
	 */
	protected AnimationPosition stopAnimationPosition;

	/**
	 * The real time when the animation was last started or continued.
	 */
	protected double lastStartRealTime;

	/**
	 * This multiplier is applied to the default multiplier. It can be set from the GUI and defaults to 1.
	 */
	protected double realTimeToAnimationTimeScale;

	/**
	 * This is the default multiplier to convert real time to animation time.
	 */
	protected double defaultRealTimeToAnimationTimeScale;

	/**
	 * The widget used to display the animation figures.
	 */
	protected AnimationCanvas canvas;

	/**
	 * A map for figures and animation related objects. Animation primitives may store information in this map and
	 * may communicate with each other.
	 */
	protected Map<Object, Object> figureMap;

	/**
	 * The log file opened in the editor.
	 */
	protected IFile file;

	/**
	 * Animation listeners are notified for various events, such as changing the event number or the simulation time.
	 */
	protected ArrayList<IReplayAnimationListener> animationListeners = new ArrayList<IReplayAnimationListener>();

	/**
	 * This reader is used to load entries on demand from the simulation log file.
	 * The reader remains open to be able to load efficiently while animation goes on.
	 */
	protected BufferedReader logFileReader;

	/**
	 * Animation position of last entry read from the log file.
	 */
	protected AnimationPosition loadAnimationPosition;

	/**
	 * Last message loaded. FIXME: this should be removed
	 */
	protected ReplayMessage lastLoadedMessage;

	/**
	 * The current animation mode.
	 */
	protected AnimationMode animationMode = AnimationMode.EVENT;

	public enum AnimationMode {
		LINEAR,		// simulation time and animation time are proportional
		EVENT,		// one event is mapped to one animation time unit
		NON_LINEAR	// simulation time is mapped to animation time using a non linear transformation
	}

	public ReplayAnimationController(AnimationCanvas canvas, IFile file) {
		this.canvas = canvas;
		this.file = file;
	}

	/**
	 * Initialize the controller.
	 */
	public void restart() {
		figureMap = new HashMap<Object, Object>();
		forward = true;
		isRunning = false;
		timerQueue = new TimerQueue();
		animationTimer = new AnimationTimer();
		realTimeToAnimationTimeScale = 1;
		defaultRealTimeToAnimationTimeScale = NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE;
		currentAnimationPosition = new AnimationPosition();
		modelAnimationPosition = new AnimationPosition();
		beginAnimationPosition = new AnimationPosition();
		endAnimationPosition = new AnimationPosition();
		loadAnimationPosition = new AnimationPosition();
		stopAnimationPosition = new AnimationPosition();
		lastStartAnimationPosition = new AnimationPosition();
		timerQueue.start();
	}

	/**
	 * Shuts down the controller and release all resources.
	 */
	public void shutdown() {
		timerQueue.stop();
	}

	/**
	 * Returns the canvas on which the animation figures will be drawn.
	 */
	public AnimationCanvas getCanvas() {
		return canvas;
	}

	/**
	 * Returns the canvas'es root figure.
	 */
	public IFigure getRootFigure() {
		return canvas.getRootFigure();
	}

	/**
	 * Returns the replay simulation environment attached to this controller.
	 */
	public ReplaySimulation getSimulation() {
		return simulation;
	}

	/**
	 * Returns the timer queue that is used to schedule timer events during the animation.
	 */
	public TimerQueue getTimerQueue() {
		return timerQueue;
	}

	/**
	 * Returns the current animation mode.
	 */
	public AnimationMode getAnimationMode() {
		return animationMode;
	}

	/**
	 * Changes the current animation mode.
	 */
	public void setAnimationMode(AnimationMode animationMode) {
		this.animationMode = animationMode;
	}

	/**
	 * Specifies the direction of the animation.
	 */
	public boolean isForward() {
		return forward;
	}

	/**
	 * Specifies whether tha animation is currently running.
	 */
	public boolean isRunning() {
		return isRunning;
	}

	/**
	 * Changes the state of the animation.
	 */
	public void setRunning(boolean isRunning) {
		this.isRunning = isRunning;

		controllerStateChanged();
	}

	/**
	 * Checks if the current animation position is valid or not.
	 */
	public boolean isCurrentAnimationPositionValid() {
		return currentAnimationPosition.isValid();
	}

	/**
	 * Checks if the current animation position is at the very beginning.
	 */
	public boolean isAtAnimationBegin() {
		return currentAnimationPosition.equals(beginAnimationPosition);
	}

	/**
	 * Checks if the current animation position is at the very end.
	 */
	public boolean isAtAnimationEnd() {
		return currentAnimationPosition.equals(endAnimationPosition);
	}

	/**
	 * Calculates the begin animation position.
	 */
	protected AnimationPosition ensureBeginAnimationPosition() {
		if (!beginAnimationPosition.isValid()) {
			long firstEventNumber = 0;
			loadAnimationPrimitivesForPosition(new AnimationPosition(firstEventNumber, -1, -1, -1));
			beginAnimationPosition = getAnimationPositionForEventNumber(firstEventNumber);
		}

		return beginAnimationPosition;
	}

	/**
	 * Calculates the end animation position.
	 */
	protected AnimationPosition ensureEndAnimationPosition() {
		if (!endAnimationPosition.isValid()) {
			loadAnimationPrimitivesForPosition(new AnimationPosition(Long.MAX_VALUE, Double.MAX_VALUE, Long.MAX_VALUE, Double.MAX_VALUE));
			long lastEventNumber = handleMessageAnimationPrimitives.get(handleMessageAnimationPrimitives.size() - 1).getEventNumber();
			endAnimationPosition = getAnimationPositionForEventNumber(lastEventNumber);
		}

		return endAnimationPosition;
	}

	/**
	 * Stores a figure or an animation related object.
	 */
	public Object getFigure(Object key) {
		return figureMap.get(key);
	}

	/**
	 * Retrieves a figure or an animation related object.
	 */
	public void setFigure(Object key, Object value) {
		figureMap.put(key, value);
	}

	/**
	 * Returns the current event number.
	 */
	public long getEventNumber() {
		return currentAnimationPosition.getEventNumber();
	}

	/**
	 * Changes the replay event number and notifies listeners.
	 */
	protected void setEventNumber(long eventNumber) {
		loadAnimationPrimitivesForPosition(new AnimationPosition(eventNumber, -1, -1, -1));
		currentAnimationPosition = new AnimationPosition(
			eventNumber,
			getSimulationTimeForEventNumber(eventNumber),
			getAnimationNumberForEventNumber(eventNumber),
			getAnimationTimeForEventNumber(eventNumber));
	}

	/**
	 * Returns the current simulation time.
	 */
	public double getSimulationTime() {
		return currentAnimationPosition.getSimulationTime();
	}

	/**
	 * Changes the replay simulation time and notifies listeners.
	 */
	protected void setSimulationTime(double simulationTime) {
		loadAnimationPrimitivesForPosition(new AnimationPosition(-1, simulationTime, -1, -1));
		long eventNumber = getLastEventNumberForSimulationTime(simulationTime);
		currentAnimationPosition = new AnimationPosition(
				eventNumber,
				simulationTime,
				getAnimationNumberForEventNumber(eventNumber),
				getAnimationTimeForSimulationTime(simulationTime));
	}

	/**
	 * Returns the current animation number.
	 */
	public long getAnimationNumber() {
		return currentAnimationPosition.getAnimationNumber();
	}

	/**
	 * Changes the replay animation number.
	 */
	protected void setAnimationNumber(long animationNumber) {
		loadAnimationPrimitivesForPosition(new AnimationPosition(-1, -1, animationNumber, -1));
		double animationTime = getAnimationTimeForAnimationNumber(animationNumber);
		currentAnimationPosition = new AnimationPosition(
				getEventNumberForAnimationNumber(animationNumber),
				getSimulationTimeForAnimationTime(animationTime),
				animationNumber,
				animationTime);
	}

	/**
	 * Returns the current animation time.
	 */
	public double getAnimationTime() {
		return currentAnimationPosition.getAnimationTime();
	}

	/**
	 * Changes the replay animation time.
	 */
	protected void setAnimationTime(double animationTime) {
		loadAnimationPrimitivesForPosition(new AnimationPosition(-1, -1, -1, animationTime));
		long animationNumber = getAnimationNumberForAnimationTime(animationTime);
		currentAnimationPosition = new AnimationPosition(
				getEventNumberForAnimationNumber(animationNumber),
				getSimulationTimeForAnimationTime(animationTime),
				animationNumber,
				animationTime);
	}

	/**
	 * Sets the current animation position.
	 */
	protected void setAnimationPosition(AnimationPosition animationPosition) {
		if (animationPosition.isValid())
			currentAnimationPosition = animationPosition;
		else if (animationPosition.isPartiallyValid()) {
			if (animationPosition.getAnimationTime() != -1)
				setAnimationTime(animationPosition.getAnimationTime());
			else if (animationPosition.getAnimationNumber() != -1)
				setAnimationNumber(animationPosition.getAnimationNumber());
			else if (animationPosition.getSimulationTime() != -1)
				setSimulationTime(animationPosition.getSimulationTime());
			else
				setEventNumber(animationPosition.getEventNumber());
		}
		else
			throw new RuntimeException("Invalid animation position");
	}

	/**
	 * Returns the current real time in seconds.
	 */
	public double getRealTime() {
		return System.currentTimeMillis() / 1000.0;
	}

	/**
	 * Returns the scaling factor between animation time and real time.
	 */
	public double getRealTimeToAnimationTimeScale() {
		return realTimeToAnimationTimeScale;
	}

	/**
	 * Changes the scaling factor between animation time and real time.
	 */
	public void setRealTimeToAnimationTimeScale(double realTimeToAnimationTimeScale) {
		this.realTimeToAnimationTimeScale = realTimeToAnimationTimeScale;
		lastStartRealTime = getRealTime();
		lastStartAnimationPosition = currentAnimationPosition;
		timerQueue.resetTimer(animationTimer);
	}

	/**
	 * Returns an animation position representing the processing of the given event number.
	 */
	public AnimationPosition getAnimationPositionForEventNumber(long eventNumber) {
		return new AnimationPosition(eventNumber,
			getSimulationTimeForEventNumber(eventNumber),
			getAnimationNumberForEventNumber(eventNumber),
			getAnimationTimeForEventNumber(eventNumber));
	}

	/**
	 * Returns the event number of the last event that occured before the given simulation time.
	 * If there are more than one event having the same simulation time then the first of them is returned.
	 */
	public long getFirstEventNumberForSimulationTime(double simulationTime) {
		throw new RuntimeException();
	}

	/**
	 * Returns the event number of the last event that occured before the given simulation time.
	 * If there are more than one event having the same simulation time then the last of them is returned.
	 */
	public long getLastEventNumberForSimulationTime(double simulationTime) {
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return handleMessageAnimationPrimitives.get(index).getBeginSimulationTime();
			}
		}, handleMessageAnimationPrimitives.size(), simulationTime, false);

		index--;

		if (index != -1)
			return handleMessageAnimationPrimitives.get(index).getEventNumber();
		else
			return -1;
	}

	/**
	 * Returns the event number of the last event that occured before the given animation time.
	 * If there are more than one event having the same animation time then the last of them is returned.
	 */
	public long getLastEventNumberForAnimationTime(double animationTime) {
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return handleMessageAnimationPrimitives.get(index).getBeginAnimationTime();
			}
		}, handleMessageAnimationPrimitives.size(), animationTime, false);

		index--;

		if (index != -1)
			return handleMessageAnimationPrimitives.get(index).getEventNumber();
		else
			return -1;
	}

	/**
	 * Returns the event number for the given animation number.
	 */
	public long getEventNumberForAnimationNumber(long animationNumber) {
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return handleMessageAnimationPrimitives.get(index).getAnimationNumber();
			}
		}, handleMessageAnimationPrimitives.size(), animationNumber, false);

		index--;

		if (index != -1)
			return handleMessageAnimationPrimitives.get(index).getEventNumber();
		else
			return -1;
	}

	/**
	 * Returns the simulation time when the given event occured.
	 */
	public double getSimulationTimeForEventNumber(long eventNumber) {
		if (0 <= eventNumber && eventNumber < handleMessageAnimationPrimitives.size())
			return handleMessageAnimationPrimitives.get((int)eventNumber).getBeginSimulationTime();
		else
			return -1;
	}

	/**
	 * Returns the simulation time for the given animation number.
	 */
	public double getSimulationTimeForAnimationNumber(long animationNumber) {
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives.get(index).getAnimationNumber();
			}
		}, beginOrderedAnimationPrimitives.size(), animationNumber, true);

		if (index >= 0 && index < beginOrderedAnimationPrimitives.size())
			return beginOrderedAnimationPrimitives.get(index).getBeginSimulationTime();
		else
			return -1;
	}

	/**
	 * Returns the simulation time for the given animation time.
	 */
	public double getSimulationTimeForAnimationTime(double animationTime) {
		switch (animationMode) {
			case LINEAR:
				return animationTime;
			case EVENT:
				long animationNumber = getAnimationNumberForAnimationTime(animationTime);
				double eventSimulationTime = getSimulationTimeForAnimationNumber(animationNumber);
				double nextEventSimulationTime = getSimulationTimeForAnimationNumber(animationNumber + 1);

				return eventSimulationTime + (nextEventSimulationTime - eventSimulationTime) * (animationTime - animationNumber);
			case NON_LINEAR:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Returns the animation number at the given animation time.
	 */
	public long getAnimationNumberForAnimationTime(double animationTime) {
		switch (animationMode) {
			case LINEAR:
				// FIXME: this is wrong
				return getLastEventNumberForSimulationTime(animationTime);
			case EVENT:
				return (long)Math.floor(animationTime);
			case NON_LINEAR:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Returns the animation number at the given event number.
	 */
	public long getAnimationNumberForEventNumber(long eventNumber) {
		if (0 <= eventNumber && eventNumber < handleMessageAnimationPrimitives.size())
			return handleMessageAnimationPrimitives.get((int)eventNumber).getAnimationNumber();
		else
			return -1;
	}

	/**
	 * Returns the animation number of the last event that occured before the given simulation time.
	 * If there are more than one event having the same simulation time then the last of them is returned.
	 */
	public long getLastAnimationNumberForSimulationTime(double simulationTime) {
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives .get(index).getBeginSimulationTime();
			}
		}, beginOrderedAnimationPrimitives.size(), simulationTime, false);

		index--;

		if (index != -1)
			return beginOrderedAnimationPrimitives.get(index).getAnimationNumber();
		else
			return -1;
	}

	/**
	 * Returns the animation time for the given real time.
	 */
	public double getAnimationTimeForRealTime(double realTime) {
		return lastStartAnimationPosition.getAnimationTime() +
			(forward ? 1 : -1) * (realTime - lastStartRealTime) * realTimeToAnimationTimeScale * defaultRealTimeToAnimationTimeScale;
	}

	/**
	 * Returns the animation time for the given simulation time.
	 */
	public double getAnimationTimeForSimulationTime(double simulationTime) {
		switch (animationMode) {
			case LINEAR:
				return simulationTime;
			case EVENT:
				// FIXME: maybe this is wrong?
				int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
					public double getValue(int index) {
						return beginOrderedAnimationPrimitives .get(index).getBeginSimulationTime();
					}
				}, beginOrderedAnimationPrimitives.size(), simulationTime, false);

				index--;

				// FIXME: check if -1 really means interpolation before the firtst event
				IAnimationPrimitive previousAnimationPrimitive;
				long animationNumber;
				double previousSimulationTime;

				if (index == -1) {
					previousAnimationPrimitive = null;
					animationNumber = 0;
					previousSimulationTime = 0;
				}
				else {
					previousAnimationPrimitive = beginOrderedAnimationPrimitives.get(index);
					animationNumber = previousAnimationPrimitive.getAnimationNumber();
					previousSimulationTime = previousAnimationPrimitive.getBeginSimulationTime();
				}

				double nextSimulationTime = getSimulationTimeForAnimationNumber(animationNumber + 1);
				double animationTimeDelta = (simulationTime - previousSimulationTime) / (nextSimulationTime - previousSimulationTime);

				if (animationTimeDelta < 0)
					return animationNumber;
				else
					return animationNumber + animationTimeDelta;
				// FIXME: maybe this is wrong?
				//return getLastAnimationNumberForSimulationTime(simulationTime);
			case NON_LINEAR:
				// TODO:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Returns the animation time for the given event number.
	 */
	public double getAnimationTimeForEventNumber(long eventNumber) {
		switch (animationMode) {
			case LINEAR:
				return getAnimationTimeForSimulationTime(getSimulationTimeForEventNumber(eventNumber));
			case EVENT:
				return getAnimationTimeForAnimationNumber(getAnimationNumberForEventNumber(eventNumber));
			case NON_LINEAR:
				// TODO:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Returns the animation time when the given animation starts.
	 */
	public double getAnimationTimeForAnimationNumber(long animationNumber) {
		switch (animationMode) {
			case LINEAR:
				// TODO:
				throw new RuntimeException();
			case EVENT:
				return animationNumber;
			case NON_LINEAR:
				// TODO:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Adds a new animation listener to the list of listeners.
	 */
	public void addAnimationListener(IReplayAnimationListener listener) {
		animationListeners.add(listener);
	}

	/**
	 * Starts animation backward from the current position with normal speed.
	 * Asynchronous operation.
	 */
	public void runAnimationBack() {
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, new AnimationPosition());
	}

	/**
	 * Starts animation backward from the current position with normal speed and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationBack() {
		AnimationPosition animationPosition = getAnimationPositionForEventNumber(getEventNumber());
		animationPosition = currentAnimationPosition.equals(animationPosition) ? getAnimationPositionForEventNumber(getEventNumber() - 1) : animationPosition;
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, animationPosition);
	}

	/**
	 * Starts animation forward from the current position with normal speed.
	 * Asynchronous operation.
	 */
	public void runAnimation() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, new AnimationPosition());
	}

	/**
	 * Starts animation forward from the current position and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimation() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, new AnimationPosition(getEventNumber() + 1, -1, -1, -1));
	}

	/**
	 * Starts animation forward from the current position with normal speed.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void runAnimationToSimulationTime(double simulationTime) {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, new AnimationPosition(-1, simulationTime, -1, -1));
	}

	/**
	 * Temporarily stops animation.
	 */
	public void stopAnimation() {
		animateStop();
		animateAtCurrentPosition();
	}

	/**
	 * Stops animation and goes to the given position.
	 */
	public void gotoAnimationPosition(AnimationPosition animationPosition) {
		animateStop();
		timerQueue.resetTimer(animationTimer);
		setAnimationPosition(animationPosition);
		animateAtCurrentPosition();
	}

	/**
	 * Stops animation and sets the current event number to 0.
	 */
	public void gotoAnimationBegin() {
		gotoAnimationPosition(ensureBeginAnimationPosition());
	}

	/**
	 * Stops animation and sets the current simulation time to the end of the animation.
	 */
	public void gotoAnimationEnd() {
		gotoAnimationPosition(ensureEndAnimationPosition());
	}

	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(double simulationTime) {
		timerQueue.resetTimer(animationTimer);
		setSimulationTime(simulationTime);
		animateAtCurrentPosition();
	}

	/**
	 * Goes to the given event number without animating.
	 */
	public void gotoEventNumber(long eventNumber) {
		timerQueue.resetTimer(animationTimer);
		setEventNumber(eventNumber);
		animateAtCurrentPosition();
	}

	/**
	 * Goes to the given animation time without animating.
	 */
	public void gotoAnimationNumber(long animationNumber) {
		timerQueue.resetTimer(animationTimer);
		setAnimationNumber(animationNumber);
		animateAtCurrentPosition();
	}

	/**
	 * Goes to the given animation time without animating.
	 */
	public void gotoAnimationTime(double animationTime) {
		timerQueue.resetTimer(animationTimer);
		setAnimationTime(animationTime);
		animateAtCurrentPosition();
	}

	/**
	 * Shows to the current event number and simulation time.
	 */
	public void animateAtCurrentPosition() {
		if (!canvas.isDisposed()) {
			ensureValidAnimationPosition();
			positionChanged();
			updateAnimationModel();

			System.out.println("Displaying " + activeAnimationPrimitives.size() + " primitives at -> " + currentAnimationPosition);

			getRootFigure().getLayoutManager().layout(getRootFigure());

			for (IAnimationPrimitive animationPrimitive : activeAnimationPrimitives)
				animationPrimitive.animateAt(currentAnimationPosition);
		}
	}

	/**
	 * Makes sure that the animation position is valid, it does not point before the begin or after the end and also checks for stop position.
	 */
	protected void ensureValidAnimationPosition() {
		// first check for stop at end
		if (forward && endAnimationPosition.isValid() && currentAnimationPosition.compareTo(endAnimationPosition) >= 0) {
			setAnimationPosition(endAnimationPosition);
			animateStop();
		}

		// stop at begin
		if (!forward && beginAnimationPosition.isValid() && currentAnimationPosition.compareTo(beginAnimationPosition) <= 0) {
			setAnimationPosition(beginAnimationPosition);
			animateStop();
		}

		// stop at requested position
		if (stopAnimationPosition.isPartiallyValid() && (forward ? currentAnimationPosition.compareTo(stopAnimationPosition) >= 0 : currentAnimationPosition.compareTo(stopAnimationPosition) <= 0)) {
			setAnimationPosition(stopAnimationPosition);
			animateStop();
		}
	}

	/**
	 * The simulation timer is responsible to change simulation time as real time elapses.
	 */
	public class AnimationTimer extends Timer {
		public AnimationTimer() {
			super(20, true, true);
		}

		/**
		 * This method is called every 20 milliseconds when animation runs.
		 * It updates animation state.
		 */
		public void run() {
			try {
				setAnimationTime(getAnimationTimeForRealTime(getRealTime()));
				animateAtCurrentPosition();
			}
			catch (RuntimeException e) {
				MessageDialog.openError(null, "Exception raised", e.toString());

				throw e;
			}
		}
	};

	/**
	 * Asynchronously starts animation in the given direction.
	 */
	protected void animateStart(boolean forward, double defaultRealTimeToAnimationTimeScale, AnimationPosition stopAnimationPosition) {
		this.forward = forward;
		this.defaultRealTimeToAnimationTimeScale = defaultRealTimeToAnimationTimeScale;
		this.stopAnimationPosition = stopAnimationPosition;
		setRunning(true);

		// start from beginning if no position
		if (!currentAnimationPosition.isValid())
			setAnimationPosition(ensureBeginAnimationPosition());

		if ((forward && (!endAnimationPosition.isValid() || currentAnimationPosition.compareTo(endAnimationPosition) < 0)) ||
			(!forward && (!beginAnimationPosition.isValid() || currentAnimationPosition.compareTo(beginAnimationPosition) > 0)))
		{
			timerQueue.resetTimer(animationTimer);
			lastStartRealTime = getRealTime();
			lastStartAnimationPosition = currentAnimationPosition;

			if (!timerQueue.hasTimer(animationTimer))
				timerQueue.addTimer(animationTimer);
		}
	}

	/**
	 * Immediately stops animation.
	 */
	protected void animateStop() {
		setRunning(false);
		forward = true;
		stopAnimationPosition = new AnimationPosition();

		if (timerQueue.hasTimer(animationTimer))
			timerQueue.removeTimer(animationTimer);
	}

	/**
	 * Updates the animation model according to the current animation position. This will result in a bunch of (de)activate
	 * calls on the appropriate animation primitives. The idea is to go through the animation primitives from
	 * the old animation time to the new animation time in both ordered animation primitive arrays at the same time
	 * and call (de)activate in order.
	 */
	protected void updateAnimationModel() {
		double oldSimulationTime = modelAnimationPosition.getSimulationTime();
		double oldAnimationTime = modelAnimationPosition.getAnimationTime();
		double newSimulationTime = currentAnimationPosition.getSimulationTime();
		double newAnimationTime = currentAnimationPosition.getAnimationTime();
		boolean forward = newAnimationTime > oldAnimationTime;

		int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives.get(index).getBeginAnimationTime();
			}
		}, beginOrderedAnimationPrimitives.size(), oldAnimationTime, forward ? true : false);

		if (forward)
			sortEndOrderedAnimationPrimitivesFragment(oldSimulationTime, newSimulationTime);
		int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return endOrderedAnimationPrimitives.get(index).getEndAnimationTime();
			}
		}, endOrderedAnimationPrimitives.size(), oldAnimationTime, forward ? true : false);

		if (!forward) {
			beginOrderedIndex--;
			endOrderedIndex--;
		}

		double previousBeginAnimationTime = forward ? 0 : Double.MAX_VALUE;
		double previousEndAnimationTime = previousBeginAnimationTime;

		while (true) {
			boolean beginOrderedIndexValid = beginOrderedIndex >= 0 && beginOrderedIndex < beginOrderedAnimationPrimitives.size();
			boolean endOrderedIndexValid = endOrderedIndex >= 0 && endOrderedIndex < endOrderedAnimationPrimitives.size();
			if (!beginOrderedIndexValid && !endOrderedIndexValid)
				break;

			IAnimationPrimitive beginOrderedAnimationPrimitive = beginOrderedIndexValid ? beginOrderedAnimationPrimitives.get(beginOrderedIndex) : null;
			IAnimationPrimitive endOrderedAnimationPrimitive = endOrderedIndexValid ? endOrderedAnimationPrimitives.get(endOrderedIndex) : null;

			double beginAnimationTime = beginOrderedIndexValid ? beginOrderedAnimationPrimitive.getBeginAnimationTime() : -1;
			double endAnimationTime = endOrderedIndexValid ? endOrderedAnimationPrimitive.getEndAnimationTime() : -1;

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
					if (!beginOrderedAnimationPrimitive.isActive()) {
						System.out.println("Forward animation activate: " +
							beginOrderedAnimationPrimitive.getClass().getSimpleName() +
							"(" + beginOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + beginOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
							":" + beginOrderedIndex);

						beginOrderedAnimationPrimitive.activate();
						activeAnimationPrimitives.add(beginOrderedAnimationPrimitive);
					}

					beginOrderedIndex++;
				}

				// compare using > to allow activations to happen before deactivations having the same time
				if (endOrderedIndexValid  && (!beginOrderedIndexValid || endAnimationTime < beginAnimationTime)) {
					if (endOrderedAnimationPrimitive.isActive()) {
						System.out.println("Forward animation deactivate: " +
							endOrderedAnimationPrimitive.getClass().getSimpleName() +
							"(" + endOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + endOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
							":" + endOrderedIndex);

						endOrderedAnimationPrimitive.deactivate();
						activeAnimationPrimitives.remove(endOrderedAnimationPrimitive);
					}

					endOrderedIndex++;
				}
			}
			else {
				if (endOrderedIndexValid && (!beginOrderedIndexValid || endAnimationTime >= beginAnimationTime)) {
					if (!endOrderedAnimationPrimitive.isActive()) {
						System.out.println("Backward animation activate: " +
							endOrderedAnimationPrimitive.getClass().getSimpleName() +
							"(" + endOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + endOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
							":" + endOrderedIndex);

						endOrderedAnimationPrimitive.activate();
						activeAnimationPrimitives.add(endOrderedAnimationPrimitive);
					}

					endOrderedIndex--;
				}

				// compare using > to allow activations to happen before deactivations having the same time
				if (beginOrderedIndexValid && (!endOrderedIndexValid || beginAnimationTime > endAnimationTime)) {
					if (beginOrderedAnimationPrimitive.isActive()) {
						System.out.println("Backward animation deactivate: " +
							beginOrderedAnimationPrimitive.getClass().getSimpleName() +
							"(" + beginOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + beginOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
							":" + beginOrderedIndex);

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
		if (debug)
			for (IAnimationPrimitive animationPrimitive : beginOrderedAnimationPrimitives)
				if ((animationPrimitive.getBeginAnimationTime() <= getAnimationTime() &&
					 getAnimationTime() <= animationPrimitive.getEndAnimationTime()) !=
					 activeAnimationPrimitives.contains(animationPrimitive))
						throw new RuntimeException("Animation primitive should be active iff its animation time range includes the current animation position");

		modelAnimationPosition = currentAnimationPosition;
	}

	/**
	 * Sorts animation primitives having the given end simulation time based on their end animation time.
	 */
	protected void sortEndOrderedAnimationPrimitivesFragment(double beginSimulationTime, double endSimulationTime) {
		int beginSortIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
				public double getValue(int index) {
					return endOrderedAnimationPrimitives.get(index).getEndSimulationTime();
				}
			}, endOrderedAnimationPrimitives.size(), beginSimulationTime, true);

		int endSortIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return endOrderedAnimationPrimitives.get(index).getEndSimulationTime();
			}
		}, endOrderedAnimationPrimitives.size(), endSimulationTime, false);

		// FIXME: hack to sort the whole bloody shit for now
		beginSortIndex = 0;
		endSortIndex = endOrderedAnimationPrimitives.size();

		if (endSortIndex - beginSortIndex > 1) {
			// copy to temporary array
			IAnimationPrimitive[] endOrderedAnimationPrimitivesFragment = new IAnimationPrimitive[endSortIndex - beginSortIndex];
			for (int i = beginSortIndex; i < endSortIndex; i++)
				endOrderedAnimationPrimitivesFragment[i - beginSortIndex] = endOrderedAnimationPrimitives.get(i);

			// sort based on end animation time
			Arrays.sort(endOrderedAnimationPrimitivesFragment, new Comparator<IAnimationPrimitive>() {
				public int compare(IAnimationPrimitive p1, IAnimationPrimitive p2) {
					double time1 = p1.getEndAnimationTime();
					double time2 = p2.getEndAnimationTime();

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
				endOrderedAnimationPrimitives.set(i, endOrderedAnimationPrimitivesFragment[i - beginSortIndex]);
		}
	}

	/**
	 * Notifies listeners about the new simulation time.
	 */
	protected void positionChanged() {
		for (IReplayAnimationListener listener : animationListeners)
			listener.replayPositionChanged(currentAnimationPosition);
	}

	/**
	 * Notifies listeners about controller state change.
	 */
	protected void controllerStateChanged() {
		for (IReplayAnimationListener listener : animationListeners)
			listener.controllerStateChanged();
	}

	/**
	 * Helper to provide the value for an index on which the binary search is working.
	 */
	protected interface IValueProvider {
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
	protected int getAnimationPrimitiveIndexForValue(IValueProvider valueProvider, int size, double value, boolean first) {
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
//FIXME this crashes on startup, when there are no animation primitives loaded (andras)
//			Assert.isTrue((first && valueProvider.getValue(index) < value) ||
//				   (!first && valueProvider.getValue(index) > value));
			return index;
		}
	}

	/**
	 * Initializes the simulation and adds the root compound module figure to the canvas.
	 */
	protected void initializeSimulation(ReplayModule rootModule) {
		simulation = new ReplaySimulation(rootModule);

		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
		rootModuleFigure.setDisplayString(new DisplayString(""));
		rootModuleFigure.setName(rootModule.getFullPath());
		setFigure(rootModule, rootModuleFigure);
		canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
		canvas.getRootFigure().add(rootModuleFigure);
	}

	/**
	 * Convenient method to get a type safe replay simulation if applicable.
	 */
	protected ReplaySimulation getReplaySimulation() {
		return (ReplaySimulation)simulation;
	}

	/**
	 * Stores a loaded animation primitive.
	 */
	protected void addAnimationPrimitive(IAnimationPrimitive animationPrimitive) {
		// keeps list ordered by begin animation time
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives.get(index).getBeginAnimationTime();
			}
		}, beginOrderedAnimationPrimitives.size(), animationPrimitive.getBeginAnimationTime(), false);
		beginOrderedAnimationPrimitives.add(index, animationPrimitive);

		// keeps list ordered by end simulation time
		// later it will be sorted in small fragments by end animation time on demand
		index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return endOrderedAnimationPrimitives.get(index).getEndSimulationTime();
			}
		}, endOrderedAnimationPrimitives.size(), animationPrimitive.getEndSimulationTime(), false);
		endOrderedAnimationPrimitives.add(index, animationPrimitive);

		// makes sure to have the correct index
		if (animationPrimitive instanceof HandleMessageAnimation)
			handleMessageAnimationPrimitives.add((int)animationPrimitive.getEventNumber(), (HandleMessageAnimation)animationPrimitive);
	}

	/**
	 * Loads all animation primitives that begins before or at the given animation position.
	 */
	protected long loadAnimationPrimitivesForPosition(AnimationPosition animationPosition) {
		try {
			System.out.println("loading animation primitives for: " + animationPosition);
			long animationPrimitivesCount = beginOrderedAnimationPrimitives.size();
			String line = null;

			if (logFileReader == null)
				logFileReader = new BufferedReader(new InputStreamReader(file.getContents()));

			if (!loadAnimationPosition.isValid())
				loadAnimationPosition = new AnimationPosition(0, 0, 0, 0);

			while (animationPosition.compareTo(loadAnimationPosition) <= 0 && (line = logFileReader.readLine()) != null)
			{
				System.out.println("  processing line: \"" + line + "\"");
				String[] tokens = splitLine(line);

				if (tokens.length == 0) {
					// blank line
					continue;
				}
				else if (tokens[0].equals("MC")) {
					// "ModuleCreation" line
					ReplayModule module = new ReplayModule();
					module.setId(getIntegerToken(tokens, "id"));
					module.setName(getToken(tokens, "n"));

					// FIXME: we show the first (root) module for now, should we get it as parameter?
					if (simulation == null)
						initializeSimulation(module);

					addAnimationPrimitive(new CreateModuleAnimation(this, loadAnimationPosition, module, getIntegerToken(tokens, "pid", -1)));
				}
				else if (tokens[0].equals("MD")) {
					// "ModuleDeletion" line
					addAnimationPrimitive(new DeleteModuleAnimation(this, loadAnimationPosition, getIntegerToken(tokens, "id")));
				}
				else if (tokens[0].equals("CC")) {
					// "ConnectionCreation" line
					GateId sourceGateId = new GateId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					GateId targetGateId = new GateId(getIntegerToken(tokens, "dm"), getIntegerToken(tokens, "dg"));
					addAnimationPrimitive(new CreateConnectionAnimation(this, loadAnimationPosition, sourceGateId, targetGateId));
				}
				else if (tokens[0].equals("E")) {
					long animationNumber = loadAnimationPosition.getAnimationNumber() + 1;
					// "Event" line
					loadAnimationPosition = new AnimationPosition(
						getIntegerToken(tokens, "#"),
						getDoubleToken(tokens, "t"),
						animationNumber,
						getAnimationTimeForAnimationNumber(animationNumber));

					// TODO: what if event numbers are not started from 0 or not continous?
					HandleMessageAnimation animationPrimitive = new HandleMessageAnimation(this, loadAnimationPosition, getIntegerToken(tokens, "m"), null);
					addAnimationPrimitive(animationPrimitive);
				}
				else if (tokens[0].equals("BS")) {
					// "BeginSend" line
					lastLoadedMessage = new ReplayMessage();
					lastLoadedMessage.setName(getToken(tokens, "n"));
					lastLoadedMessage.setClassName(getToken(tokens, "c"));
					lastLoadedMessage.setKind((short)getIntegerToken(tokens, "k"));
					lastLoadedMessage.setBitLength((long)getIntegerToken(tokens, "l")); //FIXME longToken!
					int id = getIntegerToken(tokens, "id", -1);
					lastLoadedMessage.setId(id);
					lastLoadedMessage.setTreeId(getIntegerToken(tokens, "tid", id));
					lastLoadedMessage.setEncapsulationId(getIntegerToken(tokens, "eid", id));
					lastLoadedMessage.setEncapsulationTreeId(getIntegerToken(tokens, "etid", id));
				}
				else if (tokens[0].equals("SH")) {
					// "SendHop" line
					int messageId = getIntegerToken(tokens, "id", -1);
					if (lastLoadedMessage.getId()!=messageId) {
						System.out.println("wrong trace: SH line without BS"); //XXX proper error handling
						lastLoadedMessage = null;
					}

					// TODO: handle ts different then E's t
					// FIXME: animationPrimitives are sorted by eventNumber, beginSimulationTime and animationNumber
					// and the binary search relies upon this
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					double propagationTime = getDoubleToken(tokens, "pd", 0);
					double transmissionTime = getDoubleToken(tokens, "td", 0);
					// FIXME: double loadSimulationTime = getDoubleToken(tokens, "ts", loadAnimationPosition.getSimulationTime());
					addAnimationPrimitive(new SendMessageAnimation(this, loadAnimationPosition, propagationTime, transmissionTime, connectionId, lastLoadedMessage));
				}
				else if (tokens[0].equals("SD")) {
					// "SendDirect" line
					int messageId = getIntegerToken(tokens, "id", -1);
					if (lastLoadedMessage.getId()!=messageId) {
						System.out.println("wrong trace: SD line without BS"); //XXX proper error handling
						lastLoadedMessage = null;
					}

					int senderModuleId = getIntegerToken(tokens, "sm");
					int destModuleId = getIntegerToken(tokens, "dm");
					//int destGateId = getIntegerToken(tokens, "dg");
					double transmissionDelay = getDoubleToken(tokens, "td", 0);
					double propagationDelay = getDoubleToken(tokens, "pd", 0);
					addAnimationPrimitive(new SendDirectAnimation(this, loadAnimationPosition, propagationDelay, transmissionDelay, senderModuleId, destModuleId, lastLoadedMessage));
					addAnimationPrimitive(new SendBroadcastAnimation(this, loadAnimationPosition, propagationDelay, transmissionDelay, senderModuleId, destModuleId, lastLoadedMessage));
				}
				else if (tokens[0].equals("SA")) {
					// "ScheduleAt" line
					addAnimationPrimitive(new ScheduleSelfMessageAnimation(this, loadAnimationPosition, getDoubleToken(tokens, "t")));
				}
				else if (tokens[0].equals("DS")) {
					// "DisplayString" line
					IDisplayString displayString = new DisplayString(getToken(tokens, "d"));
					addAnimationPrimitive(new SetModuleDisplayStringAnimation(this, loadAnimationPosition, getIntegerToken(tokens, "id"), displayString));
				}
				else if (tokens[0].equals("CS")) {
					// "ConnectionDisplayString" line
					IDisplayString displayString = new DisplayString(getToken(tokens, "d"));
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					addAnimationPrimitive(new SetConnectionDisplayStringAnimation(this, loadAnimationPosition, connectionId, displayString));
				}
				else if (tokens[0].equals("CE")) {
					// "CancelEvent" line
					//TODO handle
				}
				else if (tokens[0].equals("MM")) {
					// "ModuleMethodCalled" line
					//TODO create a MethodCallAnimation primitive
				}
				else if (tokens[0].equals("BU")) {
					// "Bubble" line
					String text = getToken(tokens, "txt");
					addAnimationPrimitive(new BubbleAnimation(this, loadAnimationPosition, text, getIntegerToken(tokens, "id")));
				}
				else
					System.out.println("Unknown log entry: " + tokens[0]);
			}

			// store the end of the simulation
			if (line == null)
				endAnimationPosition = loadAnimationPosition;

			long numLoaded = beginOrderedAnimationPrimitives.size() - animationPrimitivesCount;
			System.out.println("  loaded " + numLoaded + " primitives");
			return numLoaded;
		}
		catch (Throwable e) {
			e.printStackTrace();

			throw new RuntimeException(e);
		}
	}

	protected String[] splitLine(String line) {
		int lastSpaceIndex = -1;
		int lastQuoteIndex = -1;
		ArrayList<String> tokens = new ArrayList<String>(20);

		for (int i = 0; i < line.length(); i++) {
			char ch = line.charAt(i);

			if (ch == ' ') {
				if (lastQuoteIndex == -1)
					tokens.add(line.substring(lastSpaceIndex + 1, i));

				lastSpaceIndex = i;
			}
			else if (ch == '"') {
				if (lastQuoteIndex == -1) {
					lastSpaceIndex = -1;
					lastQuoteIndex = i;
				}
				else {
					tokens.add(line.substring(lastQuoteIndex + 1, i));
					lastQuoteIndex = -1;
				}
			}
		}

		if (lastSpaceIndex != -1)
			tokens.add(line.substring(lastSpaceIndex + 1, line.length()));

		return (String[])tokens.toArray(new String[0]);
	}

	protected double getDoubleToken(String[] tokens, String key) {
		return getDoubleToken(tokens, key, Double.NaN);
	}

	protected double getDoubleToken(String[] tokens, String key, double defaultValue) {
		String value = getToken(tokens, key);

		return value != null ? Double.parseDouble(value) : defaultValue;
	}

	protected int getIntegerToken(String[] tokens, String key) {
		return getIntegerToken(tokens, key, null);
	}

	protected int getIntegerToken(String[] tokens, String key, Integer defaultValue) {
		String value = getToken(tokens, key);

		return (value != null || defaultValue == null) ? Integer.parseInt(value) : defaultValue;
	}

	protected String getToken(String[] tokens, String key) {
		for (int i = 0; i < tokens.length; i++)
			if (tokens[i].equals(key))
				return tokens[i + 1];

		return null;
	}
}

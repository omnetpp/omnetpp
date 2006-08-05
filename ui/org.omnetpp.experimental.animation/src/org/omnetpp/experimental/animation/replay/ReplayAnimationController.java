package org.omnetpp.experimental.animation.replay;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.IFigure;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.simulation.model.ConnectionId;
import org.omnetpp.common.simulation.model.GateId;
import org.omnetpp.common.simulation.model.IRuntimeSimulation;
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
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.experimental.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayMessage;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;

/**
 * The animation position is identified by the following tuple:
 *  - real time
 *  - animation time
 *  - animation number
 *  - simulation time
 *  - event number
 */
public class ReplayAnimationController implements IAnimationEnvironment {
	protected final static double NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.1;
	protected final static double FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE = 1;

	/**
	 * A list of timers used during the animation. The queue contains the simulationTimer and
	 * animation primitives may add their own timers here. As real time goes by the timer queue
	 * will call its timers based on their settings, so that they can update their figures.
	 */
	protected TimerQueue timerQueue;

	/**
	 * The main timer that is responsible for updating the event number and simulation time during animation.
	 */
	protected AnimationTimer animationTimer;

	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas.
	 */
	protected ArrayList<IAnimationPrimitive> beginOrderedAnimationPrimitives = new ArrayList<IAnimationPrimitive>();
	protected ArrayList<IAnimationPrimitive> endOrderedAnimationPrimitives = new ArrayList<IAnimationPrimitive>();
	protected ArrayList<IAnimationPrimitive> activeAnimationPrimitives = new ArrayList<IAnimationPrimitive>();

	/**
	 * The list of HandleMessageAnimations in the order of event numbers.
	 */
	protected ArrayList<HandleMessageAnimation> handleMessageAnimationPrimitives = new ArrayList<HandleMessageAnimation>();

	/**
	 * The simulation is either a LiveSimulation or a ReplaySimulation.
	 */
	protected IRuntimeSimulation simulation;

	/**
	 * Direction of the simulation in time.
	 */
	protected boolean forward;

	/**
	 * Specifies whether the animation is currently running or not.
	 */
	protected boolean isRunning;

	/**
	 * The current event number. It is updated periodically from a timer callback during animation.
	 * This is where the animation is right now which may be different from the live event number.
	 */
	protected long eventNumber;

	/**
	 * The current simulation time. It is updated periodically from a timer callback during animation.
	 * This is where the animation is right now which may be different from the live simulation time.
	 */
	protected double simulationTime;

	/**
	 * The current animation number.
	 */
	protected long animationNumber;

	/**
	 * The current animation time. Animation time is always proportional to real time but may be
	 * non linear to simulation time.
	 */
	protected double animationTime;

	/**
	 * The animation time when the animation was last started or continued.
	 */
	protected double lastStartAnimationTime;

	/**
	 * The animation time which is reflected in the model state.
	 */
	protected double modelAnimationTime;

	/**
	 * The begin of the whole simulation if known, otherwise -1.
	 */
	protected double beginSimulationTime;

	/**
	 * The begin of the whole simulation if known, otherwise -1.
	 */
	protected long beginAnimationNumber;

	/**
	 * The begin of the whole simulation if known, otherwise -1.
	 */
	protected long beginEventNumber;

	/**
	 * The begin of the whole simulation if known, otherwise -1.
	 */
	protected double beginAnimationTime;

	/**
	 * The end of the whole simulation if known, otherwise -1.
	 */
	protected double endSimulationTime;

	/**
	 * The end of the whole simulation if known, otherwise -1.
	 */
	protected long endAnimationNumber;

	/**
	 * The end of the whole simulation if known, otherwise -1.
	 */
	protected long endEventNumber;

	/**
	 * The end of the whole simulation if known, otherwise -1.
	 */
	protected double endAnimationTime;

	/**
	 * The real time when the animation was last started or continued.
	 */
	protected double lastStartRealTime;

	/**
	 * The event number when the animation will be next stopped or -1 if there's no such limit.
	 */
	protected long nextStopEventNumber;

	/**
	 * The simulation time when the animation will be next stopped or -1 if there's no such limit.
	 */
	protected double nextStopSimulationTime;

	/**
	 * The animation number when the animation will be next stopped or -1 if there's no such limit.
	 */
	protected long nextStopAnimationNumber;

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
	 * The reader remans open to be able to load efficiently while animation goes on.
	 */
	protected BufferedReader logFileReader;

	/**
	 * Last event number read from the log file.
	 */
	protected long loadEventNumber;

	/**
	 * Last event's simulation time read from the log file.
	 */
	protected double loadSimulationTime;

	/**
	 * Last animation number used during reading from the log file.
	 */
	protected long loadAnimationNumber;

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
		nextStopSimulationTime = -1;
		nextStopEventNumber = -1;
		nextStopAnimationNumber = -1;
		realTimeToAnimationTimeScale = 1;
		defaultRealTimeToAnimationTimeScale = NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE;
		eventNumber = -1;
		simulationTime = -1;
		animationNumber = -1;
		animationTime = -1;
		modelAnimationTime = -1;
		beginEventNumber = 0;
		beginSimulationTime = -1;
		beginAnimationNumber = 0;
		beginAnimationTime = -1;
		endEventNumber = -1;
		endSimulationTime = -1;
		endAnimationNumber = -1;
		endAnimationTime = -1;
		timerQueue.start();
		canvas.reset();
	}

	/**
	 * Shutdown the controller and release all resources.
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
	 * Returns the simulation attached to this controller.
	 * If an underlying simulation kernel is running then it is a LiveSimulation.
	 * Otherwise it is a ReplaySimulation working on a log file.
	 */
	public IRuntimeSimulation getSimulation() {
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
	
	public boolean isAnimationPositionValid() {
		return eventNumber >= 0 &&
			simulationTime >= 0 &&
			animationNumber >= 0 &&
			animationTime >= 0;
	}
	
	public boolean isAtAnimationBegin() {
		return eventNumber == beginEventNumber &&
			simulationTime == beginSimulationTime &&
			animationNumber == beginAnimationNumber &&
			animationTime == beginAnimationTime;
	}

	public boolean isAtAnimationEnd() {
		return eventNumber == endEventNumber &&
			simulationTime == endSimulationTime &&
			animationNumber == endAnimationNumber &&
			animationTime == endAnimationTime;
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
		return eventNumber;
	}

	/**
	 * Changes the replay event number and notifies listeners.
	 */
	protected void setEventNumber(long eventNumber) {
		this.eventNumber = eventNumber;
		loadAnimationPrimitivesForPosition();

		// calculate dependent state
		simulationTime = getSimulationTimeForEventNumber(eventNumber);
		animationTime = getAnimationTimeForEventNumber(eventNumber);
		animationNumber = getAnimationNumberForEventNumber(eventNumber);
	}

	/**
	 * Returns the current simulation time.
	 */
	public double getSimulationTime() {
		return simulationTime;
	}

	/**
	 * Changes the replay simulation time and notifies listeners.
	 */
	protected void setSimulationTime(double simulationTime) {
		this.simulationTime = simulationTime;
		loadAnimationPrimitivesForPosition();

		// calculate dependent state
		eventNumber = getLastEventNumberForSimulationTime(simulationTime);
		animationTime = getAnimationTimeForSimulationTime(simulationTime);
		animationNumber = getAnimationNumberForAnimationTime(animationTime);
	}

	/**
	 * Returns the current animation number.
	 */
	public long getAnimationNumber() {
		return animationNumber;
	}

	/**
	 * Changes the replay animation number.
	 */
	protected void setAnimationNumber(long animationNumber) {
		this.animationNumber = animationNumber;
		loadAnimationPrimitivesForPosition();

		// calculate dependent state
		animationTime = getAnimationTimeForAnimationNumber(animationNumber);
		simulationTime = getSimulationTimeForAnimationTime(animationTime);
		eventNumber = getLastEventNumberForSimulationTime(simulationTime);
	}

	/**
	 * Returns the current animation time.
	 */
	public double getAnimationTime() {
		return animationTime;
	}

	/**
	 * Changes the replay animation time.
	 */
	protected void setAnimationTime(double animationTime) {
		this.animationTime = animationTime;
		loadAnimationPrimitivesForPosition();

		// calculate dependent state
		simulationTime = getSimulationTimeForAnimationTime(animationTime);
		animationNumber = getAnimationNumberForAnimationTime(animationTime);
		eventNumber = getEventNumberForAnimationNumber(animationNumber);
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
		lastStartAnimationTime = getAnimationTime();
		timerQueue.resetTimer(animationTimer);
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
				return handleMessageAnimationPrimitives.get(index).getSimulationTime();
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
		switch (animationMode ){
			case LINEAR:
				return getLastEventNumberForSimulationTime(animationTime);
			case EVENT:
				return (long)Math.floor(animationTime);
			case NON_LINEAR:
				throw new RuntimeException();
		}

		throw new RuntimeException("Unreacheable code reached");
	}

	/**
	 * Returns the event number for the given animation number.
	 */
	public long getEventNumberForAnimationNumber(long animationNumber) {
		// TODO: make this more efficient
		for (int i = Math.min((int)animationNumber, handleMessageAnimationPrimitives.size() - 1); i >= 0; i--)
			if (handleMessageAnimationPrimitives.get(i).getAnimationNumber() == animationNumber)
				return handleMessageAnimationPrimitives.get(i).getEventNumber();

		return -1;
	}

	/**
	 * Returns the simulation time when the given event occured.
	 */
	public double getSimulationTimeForEventNumber(long eventNumber) {
		if (0 <= eventNumber && eventNumber < handleMessageAnimationPrimitives.size())
			return handleMessageAnimationPrimitives.get((int)eventNumber).getSimulationTime();
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
			return beginOrderedAnimationPrimitives.get(index).getSimulationTime();
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
	 * Returns the animation time for the given real time.
	 */
	public double getAnimationTimeForRealTime(double realTime) {
		return lastStartAnimationTime +
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
				// FIXME: this is wrong
				return getLastEventNumberForSimulationTime(simulationTime);
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
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, 0, 0, 0);
	}

	/**
	 * Starts animation backward from the current position with normal speed and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimationBack() {
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, eventNumber - 1, 0, 0);
	}

	/**
	 * Starts animation forward from the current position with normal speed.
	 * Asynchronous operation.
	 */
	public void runAnimation() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}

	/**
	 * Starts animation forward from the current position and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void stepAnimation() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, eventNumber + 1, -1, -1);
	}

	/**
	 * Starts animation forward from the current position with normal speed.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void runAnimationToSimulationTime(double simulationTime) {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, simulationTime, -1);
	}

	/**
	 * Temporarily stops animation.
	 */
	public void stopAnimation() {
		animateStop();
		animateAtCurrentPosition();
	}
	
	public void gotoAnimationPosition(AnimationPosition animationPosition) {
		// TODO: when reworked animation positions using the new position class
	}

	/**
	 * Stops animation and sets the current event number to 0.
	 */
	public void gotoAnimationBegin() {
		animateStop();
		gotoEventNumber(beginEventNumber);
	}

	/**
	 * Stops animation and sets the current simulation time to the end of the animation.
	 */
	public void gotoAnimationEnd() {
		animateStop();
		gotoSimulationTime(Double.MAX_VALUE);
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
			updateAnimationModel(modelAnimationTime, animationTime);
			modelAnimationTime = animationTime;
	
			System.out.println("Displaying " + activeAnimationPrimitives.size() + " primitives at -> Event number: " + eventNumber + " Simulation time: " + simulationTime + " Animation number: " + animationNumber + " AnimationTime: " + animationTime);
	
			for (IAnimationPrimitive animationPrimitive : activeAnimationPrimitives)
				animationPrimitive.animateAt(eventNumber, simulationTime, animationNumber, animationTime);
	
			getRootFigure().getLayoutManager().layout(getRootFigure());
		}
	}

	/**
	 * Makes sure that the animation position is valid, it does not point before the begin or after the end.
	 * Also checks for stop position set by the user.
	 */
	protected void ensureValidAnimationPosition() {
		// first check for stop at end
		if (forward &&
			((endEventNumber != -1 && eventNumber > endEventNumber) ||
			 (endSimulationTime != -1 && simulationTime > endSimulationTime) ||
			 (endAnimationNumber != -1 && animationNumber > endAnimationNumber) ||
			 (endAnimationTime != -1 && animationTime > endAnimationTime)))
		{
			setEventNumber(endEventNumber);
			animateStop();
		}

		// stop at begin
		if (eventNumber < beginEventNumber ||
			simulationTime < beginSimulationTime ||
			animationNumber < beginAnimationNumber ||
			animationTime < beginAnimationTime)
		{
			if (!forward) {
				setEventNumber(beginEventNumber);
				animateStop();
			}
			else
				setEventNumber(beginEventNumber);
		}

		// stop at event number
		if (nextStopEventNumber != -1 && forward ? eventNumber >= nextStopEventNumber : eventNumber < nextStopEventNumber) {
			setEventNumber(nextStopEventNumber);
			animateStop();
		}

		// stop at simulation time
		if (nextStopSimulationTime != -1 && forward ? simulationTime >= nextStopSimulationTime : simulationTime < nextStopSimulationTime) {
			setSimulationTime(nextStopSimulationTime);
			animateStop();
		}

		// stop at animation number
		if (nextStopAnimationNumber != -1 && forward ? animationNumber >= nextStopAnimationNumber : animationNumber < nextStopAnimationNumber) {
			setAnimationNumber(nextStopAnimationNumber);
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
			setAnimationTime(getAnimationTimeForRealTime(getRealTime()));
			animateAtCurrentPosition();
		}
	};

	/**
	 * Asynchronously starts animation in the given direction.
	 */
	protected void animateStart(boolean forward, double defaultRealTimeToAnimationTimeScale, long nextStopEventNumber, double nextStopSimulationTime, long nextStopAnimationNumber) {
		this.forward = forward;
		this.defaultRealTimeToAnimationTimeScale = defaultRealTimeToAnimationTimeScale;
		this.nextStopEventNumber = nextStopEventNumber;
		this.nextStopSimulationTime = nextStopSimulationTime;
		this.nextStopAnimationNumber = nextStopAnimationNumber;
		setRunning(true);
		
		if (!isAnimationPositionValid())
			gotoEventNumber(0);

		if ((forward && (endSimulationTime == -1 || simulationTime < endSimulationTime)) ||
			(!forward && simulationTime > 0))
		{
			timerQueue.resetTimer(animationTimer);
			lastStartRealTime = getRealTime();
			lastStartAnimationTime = animationTime;

			if (!timerQueue.hasTimer(animationTimer))
				timerQueue.addTimer(animationTimer);
		}
	}

	protected void animateStop() {
		setRunning(false);
		forward = true;
		nextStopEventNumber = -1;
		nextStopSimulationTime = -1;
		nextStopAnimationNumber = -1;

		if (timerQueue.hasTimer(animationTimer))
			timerQueue.removeTimer(animationTimer);
	}

	/**
	 * Updates the animation model according to the new animation time. This will result in a bunch of undo/redo
	 * calls on the appropriate animation primitives. The idea is to go through the animation primitives from
	 * the old animation time to the new animation time in both ordered animation primitive arrays at the same time.
	 */
	protected void updateAnimationModel(double oldAnimationTime, double newAnimationTime) {
		boolean forward = newAnimationTime > oldAnimationTime;

		int beginOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives.get(index).getBeginAnimationTime();
			}
		}, beginOrderedAnimationPrimitives.size(), oldAnimationTime, false);

		int endOrderedIndex = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return endOrderedAnimationPrimitives.get(index).getEndAnimationTime();
			}
		}, endOrderedAnimationPrimitives.size(), oldAnimationTime, false);
		
		if (!forward) {
			beginOrderedIndex--;
			endOrderedIndex--;
		}

		while (true) {
			boolean beginOrderedIndexValid = beginOrderedIndex >= 0 && beginOrderedIndex < beginOrderedAnimationPrimitives.size();
			boolean endOrderedIndexValid = endOrderedIndex >= 0 && endOrderedIndex < endOrderedAnimationPrimitives.size();
			if (!beginOrderedIndexValid && !endOrderedIndexValid)
				break;

			IAnimationPrimitive beginOrderedAnimationPrimitive = beginOrderedIndexValid ? beginOrderedAnimationPrimitives.get(beginOrderedIndex) : null;
			IAnimationPrimitive endOrderedAnimationPrimitive = endOrderedIndexValid ? endOrderedAnimationPrimitives.get(endOrderedIndex) : null;

			double beginAnimationTime = beginOrderedIndexValid ? beginOrderedAnimationPrimitive.getBeginAnimationTime() : -1;
			double endAnimationTime = endOrderedIndexValid ? endOrderedAnimationPrimitive.getEndAnimationTime() : -1;
			
			if (forward ? beginAnimationTime > newAnimationTime : beginAnimationTime <= newAnimationTime) {
				beginOrderedIndex = -1;
				beginOrderedIndexValid = false;
				beginOrderedAnimationPrimitive = null;
			}
			
			if (forward ? endAnimationTime > newAnimationTime : endAnimationTime <= newAnimationTime) {
				endOrderedIndex = -1;
				endOrderedIndexValid = false;
				endOrderedAnimationPrimitive = null;
			}

			if (forward) {	
				if (beginOrderedIndexValid && (!endOrderedIndexValid || beginAnimationTime <= endAnimationTime)) {
					System.out.println("Forward animation redo: " +
						beginOrderedAnimationPrimitive.getClass().getSimpleName() +
						"(" + beginOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + beginOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
						":" + beginOrderedIndex);

					beginOrderedAnimationPrimitive.redo();
					beginOrderedIndex++;
					activeAnimationPrimitives.add(beginOrderedAnimationPrimitive);
				}
	
				if (endOrderedIndexValid && (!beginOrderedIndexValid || endAnimationTime <= beginAnimationTime)) {
					System.out.println("Forward animation undo: " +
						endOrderedAnimationPrimitive.getClass().getSimpleName() + 
						"(" + endOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + endOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
						":" + endOrderedIndex);

					endOrderedAnimationPrimitive.undo();
					endOrderedIndex++;
					activeAnimationPrimitives.remove(endOrderedAnimationPrimitive);
				}
			}
			else {
				if (endOrderedIndexValid && (!beginOrderedIndexValid || endAnimationTime >= beginAnimationTime)) {
					System.out.println("Backward animation redo: " +
						endOrderedAnimationPrimitive.getClass().getSimpleName() +
						"(" + endOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + endOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
						":" + endOrderedIndex);

					endOrderedAnimationPrimitive.redo();
					endOrderedIndex--;
					activeAnimationPrimitives.add(endOrderedAnimationPrimitive);
				}

				if (beginOrderedIndexValid && (!endOrderedIndexValid || beginAnimationTime >= endAnimationTime)) {
					System.out.println("Backward animation undo: " +
						beginOrderedAnimationPrimitive.getClass().getSimpleName() +
						"(" + beginOrderedAnimationPrimitive.getBeginAnimationTime() + "-" + beginOrderedAnimationPrimitive.getEndAnimationTime() + ")" +
						":" + beginOrderedIndex);

					beginOrderedAnimationPrimitive.undo();
					beginOrderedIndex--;
					activeAnimationPrimitives.remove(beginOrderedAnimationPrimitive);
				}
			}
		}
	}

	/**
	 * Notifies listeners about the new simulation time.
	 */
	protected void positionChanged() {
		for (IReplayAnimationListener listener : animationListeners)
			listener.replayPositionChanged(eventNumber, simulationTime, animationNumber, animationTime);
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
			assert((first && valueProvider.getValue(index) < value) ||
				   (!first && valueProvider.getValue(index) > value));
			return index;
		}
	}

	/**
	 * Initalizes the simulation and adds the root compound module figure to the canvas.
	 */
	protected void initializeSimulation(ReplayModule rootModule) {
		simulation = new ReplaySimulation(rootModule);

		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
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
		int index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return beginOrderedAnimationPrimitives.get(index).getBeginAnimationTime();
			}
		}, beginOrderedAnimationPrimitives.size(), animationPrimitive.getBeginAnimationTime(), false);
		beginOrderedAnimationPrimitives.add(index, animationPrimitive);

		index = getAnimationPrimitiveIndexForValue(new IValueProvider() {
			public double getValue(int index) {
				return endOrderedAnimationPrimitives.get(index).getEndAnimationTime();
			}
		}, endOrderedAnimationPrimitives.size(), animationPrimitive.getEndAnimationTime(), false);
		endOrderedAnimationPrimitives.add(index, animationPrimitive);

		if (animationPrimitive instanceof HandleMessageAnimation)
			handleMessageAnimationPrimitives.add((int)animationPrimitive.getEventNumber(), (HandleMessageAnimation)animationPrimitive);
	}

	/**
	 * Loads all animation primitives that begins before or at the current replay position.
	 */
	protected long loadAnimationPrimitivesForPosition() {
		try {
			long animationPrimitivesCount = beginOrderedAnimationPrimitives.size();
			int lineCount = 0;
			String line = null;

			if (logFileReader == null)
				logFileReader = new BufferedReader(new InputStreamReader(file.getContents()));

			while ((loadEventNumber <= eventNumber ||
					loadSimulationTime <= simulationTime ||
					loadAnimationNumber <= animationNumber ||
					getAnimationTimeForSimulationTime(loadSimulationTime) <= animationTime ||
					lineCount < 10) &&
				   (line = logFileReader.readLine()) != null)
			{
				lineCount++;
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

					addAnimationPrimitive(new CreateModuleAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, module, getIntegerToken(tokens, "pid", -1)));
				}
				else if (tokens[0].equals("MD")) {
					// "ModuleDeletion" line
					addAnimationPrimitive(new DeleteModuleAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, getIntegerToken(tokens, "id")));
				}
				else if (tokens[0].equals("CC")) {
					// "ConnectionCreation" line
					GateId sourceGateId = new GateId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					GateId targetGateId = new GateId(getIntegerToken(tokens, "dm"), getIntegerToken(tokens, "dg"));
					addAnimationPrimitive(new CreateConnectionAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, sourceGateId, targetGateId));
				}
				else if (tokens[0].equals("E")) {
					// "Event" line
					loadEventNumber = getIntegerToken(tokens, "#");
					loadSimulationTime = getDoubleToken(tokens, "t");
					loadAnimationNumber++;

					// TODO: what if event numbers are not started from 0 or not continous?
					IAnimationPrimitive handleMessageAnimationPrimitive = new HandleMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, getIntegerToken(tokens, "m"), null);

					// store the beginning of the simulation
					if (loadEventNumber == 0) {
						beginEventNumber = loadEventNumber;
						beginSimulationTime = loadSimulationTime;
						beginAnimationNumber = loadAnimationNumber;
						beginAnimationTime = handleMessageAnimationPrimitive.getBeginAnimationTime();
					}

					addAnimationPrimitive(handleMessageAnimationPrimitive );
				}
				else if (tokens[0].equals("BS")) {
					// "BeginSend" line
					lastLoadedMessage = new ReplayMessage();
					lastLoadedMessage.setName(getToken(tokens, "n"));
					lastLoadedMessage.setClassName(getToken(tokens, "c"));
					lastLoadedMessage.setKind(getIntegerToken(tokens, "k"));
					lastLoadedMessage.setLength(getIntegerToken(tokens, "l"));
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
					double loadSimulationTime = getDoubleToken(tokens, "ts", this.loadSimulationTime);
					addAnimationPrimitive(new SendMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, propagationTime, transmissionTime, connectionId, lastLoadedMessage));
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
					//addAnimationPrimitive(new SendDirectAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, propagationDelay, transmissionDelay, senderModuleId, destModuleId, lastLoadedMessage));
					addAnimationPrimitive(new SendBroadcastAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, propagationDelay, transmissionDelay, senderModuleId, destModuleId, lastLoadedMessage));
				}
				else if (tokens[0].equals("SA")) {
					// "ScheduleAt" line
					addAnimationPrimitive(new ScheduleSelfMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, getDoubleToken(tokens, "t")));
				}
				else if (tokens[0].equals("DS")) {
					// "DisplayString" line
					IDisplayString displayString = new DisplayString(null, null, getToken(tokens, "d"));
					addAnimationPrimitive(new SetModuleDisplayStringAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, getIntegerToken(tokens, "id"), displayString));
				}
				else if (tokens[0].equals("CS")) {
					// "ConnectionDisplayString" line
					IDisplayString displayString = new DisplayString(null, null, getToken(tokens, "d"));
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					addAnimationPrimitive(new SetConnectionDisplayStringAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, connectionId, displayString));
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
					addAnimationPrimitive(new BubbleAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, text, getIntegerToken(tokens, "id")));
				}
				else
					throw new RuntimeException("Unknown log entry: "+tokens[0]);
			}

			// store the end of the simulation
			if (line == null) {
				endEventNumber = loadEventNumber;
				endSimulationTime = loadSimulationTime;
				endAnimationNumber = loadAnimationNumber;
				endAnimationTime = getAnimationTimeForAnimationNumber(endAnimationNumber);
			}

			return beginOrderedAnimationPrimitives.size() - animationPrimitivesCount;
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

		return value != null || defaultValue == null ? Integer.parseInt(value) : defaultValue;
	}

	protected String getToken(String[] tokens, String key) {
		for (int i = 0; i < tokens.length; i++)
			if (tokens[i].equals(key))
				return tokens[i + 1];

		return null;
	}
}

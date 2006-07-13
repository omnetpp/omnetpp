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
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.controller.IAnimationListener;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.DeleteModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationEnvironment;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.ScheduleSelfMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetConnectionDisplayStringAnimation;
import org.omnetpp.experimental.animation.primitives.SetModuleDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class ReplayAnimationController implements IAnimationController, IAnimationEnvironment {
	private final static double NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.01;
	private final static double FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.1;
	private final static double EXPRESS_REAL_TIME_TO_ANIMATION_TIME_SCALE = 1;
	
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
	protected ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
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
	 * The end of the whole simulation if known, otherwise -1.
	 */
	protected double endSimulationTime;
	
	/**
	 * The animation time when the animation was last started or continued.
	 */
	protected double lastStartAnimationTime;

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
	protected ArrayList<IAnimationListener> animationListeners = new ArrayList<IAnimationListener>();

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
		this.figureMap = new HashMap<Object, Object>(); 
		this.forward = true;
		this.timerQueue = new TimerQueue();
		this.animationTimer = new AnimationTimer();
		this.endSimulationTime = -1;
		this.nextStopSimulationTime = -1;
		this.nextStopEventNumber = -1;
		this.nextStopAnimationNumber = -1;
		this.realTimeToAnimationTimeScale = 1;
		this.defaultRealTimeToAnimationTimeScale = NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE;
		this.simulationTime = 0;
		this.animationTime = 0;
		this.eventNumber = 0;

		timerQueue.start();
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
	 * Delegates to simulation.
	 */
	public IRuntimeModule getModuleByID(int id) {
		return simulation.getModuleByID(id);
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
		animationNumber =getAnimationNumberForAnimationTime(animationTime);
		positionChanged();
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
		positionChanged();
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
		positionChanged();
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
		eventNumber = getLastEventNumberForSimulationTime(simulationTime);
		animationNumber = getAnimationNumberForAnimationTime(animationTime);
		positionChanged();
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
				return animationPrimitives.get(index).getBeginSimulationTime();
			}
		}, simulationTime, false);
		
		if (index != -1)
			return animationPrimitives.get(index).getEventNumber();
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
				return animationPrimitives.get(index).getAnimationNumber();
			}
		}, animationNumber, false);
		
		if (index != -1)
			return animationPrimitives.get(index).getBeginSimulationTime();
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
	 * Returns the event number of the last event that occured before the given animation time.
	 * If there are more than one event having the same animation time then the last of them is returned.
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
	 * Returns the animation time for the given real time.
	 */
	public double getAnimationTimeForRealTime(double realTime) {
		return lastStartAnimationTime +
			(forward ? 1 : -1) * (realTime - lastStartRealTime) * realTimeToAnimationTimeScale * defaultRealTimeToAnimationTimeScale;
	}

	/**
	 * Returns the animation time for the given simulation time.
	 */
	private double getAnimationTimeForSimulationTime(double simulationTime) {
		switch (animationMode) {
			case LINEAR:
				return simulationTime;
			case EVENT:
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
	private double getAnimationTimeForEventNumber(long eventNumber) {
		return getAnimationTimeForSimulationTime(getSimulationTimeForEventNumber(eventNumber));
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
	public void addAnimationListener(IAnimationListener listener) {
		animationListeners.add(listener);
	}

	/**
	 * Starts animation backward from the current simulation time with normal speed.
	 * Asynchronous operation.
	 */
	public void animateBack() {
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, 0, 0, 0);
	}

	/**
	 * Starts animation forward from the current simulation time with express speed.
	 * Asynchronous operation.
	 */
	public void animateExpress() {
		animateStart(true, EXPRESS_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}

	/**
	 * Starts animation forward from the current simulation time with fast speed.
	 * Asynchronous operation.
	 */
	public void animateFast() {
		animateStart(true, FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}
	
	/**
	 * Starts animation forward from the current event number and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void animateStep() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, animationNumber + 1);
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Asynchronous operation.
	 */
	public void animatePlay() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1, -1);
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void animateToSimulationTime(double simulationTime) {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, simulationTime, -1);
	}

	/**
	 * Temporarily stops animation.
	 */
	public void animateStop() {
		forward = true;
		nextStopEventNumber = -1;
		nextStopSimulationTime = -1;
		nextStopAnimationNumber = -1;
		
		if (timerQueue.hasTimer(animationTimer))
			timerQueue.removeTimer(animationTimer);
	}

	/**
	 * Stops animation and sets the current simulation time to 0.
	 */
	public void gotoBegin() {
		animateStop();
		gotoAnimationTime(0);
	}

	/**
	 * Stops animation and sets the current simulation time to the end of the animation.
	 */
	public void gotoEnd() {
		animateStop();
		gotoAnimationTime(Double.MAX_VALUE);
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
		// stop at begin
		if (!forward && simulationTime < 0) {
			setSimulationTime(0);
			animateStop();
		}

		// stop at end
		if (forward && endSimulationTime != -1 && (simulationTime > endSimulationTime || simulationTime == -1)) {
			setSimulationTime(endSimulationTime);
			animateStop();
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

		System.out.println("Displaying -> Event number: " + eventNumber + " Simulation time: " + simulationTime + " Animation number: " + animationNumber + " AnimationTime: " + animationTime);

		for (IAnimationPrimitive animationPrimitive : animationPrimitives)
			animationPrimitive.animateAt(eventNumber, simulationTime, animationNumber, animationTime);

		getRootFigure().getLayoutManager().layout(getRootFigure());
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
	protected void animateStart(boolean forward, double defaultRealTimeToAnimationTimeScale, long nextStopEventNumber, double stopSimulationTime, long nextStopAnimationNumber) {
		this.forward = forward;
		this.defaultRealTimeToAnimationTimeScale = defaultRealTimeToAnimationTimeScale;
		this.nextStopEventNumber = nextStopEventNumber;
		this.nextStopSimulationTime = stopSimulationTime;
		this.nextStopAnimationNumber = nextStopAnimationNumber;
		
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

	/**
	 * Notifies listeners about the new simulation time.
	 */
	protected void positionChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.replayPositionChanged(eventNumber, simulationTime, animationNumber, animationTime);
	}

	/**
	 * Helper to provide the value for an index on which the binary search is working.
	 */
	protected interface IValueProvider {
		public double getValue(int index);
	}

	/**
	 * Binary search among animation primitives.
	 */
	protected int getAnimationPrimitiveIndexForValue(IValueProvider valueProvider, double value, boolean first) {
		int index = -1;
		int left = 0;
		int size = animationPrimitives.size();
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
	        	break;
	        }
            else if (value < midValue)
	            right = mid - 1;
	        else
	            left = mid + 1;
		}

		if (left > right)
			if (first && left < size)
				if (value < valueProvider.getValue(left))
					index = left - 1;
				else
					index = left;
			else if (!first && right >= 0)
				if (value > valueProvider.getValue(right))
					index = right + 1;
				else
					index = right;

		if (index < 0 || index >= size)
			return -1;
		else {
			assert((first && valueProvider.getValue(index) < value) ||
				   (!first && valueProvider.getValue(index) > value));
			return index;
		}
	}
	
	/**
	 * Returns the nth animation primitive. Returns null if index falls outside of range.
	 */
	protected IAnimationPrimitive getAnimationPrimitive(int index) {
		return (0 <= index && index < animationPrimitives.size()) ? animationPrimitives.get(index) : null;
	}

	/**
	 * Initalizes the simulation and adds the root compound module figure to the canvas.
	 */
	protected void initializeSimulation(IRuntimeModule rootModule) {
		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
		rootModuleFigure.setDisplayString(new DisplayString(null, null, "bgb=600,600;bgi=background/hungary,stretch"));
		setFigure(new GateId(rootModule.getId(), 0), new GateAnchor(rootModuleFigure));
		setFigure(new GateId(rootModule.getId(), 1), new GateAnchor(rootModuleFigure));
		canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
		canvas.getRootFigure().add(rootModuleFigure);
		
		simulation = createSimulation(rootModule);
	}
	
	/**
	 * Factory method to create the simulation object.
	 */
	protected IRuntimeSimulation createSimulation(IRuntimeModule rootModule) {
		return new ReplaySimulation((ReplayModule)rootModule);
	}

	/**
	 * Convenient method to get a type safe replay simulation if applicable.
	 */
	protected ReplaySimulation getReplaySimulation() {
		return (ReplaySimulation)simulation;
	}

	/**
	 * Loads all animation primitives that begins before or at the current replay position.
	 */
	protected long loadAnimationPrimitivesForPosition() {
		return loadAnimationPrimitivesForPosition(eventNumber, simulationTime, animationNumber, animationTime);
	}

	/**
	 * Loads all animation primitives that begins before or at the given position.
	 */
	protected long loadAnimationPrimitivesForPosition(long minimumEventNumber, double minimumSimulationTime, long minimumAnimationNumber, double minimumAnimationTime) {
		try {
			long animationPrimitivesCount = animationPrimitives.size();
			int lineCount = 0;
			String line = null;

			if (logFileReader == null)
				logFileReader = new BufferedReader(new InputStreamReader(file.getContents()));
			
			while ((loadEventNumber <= minimumEventNumber ||
					loadSimulationTime <= minimumEventNumber ||
					loadAnimationNumber <= minimumAnimationNumber ||
					getAnimationTimeForSimulationTime(loadSimulationTime) <= minimumAnimationTime ||
					lineCount < 10) &&
				   (line = logFileReader.readLine()) != null)
			{
				lineCount++;
				String[] tokens = splitLine(line);
				
				if (tokens[0].equals("MC")) {
					ReplayModule module = new ReplayModule(
						simulation != null ? (ReplayModule)simulation.getModuleByPath(getToken(tokens, "p")) : null,
						getIntegerToken(tokens, "id"));
					module.setName(getToken(tokens, "n"));
				
					// FIXME: we show the first module for now, should we get it as parameter?
					if (simulation == null)
						initializeSimulation(module);

					getReplaySimulation().addModule(module);
					animationPrimitives.add(new CreateModuleAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, module));
				}
				else if (tokens[0].equals("MD")) {
					ReplayModule module = (ReplayModule)simulation.getModuleByID(getIntegerToken(tokens, "id"));
					animationPrimitives.add(new DeleteModuleAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, module));
				}
				else if (tokens[0].equals("CC")) {
					GateId sourceGateId = new GateId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					GateId targetGateId = new GateId(getIntegerToken(tokens, "dm"), getIntegerToken(tokens, "dg"));
					animationPrimitives.add(new CreateConnectionAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, sourceGateId, targetGateId));
				}
				else if (tokens[0].equals("E")) {
					loadEventNumber = getIntegerToken(tokens, "#");
					loadSimulationTime = getDoubleToken(tokens, "t");
					loadAnimationNumber++;
					HandleMessageAnimation handleMessageAnimation = new HandleMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, simulation.getModuleByID(getIntegerToken(tokens, "m")), null);
					animationPrimitives.add(handleMessageAnimation);
					handleMessageAnimationPrimitives.add(handleMessageAnimation);
				}
				else if (tokens[0].equals("BS")) {
				}
				else if (tokens[0].equals("SH")) {
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					// TODO: handle ts different then E's t
					// FIXME: animationPrimitives are sorted by eventNumber, beginSimulationTime and animationNumber
					// and the binary search relies upon this
					double propagationTime = getDoubleToken(tokens, "pd", 0);
					double transmissionTime = getDoubleToken(tokens, "td", 0);
					animationPrimitives.add(new SendMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, propagationTime, transmissionTime, connectionId));
				}
				else if (tokens[0].equals("SA")) {
					animationPrimitives.add(new ScheduleSelfMessageAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, getDoubleToken(tokens, "t")));
				}
				else if (tokens[0].equals("DS")) {
					ReplayModule module = (ReplayModule)simulation.getModuleByID(getIntegerToken(tokens, "id"));
					String displayString = getToken(tokens, "d");
					animationPrimitives.add(new SetModuleDisplayStringAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, module, displayString));
				}
				else if (tokens[0].equals("CS")) {
					String displayString = getToken(tokens, "d");
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					animationPrimitives.add(new SetConnectionDisplayStringAnimation(this, loadEventNumber, loadSimulationTime, loadAnimationNumber, connectionId, displayString));
				}
				else
					throw new RuntimeException("Unknown log entry");
			}
			
			if (line == null)
				endSimulationTime = loadSimulationTime;
			
			return animationPrimitives.size() - animationPrimitivesCount;
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
		return Integer.parseInt(getToken(tokens, key));
	}
	
	protected String getToken(String[] tokens, String key) {
		for (int i = 0; i < tokens.length; i++)
			if (tokens[i].equals(key))
				return tokens[i + 1];

		return null;
	}
}

package org.omnetpp.experimental.animation.replay;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.controller.IAnimationListener;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeModule;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.HandleMessageAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.ScheduleSelfMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class ReplayAnimationController {
	private final static double NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.01;
	private final static double FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE = 0.1;
	private final static double EXPRESS_REAL_TIME_TO_ANIMATION_TIME_SCALE = 100;
	
	/**
	 * A list of timers used during the animation. The queue contains the simulationTimer and
	 * animation primitives may add their own timers here. As real time goes by the timer queue
	 * will call its timers based on their settings, so that they can update their figures.
	 */
	protected TimerQueue timerQueue;
	
	/**
	 * The main timer that is responsible for updating the event number and simulation time during animation.
	 */
	protected SimulationTimer simulationTimer;
	
	/**
	 * The list of loaded animation primitives. This may contain more animation primitives than
	 * that is rendered to the canvas.
	 */
	protected ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
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
	 * The animation time when the animation was last started or continued.
	 */
	protected double lastStartAnimationTime;
	
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
	 * The simulation time when the animation will be next stopped or -1 if there's no such limit.
	 */
	protected double nextStopSimulationTime;
	
	/**
	 * The event number when the animation will be next stopped or -1 if there's no such limit.
	 */
	protected long nextStopEventNumber;
	
	/**
	 * This multiplier is applied to the default multiplier. It can be set from the GUI and starts from 1.
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
	 * The current animation mode.
	 */
	protected AnimationMode animationMode = AnimationMode.LINEAR;
	
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
		this.simulationTimer = new SimulationTimer();
		this.nextStopSimulationTime = -1;
		this.nextStopEventNumber = -1;
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
	 * Returns the simulation attached to this controller.
	 * If an underlying simulation kernel is running then it is a LiveSimulation.
	 * Otherwise it is a ReplaySimulation working on a log file.
	 */
	public IRuntimeSimulation getSimulation() {
		return simulation;
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
	 * Returns the timer queue that is used to schedule timer events during the animation.
	 */
	public TimerQueue getTimerQueue() {
		return timerQueue;
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
		loadAnimationPrimitivesForSimulationTime(simulationTime);
		simulationTimeChanged();
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
		loadAnimationPrimitivesForEventNumber(eventNumber);
		eventNumberChanged();
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
	}
	
	/**
	 * Returns the current real time.
	 */
	protected double getRealTime() {
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
		timerQueue.resetTimer(simulationTimer);
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
		animateStart(false, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, 0, 0);
	}

	/**
	 * Starts animation forward from the current simulation time with express speed.
	 * Asynchronous operation.
	 */
	public void animateExpress() {
		animateStart(true, EXPRESS_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1);
	}

	/**
	 * Starts animation forward from the current simulation time with fast speed.
	 * Asynchronous operation.
	 */
	public void animateFast() {
		animateStart(true, FAST_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1);
	}
	
	/**
	 * Starts animation forward from the current event number and stops at the next event number.
	 * Asynchronous operation.
	 */
	public void animateStep() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, eventNumber + 1, -1);
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Asynchronous operation.
	 */
	public void animatePlay() {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, -1);
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void animationToSimulationTime(double simulationTime) {
		animateStart(true, NORMAL_REAL_TIME_TO_ANIMATION_TIME_SCALE, -1, simulationTime);
	}

	/**
	 * Temporarily stops animation.
	 */
	public void animateStop() {
		timerQueue.removeTimer(simulationTimer);
	}

	/**
	 * Stops animation and sets the current simulation time to 0.
	 */
	public void gotoBegin() {
		forward = true;
		nextStopSimulationTime = -1;
		nextStopEventNumber = -1;
		animateStop();
		gotoSimulationTime(0);
	}

	/**
	 * Stops animation and sets the current simulation time to the end of the animation.
	 */
	public void gotoEnd() {
		forward = true;
		nextStopSimulationTime = -1;
		nextStopEventNumber = -1;
		animateStop();
		gotoSimulationTime(Double.MAX_VALUE);
	}
	
	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(double simulationTime) {
		setSimulationTime(simulationTime);
		setEventNumber(getEventNumberForSimulationTime(simulationTime));

		switch (animationMode) {
			case LINEAR:
				setAnimationTime(simulationTime);
				break;
			case EVENT:
				setAnimationTime(getEventNumber());
				break;
			case NON_LINEAR:
				// TODO:
				throw new RuntimeException();
		}

		timerQueue.resetTimer(simulationTimer);
		animateAtCurrentEventNumberAndSimulationTime();
	}

	/**
	 * Shows to the current event number and simulation time.
	 */
	public void animateAtCurrentEventNumberAndSimulationTime() {
		// stop at event number
		if (nextStopEventNumber != -1 && forward ? eventNumber >= nextStopEventNumber : eventNumber < nextStopEventNumber) {
			setEventNumber(nextStopEventNumber);
			setSimulationTime(getSimulationTimeForEventNumber(eventNumber));
			animateStop();
		}

		// stop at simulation time
		if (nextStopSimulationTime != -1 && forward ? simulationTime >= nextStopSimulationTime : simulationTime < nextStopSimulationTime) {
			setSimulationTime(nextStopSimulationTime);
			setEventNumber(getEventNumberForSimulationTime(simulationTime));
			animateStop();
		}
		
		for (IAnimationPrimitive animationPrimitive : animationPrimitives)
			animationPrimitive.animateAt(eventNumber, simulationTime);

		canvas.getRootFigure().getLayoutManager().layout(canvas.getRootFigure());
	}
	
	/**
	 * The simulation timer is responsible to change simulation time as real time elapses.
	 */
	public class SimulationTimer extends Timer {
		public SimulationTimer() {
			super(20, true, true);
		}

		public void run() {
			setAnimationTime(lastStartAnimationTime + (getRealTime() - lastStartRealTime) * realTimeToAnimationTimeScale * defaultRealTimeToAnimationTimeScale);

			switch (animationMode)
			{
				case LINEAR:
					setSimulationTime(animationTime);
					loadAnimationPrimitivesForSimulationTime(getSimulationTime());
					setEventNumber(getEventNumberForSimulationTime(simulationTime));
					break;
				case EVENT:
					long eventNumberDelta = (long)Math.floor(animationTime);
					setEventNumber(getEventNumber() + eventNumberDelta);
					loadAnimationPrimitivesForEventNumber(getEventNumber());
					double eventSimulationTime = getSimulationTimeForEventNumber(eventNumber);
					setSimulationTime(eventSimulationTime + (getSimulationTimeForEventNumber(eventNumber + 1) - eventSimulationTime) * (animationTime - eventNumberDelta));
					break;
				case NON_LINEAR:
					// TODO:
					throw new RuntimeException();
			}
			
			System.out.println("Event number: " + eventNumber + " Simulation Time: " + simulationTime);
			animateAtCurrentEventNumberAndSimulationTime();
		}
	};
	
	protected void ensureNextStopSimulationTime() {
		if (nextStopSimulationTime == -1 && forward && getNextAnimationPrimitive() == null)
			nextStopSimulationTime = animationPrimitives.size() != 0 ? getAnimationPrimitive(animationPrimitives.size() - 1).getEndSimulationTime() : 0;
	}
	
	/**
	 * Loads all animation primitives to be able to correctly display animation at the given event number.
	 */
	protected void loadAnimationPrimitivesForEventNumber(long eventNumber) {
		loadAnimationPrimitivesForPosition(eventNumber, 0);
		ensureNextStopSimulationTime();
	}

	/**
	 * Loads all animation primitives to be able to correctly display animation at the given simulation time.
	 */
	protected void loadAnimationPrimitivesForSimulationTime(double simulationTime) {
		loadAnimationPrimitivesForPosition(0, simulationTime);
		ensureNextStopSimulationTime();
	}

	/**
	 * Asynchronously starts animation in the given direction.
	 */
	protected void animateStart(boolean forward, double defaultRealTimeToAnimationTimeScale, long nextStopEventNumber, double stopSimulationTime) {
		this.forward = forward;
		this.defaultRealTimeToAnimationTimeScale = defaultRealTimeToAnimationTimeScale;
		this.nextStopEventNumber = nextStopEventNumber;
		this.nextStopSimulationTime = stopSimulationTime;
		
		timerQueue.resetTimer(simulationTimer);
		lastStartRealTime = getRealTime();
		lastStartAnimationTime = animationTime;
		timerQueue.addTimer(simulationTimer);
	}

	/**
	 * Notifies listeners about the new simulation time.
	 */
	protected void simulationTimeChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.replaySimulationTimeChanged(simulationTime);
	}

	/**
	 * Notifies listeners about the new event number.
	 */
	protected void eventNumberChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.replayEventNumberChanged(getEventNumber());
	}

	/**
	 * Returns the event number of the last HandleMessageAnimation that is before the given simulation time. 
	 */
	protected long getEventNumberForSimulationTime(double simulationTime) {
		HandleMessageAnimation lastHandleMessageAnimation = null;
		
		for (IAnimationPrimitive animationPrimitive : animationPrimitives) {
			if (animationPrimitive.getBeginSimulationTime() <= simulationTime && animationPrimitive instanceof HandleMessageAnimation)
				lastHandleMessageAnimation = (HandleMessageAnimation)animationPrimitive;
		}
		
		if (lastHandleMessageAnimation != null)
			return lastHandleMessageAnimation.getEventNumber();
		else
			return -1;
	}
	
	/**
	 * Returns the simulation time for the given event number.
	 */
	protected double getSimulationTimeForEventNumber(long eventNumber) {
		for (IAnimationPrimitive animationPrimitive : animationPrimitives) {
			if (animationPrimitive.getEventNumber() == eventNumber && animationPrimitive instanceof HandleMessageAnimation)
				return ((HandleMessageAnimation)animationPrimitive).getBeginSimulationTime();
		}
		
		return -1;
	}

	/**
	 * Returns the index of the last animation primitive that has already been reached by the simulation.
	 * TODO: this is inefficient, should use binary search, because animationPrimitives are in order of begin simulation time
	 */
	protected int getLastAnimationPrimitiveIndex() {
		// FIXME: this is inefficient
		for (int i = 0; i < animationPrimitives.size(); i++)
			if (animationPrimitives.get(i).getBeginSimulationTime() > simulationTime)
				return i - 1;
		
		return animationPrimitives.size();
	}
	
	/**
	 * Returns the last animation primitive which ends before the current simulation time.
	 */
	protected IAnimationPrimitive getPreviousAnimationPrimitive() {
		// FIXME: animation primitives are sorted based on their begin simulation times
		return getAnimationPrimitive(getLastAnimationPrimitiveIndex() - 1);
	}

	/**
	 * Returns the first animation primitive which begins after the current simulation time.
	 */
	protected IAnimationPrimitive getNextAnimationPrimitive() {
		return getAnimationPrimitive(getLastAnimationPrimitiveIndex() + 1);
	}
	
	/**
	 * Returns the nth animation primitive. Returns null if index falls outside of range.
	 */
	protected IAnimationPrimitive getAnimationPrimitive(int index) {
		return (0 <= index && index < animationPrimitives.size()) ? animationPrimitives.get(index) : null;
	}

	protected void initializeSimulation(IRuntimeModule rootModule) {
		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
		rootModuleFigure.setDisplayString(new DisplayString(null, null, "bgb=600,600;bgi=background/hungary,stretch"));
		setFigure(new GateId(rootModule.getId(), 0), new GateAnchor(rootModuleFigure, "in"));
		setFigure(new GateId(rootModule.getId(), 1), new GateAnchor(rootModuleFigure, "out"));
		canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
		canvas.getRootFigure().add(rootModuleFigure);
		
		simulation = createSimulation(rootModule);
	}
	
	protected IRuntimeSimulation createSimulation(IRuntimeModule rootModule) {
		return new ReplaySimulation((ReplayModule)rootModule);
	}
	
	protected ReplaySimulation getReplaySimulation() {
		return (ReplaySimulation)simulation;
	}

	/**
	 * Loads all animation primitives that begins before or at the given event number and simulation time.
	 */
	protected void loadAnimationPrimitivesForPosition(long minimumEventNumber, double minimumSimulationTime) {
		try {
			int lineCount = 0;
			String line = null;

			if (logFileReader == null)
				logFileReader = new BufferedReader(new InputStreamReader(file.getContents()));
			
			while ((loadEventNumber <= minimumEventNumber || loadSimulationTime <= minimumEventNumber || lineCount < 10) &&
				   (line = logFileReader.readLine()) != null)
			{
				lineCount++;
				String[] tokens = line.split(" ");
				
				if (tokens[0].equals("MC")) {
					ReplayModule module = new ReplayModule(
						simulation != null ? (ReplayModule)simulation.getModuleByPath(getToken(tokens, "p")) : null,
						getIntegerToken(tokens, "id"));
					module.setName(getToken(tokens, "n"));
				
					// FIXME: we show the first module for now, should get as parameter?
					if (simulation == null)
						initializeSimulation(module);

					getReplaySimulation().addModule(module);
					animationPrimitives.add(new CreateModuleAnimation(this, loadEventNumber, loadSimulationTime, module));
				}
				else if (tokens[0].equals("DS")) {
					ReplayModule module = (ReplayModule)simulation.getModuleByID(getIntegerToken(tokens, "id"));
					String displayString = getToken(tokens, "d");
					animationPrimitives.add(new SetDisplayStringAnimation(this, loadEventNumber, loadSimulationTime, module, displayString.substring(1, displayString.length() - 1)));
				}
				else if (tokens[0].equals("CC")) {
					GateId sourceGateId = new GateId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					GateId targetGateId = new GateId(getIntegerToken(tokens, "dm"), getIntegerToken(tokens, "dg"));
					animationPrimitives.add(new CreateConnectionAnimation(this, loadEventNumber, loadSimulationTime, sourceGateId, targetGateId));
				}
				else if (tokens[0].equals("E")) {
					loadEventNumber = getIntegerToken(tokens, "#");
					loadSimulationTime = getDoubleToken(tokens, "t");
					animationPrimitives.add(new HandleMessageAnimation(this, loadEventNumber, loadSimulationTime, simulation.getModuleByID(getIntegerToken(tokens, "m")), null));
				}
				else if (tokens[0].equals("BS")) {
				}
				else if (tokens[0].equals("SH")) {
					ConnectionId connectionId = new ConnectionId(getIntegerToken(tokens, "sm"), getIntegerToken(tokens, "sg"));
					double propagationTime = getDoubleToken(tokens, "d", 0);
					double transmissionTime = getDoubleToken(tokens, "td", 0);
					animationPrimitives.add(new SendMessageAnimation(this, loadEventNumber, loadSimulationTime, propagationTime, transmissionTime, connectionId));
				}
				else if (tokens[0].equals("SA")) {
					animationPrimitives.add(new ScheduleSelfMessageAnimation(this, loadEventNumber, loadSimulationTime, getDoubleToken(tokens, "t")));
				}
			}
		}
		catch (Throwable e) {
			e.printStackTrace();
			
			throw new RuntimeException(e);
		}
	}

	private double getDoubleToken(String[] tokens, String key) {
		return getDoubleToken(tokens, key, Double.NaN);
	}

	private double getDoubleToken(String[] tokens, String key, double defaultValue) {
		String value = getToken(tokens, key);

		return value != null ? Double.parseDouble(value) : defaultValue;
	}

	private int getIntegerToken(String[] tokens, String key) {
		return Integer.parseInt(getToken(tokens, key));
	}
	
	protected String getToken(String[] tokens, String key) {
		for (int i = 0; i < tokens.length; i++)
			if (tokens[i].equals(key))
				return tokens[i + 1];

		return null;
	}
}

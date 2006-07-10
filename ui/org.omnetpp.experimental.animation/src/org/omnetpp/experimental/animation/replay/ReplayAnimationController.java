package org.omnetpp.experimental.animation.replay;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.draw2d.geometry.Point;
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
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.primitives.SetDisplayStringAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class ReplayAnimationController {
	/**
	 * A list of timers used during the animation. The queue contains the simulationTimer and
	 * animation primitives may add their own timers here.
	 */
	protected TimerQueue timerQueue;
	
	/**
	 * The main timer that is responsible for updating the simulation time during animation.
	 */
	protected SimulationTimer simulationTimer;
	
	/**
	 * The list of loaded or created animation primitives.
	 */
	protected ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
	/**
	 * The current simulation time. It is updated periodically from a timer callback.
	 * This simulation time is where the animation is right now which may be different from the live simulation time.
	 */
	protected double simulationTime;
	
	/**
	 * The simulation time when the animation was last started or continued.
	 */
	protected double lastStartSimulationTime;
	
	/**
	 * The simulation time when the animation will be next stopped or -1 if there's no limit.
	 */
	protected double nextStopSimulationTime;
	
	/**
	 * The simulation time is increased in this amount of steps from the timer.
	 */
	protected double simulationTimeStep;

	/**
	 * The widget used to display the animation figures.
	 */
	protected AnimationCanvas canvas;

	/**
	 * A map for figures and animation related objects.
	 */
	protected Map<Object, Object> figureMap;
	
	/**
	 * The simulation is either a LiveSimulation or a ReplaySimulation.
	 */
	protected IRuntimeSimulation simulation;
	
	/**
	 * Direction of the simulation in time.
	 */
	protected boolean forward;
	
	protected IFile file;
	
	/**
	 * Animation listeners are notified for various events, such as changing the simulation time.
	 */
	protected ArrayList<IAnimationListener> animationListeners = new ArrayList<IAnimationListener>();

	public ReplayAnimationController(AnimationCanvas canvas, IFile file) {
		this.canvas = canvas;
		this.file = file;
		this.figureMap = new HashMap<Object, Object>(); 
		this.forward = true;
		this.timerQueue = new TimerQueue();
		this.simulationTimer = new SimulationTimer();
		this.nextStopSimulationTime = -1;

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
	public void setSimulationTime(double simulationTime) {
		this.simulationTime = simulationTime;
		simulationTimeChanged();		
	}
	
	public int getEventNumber() {
		// TODO:
		return -1;
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
		animateStart(false, 0.01, 0);
	}

	/**
	 * Starts animation forward from the current simulation time with express speed.
	 * Asynchronous operation.
	 */
	public void animateExpress() {
		animateStart(true, 1, -1);
	}

	/**
	 * Starts animation forward from the current simulation time with fast speed.
	 * Asynchronous operation.
	 */
	public void animateFast() {
		animateStart(true, 0.1, -1);
	}
	
	/**
	 * Starts animation forward from the current simulation time and stops at next animation primitive.
	 * Asynchronous operation.
	 */
	public void animateStep() {
		IAnimationPrimitive animationPrimitive = getNextAnimationPrimitive();
		
		if (animationPrimitive == null) {
			loadNextAnimationPrimitives();
			animationPrimitive = getNextAnimationPrimitive();
		}

		if (animationPrimitive != null)
			animateStart(true, 0.01, animationPrimitive.getBeginSimulationTime());
		else if (animationPrimitives.size() != 0)
			animateStart(true, 0.01, animationPrimitives.get(animationPrimitives.size() - 1).getEndSimulationTime());
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Asynchronous operation.
	 */
	public void animatePlay() {
		animateStart(true, 0.01, -1);
	}
	
	/**
	 * Starts animation forward from the current simulation time with normal speed.
	 * Animation stops when the given simulation time is reached.
	 * Asynchronous operation.
	 */
	public void animationToSimulationTime(double simulationTime) {
		animateStart(true, 0.01, simulationTime);
	}

	/**
	 * Temporarily stops animation.
	 */
	public void animateStop() {
		timerQueue.removeTimer(simulationTimer);
		lastStartSimulationTime = simulationTime;
	}

	/**
	 * Stops animation and sets the current simulation time to 0.
	 */
	public void gotoBegin() {
		forward = true;
		nextStopSimulationTime = -1;
		animateStop();
		gotoSimulationTime(0);
	}

	/**
	 * Stops animation and sets the current simulation time to the end of the animation.
	 */
	public void gotoEnd() {
		forward = true;
		nextStopSimulationTime = -1;
		animateStop();
		gotoSimulationTime(Double.MAX_VALUE);
	}
	
	/**
	 * Goes to the given simulation time without animating.
	 */
	public void gotoSimulationTime(double simulationTime) {
		setSimulationTime(simulationTime);
		lastStartSimulationTime = simulationTime;
		timerQueue.resetTimer(simulationTimer);
		gotoSimulationTime();
	}

	/**
	 * Goes to the current simulation time without animating.
	 */
	public void gotoSimulationTime() {
		IAnimationPrimitive nextAnimationPrimitive = getNextAnimationPrimitive();
		
		if (nextAnimationPrimitive == null || simulationTime > nextAnimationPrimitive.getBeginSimulationTime()) {
			while (true) {
				loadNextAnimationPrimitives();
				nextAnimationPrimitive = getNextAnimationPrimitive();
				
				if (nextAnimationPrimitive == null) {
					if (forward && nextStopSimulationTime == -1)
						nextStopSimulationTime = getAnimationPrimitive(animationPrimitives.size() - 1).getEndSimulationTime();
					break;
				}
				if (nextAnimationPrimitive.getBeginSimulationTime() > simulationTime)
					break;
			}
		}

		if (nextStopSimulationTime != -1 && forward ? simulationTime >= nextStopSimulationTime : simulationTime <= nextStopSimulationTime) {
			setSimulationTime(nextStopSimulationTime);
			animateStop();
		}

		for (IAnimationPrimitive animationPrimitive : animationPrimitives)
			animationPrimitive.gotoSimulationTime(simulationTime);

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
			setSimulationTime(lastStartSimulationTime + (forward ? 1 : -1) * simulationTimeStep * simulationTimer.getNumberOfExecutions());
			//System.out.println("Setting simulation time at: " + System.currentTimeMillis() + " to: " + getSimulationTime());
			gotoSimulationTime();
		}
	};

	public void setSpeed(int speed) {
		// TODO: this is wrong simulationTimeStep = (double)speed / 100.0;
	}

	/**
	 * Asynchronously starts animation in the given direction.
	 */
	protected void animateStart(boolean forward, double simulationTimeStep, double stopSimulationTime) {
		this.forward = forward;
		this.simulationTimeStep = simulationTimeStep;
		this.nextStopSimulationTime = stopSimulationTime;
		
		timerQueue.resetTimer(simulationTimer);

		if (timerQueue.hasTimer(simulationTimer))
			lastStartSimulationTime = simulationTime;
		else
			timerQueue.addTimer(simulationTimer);
	}

	protected void simulationTimeChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.replaySimulationTimeChanged(simulationTime);
	}

	protected void eventNumberChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.replayEventNumberChanged(getEventNumber());
	}

	/**
	 * Returns the index of the last animation primitive that has already been reached by the simulation.
	 * TODO: this is inefficient, should use binary search, because animationPrimitives are in order of begin simulation time
	 */
	protected int getLastAnimationPrimitiveIndex() {
		for (int i = 0; i < animationPrimitives.size(); i++)
			if (animationPrimitives.get(i).getBeginSimulationTime() > simulationTime)
				return i - 1;
		
		return animationPrimitives.size();
	}
	
	/**
	 * Returns the last animation primitive which ends before the current simulation time.
	 */
	protected IAnimationPrimitive getPreviousAnimationPrimitive() {
		// TODO: animation primitives are sorted based on their begin simulation times
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

	/**
	 * Loads at least one animation primitive that begins after the current simulation time.
	 */
	protected void loadNextAnimationPrimitives() {
		if (simulation == null) {
			BufferedReader reader;
			try {
				String line = null;
				double simulationTime = 0;
				reader = new BufferedReader(new InputStreamReader(file.getContents()));
				
				while ((line = reader.readLine()) != null) {
					String[] tokens = line.split(" ");
					
					if (tokens[0].equals("MC")) {
						String parentModuleName = getToken(tokens, "p");
						//if (!parentModuleName.equals("-"))
						//	parentModuleName = parentModuleName.substring(0, parentModuleName.lastIndexOf('.'));
						
						ReplayModule module = new ReplayModule(
							simulation != null ? (ReplayModule)simulation.getModuleByPath(parentModuleName) : null,
							Integer.parseInt(getToken(tokens, "id")));
						module.setName(getToken(tokens, "n"));
						((ReplaySimulation)simulation).addModule(module); //XXX simulation must be non-null here already
					
						// TODO: we show the first module for now
						if (simulation == null) //FIXME this should not happen -- simulation must be create earlier
							initializeSimulation(module);
						else
							animationPrimitives.add(new CreateModuleAnimation(this, simulationTime, module, new Point(0, 0)));
					}
					else if (tokens[0].equals("MS")) {
						ReplayModule module = (ReplayModule)simulation.getModuleByID(Integer.parseInt(getToken(tokens, "id")));
						animationPrimitives.add(new SetDisplayStringAnimation(this, simulationTime, module, getToken(tokens, "d")));
					}
					else if (tokens[0].equals("CC")) {
						GateId sourceGateId = new GateId(Integer.parseInt(getToken(tokens, "sm")), Integer.parseInt(getToken(tokens, "sg")));
						GateId targetGateId = new GateId(Integer.parseInt(getToken(tokens, "dm")), Integer.parseInt(getToken(tokens, "dg")));
						animationPrimitives.add(new CreateConnectionAnimation(this, simulationTime, sourceGateId, targetGateId));
					}
					else if (tokens[0].equals("E")) {
						simulationTime = Double.parseDouble(getToken(tokens, "T"));
						animationPrimitives.add(new HandleMessageAnimation(this, simulationTime, Integer.parseInt(getToken(tokens, "#"))));
					}
					else if (tokens[0].equals("ms")) {
						ConnectionId connectionId = new ConnectionId(Integer.parseInt(getToken(tokens, "sm")), Integer.parseInt(getToken(tokens, "sg")));
						// TODO: arraivalTime
						animationPrimitives.add(new SendMessageAnimation(this, simulationTime, simulationTime + 1, connectionId));
					}
				}
			}
			catch (Throwable e) {
				e.printStackTrace();
			}
		}
	}
	
	protected String getToken(String[] tokens, String key) {
		for (int i = 0; i < tokens.length; i++)
			if (tokens[i].equals(key))
				return tokens[i + 1];

		return null;
	}
}

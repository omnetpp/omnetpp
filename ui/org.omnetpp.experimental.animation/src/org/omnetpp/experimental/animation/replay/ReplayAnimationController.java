package org.omnetpp.experimental.animation.replay;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.controller.IAnimationListener;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.replay.model.ReplayModule;
import org.omnetpp.experimental.animation.replay.model.ReplaySimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class ReplayAnimationController {
	/**
	 * A list of timers used during animation. There is always at least one timer the simulationTimer.
	 * Also animation primitives may add there own timers here.
	 */
	protected TimerQueue timerQueue;
	
	/**
	 * The main timer that is responsible for updating the simulation time during animation.
	 */
	protected SimulationTimer simulationTimer;
	
	/**
	 * The list of known animation primitives.
	 */
	protected ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
	/**
	 * The current simulation time. It is updated periodically from a timer callback.
	 */
	protected double simulationTime;
	
	/**
	 * The simulation time when the animation was last started.
	 */
	protected double startSimulationTime;
	
	/**
	 * The simulation time when the animation should stop.
	 */
	protected double stopSimulationTime;
	
	/**
	 * The simulation time is increased in this amount of steps from the timer.
	 */
	protected double simulationTimeStep;

	/**
	 * The widget used to display the animation figures.
	 */
	protected AnimationCanvas canvas;

	protected Map<Object, Object> figureMap;
	
	protected IRuntimeSimulation simulation;
	
	/**
	 * Direction of the simulation in time.
	 */
	protected boolean forward;
	
	protected ArrayList<IAnimationListener> animationListeners = new ArrayList<IAnimationListener>();

	public ReplayAnimationController(AnimationCanvas canvas) {
		this.canvas = canvas;
		this.figureMap = new HashMap<Object, Object>(); 
		this.forward = true;
		this.timerQueue = new TimerQueue();
		this.simulationTimer = new SimulationTimer();
		this.stopSimulationTime = -1;
		this.simulation = createSimulation();

		timerQueue.start();
	}
	
	/**
	 * Returns the canvas on which the animation figures are drawn.
	 */
	public AnimationCanvas getCanvas() {
		return canvas;
	}
	
	/**
	 * Returns the simulation attached to this controller.
	 * It is currently either a LiveSimulation or a ReplaySimulation.
	 */
	public IRuntimeSimulation getSimulation() {
		return simulation;
	}

	/**
	 * Stores a figure or related object.
	 */
	public Object getFigure(Object key) {
		return figureMap.get(key);
	}

	/**
	 * Retrieves a figure or related object.
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
	 * Returns the current simulation time
	 */
	public double getSimulationTime() {
		return startSimulationTime + (forward ? 1 : -1) * simulationTimeStep * simulationTimer.getNumberOfExecutions();
	}
	
	public void setSimulationTime(double simulationTime) {
		this.simulationTime = simulationTime;
		simulationTimeChanged();		
	}
	
	public int getEventNumber() {
		// TODO:
		return -1;
	}
	
	public void addAnimationListener(IAnimationListener listener) {
		animationListeners.add(listener);
	}
	
	public void animateBack() {
		animateStart(false, 0.01, 0);
	}

	public void animateExpress() {
		animateStart(true, 1, -1);
	}

	public void animateFast() {
		animateStart(true, 0.1, -1);
	}
	
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
	
	public void animatePlay() {
		animateStart(true, 0.01, -1);
	}

	public void animateStop() {
		timerQueue.removeTimer(simulationTimer);
		startSimulationTime = simulationTime;
	}

	public void gotoBegin() {
		forward = true;
		stopSimulationTime = -1;
		animateStop();
		gotoSimulationTime(0);
	}

	public void gotoEnd() {
		forward = true;
		stopSimulationTime = -1;
		animateStop();
		gotoSimulationTime(Double.MAX_VALUE);
	}
	
	public void setSpeed(int speed) {
		// TODO: this is wrong simulationTimeStep = (double)speed / 100.0;
	}

	public void gotoSimulationTime(double simulationTime) {
		setSimulationTime(simulationTime);
		startSimulationTime = simulationTime;
		simulationTimer.reset();
		gotoSimulationTime();
	}

	public void gotoSimulationTime() {
		IAnimationPrimitive nextAnimationPrimitive = getNextAnimationPrimitive();
		
		if (nextAnimationPrimitive == null || simulationTime > nextAnimationPrimitive.getBeginSimulationTime()) {
			while (true) {
				loadNextAnimationPrimitives();
				nextAnimationPrimitive = getNextAnimationPrimitive();
				
				if (nextAnimationPrimitive == null) {
					if (forward && stopSimulationTime == -1)
						stopSimulationTime = getAnimationPrimitive(animationPrimitives.size() - 1).getEndSimulationTime();
					break;
				}
				if (nextAnimationPrimitive.getBeginSimulationTime() > simulationTime)
					break;
			}
		}

		if (stopSimulationTime != -1 && forward ? simulationTime >= stopSimulationTime : simulationTime <= stopSimulationTime) {
			setSimulationTime(stopSimulationTime);
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
			super(20, true, false);
		}

		public void run() {
			setSimulationTime(getSimulationTime());
			//System.out.println("Setting simulation time at: " + System.currentTimeMillis() + " to: " + simulationTime);
			gotoSimulationTime();
		}
	};

	protected void animateStart(boolean forward, double simulationTimeStep, double stopSimulationTime) {
		this.forward = forward;
		this.simulationTimeStep = simulationTimeStep;
		this.stopSimulationTime = stopSimulationTime;
		
		timerQueue.resetTimer(simulationTimer);

		if (timerQueue.hasTimer(simulationTimer))
			startSimulationTime = simulationTime;
		else
			timerQueue.addTimer(simulationTimer);
	}

	protected void simulationTimeChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.simulationTimeChanged(simulationTime);
	}

	protected void eventNumberChanged() {
		for (IAnimationListener listener : animationListeners)
			listener.eventNumberChanged(getEventNumber());
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
	
	protected IRuntimeSimulation createSimulation() {
		ReplayModule rootModule = new ReplayModule(null, 0);

		CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
		rootModuleFigure.setDisplayString(new DisplayString(null, null, "bgb=400,400;bgi=background/hungary,stretch"));
		setFigure(new GateId(rootModule.getId(), 0), new GateAnchor(rootModuleFigure, "in"));
		setFigure(new GateId(rootModule.getId(), 1), new GateAnchor(rootModuleFigure, "out"));
		canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
		canvas.getRootFigure().add(rootModuleFigure);
		
		return new ReplaySimulation(rootModule);
	}

	// TODO: this should read animation primitives from a file
	protected ArrayList<IAnimationPrimitive> animationPrimitivesToBeRead;
	
	/**
	 * Loads at least one animation primitive that begins after the current simulation time.
	 */
	protected void loadNextAnimationPrimitives() {
		if (animationPrimitivesToBeRead == null) {
			ReplayModule childModule1 = new ReplayModule((ReplayModule)simulation.getRootModule(), 1);
			ReplayModule childModule2 = new ReplayModule((ReplayModule)simulation.getRootModule(), 2);
			
			animationPrimitivesToBeRead = new ArrayList<IAnimationPrimitive>();
			animationPrimitivesToBeRead.add(new CreateModuleAnimation(this, 0, childModule1, new Point(200, 100)));
			animationPrimitivesToBeRead.add(new CreateModuleAnimation(this, 0, childModule2, new Point(200, 300)));
			animationPrimitivesToBeRead.add(new CreateConnectionAnimation(this, 0, new GateId(0, 0), new GateId(1, 0)));
			animationPrimitivesToBeRead.add(new CreateConnectionAnimation(this, 0, new GateId(1, 1), new GateId(2, 0)));
			animationPrimitivesToBeRead.add(new CreateConnectionAnimation(this, 0, new GateId(2, 1), new GateId(0, 1)));
			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 0, 3, new ConnectionId(0, 0)));
			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 3, 6, new ConnectionId(1, 1)));
			animationPrimitivesToBeRead.add(new SendMessageAnimation(this, 6, 9, new ConnectionId(2, 1)));
		}
		
		IAnimationPrimitive animationPrimitive = null;
		
		while (animationPrimitivesToBeRead.size() != 0) {
			animationPrimitive = animationPrimitivesToBeRead.remove(0);
			
			if (animationPrimitive != null)
				animationPrimitives.add(animationPrimitive);
			
			if (animationPrimitive.getBeginSimulationTime() > simulationTime)
				break;
		}
	}
}

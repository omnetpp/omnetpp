package org.omnetpp.experimental.animation.replay;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.draw2d.geometry.Rectangle;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.controller.Timer;
import org.omnetpp.experimental.animation.controller.TimerQueue;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.model.IRuntimeSimulation;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.replay.model.RuntimeModule;
import org.omnetpp.experimental.animation.replay.model.RuntimeSimulation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;
import org.omnetpp.figures.CompoundModuleFigure;
import org.omnetpp.figures.GateAnchor;

public class ReplayAnimationController implements IAnimationController {
	private AnimationCanvas canvas;
	
	private TimerQueue timerQueue;
	
	private SimulationTimer simulationTimer;
	
	private ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
	private Map<Object, Object> figureMap = new HashMap<Object, Object>();
	
	private double simulationTime;
	
	private double startSimulationTime;
	
	private double stopSimulationTime;
	
	private double simulationTimeStep;
	
	private boolean forward;
	
	private IRuntimeSimulation simulation;

	public ReplayAnimationController(AnimationCanvas canvas) {
		this.canvas = canvas;
		this.timerQueue = new TimerQueue();
		this.simulationTimer = new SimulationTimer();
		this.stopSimulationTime = -1;
		this.forward = true;
		timerQueue.addTimer(simulationTimer);
	}
	
	public AnimationCanvas getCanvas() {
		return canvas;
	}
	
	public TimerQueue getTimerQueue() {
		return timerQueue;
	}
	
	public IRuntimeSimulation getSimulation() {
		return simulation;
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
		IAnimationPrimitive animationPrimitive = forward ? getNextAnimationPrimitive() : getPreviousAnimationPrimitive();
		
		if (animationPrimitive == null && forward)
			animationPrimitive = loadNextAnimationPrimitive();

		if (animationPrimitive != null)
			animateStart(forward, 0.01, forward ? animationPrimitive.getBeginSimulationTime() : animationPrimitive.getEndSimulationTime());
		else if (animationPrimitives.size() != 0)
			if (forward)
				animateStart(forward, 0.01, animationPrimitives.get(animationPrimitives.size() - 1).getEndSimulationTime());
			else
				animateStart(forward, 0.01, animationPrimitives.get(0).getBeginSimulationTime());				
	}
	
	public void animatePlay() {
		animateStart(true, 0.01, -1);
	}

	private void animateStart(boolean forward, double simulationTimeStep, double stopSimulationTime) {
		this.forward = forward;
		this.simulationTimeStep = simulationTimeStep;
		this.stopSimulationTime = stopSimulationTime;
		
		if (timerQueue.isRunning())
			startSimulationTime = simulationTime;

		simulationTimer.reset();
		timerQueue.start();
	}

	public void animateStop() {
		timerQueue.stop();
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
		simulationTimeStep = (double)speed / 100.0;
	}

	public void gotoSimulationTime(double simulationTime) {
		this.simulationTime = startSimulationTime = simulationTime;
		simulationTimer.reset();
		gotoSimulationTime();
	}

	public void gotoSimulationTime() {
		IAnimationPrimitive nextAnimationPrimitive = getNextAnimationPrimitive();
		
		if (nextAnimationPrimitive == null || simulationTime > nextAnimationPrimitive.getBeginSimulationTime()) {
			while (true) {
				nextAnimationPrimitive = loadNextAnimationPrimitive();
				
				if (nextAnimationPrimitive == null) {
					if (forward && stopSimulationTime == -1)
						stopSimulationTime = getAnimationPrimitive(animationPrimitives.size() - 1).getEndSimulationTime();
					break;
				}
				if (nextAnimationPrimitive.getBeginSimulationTime() > simulationTime)
					break;
			}
		}

		if (stopSimulationTime != -1 && forward ? simulationTime > stopSimulationTime : simulationTime < stopSimulationTime) {
			simulationTime = stopSimulationTime;
			animateStop();
		}

		for (IAnimationPrimitive animationPrimitive : animationPrimitives)
			animationPrimitive.gotoSimulationTime(simulationTime);

		canvas.getRootFigure().getLayoutManager().layout(canvas.getRootFigure());
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
	 * The simulation timer is responsible to change simulation time as real time elapses.
	 */
	public class SimulationTimer extends Timer {
		public SimulationTimer() {
			super(10, true, false);
		}

		public void run() {
			simulationTime = startSimulationTime + (forward ? 1 : -1) * simulationTimeStep * getNumberOfExecutions();
			//System.out.println("Setting simulation time at: " + System.currentTimeMillis() + " to: " + simulationTime);
			gotoSimulationTime();
		}
	};

	/**
	 * Returns the index of the last animation primitive that has already been reached by the simulation.
	 * TODO: this is inefficient, should use binary search, because animationPrimitives are in order of begin simulation time
	 */
	private int getLastAnimationPrimitiveIndex() {
		for (int i = 0; i < animationPrimitives.size(); i++)
			if (animationPrimitives.get(i).getBeginSimulationTime() > simulationTime)
				return i - 1;
		
		return animationPrimitives.size();
	}
	
	/**
	 * Returns the last animation primitive which ends before the current simulation time.
	 */
	private IAnimationPrimitive getPreviousAnimationPrimitive() {
		// TODO: animation primitives are sorted based on their begin simulation times
		return getAnimationPrimitive(getLastAnimationPrimitiveIndex() - 1);
	}

	/**
	 * Returns the first animation primitive which begins after the current simulation time.
	 */
	private IAnimationPrimitive getNextAnimationPrimitive() {
		return getAnimationPrimitive(getLastAnimationPrimitiveIndex() + 1);
	}
	
	/**
	 * Returns the nth animation primitive. Returns null if index falls outside of range.
	 */
	private IAnimationPrimitive getAnimationPrimitive(int index) {
		return (0 <= index && index < animationPrimitives.size()) ? animationPrimitives.get(index) : null;
	}
	
	// TODO: this should read animation primitives from a file
	private ArrayList<IAnimationPrimitive> animationPrimitivesToBeRead;
	
	private IAnimationPrimitive loadNextAnimationPrimitive() {
		if (animationPrimitivesToBeRead == null) {
			RuntimeModule rootModule = new RuntimeModule(null, 0);
			RuntimeModule childModule1 = new RuntimeModule(rootModule, 1);
			RuntimeModule childModule2 = new RuntimeModule(rootModule, 2);
			
			simulation = new RuntimeSimulation(rootModule);
			
			CompoundModuleFigure rootModuleFigure = new CompoundModuleFigure();
			rootModuleFigure.setDisplayString(new DisplayString(null, null, "bgb=400,400;bgi=background/hungary,stretch"));
			setFigure(new GateId(rootModule.getId(), 0), new GateAnchor(rootModuleFigure, "in"));
			setFigure(new GateId(rootModule.getId(), 1), new GateAnchor(rootModuleFigure, "out"));
			canvas.getRootFigure().getLayoutManager().setConstraint(rootModuleFigure, new Rectangle(0, 0, -1, -1));
			canvas.getRootFigure().add(rootModuleFigure);
			
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
		
		if (animationPrimitivesToBeRead.size() != 0)
			animationPrimitive = animationPrimitivesToBeRead.remove(0);
		
		if (animationPrimitive != null)
			animationPrimitives.add(animationPrimitive);
		
		return animationPrimitive;
	}
}

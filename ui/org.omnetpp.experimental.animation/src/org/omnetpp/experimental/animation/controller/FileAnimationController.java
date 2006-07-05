package org.omnetpp.experimental.animation.controller;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.experimental.animation.editors.Timer;
import org.omnetpp.experimental.animation.editors.TimerQueue;
import org.omnetpp.experimental.animation.model.ConnectionId;
import org.omnetpp.experimental.animation.model.GateId;
import org.omnetpp.experimental.animation.primitives.CreateConnectionAnimation;
import org.omnetpp.experimental.animation.primitives.CreateModuleAnimation;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.SendMessageAnimation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class FileAnimationController implements IAnimationController {
	private AnimationCanvas canvas;
	
	private TimerQueue timerQueue;
	
	private SimulationTimer simulationTimer;
	
	private ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
	private Map<Object, Object> figureMap = new HashMap<Object, Object>();
	
	private double simulationTime;
	
	private double baseSimulationTime;
	
	private double simulationTimeStep;
	
	private boolean forward;

	public FileAnimationController(AnimationCanvas canvas) {
		this.canvas = canvas;
		this.timerQueue = new TimerQueue();
		this.simulationTimer = new SimulationTimer();
		timerQueue.addTimer(simulationTimer);
		
		// add some animation
		/*
		animationPrimitives.add(new LabelAnimation(this, "Hello", 0, 10, new Point(0, 0), new Point(100, 100)));

		Random r = new Random();
		for (int i = 0; i < 100; i++)
			animationPrimitives.add(new LabelAnimation(this, "Ciao", r.nextDouble() * 10, 10, new Point(i * 10, 0), new Point(0, i * 10)));
		*/
		animationPrimitives.add(new CreateModuleAnimation(this, 0, 0, new Point(0, 0)));
		animationPrimitives.add(new CreateModuleAnimation(this, 0, 1, new Point(400, 0)));
		animationPrimitives.add(new CreateModuleAnimation(this, 0, 2, new Point(0, 400)));
		animationPrimitives.add(new CreateConnectionAnimation(this, 0, new GateId(0, 0), new GateId(1, 0)));
		animationPrimitives.add(new CreateConnectionAnimation(this, 0, new GateId(1, 1), new GateId(2, 0)));
		animationPrimitives.add(new CreateConnectionAnimation(this, 0, new GateId(2, 1), new GateId(0, 1)));
		animationPrimitives.add(new SendMessageAnimation(this, 0, 3, new ConnectionId(0, 0)));
		animationPrimitives.add(new SendMessageAnimation(this, 3, 6, new ConnectionId(1, 1)));
		animationPrimitives.add(new SendMessageAnimation(this, 6, 9, new ConnectionId(2, 1)));
	}
	
	public AnimationCanvas getCanvas() {
		return canvas;
	}
	
	public TimerQueue getTimerQueue() {
		return timerQueue;
	}

	public void animateBack() {
		animateStart(false, 0.01);
	}

	public void animateExpress() {
		animateStart(true, 1);
	}

	public void animateFast() {
		animateStart(true, 0.1);
	}

	public void animateStep() {
	}
	
	public void animatePlay() {
		animateStart(true, 0.01);
	}

	private void animateStart(boolean forward, double simulationTimeStep) {
		this.forward = forward;
		this.simulationTimeStep = simulationTimeStep;
		
		if (timerQueue.isRunning())
			baseSimulationTime = simulationTime;

		simulationTimer.reset();
		timerQueue.start();
	}

	public void animateStop() {
		timerQueue.stop();
		baseSimulationTime = simulationTime;
	}

	public void gotoBegin() {
		baseSimulationTime = 0;
		simulationTimer.reset();
		gotoSimulationTime(0);
	}

	public void gotoEnd() {
	}
	
	public void gotoSimulationTime(double t) {
		for (IAnimationPrimitive primitive : animationPrimitives)
			primitive.gotoSimulationTime(t);

		canvas.getRootFigure().getLayoutManager().layout(canvas.getRootFigure());
	}
	
	public Object getFigure(Object key) {
		return figureMap.get(key);
	}
	
	public void setFigure(Object key, Object value) {
		figureMap.put(key, value);
	}
	
	public class SimulationTimer extends Timer {
		public SimulationTimer() {
			super(10, true, false);
		}

		public void run() {
			simulationTime = baseSimulationTime + (forward ? 1 : -1) * simulationTimeStep * getNumberOfExecutions();
			//System.out.println("Setting simulation time at: " + System.currentTimeMillis() + " to: " + simulationTime);
			gotoSimulationTime(simulationTime);
		}
	};
}

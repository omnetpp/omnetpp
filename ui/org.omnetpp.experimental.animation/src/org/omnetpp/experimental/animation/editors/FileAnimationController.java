package org.omnetpp.experimental.animation.editors;

import java.util.ArrayList;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import org.eclipse.draw2d.geometry.Point;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.experimental.animation.controller.IAnimationController;
import org.omnetpp.experimental.animation.primitives.IAnimationPrimitive;
import org.omnetpp.experimental.animation.primitives.LabelAnimation;
import org.omnetpp.experimental.animation.widgets.AnimationCanvas;

public class FileAnimationController implements IAnimationController {
	private boolean stopped;
	
	private AnimationCanvas canvas;
	
	private ArrayList<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>(); // holds all animation primitives since last key frame
	
	private double simulationTime;
	
	public FileAnimationController(AnimationCanvas canvas) {
		this.canvas = canvas;
		animationPrimitives.add(new LabelAnimation(this, "Hello", 0, 10, new Point(0, 0), new Point(100, 100)));

		Random r = new Random();
		for (int i = 0; i < 1000; i++)
			animationPrimitives.add(new LabelAnimation(this, "Ciao", r.nextDouble() * 10, 10, new Point(i * 10, 0), new Point(0, i * 10)));
	}
	
	public void animateBack() {
	}

	public void animateExpress() {
	}

	public void animateFast() {
	}

	public void animatePlay() {
		stopped = false;
		
		while (!stopped) {
			long begin = System.currentTimeMillis();

			while (System.currentTimeMillis() - begin < 10) {
				Display.getDefault().readAndDispatch();
			}
/*			
			Timer t = new Timer();
			t.schedule(new TimerTask() {
				@Override
				public void run() {
				}
				
			}, 10);
*/		
			simulationTime += 0.01;
			gotoSimulationTime(simulationTime);
//			System.out.println("Simulation Time: " + simulationTime);
			canvas.getRootFigure().getLayoutManager().layout(canvas.getRootFigure());
		}
	}

	public void animateStop() {
		stopped = true;
	}

	public void gotoBegin() {
		simulationTime = 0;
	}

	public void gotoEnd() {
	}
	
	public AnimationCanvas getCanvas() {
		return canvas;
	}

	public void gotoSimulationTime(double t) {
		for (IAnimationPrimitive primitive : animationPrimitives)
			primitive.gotoSimulationTime(t);
	}
}

package org.omnetpp.simulation.liveanimation;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.EventEntry;
import org.omnetpp.simulation.controller.LogBuffer;
import org.omnetpp.simulation.controller.SimulationController;

/**
 *
 * @author Andras
 */
public class LiveAnimationController {
    private static final int TICK_MILLIS = 10;

    private SimulationController simulationController;
    private AnimationDirector animationDirector;

    private List<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();

    private double animationSpeed = 1.0;
    private long animationStartTimeMillis;
    private long numUpdates; // since animationStartTimeMillis
    
    private Runnable invokeTick = new Runnable() {
        @Override
        public void run() {
            tick();
        }
    };

    public LiveAnimationController(SimulationController simulationController, AnimationDirector animationDirector) {
        this.simulationController = simulationController;
        this.animationDirector = animationDirector;
    }

    public void startAnimatingLastEvent() {
        // add animations for last event
        addAnimationsForLastEvent();

        // start animating
        animationStartTimeMillis = System.currentTimeMillis();
        numUpdates = 0;
        startTicking();
    }

    protected void addAnimationsForLastEvent() {
        LogBuffer logBuffer = simulationController.getLogBuffer();
        EventEntry event = logBuffer.getLastEventEntry();
        Assert.isNotNull(event);

        animationPrimitives.addAll(animationDirector.getAnimationsForLastEvent(event));
        
    }

    protected void startTicking() {
        tick(); // further ticks are scheduled from tick() itself
    }

    public void cancelAnimation() {
        Display.getCurrent().timerExec(-1, invokeTick);
        animationFinished();
    }

    protected void tick() {
        try {
            // update the animation
            double time = (System.currentTimeMillis() - animationStartTimeMillis) / 1000.0 * animationSpeed;
            boolean needMoreTicks = updateAnimationFor(time);
            if (needMoreTicks)
                Display.getCurrent().timerExec(TICK_MILLIS, invokeTick);
            else
                animationFinished();
        }
        catch (Exception e) {
            SimulationPlugin.logError("Error during animation", e);
            animationFinished();
        }
    }

    protected boolean updateAnimationFor(double time) {
        //System.out.println("updateAnimationFor(t=" + time + ")");
        numUpdates++;
        boolean needMoreTicks = false;
        for (IAnimationPrimitive primitive : animationPrimitives)
            if (primitive.updateFor(time))
                needMoreTicks = true;
        return needMoreTicks;
    }

    protected void animationFinished() {
        // purge obsolete animation primitives
        animationPrimitives.clear(); //XXX for now...

        // notify controller
        if (simulationController != null)
            simulationController.animationStopped();
        
        // statistics
        if (numUpdates > 5) {
            long millis = System.currentTimeMillis() - animationStartTimeMillis;
            long fps = numUpdates * 1000 / millis;
            System.out.println("Animation: " + numUpdates + " updates in " + millis + "ms, " + fps + "fps");
        }
    }
}


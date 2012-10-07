package org.omnetpp.simulation.liveanimation;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.EventEntry;

/**
 *
 * @author Andras
 */
//
//XXX Notes for smooth animations:
// Problem 1: System.currentTimeMillis() is inaccurate, granularity is ~10ms ==> use System.nanoTime() instead
// Problem 2: if drawing is not synchronized to vertical retrace (vsync), animations will be choppy! No current easy solution.
// Explanation of problem: http://www.java-gaming.org/topics/why-java-games-look-choppy-vertical-retrace/14696/view.html
// Feature request for WaitForVerticalBlank(): http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=6378181
// More overview: http://today.java.net/pub/a/today/2006/02/23/smooth-moves-solutions.html#vertical-retrace
// Native lib for WaitForVerticalBlank(): http://www.newdawnsoftware.com/resources/pxsync/
//
public class LiveAnimationController {
    private static final int TICK_MILLIS = 10;

    private AnimationDirector animationDirector;

    private List<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();

    // configuration
    private double animationSpeed = 1.0;

    // state 
    private Runnable todoWhenDone;
    private boolean isAnimating = false;
    private boolean isStopping = false; // needed if we call Display.readAndDispatch() from cancelAnimation()
    private double animationTime;
    private long animationStartTimeNanos;
    private long lastTickTimeNanos;
    private long numUpdates; // since animationStartTimeNanos

    private Runnable invokeTick = new Runnable() {
        @Override
        public void run() {
            tick();
        }
    };

    public LiveAnimationController(AnimationDirector animationDirector) {
        this.animationDirector = animationDirector;
    }

    public double getAnimationSpeed() {
        return animationSpeed;
    }

    public void setAnimationSpeed(double animationSpeed) {
        Assert.isTrue(animationSpeed > 0);
        this.animationSpeed = animationSpeed;
    }

    public boolean isAnimating() {
        return isAnimating;
    }

    public boolean isStopping() {
        return isStopping;
    }

    public void startAnimatingLastEvent(EventEntry event, Runnable todoWhenDone) {
        // add animations for last event
        addAnimationsForLastEvent(event);

        // start animating
        this.todoWhenDone = todoWhenDone;
        isAnimating = true;
        isStopping = false;
        animationStartTimeNanos = System.nanoTime();
        numUpdates = 0;
        startTicking();
    }

    protected void addAnimationsForLastEvent(EventEntry event) {
        animationPrimitives.addAll(animationDirector.getAnimationsForLastEvent(event));
    }

    protected void startTicking() {
        animationTime = 0;
        lastTickTimeNanos = System.nanoTime();
        tick(); // further ticks are scheduled from tick() itself
    }

    public void cancelAnimation() {
        if (!isAnimating)
            throw new IllegalStateException("animation not running");

        if (isStopping) 
            return;
        isStopping = true;

        // cancel next tick
        Display.getCurrent().timerExec(-1, invokeTick);

        // quickly run through the animation till the end (needed so that animation primitives
        // can remove the figures they added, e.g. message discs)
        try {
            while (true) {
                boolean needMoreTicks = updateAnimationFor(animationTime);
                if (!needMoreTicks)
                    break;
                animationTime += TICK_MILLIS/1000.0;

                //Display.getCurrent().update(); // would we exactly what we need, but doesn't appear to work on Windows 7
                Display.getCurrent().readAndDispatch(); //FIXME works but a little dangerous (user might invoke actions, even close the editor etc!)
            }
        }
        catch (Exception e) {
            SimulationPlugin.logError("Error during animation", e);
        }

        animationFinished();
    }

    protected void tick() {
        try {
            // update the animation
            long currentTimeNanos = System.nanoTime();
            animationTime += (currentTimeNanos - lastTickTimeNanos) / 1000000000.0 * animationSpeed;
            lastTickTimeNanos = currentTimeNanos;
            boolean needMoreTicks = updateAnimationFor(animationTime);
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
        // statistics
        if (numUpdates > 5) {
            long nanos = System.nanoTime() - animationStartTimeNanos;
            long fps = numUpdates * 1000000000 / nanos;
            System.out.println("Animation: " + numUpdates + " updates in " + nanos + "ms, " + fps + "fps");
        }

        // update state
        isAnimating = false;
        isStopping = false;

        // purge obsolete animation primitives
        animationPrimitives.clear();

        // notify controller
        if (todoWhenDone != null)
            todoWhenDone.run();
    }
}


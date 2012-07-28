package org.omnetpp.simulation.liveanimation;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Layer;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.controller.Anim;
import org.omnetpp.simulation.controller.EventEntry;
import org.omnetpp.simulation.controller.LogBuffer;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.inspectors.SimulationCanvas;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cModule;

/**
 *
 * @author Andras
 */
public class LiveAnimationController {
    private static final int TICK_MILLIS = 20;

    private SimulationCanvas simulationCanvas;
    private SimulationController simulationController;

    private List<AnimationPrimitive> animationPrimitives = new ArrayList<AnimationPrimitive>();

    private Runnable invokeTick = new Runnable() {
        @Override
        public void run() {
            tick();
        }
    };

    public LiveAnimationController(SimulationCanvas simulationCanvas, SimulationController simulationController) {
        this.simulationCanvas = simulationCanvas;
        this.simulationController = simulationController;
    }

    public void startAnimatingLastEvent() {
        // add animations for last event
        addAnimationsForLastEvent();

        // start animating
        startTicking();
    }

    protected void addAnimationsForLastEvent() {
        //animationPrimitives.add(new DummyAnimationPrimitive(50));  //TODO
        //animationPrimitives.add(new DummyMoveMessageAnimation(simulationCanvas.getContents()));  //TODO

        LogBuffer logBuffer = simulationController.getLogBuffer();
        EventEntry event = logBuffer.getLastEventEntry();
        Assert.isNotNull(event);

        for (Object o : event.logItems) {
            if (o instanceof Anim.MessageSendHopEntry) {
                Anim.MessageSendHopEntry e = (Anim.MessageSendHopEntry)o;
                cGate srcGate = e.srcGate;
                loadContextualObjects(srcGate);
                cGate destGate = srcGate.getNextGate();
                loadContextualObjects(destGate);
                if (srcGate.getOwnerModule().getParentModule() == destGate.getOwnerModule().getParentModule()) { //TODO we only deal with this case for now
                    System.out.println("sendHop: " + srcGate.getFullPath() + " --> " + destGate.getFullPath());
                    cModule containingModule = srcGate.getOwnerModule().getParentModule();
                    IInspectorPart inspector = simulationCanvas.findInspectorFor(containingModule);
                    if (inspector instanceof GraphicalModuleInspectorPart) {
                        System.out.println("Animating on inspector " + inspector.getObject().getFullPath());
                        GraphicalModuleInspectorPart moduleInspector = (GraphicalModuleInspectorPart)inspector;
                        ConnectionFigure connectionFigure = moduleInspector.getConnectionFigure(srcGate);
                        if (connectionFigure != null) {
                            Layer messageFigureParent = moduleInspector.getFigure().getInternalModuleFigure().getForegroundDecorationLayer();
                            animationPrimitives.add(new DummyMoveMessageAnimation(messageFigureParent, connectionFigure.getStart(), connectionFigure.getEnd()));
                        }
                    }
                }
            }
        }
    }

    protected void loadContextualObjects(cGate gate) {
        if (!gate.isFilledIn())
            gate.safeLoad();
        cModule module = gate.getOwnerModule();
        if (!module.isFilledIn())
            module.safeLoad();
        cModule parentModule = module.getParentModule();
        if (!parentModule.isFilledIn())
            parentModule.safeLoad();
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
            // update stuff
            boolean needMoreTicks = updateAnimations();
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

    protected boolean updateAnimations() {
        boolean needMoreTicks = false;
        for (AnimationPrimitive ap : animationPrimitives)
            if (ap.update())
                needMoreTicks = true;
        return needMoreTicks;
    }

    protected void animationFinished() {
        // purge obsolete animation primitives
        animationPrimitives.clear(); //XXX for now...

        // notify controller
        if (simulationController != null)
            simulationController.animationStopped();
    }
}


package org.omnetpp.simulation.liveanimation;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;

import org.eclipse.core.runtime.Assert;
import org.eclipse.draw2d.Layer;
import org.eclipse.draw2d.geometry.Point;
import org.omnetpp.common.Debug;
import org.omnetpp.figures.ConnectionFigure;
import org.omnetpp.simulation.canvas.SimulationCanvas;
import org.omnetpp.simulation.controller.Anim.BeginSendEntry;
import org.omnetpp.simulation.controller.Anim.ComponentMethodBeginEntry;
import org.omnetpp.simulation.controller.Anim.ComponentMethodEndEntry;
import org.omnetpp.simulation.controller.Anim.EndSendEntry;
import org.omnetpp.simulation.controller.Anim.MessageSendDirectEntry;
import org.omnetpp.simulation.controller.Anim.MessageSendHopEntry;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.EventEntry;
import org.omnetpp.simulation.controller.Simulation;
import org.omnetpp.simulation.inspectors.GraphicalModuleInspectorPart;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.model.cGate;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cSimulation;


/**
 * Director of the animated movie: translates log entries into a set of animation primitives.
 * Animations appear on the simulation canvas.
 *
 * @author Andras
 */
public class AnimationDirector {
    public static boolean debug = true;
    private SimulationCanvas simulationCanvas;
    private Simulation simulation;

    private class AnimStep {
        cModule src, dest;
        AnimStep(cModule src, cModule dest) {this.src = src; this.dest = dest;}
        public String toString() { return src + "-->" + dest; }
    };

    /**
     * An ad-hoc collection of variables needed while we are generating the animation primitives
     */
    protected class State {
        double time = 0;  // current animation time
        cMessage messageBeingSent = null;  // set by BeginSend, cleared by EndSend
        Stack<List<MethodCallAnimation>> methodCallStack = new Stack<List<MethodCallAnimation>>(); // active method calls (every call may consist of several arrows)
        cSimulation csimulation = (cSimulation) simulation.getRootObject(Simulation.ROOTOBJ_SIMULATION);
    };

    public AnimationDirector(SimulationCanvas simulationCanvas, Simulation simulation) {
        this.simulationCanvas = simulationCanvas;
        this.simulation= simulation;
    }

    public List<IAnimationPrimitive> getAnimationsForLastEvent(EventEntry event) {
        try {
            Assert.isNotNull(event);
            State state = new State();
            List<IAnimationPrimitive> animationPrimitives = new ArrayList<IAnimationPrimitive>();

            // Act One: animate current event (zero time duration)
            for (Object o : event.logItems) {
                if (o instanceof BeginSendEntry)
                    doBeginSendEntry((BeginSendEntry)o, animationPrimitives, state);
                else if (o instanceof EndSendEntry)
                    doEndSendEntry((EndSendEntry)o, animationPrimitives, state);
                else if (o instanceof MessageSendHopEntry)
                    doSendHopEntry((MessageSendHopEntry)o, animationPrimitives, state);
                else if (o instanceof MessageSendDirectEntry)
                    doSendDirectEntry((MessageSendDirectEntry)o, animationPrimitives, state);
                else if (o instanceof ComponentMethodBeginEntry)
                    doMethodBeginEntry((ComponentMethodBeginEntry)o, animationPrimitives, state);
                else if (o instanceof ComponentMethodEndEntry)
                    doMethodEndEntry((ComponentMethodEndEntry)o, animationPrimitives, state);
                else
                    /*skip*/;
            }
            Assert.isTrue(state.messageBeingSent == null, "missing EndSend");
            Assert.isTrue(state.methodCallStack.isEmpty(), "unterminated ComponentMethodBegin");

            // Act Two: animate simulation time duration up to the next event: move messages that are underway, etc.
            state.csimulation.getMessageQueue();
            //TODO

            return animationPrimitives;
        }
        catch (CommunicationException e) {
            return new ArrayList<IAnimationPrimitive>(); // forget animation if we cannot even talk to the simulation process!
        }
    }

    protected void doBeginSendEntry(BeginSendEntry e, List<IAnimationPrimitive> animationPrimitives, State state) {
        Assert.isTrue(state.messageBeingSent == null, "nesting BeginSend..EndSend blocks is not allowed");
        state.messageBeingSent = e.msg;
    }

    protected void doEndSendEntry(EndSendEntry e, List<IAnimationPrimitive> animationPrimitives, State state) {
        Assert.isTrue(state.messageBeingSent != null, "stray EndSend entry");
        state.messageBeingSent = null;
    }

    protected void doSendHopEntry(MessageSendHopEntry e, List<IAnimationPrimitive> animationPrimitives, State state) throws CommunicationException {
        Assert.isTrue(state.messageBeingSent != null, "missing BeginSend");
        cGate srcGate = e.srcGate;
        loadContextualObjects(srcGate);
        cGate destGate = srcGate.getNextGate();
        loadContextualObjects(destGate);

        cModule srcModule = srcGate.getOwnerModule();
        cModule containingModule = srcGate.getType()==cGate.Type.INPUT ? srcModule : srcModule.getParentModule();
        if (debug) Debug.println("sendHop: " + srcGate.getFullPath() + " --> " + destGate.getFullPath() + " within " + containingModule.getFullPath());

        IInspectorPart inspector = simulationCanvas.findInspectorFor(containingModule);
        if (inspector instanceof GraphicalModuleInspectorPart) {
            if (debug) Debug.println("Animating on inspector " + inspector.getObject().getFullPath());
            GraphicalModuleInspectorPart moduleInspector = (GraphicalModuleInspectorPart)inspector;
            ConnectionFigure connectionFigure = moduleInspector.getConnectionFigure(srcGate);
            if (connectionFigure != null) {
                Layer layer = moduleInspector.getCompoundModuleFigure().getMessageLayer();
                double duration = 1.0; // seconds; XXX for now
                String label = makeMessageLabel(state.messageBeingSent);
                animationPrimitives.add(new MessageMoveAnimation(layer, label, connectionFigure.getStart(), connectionFigure.getEnd(), state.time, state.time+duration));
                state.time += duration; // this serializes the animations
            }
        }
    }

    protected void doSendDirectEntry(MessageSendDirectEntry e, List<IAnimationPrimitive> animationPrimitives, State state) throws CommunicationException {
        Assert.isTrue(state.messageBeingSent != null, "missing BeginSend");
        cModule srcModule = e.srcModule;
        loadContextualObjects(srcModule);
        cGate destGate = e.destGate;
        loadContextualObjects(destGate);
        cModule destModule = destGate.getOwnerModule();

        // Note: srcModule and destModule may be at completely unrelated places in the module hierarchy,
        // so we need to animate from srcModule up to their common ancestor compound module,
        // then between two submodules inside the common ancestor, then down to destModule

        // first, assemble the path
        List<AnimStep> path = findSendDirectPath(srcModule, destModule);

        // then add the animations
        for (AnimStep step : path) {
            cModule containingModule = step.src != null ? step.src.getParentModule() : step.dest.getParentModule();
            IInspectorPart inspector = simulationCanvas.findInspectorFor(containingModule);
            if (inspector instanceof GraphicalModuleInspectorPart) {
                if (debug) Debug.println("Animating on inspector " + inspector.getObject().getFullPath());
                GraphicalModuleInspectorPart moduleInspector = (GraphicalModuleInspectorPart)inspector;
                Layer layer = moduleInspector.getCompoundModuleFigure().getMessageLayer();
                double duration = 1.0; // seconds; XXX for now
                Point startPos = step.src==null ? new Point(0,0) : moduleInspector.getSubmoduleFigure(step.src).getCenterPosition();
                Point endPos = step.dest==null ? new Point(0,0) : moduleInspector.getSubmoduleFigure(step.dest).getCenterPosition();
                String label = makeMessageLabel(state.messageBeingSent);
                animationPrimitives.add(new MessageMoveAnimation(layer, label, startPos, endPos, state.time, state.time+duration));
                state.time += duration; // this serializes the animations
            }
        }
    }

    protected void doMethodBeginEntry(ComponentMethodBeginEntry e, List<IAnimationPrimitive> animationPrimitives, State state) throws CommunicationException {
        cModule srcModule = state.csimulation.getModuleById(e.srcModuleId);
        cModule destModule = state.csimulation.getModuleById(e.destModuleId);

        // src and/or dest module may have been deleted later during the same event; if so, skip the animation
        if (srcModule != null && destModule != null) {

            // Note: srcModule and destModule may be at completely unrelated places in the module hierarchy,
            // so we need to animate from srcModule up to their common ancestor compound module,
            // then between two submodules inside the common ancestor, then down to destModule

            // first, assemble the path
            List<AnimStep> path = findSendDirectPath(srcModule, destModule);

            // then add the animations
            List<MethodCallAnimation> methodCallAnimationList = new ArrayList<MethodCallAnimation>();  // one call may consist of a sequence of arrows
            for (AnimStep step : path) {
                cModule containingModule = step.src != null ? step.src.getParentModule() : step.dest.getParentModule();
                IInspectorPart inspector = simulationCanvas.findInspectorFor(containingModule);
                if (inspector instanceof GraphicalModuleInspectorPart) {
                    if (debug) Debug.println("Animating on inspector " + inspector.getObject().getFullPath());
                    GraphicalModuleInspectorPart moduleInspector = (GraphicalModuleInspectorPart)inspector;
                    Layer layer = moduleInspector.getCompoundModuleFigure().getMessageLayer();
                    Point startPos = step.src==null ? new Point(0,0) : moduleInspector.getSubmoduleFigure(step.src).getCenterPosition();
                    Point endPos = step.dest==null ? new Point(0,0) : moduleInspector.getSubmoduleFigure(step.dest).getCenterPosition();
                    MethodCallAnimation methodCallAnimation = new MethodCallAnimation(layer, e.txt, startPos, endPos, state.time);
                    methodCallAnimationList.add(methodCallAnimation);
                    animationPrimitives.add(methodCallAnimation);
                    state.time += 0.5;
                }
            }

            state.methodCallStack.push(methodCallAnimationList); // note: even if it's null, so we can find the matching MethodEndEntry
        }
    }

    protected void doMethodEndEntry(ComponentMethodEndEntry e, List<IAnimationPrimitive> animationPrimitives, State state) {
        Assert.isTrue(!state.methodCallStack.isEmpty(), "unmatched ComponentMethodEnd");

        // remove all arrows that this call consists of
        List<MethodCallAnimation> methodCallAnimationList = state.methodCallStack.pop();
        if (methodCallAnimationList != null) { // i.e. it was animated
            state.time += 0.5;
            for (MethodCallAnimation a : methodCallAnimationList)
                a.setRemovalTime(state.time);
        }
    }

    protected String makeMessageLabel(cMessage message) throws CommunicationException {
        message.loadIfUnfilled();
        return "(" + message.getClassName() + ")" + message.getFullName();
    }

    protected List<AnimStep> findSendDirectPath(cModule srcModule, cModule destModule) throws CommunicationException {
        List<AnimStep> path = new ArrayList<AnimStep>();
        destModule.loadIfUnfilled();
        if (destModule.isAncestorModuleOf(srcModule)) {
            for (cModule m = srcModule; m != destModule; m = m.getParentModule())
                path.add(new AnimStep(m, null));
        }
        else if (srcModule.isAncestorModuleOf(destModule)) {
            for (cModule m = destModule; m != srcModule; m = m.getParentModule())
                path.add(0, new AnimStep(null, m));  // add at 0, for reverse order
        }
        else {
            // there'll be a "horizontal" sending between two submodules within the common ancestor compound module
            cModule commonAncestor = srcModule.getCommonAncestorModule(destModule);
            cModule upSubm = srcModule;
            for (upSubm = srcModule; upSubm != commonAncestor && upSubm.getParentModule() != commonAncestor; upSubm = upSubm.getParentModule())
                path.add(new AnimStep(upSubm, null));
            int pos = path.size();
            cModule downSubm;
            for (downSubm = destModule; downSubm != commonAncestor && downSubm.getParentModule() != commonAncestor; downSubm = downSubm.getParentModule())
                path.add(pos, new AnimStep(null, downSubm)); // add at pos, for reverse order
            path.add(pos, new AnimStep(upSubm, downSubm));
        }
        return path;
    }

    protected void loadContextualObjects(cGate gate) throws CommunicationException {
        gate.loadIfUnfilled();
        cModule module = gate.getOwnerModule();
        module.loadIfUnfilled();
        cModule parentModule = module.getParentModule();
        parentModule.loadIfUnfilled();
    }

    protected void loadContextualObjects(cModule module) throws CommunicationException {
        module.loadIfUnfilled();
        cModule parentModule = module.getParentModule();
        parentModule.loadIfUnfilled();
    }

}


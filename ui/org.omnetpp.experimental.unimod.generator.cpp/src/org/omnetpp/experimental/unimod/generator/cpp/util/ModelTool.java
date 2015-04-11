/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.experimental.unimod.generator.cpp.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.apache.commons.collections.functors.AndPredicate;
import org.apache.commons.collections.functors.UniquePredicate;

import antlr.RecognitionException;

import com.evelopers.unimod.compilation.StateMachineCompiler;
import com.evelopers.unimod.core.stateworks.Action;
import com.evelopers.unimod.core.stateworks.ClassElement;
import com.evelopers.unimod.core.stateworks.ControlledObjectHandler;
import com.evelopers.unimod.core.stateworks.Event;
import com.evelopers.unimod.core.stateworks.Guard;
import com.evelopers.unimod.core.stateworks.Model;
import com.evelopers.unimod.core.stateworks.StateMachine;

public class ModelTool extends BaseTool {
    private Model model;
    private String fileName;
    private String className;
    private String baseMachineClassName;
    private Map smTools = new HashMap();
    private Collection stateMachines;
    private Collection coHandlers;
    private Map instanceNames = new HashMap();
    private Map paramNames = new HashMap();
    private Map actions = new HashMap();
    private Map eventsConsts = new HashMap();
    private Collection events;

    public ModelTool(Model model) {
        this.model = model;
    }

    public Model getModel() {
        return model;
    }

    public String getClassName() {
        if (className == null) {
            className = getClassName("", model.getName(), "_Engine");
        }
        return className;
    }

    public String getFileName() {
        if (fileName == null) {
            fileName = getClassName("", model.getName(), "_Engine");
        }
        return fileName;
    }

    public String getBaseMachineClassName() {
        if (baseMachineClassName == null) {
            baseMachineClassName = getClassName("", model.getName(), "BaseMachine");
        }
        return baseMachineClassName;
    }

    public StateMachineTool getSMTool(StateMachine stateMachine) {
        if (! smTools.containsKey(stateMachine)) {
            StateMachineTool tool = new StateMachineTool(stateMachine);
            smTools.put(stateMachine, tool);
        }
        return (StateMachineTool) smTools.get(stateMachine);
    }

    public Collection getCOHandlers() {
        if (coHandlers == null) {
            List controlledObjectHandlers = new ArrayList(model.getControlledObjectHandlers());
            Collections.sort(controlledObjectHandlers, new ClassElementComparator());
            coHandlers = controlledObjectHandlers;
        }
        return coHandlers;
    }

    public Collection getStateMachines() {
        if (stateMachines == null) {
            List stateMachines = new ArrayList(model.getStateMachines());
            Collections.sort(stateMachines, new ClassElementComparator());
            this.stateMachines = stateMachines;
        }
        return stateMachines;
    }

    public String getParamName(ClassElement classElement) {
        if (! paramNames.containsKey(classElement)) {
            paramNames.put(classElement, createIdentifier(
                    classElement.getName().toLowerCase().startsWith("a") ?
                    "an" : "a", classElement.getName(), "", true, false));
        }
        return (String) paramNames.get(classElement);
    }

    public String getInstanceName(ClassElement classElement) {
        if (! instanceNames.containsKey(classElement)) {
            instanceNames.put(classElement, createIdentifier("i", classElement.getName(), "", true, false));
        }
        return (String) instanceNames.get(classElement);
    }

    public String getCOHeaderFileName(ControlledObjectHandler coHandler) {
        String name = coHandler.getImplName();
        if (name.charAt(0) == 'C' || name.charAt(0) == 'T') {
            name = name.substring(1);
        }
        return name + ".h";
    }

    public String getAction(Action action) {
        if (! actions.containsKey(action)) {
            StringBuffer invocation = new StringBuffer();
            invocation.append(
                    createIdentifier("i",
                            StateMachineCompiler.getObjectName(action.getIdentifier()),
                            "", true, false));
            invocation.append("->");
            if (action.getObject() instanceof StateMachine) {
                invocation.append("HandleL(");
                invocation.append(getClassName());
                invocation.append("::EEvent::");
                Event event = null;
                for (Iterator i = getEvents().iterator(); i.hasNext();) {
                    Event e = (Event) i.next();
                    if (e.getName().equals(action.getActionName())) {
                        event = e;
                        break;
                    }
                }
                invocation.append(event != null ? getEventConst(event) : "ANY_OTHER");
                invocation.append(", aContext)");
            } else {
                invocation.append(createIdentifier("", action.getActionName(), "L", true, false));
                invocation.append("(aContext)");
            }
            actions.put(action, invocation.toString());
        }
        return (String) actions.get(action);
    }

    public Collection getEvents() {
        if (events == null) {
            Collection modelTransitions = new LinkedList();
            for (Iterator i = model.getStateMachines().iterator(); i.hasNext();) {
                StateMachine stateMachine = (StateMachine) i.next();
                modelTransitions.addAll(stateMachine.getAllTransition());
            }
            events = CollectionsTool.filter(
                    modelTransitions,
                    CollectionsTool.transitionToEventTransformer(),
                    AndPredicate.getInstance(
                            CollectionsTool.realEventPredicate(),
                            UniquePredicate.getInstance()),
                    CollectionsTool.eventComparator());
        }
        return events;
    }

    public String getEventConst(Event event) {
        if (! eventsConsts.containsKey(event)) {
            eventsConsts.put(event, createIdentifier("", event.getName(), "", true, true));
        }
        return (String) eventsConsts.get(event);
    }

    public String getGuard(Guard guard) throws RecognitionException {
        return CppFormulaGenerator.generate(guard.getAST());
    }
}

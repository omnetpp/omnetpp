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
import java.util.List;
import java.util.Map;

import com.evelopers.unimod.core.stateworks.Association;
import com.evelopers.unimod.core.stateworks.ControlledObjectHandler;
import com.evelopers.unimod.core.stateworks.State;
import com.evelopers.unimod.core.stateworks.StateMachine;

public class StateMachineTool extends BaseTool {
    private StateMachine stateMachine;
    private String className;
    private Collection cOAssocs;
    private Collection sMAssocs;
    private Collection submachines;
    private Collection states;
    private Collection stableStates;
    private Collection compositeStates;
    private Map stateTools = new HashMap();
    private Map setterNames = new HashMap();
    private Map paramNames = new HashMap();
    private Map instanceNames = new HashMap();

    public StateMachineTool(StateMachine sm) {
        this.stateMachine = sm;
    }

    public String getClassName() {
        if (className == null) {
            className = getClassName("", stateMachine.getName(), "");
        }
        return className;
    }

    public String getSetterName(Association association) {
        if (! setterNames.containsKey(association)) {
            setterNames.put(association, createIdentifier("Set", association.getSupplierRole(), "", true, false));
        }
        return (String) setterNames.get(association);
    }

    public String getParamName(Association association) {
        if (! paramNames.containsKey(association)) {
            paramNames.put(association, createIdentifier(
                    association.getSupplierRole().toLowerCase().startsWith("a") ?
                    "an" : "a", association.getSupplierRole(), "", true, false));
        }
        return (String) paramNames.get(association);
    }

    public String getInstanceName(Association association) {
        if (! instanceNames.containsKey(association)) {
            instanceNames.put(association, createIdentifier("i", association.getSupplierRole(), "", true, false));
        }
        return (String) instanceNames.get(association);
    }

    public Collection getCOAssocs() {
        if (cOAssocs == null) {
            cOAssocs = getFilteredAssociations(stateMachine, ControlledObjectHandler.class);
        }
        return cOAssocs;
    }

    public Collection getSMAssocs() {
        if (sMAssocs == null) {
            sMAssocs = getFilteredAssociations(stateMachine, StateMachine.class);
        }
        return sMAssocs;
    }

    public Collection getSubmachines() {
        if (submachines == null) {
            List submachines = new ArrayList(stateMachine.getSubmachines());
            Collections.sort(submachines, new ClassElementComparator());
            this.submachines = submachines;
        }
        return submachines;
    }

    public Collection getStates() {
        if (states == null) {
            states = CollectionsTool.filter(
                    stateMachine.getAllStates(),
                    CollectionsTool.notInitialStatePredicate(),
                    CollectionsTool.stateComparator());
        }
        return states;
    }

    public Collection getStableStates() {
        if (stableStates == null) {
            stableStates = CollectionsTool.filter(
                    stateMachine.getAllStates(),
                    CollectionsTool.stableStatePredicate(),
                    CollectionsTool.stateComparator());
        }
        return stableStates;
    }

    public Collection getCompositeStates() {
        if (compositeStates == null) {
            compositeStates = CollectionsTool.filter(
                    stateMachine.getAllStates(),
                    CollectionsTool.compositeStatePredicate(),
                    CollectionsTool.stateComparator());
        }
        return compositeStates;
    }

    public StateTool getStateTool(State state) {
        if (! stateTools.containsKey(state)) {
            stateTools.put(state, new StateTool(state));
        }
        return (StateTool) stateTools.get(state);
    }

    private Collection getFilteredAssociations(StateMachine stateMachine,
            Class targetClass) {
        List associations = new ArrayList();
        for (Iterator i = stateMachine.getOutgoingAssociations().iterator(); i
                .hasNext();) {
            Association association = (Association) i.next();
            if (targetClass
                    .isAssignableFrom(association.getTarget().getClass())) {
                associations.add(association);
            }
        }
        Collections.sort(associations, new AssociationComparator());
        return associations;
    }

}

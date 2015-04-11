/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.computedstate;

import java.util.ArrayList;
import java.util.List;
import java.util.Stack;


@SuppressWarnings("unchecked")
public class ComputedContext {
    // TODO: for performance reason this should not be a synchronized stack
    private static Stack<ComputedContext> computedContextStack = new Stack<ComputedContext>();
    private ComputedState owner;
    private List<ComputedState> dependsOn = new ArrayList<ComputedState>();

    public ComputedContext(ComputedState owner) {
        this.owner = owner;
    }

    public static boolean isComputedContextStackEmpty() {
        return computedContextStack.isEmpty();
    }

    public static void pushComputedContextStack(ComputedState owner) {
        computedContextStack.push(new ComputedContext(owner));
    }

    public static ComputedContext popComputedContextStack() {
        return computedContextStack.pop();
    }

    public static ComputedContext peekComputedContextStack() {
        return computedContextStack.peek();
    }

    public List<ComputedState> getDependsOn() {
        return dependsOn;
    }

    public void addComputedState(ComputedState depdendsOn) {
        this.dependsOn.add(depdendsOn);
    }

    public ComputedState getOwner() {
        return owner;
    }

    public static boolean hasOwnerOnStack(ComputedState owner) {
        for (ComputedContext c : computedContextStack) {
            if (c.owner == owner)
                return true;
        }

        return false;
    }
}

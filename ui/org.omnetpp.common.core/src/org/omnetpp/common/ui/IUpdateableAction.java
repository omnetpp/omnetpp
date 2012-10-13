package org.omnetpp.common.ui;

import org.eclipse.jface.action.IAction;

/**
 * Extends IAction with an update() method. It often occurs that the state
 * of actions (enablement, toggle state, etc) needs to be updated in response
 * to some change notification. In such cases it is useful if one can just
 * call update() on all actions, and each action class would implement update()
 * to update their own states.
 *
 * @author Andras
 */
public interface IUpdateableAction extends IAction {
    /**
     * Update the state of the action (enablement, checked/unchecked, etc).
     */
    void update();
}

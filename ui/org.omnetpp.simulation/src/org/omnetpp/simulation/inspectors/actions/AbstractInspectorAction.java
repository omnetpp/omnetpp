package org.omnetpp.simulation.inspectors.actions;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.canvas.SelectionUtils;
import org.omnetpp.simulation.controller.SimulationController;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.model.cMessage;
import org.omnetpp.simulation.model.cModule;
import org.omnetpp.simulation.model.cObject;

/**
 * Abstract base class for inspector-related actions
 *
 * @author Andras
 */
public class AbstractInspectorAction extends Action implements IInspectorAction {
    private IInspectorContainer container;
    private IInspectorPart inspector; // if not null: action should operate on this inspector
    private ISelection selection; // if not null: selection associated with the action

    public AbstractInspectorAction() {
    }

    public AbstractInspectorAction(String text, int style, ImageDescriptor image) {
        super(text, style);
        setToolTipText(text);  //XXX as a default
        setImageDescriptor(image);
    }

    @Override
    public void setContext(IInspectorContainer container, ISelection selection) {
        this.container = container;
        this.selection = selection;
        update();
    }

    @Override
    public void setContext(IInspectorPart inspector) {
        this.container = inspector.getContainer();
        this.inspector = inspector;
        update();
    }

    public IInspectorContainer getInspectorContainer() {
        return container;
    }

    public SimulationController getSimulationController() {
        return getInspectorContainer().getEditor().getSimulationController();
    }

    /**
     * Returns the inspector part associated with the action, or null.
     */
    public IInspectorPart getInspectorPart() {
        return inspector;
    }

    /**
     * Returns the selection associated with the action. Note that this is
     * NOT necessarily the same as the canvas selection, the editor selection
     * or the workbench selection.
     */
    public ISelection getSelection() {
        return selection;
    }

    public void update() {
    }

    /**
     * Utility function: returns the (one) module associated with this action
     * (either as inspector or in the selection), or null.
     */
    protected cModule getModule() {
        if (getInspectorPart() != null && getInspectorPart().getObject() instanceof cModule)
            return (cModule) getInspectorPart().getObject();
        if (getSelection() != null && !getSelection().isEmpty()) {
            List<cModule> modules = SelectionUtils.getObjects(getSelection(), cModule.class);
            if (modules.size() == 1)
                return modules.get(0);
        }
        return null;
    }

    /**
     * Utility function: returns the (one) message associated with this action
     * (either as inspector or in the selection), or null.
     */
    protected cMessage getMessage() {
        if (getInspectorPart() != null && getInspectorPart().getObject() instanceof cMessage)
            return (cMessage) getInspectorPart().getObject();
        if (getSelection() != null && !getSelection().isEmpty()) {
            List<cMessage> messages = SelectionUtils.getObjects(getSelection(), cMessage.class);
            if (messages.size() == 1)
                return messages.get(0);
        }
        return null;
    }

    /**
     * Utility function: returns the objects module associated with this action
     * (either as inspector or in the selection).
     */
    protected List<cObject> getObjects() {
        if (getInspectorPart() != null)
            return Arrays.asList(new cObject[] { getInspectorPart().getObject() });
        if (getSelection() != null)
            return SelectionUtils.getObjects(getSelection(), cObject.class);
        return new ArrayList<cObject>();
    }

}

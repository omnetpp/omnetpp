package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.action.Action;
import org.eclipse.jface.resource.ImageDescriptor;
import org.omnetpp.simulation.inspectors.IInspectorPart;

/**
 * Abstract base class for inspector-related actions
 *
 * @author Andras
 */
public class AbstractInspectorAction extends Action implements IInspectorAction {
    private IInspectorPart inspector;

    public AbstractInspectorAction() {
    }

    public AbstractInspectorAction(String text, int style, ImageDescriptor image) {
        super(text, style);
        setToolTipText(text);  //XXX as a default
        setImageDescriptor(image);
    }

//    public AbstractInspectorAction(String text, int style, String imagePath) {
//        super(text, style);
//        setToolTipText(text);  //XXX as a default
//        setImageDescriptor(SimulationPlugin.getImageDescriptor(imagePath)); --specific to this plugin
//    }

    @Override
    public void setInspectorPart(IInspectorPart inspector) {
        this.inspector = inspector;
        update();
    }

    public IInspectorPart getInspectorPart() {
        Assert.isNotNull(inspector, "setInspectorPart() should have been called");
        return inspector;
    }

    public void update() {
    }
}

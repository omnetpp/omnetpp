package org.omnetpp.simulation.inspectors.actions;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.simulation.SimulationPlugin;
import org.omnetpp.simulation.SimulationUIConstants;
import org.omnetpp.simulation.canvas.IInspectorContainer;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.inspectors.IInspectorPart;
import org.omnetpp.simulation.model.cObject;

/**
 *
 * @author Andras
 */
public class InspectParentAction extends AbstractInspectorAction {
    public InspectParentAction() {
        super("Inspect parent", AS_PUSH_BUTTON, SimulationPlugin.getImageDescriptor(SimulationUIConstants.IMG_TOOL_PARENT));
    }

    @Override
    public void run() {
        try {
            cObject object = getInspectorPart().getObject();
            object.loadIfUnfilled();
            cObject parent = object.getOwner();
            if (parent != null) {
                IInspectorContainer container = getInspectorContainer();
                container.inspect(parent);
            }
        }
        catch (CommunicationException e) {
            // nothing -- error dialog and logging is already taken care of in the lower layers
        }
        catch (Exception e) {
            MessageDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error: " + e.toString());
            SimulationPlugin.logError(e);
        }
    }

    @Override
    public void update() {
        try {
            IInspectorPart inspector = getInspectorPart();
            if (inspector == null)
                setEnabled(false);
            else {
                cObject object = inspector.getObject();
                object.loadIfUnfilled();
                setEnabled(object.getOwner() != null);
            }
        } catch (CommunicationException e) {
            setEnabled(false);
        }
    }
}

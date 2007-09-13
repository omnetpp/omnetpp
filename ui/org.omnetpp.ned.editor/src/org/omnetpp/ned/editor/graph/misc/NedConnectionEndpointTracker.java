package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.RequestConstants;
import org.eclipse.gef.tools.ConnectionEndpointTracker;
import org.omnetpp.ned.editor.graph.commands.ReconnectCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleConnectionEditPart;
import org.omnetpp.ned.model.pojo.ConnectionElement;

/**
 * Special drag tracker for connection handles that requests gate names if the source or
 * target handle is reconnected.
 *
 * @author rhornig
 */
public class NedConnectionEndpointTracker extends ConnectionEndpointTracker {

	public NedConnectionEndpointTracker(ConnectionEditPart cep) {
		super(cep);
	}

	@Override
	protected boolean handleButtonUp(int button) {
		if (stateTransition(STATE_DRAG_IN_PROGRESS, STATE_TERMINAL)) {
			ReconnectCommand connCommand = (ReconnectCommand)getCommand();
	    	if (connCommand == null)
	    	    return false;
            // depending on which side we are reconnecting, we offer a full gate list on that side
            if (RequestConstants.REQ_RECONNECT_TARGET.equals(getCommandName()))
                connCommand.setDestGate(null);
            if (RequestConstants.REQ_RECONNECT_SOURCE.equals(getCommandName()))
                connCommand.setSrcGate(null);

	    	// ask the user about which gates should be connected
			ConnectionElement selectedConn
				= ConnectionChooser.open(connCommand);

			eraseSourceFeedback();
			eraseTargetFeedback();

	    	// if no selection was made, cancel the command
			if (selectedConn == null)
                // revert the connection change (user cancel - do not execute the command)
				return false;

			// copy the selected connection attributes to the command
	    	ConnectionElement templateConn = connCommand.getConnection();
			ReconnectCommand.copyConn(selectedConn, templateConn);

			// execute the command
	    	setCurrentCommand(connCommand);
			executeCurrentCommand();
		}
		return true;
	}

	@Override
	protected EditPartViewer.Conditional getTargetingConditional() {
		return new EditPartViewer.Conditional() {
			public boolean evaluate(EditPart editpart) {
				// if the target is a compound module, allow attachment only at the borders
				if (editpart instanceof CompoundModuleEditPart) {
					CompoundModuleEditPart cmep = (CompoundModuleEditPart)editpart;
					return cmep.isOnBorder(getLocation().x, getLocation().y);
				}

				return editpart.getTargetEditPart(getTargetRequest()) != null;
			}
		};
	}

    @Override
    protected boolean handleDragInProgress() {
        // allow dragging ONLY if the connection is editable
        if (((ModuleConnectionEditPart)getConnectionEditPart()).isEditable())
            return super.handleDragInProgress();

        return false;
    }
}

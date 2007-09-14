package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.ConnectionEditPart;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.tools.ConnectionEndpointTracker;

import org.omnetpp.ned.editor.graph.commands.ReconnectCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleConnectionEditPart;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;

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
		    Command cmd = getCommand();
		    if (cmd == null || !(cmd instanceof ReconnectCommand)) 
		        return false;
			ReconnectCommand connCommand = (ReconnectCommand)cmd;

			ModuleConnectionEditPart connPart = (ModuleConnectionEditPart)getConnectionEditPart();
	        CompoundModuleElementEx compoundMod = connPart.getCompoundModulePart().getCompoundModuleModel(); 
	        // ask the user about which gates should be connected, ask for both source and destination gates
	        // FIXME we should extract the direction (src/dest) locally and not from the command 
	        ConnectionElementEx selectedConn = ConnectionChooser.open(compoundMod, connCommand.getTemplateConnection(), 
	                    !connCommand.isDestReconnect(), connCommand.isDestReconnect());
			
			eraseSourceFeedback();
			eraseTargetFeedback();

	    	// if no selection was made, cancel the command
			if (selectedConn == null)
                // revert the connection change (user cancel - do not execute the command)
				return false;

			ConnectionElementEx templateConn = connCommand.getTemplateConnection();
			// copy the selected connection attributes to the command
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

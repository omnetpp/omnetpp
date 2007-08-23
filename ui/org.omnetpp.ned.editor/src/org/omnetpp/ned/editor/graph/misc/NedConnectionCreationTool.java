package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.tools.ConnectionCreationTool;
import org.omnetpp.ned.editor.graph.commands.ConnectionCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.model.pojo.ConnectionElement;

/**
 * Special connection tool that requests additional information regarding gate association at the
 * end of connection creation. It pops up a menu with all gate pairs for selection.
 *
 * @author rhornig
 */
// CHECKME we can use ConnectionDragCreationTool for a dran'n drop type behvior
public class NedConnectionCreationTool extends ConnectionCreationTool {

	// override the method to fix a GEF BUGFIX
	@Override
	protected boolean handleButtonDown(int button) {
		// BUGFIX START
		// The original implementation reaches the TERMINAL state BEFORE calling
		// handleCreateConnect(). This causes a problem if we place an event loop in
		// handleCreateConnect() as the tool resets to initial state (no source or target parts)
		// leaving the previous sourceFeedback figure in place.
		if (button == 1 && isInState(STATE_CONNECTION_STARTED)) {
			boolean creationResult = handleCreateConnection();
			setState(STATE_TERMINAL);
			return creationResult;
		}
		// BUGFIX END

		super.handleButtonDown(button);
		if (isInState(STATE_CONNECTION_STARTED))
			//Fake a drag to cause feedback to be displayed immediately on mouse down.
			handleDrag();
		return true;
	}

	// overridden to enable a popup menu during connection creation asking the user
	// which gates should be connected
	@Override
	protected boolean handleCreateConnection() {
		ConnectionCommand endCommand = (ConnectionCommand)getCommand();
    	setCurrentCommand(endCommand);

        if (endCommand == null)
            return false;

        endCommand.setDestGate(null);
        endCommand.setSrcGate(null);
    	// ask the user about which gates should be connected, ask for both source and destination gates
		ConnectionElement selectedConn = ConnectionChooser.open(endCommand);

    	eraseSourceFeedback();

    	// if no selection was made, cancel the command
		if (selectedConn == null)
            // revert the connection change (user cancel - do not execute the command)
			return false;

		// copy the selected connection attributes to the command
    	ConnectionElement templateConn = endCommand.getConnectionTemplate();
		ConnectionCommand.copyConn(selectedConn, templateConn);
		// execute the command
    	executeCurrentCommand();
    	return true;
	}



	// filter which editparts can be used as connection source or target
	@Override
	protected EditPartViewer.Conditional getTargetingConditional() {

		return new EditPartViewer.Conditional() {
			public boolean evaluate(EditPart editpart) {
				// during the connection creation, check if the target editpart is a valid editpart
				// ie: submodule in the same compound module or the parent compound module
				if (isInState(STATE_CONNECTION_STARTED)) {
					EditPart srcEditPart = ((CreateConnectionRequest)getTargetRequest()).getSourceEditPart();
					EditPart destEditPart = editpart;

                    if (srcEditPart == null || destEditPart == null ||
                            !(srcEditPart instanceof ModuleEditPart) || !(destEditPart instanceof ModuleEditPart))
                        return false;

                    if (((ModuleEditPart)srcEditPart).getCompoundModulePart() != ((ModuleEditPart)destEditPart).getCompoundModulePart())
                        return false;
				}
				// if the selection target is a CompoundModule, allow selection ONLY using it's borders
				if (editpart instanceof CompoundModuleEditPart) {
					CompoundModuleEditPart cmep = (CompoundModuleEditPart)editpart;
					return cmep.isOnBorder(getLocation().x, getLocation().y);
				}

				return editpart.isSelectable();
			}
		};
	}
}

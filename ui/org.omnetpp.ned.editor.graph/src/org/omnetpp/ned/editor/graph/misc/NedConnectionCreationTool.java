package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.requests.CreateConnectionRequest;
import org.eclipse.gef.tools.ConnectionCreationTool;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.SubmoduleEditPart;
import org.omnetpp.ned.editor.graph.model.commands.ConnectionCommand;
import org.omnetpp.ned2.model.pojo.ConnectionNode;

/**
 * @author rhornig
 * Special connection tool that requests additional information regarding gate association at the 
 * end of connection creation. It pops up a menu with all gate pairs for selection.
 */
public class NedConnectionCreationTool extends ConnectionCreationTool {

	// override the method to fix a GEF BUG 
	@Override
	protected boolean handleButtonDown(int button) {
		// BUGFIX START
		// The original implementation reaches the TERMINAL state BEFORE calling
		// handleCreateConnect(). This causes a problem if we place an event loop in
		// handleCreateConnect() as the tool resetets to initial state (no source or target parts)
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
    	
    	// ask the user about which gates should be connected
		ConnectionNode selectedConn 
			= ConnectionChooser.open(endCommand.getSrcModule(), endCommand.getDestModule());
		
    	eraseSourceFeedback();

    	// if no selection was made, cancel the command
		if (selectedConn == null) {
			// revert the connection change (user cancel - do not execute the command)
			return false;
		}
    	
		// copy the selected connection attributes to the command
    	ConnectionNode templateConn = endCommand.getConnectionTemplate();
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

					// if both editpart is Submodule, they must share the same parent (ie. compound module)
					if (srcEditPart instanceof SubmoduleEditPart 
							&& destEditPart instanceof SubmoduleEditPart
							&& srcEditPart.getParent() != destEditPart.getParent()) 
						return false;
					// if one of them is Submodule and the other is compound module then the
					// submodule's parent must be the compound module (ie. submodule can be connected ony to it's parent)
					if (srcEditPart instanceof SubmoduleEditPart 
							&& destEditPart instanceof CompoundModuleEditPart
							&& srcEditPart.getParent() != destEditPart) 
						return false;
					if (srcEditPart instanceof CompoundModuleEditPart 
							&& destEditPart instanceof SubmoduleEditPart
							&& srcEditPart != destEditPart.getParent()) 
						return false;
					// do not allow connetction between compound modules at all
					if (srcEditPart instanceof CompoundModuleEditPart 
							&& destEditPart instanceof CompoundModuleEditPart) {
						return false;
					}
				}
				// if the selection target is a CompoundModule, allow selection ONLY using it's borders
				if (editpart instanceof CompoundModuleEditPart) {
					CompoundModuleEditPart cmep = (CompoundModuleEditPart)editpart;
					return cmep.isSelectable(getLocation().x, getLocation().y);
				}
				
				return editpart.isSelectable();
			}
		};
	}
}

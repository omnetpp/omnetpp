package org.omnetpp.ned.editor.graph.misc;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartViewer;
import org.eclipse.gef.tools.PanningSelectionTool;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;

/**
 * Special selection tool that allows the dragging of compound modules ONLY by clicking
 * on the border area of a compound module.
 *
 * @author rhornig
 */
public class NedSelectionTool extends PanningSelectionTool {

	@Override
	protected EditPartViewer.Conditional getTargetingConditional() {
		
		return new EditPartViewer.Conditional() {
			public boolean evaluate(EditPart editpart) {
				// if the selection target is a CompoundModule, allow selection ONLY using it's borders
				if (editpart instanceof CompoundModuleEditPart) {
					CompoundModuleEditPart cmep = (CompoundModuleEditPart)editpart;
					// if we used the primary mouse button we should select the compound module ONLY
					// if the mouse coordinates are on the border of the compound module
					if (getCurrentInput().isMouseButtonDown(1))
						return cmep.isOnBorder(getLocation().x, getLocation().y);
				}
				
				return editpart.isSelectable();
			}
		};
	}
}

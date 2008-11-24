package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.ui.actions.Clipboard;
import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.INedModelProvider;


/**
 * Implements "Cut to clipboard".
 *
 * @author Andras
 */
public class CutAction extends SelectionAction {
    public static final String ID = ActionFactory.CUT.getId();
    public static final String MENUNAME = "Cu&t";
    public static final String TOOLTIP = "Cut to clipboard";

    public CutAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
    	setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_CUT));
    	setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_CUT_DISABLED));
    }

    // FIXME non editable parts should not be deleted (cut should not be allowed here)
    @Override
    protected boolean calculateEnabled() {
        return getSelectedObjects().size() > 0;
    }

	@Override @SuppressWarnings("unchecked")
    public void run() {
		// holder for the Delete commands
		CompoundCommand compoundCommand = new CompoundCommand();

		// collect selected editparts
		List<EditPart> selectedEditParts = new ArrayList<EditPart>(getSelectedObjects());
		selectedEditParts.addAll(EditPartUtil.getAttachedConnections(selectedEditParts));

		// translate from editparts to model elements
		List<INEDElement> selectedModelObjects = new ArrayList<INEDElement>();
		for (EditPart editPart : selectedEditParts) {
        	if (editPart instanceof INedModelProvider) {
				INEDElement model = ((INedModelProvider)editPart).getNedModel();
				selectedModelObjects.add(model.deepDup());
				compoundCommand.add(new DeleteCommand(model));
			}
        }

        // copy to clipboard, and fire the deletion command
        if (selectedModelObjects.size() > 0) {
        	Clipboard.getDefault().setContents(selectedModelObjects.toArray(new INEDElement[]{}));
        	compoundCommand.setLabel("Cut " + StringUtils.formatCounted(selectedModelObjects.size(), "object"));
        	execute(compoundCommand);
        }
    }
}

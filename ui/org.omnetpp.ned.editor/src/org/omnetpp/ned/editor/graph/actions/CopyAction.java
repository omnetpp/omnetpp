/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.ui.actions.Clipboard;
import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.interfaces.INedModelProvider;


/**
 * Implements "Copy to clipboard".
 *
 * @author Andras
 */
public class CopyAction extends SelectionAction {
    public static final String ID = ActionFactory.COPY.getId();
    public static final String MENUNAME = "&Copy";
    public static final String TOOLTIP = "Copy to clipboard";

    public CopyAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
        setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_COPY));
        setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_COPY_DISABLED));
    }

    @Override
    protected boolean calculateEnabled() {
        return getSelectedObjects().size() > 0;
    }

    @Override @SuppressWarnings("unchecked")
    public void run() {
        // collect selected editparts
        List<EditPart> selectedEditParts = new ArrayList<EditPart>((Collection<? extends EditPart>) getSelectedObjects());
        selectedEditParts.addAll(EditPartUtil.getAttachedConnections(selectedEditParts));  // FIXME skip connections that already selected

        // translate from editparts to model elements
        List<INedElement> selectedModelObjects = new ArrayList<INedElement>();
        for (EditPart editPart : selectedEditParts)
            if (editPart instanceof INedModelProvider)
                selectedModelObjects.add(((INedModelProvider)editPart).getModel().deepDup());

        // copy to clipboard
        if (selectedModelObjects.size() > 0)
            Clipboard.getDefault().setContents(selectedModelObjects.toArray(new INedElement[]{}));
    }

}

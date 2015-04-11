/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.requests.GroupRequest;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;

import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.commands.ChangeLayoutSeedCommand;
import org.omnetpp.ned.editor.graph.parts.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.ModuleEditPart;
import org.omnetpp.ned.editor.graph.parts.SubmoduleEditPart;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

/**
 * Re-position unpinned submodules in the currently selected compound module
 *
 * @author rhornig
 */
public class RelayoutAction extends org.eclipse.gef.ui.actions.SelectionAction {

    public static final String ID = "Relayout";
    public static final String MENUNAME = "Re-&layout";
    public static final String TOOLTIP = "Re-layout compound module";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_LAYOUT);

    GroupRequest request;

    public RelayoutAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
    }

    /**
     * This command can be executed when a single compound module or a submodule is selected
     */
    @Override
    protected boolean calculateEnabled() {
        // only support a single selected object
        if (getSelectedObjects().size() != 1)
            return false;
        if (getSelectedObjects().get(0) instanceof CompoundModuleEditPart)
            return true;
        if (getSelectedObjects().get(0) instanceof SubmoduleEditPart)
            return true;
        return false;
    }

    /**
     * Created and returns a command for the request from the containing compound module
     */
    @SuppressWarnings("unchecked")
    private Command getCommand() {
        List selEditParts = getSelectedObjects();

        if (!calculateEnabled())
            return null;

        // get the parent of the currently selected
        CompoundModuleElementEx compoundModuleElementEx
            = ((ModuleEditPart)selEditParts.get(0)).getCompoundModulePart().getModel();
        // create command that changes the compound modules layout seed
        return new ChangeLayoutSeedCommand(compoundModuleElementEx);
    }

    @Override
    public void run() {
        execute(getCommand());
    }

}

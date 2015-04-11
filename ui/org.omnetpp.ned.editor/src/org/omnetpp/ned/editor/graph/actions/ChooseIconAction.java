/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.core.resources.IProject;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.image.ImageSelectionDialog;
import org.omnetpp.ned.editor.graph.commands.ChangeDisplayPropertyCommand;
import org.omnetpp.ned.editor.graph.parts.EditPartUtil;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasDisplayString;
import org.omnetpp.ned.model.interfaces.INedModelProvider;


/**
 * Action to edit the icon property (unused)
 *
 * @author rhornig
 */
public class ChooseIconAction extends org.eclipse.gef.ui.actions.SelectionAction {
    public static final String ID = "ChooseIcon";
    public static final String MENUNAME = "C&hoose an Icon...";
    public static final String TOOLTIP = "Choose an icon for the module";
    public static final ImageDescriptor IMAGE = ImageFactory.global().getDescriptor(ImageFactory.TOOLBAR_IMAGE_CHOOSEICON);

    private final IProject project; // scope of the icon search, can be null

    public ChooseIconAction(IWorkbenchPart part, IProject project) {
        super(part);
        this.project = project;
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
    }

    @Override
    protected boolean calculateEnabled() {
        return getCommand().canExecute();
    }

    @Override
    public void run() {
        Command command = getCommand();

        if (command instanceof ChangeDisplayPropertyCommand) {
            ChangeDisplayPropertyCommand cdpCommand = (ChangeDisplayPropertyCommand)command;
            String value = openDialogBox(cdpCommand.getOldValue());
            if (value != null) {
                // set the new value for the image
                cdpCommand.setValue(value);
                command.setLabel("Change Icon");
                execute(command);
            }
        }
    }

    /**
     * Returns the id of the selected image
     */
    protected String openDialogBox(String initialValue) {
        ImageSelectionDialog dialog =    //FIXME this is always called with initialValue==null!! --Andras
            new ImageSelectionDialog(Display.getDefault().getActiveShell(), initialValue, project);

        if (dialog.open() == Dialog.OK)
            return dialog.getImageId();
        // cancelled
        return null;
    }

    /**
     * Returns the command used for changing the image property
     */
    protected Command getCommand() {
        if (getSelectedObjects().size() > 0) {
            Object obj = getSelectedObjects().get(0);
            // check if the part is editable at all
            if (!EditPartUtil.isEditable(obj))
                return UnexecutableCommand.INSTANCE;

            if (obj instanceof INedModelProvider) {
                INedElement element = ((INedModelProvider)obj).getModel();
                // return command only for those elements which support the icon property
                if (element instanceof IHasDisplayString &&
                        (element instanceof SubmoduleElementEx || element instanceof SimpleModuleElementEx
                                || element instanceof CompoundModuleElementEx || element instanceof ModuleInterfaceElementEx))
                    return new ChangeDisplayPropertyCommand((IHasDisplayString)element, IDisplayString.Prop.IMAGE);
            }
        }
        return UnexecutableCommand.INSTANCE;
    }

}

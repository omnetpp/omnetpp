package org.omnetpp.ide.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.ui.IWorkbenchWindow;
import org.omnetpp.ide.OmnetIdePlugin;
import org.omnetpp.ide.misc.ICommandIds;


public class MessagePopupAction extends Action {

    private final IWorkbenchWindow window;

    MessagePopupAction(String text, IWorkbenchWindow window) {
        super(text);
        this.window = window;
        // The id is used to refer to the action in a menu or toolbar
        setId(ICommandIds.CMD_OPEN_MESSAGE);
        // Associate the action with a pre-defined command, to allow key bindings.
        setActionDefinitionId(ICommandIds.CMD_OPEN_MESSAGE);
        setImageDescriptor(org.omnetpp.ide.OmnetIdePlugin.getImageDescriptor("/icons/sample3.gif"));
    }

    public void run() {
        MessageDialog.openInformation(window.getShell(), "Open", "Open Message Dialog!");
    }
}
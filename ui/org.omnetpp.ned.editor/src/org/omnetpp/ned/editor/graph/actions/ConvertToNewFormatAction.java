package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.commands.ConvertNedVersionCommand;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.interfaces.IModelProvider;


/**
 * This action converts the ned model to new format (if it is in old format currently)
 *
 * @author rhornig
 */
public class ConvertToNewFormatAction extends org.eclipse.gef.ui.actions.SelectionAction {

    public static final String ID = "ConvertToNewFormat";
    public static final String MENUNAME = "Convert to 4.0 Format";
    public static final String DESCRIPTION = "Converts the NED file into the new OMNeT++ 4.0 format";

    public ConvertToNewFormatAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setDescription(DESCRIPTION);
        setToolTipText(DESCRIPTION);
    }

    @Override
    protected boolean calculateEnabled() {
        return getCommand().canExecute();
    }

    @Override
    public void run() {
        execute(getCommand());
    }

    private Command getCommand() {
        if (getSelectedObjects().size() > 0) {
            Object obj = getSelectedObjects().get(0);
            if (obj instanceof IModelProvider) {
                INEDElement element = ((IModelProvider)obj).getNEDModel();
                return new ConvertNedVersionCommand(element);
            }
        }
        return UnexecutableCommand.INSTANCE;
    }

}

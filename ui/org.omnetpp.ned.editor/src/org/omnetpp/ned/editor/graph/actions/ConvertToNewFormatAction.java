package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.ned.editor.graph.commands.ConvertNedVersionCommand;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IModelProvider;


/**
 * This action converts the ned model to new format (if it is in old format cuurently)
 * 
 * @author rhornig
 */
public class ConvertToNewFormatAction extends org.eclipse.gef.ui.actions.SelectionAction {

    public static final String ID = "ConvertToNewFormat";
    public static final String MENUNAME = "Convert to OMNeT++ 4.0 format";
    public static final String TOOLTIP = "Converts the NED file into the new OMNeT++ 4.0 format";

    public ConvertToNewFormatAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
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
                NEDElement element = ((IModelProvider)obj).getNEDModel();
                return new ConvertNedVersionCommand(element);
            }
        }
        return UnexecutableCommand.INSTANCE;
    }
    
}

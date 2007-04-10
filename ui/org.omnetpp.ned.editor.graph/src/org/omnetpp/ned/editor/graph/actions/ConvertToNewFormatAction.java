package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
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
    public static final String MENUNAME = "Convert to New Format";
    public static final String TOOLTIP = "Converts the ned file to new format";
    public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_PROPERTIES);

    public ConvertToNewFormatAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
        setHoverImageDescriptor(IMAGE);
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

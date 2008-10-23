package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPart;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.misc.ParametersDialog;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.INedModelProvider;


/**
 * Action to edit the parameters
 *
 * @author rhornig
 */
public class ParametersDialogAction extends org.eclipse.gef.ui.actions.SelectionAction {
    public static final String ID = "Parameters";
    public static final String MENUNAME = "Edit Parameters...";
    public static final String TOOLTIP = "Edit the module parameters";
    public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.MODEL_IMAGE_PARAM);

    public ParametersDialogAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
    }

    @Override
    protected boolean calculateEnabled() {
        if (getSelectedObjects().size() == 1 && getSelectedObjects().get(0) instanceof INedModelProvider) {
           INEDElement element = ((INedModelProvider)getSelectedObjects().get(0)).getNedModel();
           return element instanceof IHasParameters && 
               element.getConsistencyProblemMaxCumulatedSeverity() == NEDElement.SEVERITY_NONE &&
               element.getSyntaxProblemMaxCumulatedSeverity() == NEDElement.SEVERITY_NONE;
        }
        else
            return false;
    }

    @Override
    public void run() {
        IHasParameters paramsParent = (IHasParameters)((INedModelProvider)getSelectedObjects().get(0)).getNedModel();
        ParametersDialog dialog = new ParametersDialog(Display.getDefault().getActiveShell(), paramsParent);

        // if the dialog is cancelled, the command is not executable
        dialog.open();
        execute(dialog.getResultCommand());
    }
}

package org.omnetpp.ned.editor.graph.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IWorkbenchPart;

import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
import org.omnetpp.ned.editor.graph.commands.DeleteCommand;
import org.omnetpp.ned.editor.graph.misc.EditParametersDialog;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.ParametersNode;


/**
 * Action to edit the parameters
 * @author rhornig
 */
public class ParametersDialogAction extends org.eclipse.gef.ui.actions.SelectionAction {
    public static final String ID = "Parameters";
    public static final String MENUNAME = "Edit Parameters...";
    public static final String TOOLTIP = "Edit the module parameters";
    public static final ImageDescriptor IMAGE = ImageFactory.getDescriptor(ImageFactory.TOOLBAR_IMAGE_CHOOSEICON);

    public ParametersDialogAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        setImageDescriptor(IMAGE);
    }

    @Override
    protected boolean calculateEnabled() {
        return getSelectedObjects().size() == 1 &&
                getSelectedObjects().get(0) instanceof IModelProvider &&
                ((IModelProvider)getSelectedObjects().get(0)).getNEDModel() instanceof IHasParameters;
    }

    @Override
    public void run() {
        INEDElement paramsParent = ((IModelProvider)getSelectedObjects().get(0)).getNEDModel();
        List<EditParametersDialog.ParamLine> paramList = new ArrayList<EditParametersDialog.ParamLine>();
        INEDElement oldParamsNode = paramsParent.getFirstChildWithTag(NEDElementTags.NED_PARAMETERS);
        // build ParamLine list (value objects) for dialog editing
        // TODO build the list according to the inheritance/non inheritance
        if (oldParamsNode != null)
            for(INEDElement param : oldParamsNode)
                if (param instanceof ParamNodeEx)
                    paramList.add(new EditParametersDialog.ParamLine((ParamNodeEx)param, false));

        // create and execute the parameter editor dialog
        EditParametersDialog dialog =
            new EditParametersDialog(Display.getDefault().getActiveShell(), paramList);

        if (dialog.open() == Dialog.CANCEL)
            return;

        // create an OFF MODELL copy of the parameters node
        ParametersNode newParamsNode = oldParamsNode != null ? (ParametersNode)oldParamsNode.deepDup(null) :
                                    (ParametersNode)NEDElementFactoryEx.getInstance().createNodeWithTag(NEDElementTags.NED_PARAMETERS);

        // TODO synchronize only the changes
        for(INEDElement paramNode : newParamsNode)
            if (paramNode instanceof ParamNodeEx)
                paramNode.removeFromParent();

        // add the new nodes
        for(EditParametersDialog.ParamLine paramLine : dialog.getParams()) {
            newParamsNode.appendChild(paramLine.getChangedParamNode());
        }

        // create a replace / compound command
        CompoundCommand paramReplaceCommand = new CompoundCommand("Change Parameters");
        if (oldParamsNode != null)
            paramReplaceCommand.add(new DeleteCommand(oldParamsNode));

        paramReplaceCommand.add(new AddNEDElementCommand(paramsParent, newParamsNode));

        execute(paramReplaceCommand);
    }

}

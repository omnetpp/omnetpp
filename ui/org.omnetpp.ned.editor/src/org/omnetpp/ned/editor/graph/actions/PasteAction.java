package org.omnetpp.ned.editor.graph.actions;

import java.util.List;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.ui.actions.Clipboard;
import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
import org.omnetpp.ned.editor.graph.edit.CompoundModuleEditPart;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.NEDElementTags;
import org.omnetpp.ned.model.pojo.SubmodulesElement;


/**
 * Implements "Paste from clipboard".
 * 
 * @author Andras
 */
//FIXME as retargetAction!
public class PasteAction extends SelectionAction {
    public static final String ID = ActionFactory.PASTE.getId();
    public static final String MENUNAME = "Paste";
    public static final String TOOLTIP = "Paste from clipboard";

    public PasteAction(IWorkbenchPart part) {
        super(part);
        setText(MENUNAME);
        setId(ID);
        setToolTipText(TOOLTIP);
        ISharedImages sharedImages = PlatformUI.getWorkbench().getSharedImages();
    	setImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_PASTE));
    	setDisabledImageDescriptor(sharedImages.getImageDescriptor(ISharedImages.IMG_TOOL_PASTE_DISABLED));
    }

    @Override
    protected boolean calculateEnabled() {
    	//XXX getCommand().canExecute() ?
        Object contents = Clipboard.getDefault().getContents();
        return contents instanceof INEDElement[];
    }

	@Override @SuppressWarnings("unchecked")
    public void run() {
		Command command = getCommand();
		if (command.canExecute())
			execute(command);
    }

    /**
     * Returns the command used for pasting the objects
     */
    protected Command getCommand() {
        Object contents = Clipboard.getDefault().getContents();
        if (contents instanceof INEDElement[]) {
        	CompoundCommand result = new CompoundCommand();

            GraphicalViewer viewer = (GraphicalViewer)getWorkbenchPart().getAdapter(GraphicalViewer.class);
            EditPart toplevelEditPart = viewer.getContents();
//            // or we check if there is a selection. in this case parent is the compound module
//            // in which the selection is present
//            List selection = viewer.getSelectedEditParts();
//            if (selection.size() > 0 && selection.get(0) instanceof ModuleEditPart)
//            	toplevelEditPart = ((ModuleEditPart)selection.get(0)).getCompoundModulePart();

            // we have the parent where all child part have to be selected
//            viewer.setSelection(new StructuredSelection(toplevelEditPart.getChildren()));

        	
        	
        	
        	INEDElement[] elements = (INEDElement[]) contents;
        	for (INEDElement element : elements) {
    			System.out.println("pasting " + element);
        		if (element instanceof INedTypeElement) {
        			NedFileElementEx parent = (NedFileElementEx)toplevelEditPart.getModel();
        			//FIXME refine insertion point: maybe before the first selected element's INedTypeElement parent?
        			result.add(new AddNEDElementCommand(parent, element.deepDup()));
        		}
        		else if (element instanceof SubmoduleElementEx) {
        			List selection = viewer.getSelectedEditParts();
        			if (selection.size() == 0 || !(selection.get(0) instanceof ModuleEditPart)) {
        				//FIXME error: dunno where to insert
					}
        			else {
        				CompoundModuleEditPart compoundModulePart = ((ModuleEditPart)selection.get(0)).getCompoundModulePart();
        				CompoundModuleElementEx compoundModule = compoundModulePart.getCompoundModuleModel();
        				SubmodulesElement submodulesElement = compoundModule.getFirstSubmodulesChild();
        				if (submodulesElement == null) {
        					submodulesElement = (SubmodulesElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_SUBMODULES);
        					submodulesElement.appendChild(element.deepDup());
                			result.add(new AddNEDElementCommand(compoundModule, submodulesElement));
        				}
        				else {
                			result.add(new AddNEDElementCommand(submodulesElement, element.deepDup()));
        				}
        			}
        		}
        		else if (element instanceof ConnectionElementEx) {
        			List selection = viewer.getSelectedEditParts();
        			if (selection.size() == 0 || !(selection.get(0) instanceof ModuleEditPart)) {
        				//FIXME error: dunno where to insert
					}
        			else {
        				CompoundModuleEditPart compoundModulePart = ((ModuleEditPart)selection.get(0)).getCompoundModulePart();
        				CompoundModuleElementEx compoundModule = compoundModulePart.getCompoundModuleModel();
        				ConnectionsElement connectionsElement = compoundModule.getFirstConnectionsChild();
        				if (connectionsElement == null) {
        					connectionsElement = (ConnectionsElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTIONS);
        					connectionsElement.appendChild(element.deepDup());
                			result.add(new AddNEDElementCommand(compoundModule, connectionsElement));
        				}
        				else {
                			result.add(new AddNEDElementCommand(connectionsElement, element.deepDup()));
        				}
        			}
        		}
        		else {
        			System.out.println("don't know how to paste " + element);  //XXX
        		}
        	}
        	return result;
        }
        else {
        	return UnexecutableCommand.INSTANCE;
        }
    	
    }
}

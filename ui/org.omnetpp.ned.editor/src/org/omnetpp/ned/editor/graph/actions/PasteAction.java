package org.omnetpp.ned.editor.graph.actions;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.commands.Command;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.commands.UnexecutableCommand;
import org.eclipse.gef.ui.actions.Clipboard;
import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
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
	 * Returns the command for pasting the objects
	 */
	protected Command getCommand() {
		Object contents = Clipboard.getDefault().getContents();
		if (contents instanceof INEDElement[]) {
			CompoundCommand compoundCommand = new CompoundCommand();

			INEDElement[] elements = (INEDElement[]) contents;
			CompoundModuleElementEx compoundModule = getTargetCompoundModule(); // for submodules and connections
			//FIXME if needed, create a compound module on demand? or pop up an error dialog?
			Map<String, String> submoduleNameMap = new HashMap<String, String>();
			for (INEDElement element : elements) {
				System.out.println("pasting " + element);
				if (element instanceof INedTypeElement)
					pasteNedTypeElement((INedTypeElement)element.deepDup(), compoundCommand);
				else if (element instanceof SubmoduleElementEx && compoundModule != null)
					pasteSubmodule((SubmoduleElementEx)element.deepDup(), compoundModule, submoduleNameMap, compoundCommand);
				else if (element instanceof ConnectionElementEx && compoundModule != null)
					pasteConnection((ConnectionElementEx)element.deepDup(), compoundModule, submoduleNameMap, compoundCommand);
				else
					System.out.println("don't know how to paste " + element);  //XXX
			}

			compoundCommand.setLabel("Paste " + StringUtils.formatCounted(elements.length, "object"));
			return compoundCommand;
		}
		else {
			return UnexecutableCommand.INSTANCE;
		}

	}

	protected void pasteNedTypeElement(INedTypeElement element, CompoundCommand compoundCommand) {
		EditPart toplevelEditPart = getGraphicalViewer().getContents();
		NedFileElementEx parent = (NedFileElementEx)toplevelEditPart.getModel();
		//FIXME refine insertion point: maybe before the first selected element's INedTypeElement parent?
		compoundCommand.add(new AddNEDElementCommand(parent, element));
	}

	protected void pasteSubmodule(SubmoduleElementEx submodule, CompoundModuleElementEx targetModule, Map<String, String> submoduleNameMap, CompoundCommand compoundCommand) {
		SubmodulesElement submodulesElement = targetModule.getFirstSubmodulesChild();
		if (submodulesElement == null) {
			submodulesElement = (SubmodulesElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_SUBMODULES);
			submodulesElement.appendChild(submodule);
			compoundCommand.add(new AddNEDElementCommand(targetModule, submodulesElement));
		}
		else {
			if (submodulesElement.getFirstChildWithAttribute(NEDElementTags.NED_SUBMODULE, SubmoduleElementEx.ATT_NAME, submodule.getName()) != null) {
				String newName = "_" + submodule.getName();  // FIXME better/safer way to make it unique!! i.e. regard inherited submods too!
				submoduleNameMap.put(submodule.getName(), newName);
				submodule.setName(newName);
			}
			compoundCommand.add(new AddNEDElementCommand(submodulesElement, submodule));
		}
	}

	protected void pasteConnection(ConnectionElementEx connection, CompoundModuleElementEx targetModule, Map<String, String> submoduleNameMap, CompoundCommand compoundCommand) {
		// adjust src/dest submodule name if needed 
		if (submoduleNameMap.containsKey(connection.getSrcModule()))
			connection.setSrcModule(submoduleNameMap.get(connection.getSrcModule()));
		if (submoduleNameMap.containsKey(connection.getDestModule()))
			connection.setDestModule(submoduleNameMap.get(connection.getDestModule()));

		// insert
		ConnectionsElement connectionsElement = targetModule.getFirstConnectionsChild();
		if (connectionsElement == null) {
			connectionsElement = (ConnectionsElement) NEDElementFactoryEx.getInstance().createElement(NEDElementTags.NED_CONNECTIONS);
			connectionsElement.appendChild(connection);
			compoundCommand.add(new AddNEDElementCommand(targetModule, connectionsElement));
		}
		else {
			compoundCommand.add(new AddNEDElementCommand(connectionsElement, connection));
		}
		
		//FIXME verify if src/dest submodule exists in the compound module?
	}

	@SuppressWarnings("unchecked")
	protected CompoundModuleElementEx getTargetCompoundModule() {
		// return the selected compound module or the compound module of the first selected submodule
		GraphicalViewer graphicalViewer = getGraphicalViewer();
		List selectedEditParts = graphicalViewer.getSelectedEditParts();
		for (Object editPart : selectedEditParts)
			if (editPart instanceof ModuleEditPart) 
				return ((ModuleEditPart)editPart).getCompoundModulePart().getCompoundModuleModel();

		// or, return the first compound module in the file; in the worst case return null
		EditPart toplevelEditPart = graphicalViewer.getContents();
		NedFileElementEx nedFileElement = (NedFileElementEx)toplevelEditPart.getModel();
		return (CompoundModuleElementEx) nedFileElement.getFirstCompoundModuleChild(); // might be null
	}

	protected GraphicalViewer getGraphicalViewer() {
		GraphicalViewer viewer = (GraphicalViewer)getWorkbenchPart().getAdapter(GraphicalViewer.class);
		return viewer;
	}
}

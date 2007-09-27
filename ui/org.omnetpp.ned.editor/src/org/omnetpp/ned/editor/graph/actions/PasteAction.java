package org.omnetpp.ned.editor.graph.actions;

import static org.omnetpp.ned.model.pojo.NEDElementTags.NED_CONNECTIONS;
import static org.omnetpp.ned.model.pojo.NEDElementTags.NED_GATES;
import static org.omnetpp.ned.model.pojo.NEDElementTags.NED_PARAMETERS;
import static org.omnetpp.ned.model.pojo.NEDElementTags.NED_SUBMODULES;
import static org.omnetpp.ned.model.pojo.NEDElementTags.NED_TYPES;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IProject;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalViewer;
import org.eclipse.gef.commands.CompoundCommand;
import org.eclipse.gef.ui.actions.Clipboard;
import org.eclipse.gef.ui.actions.SelectionAction;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.actions.ActionFactory;

import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.commands.AddNEDElementCommand;
import org.omnetpp.ned.editor.graph.edit.ModuleEditPart;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.NEDElementFactoryEx;
import org.omnetpp.ned.model.ex.NEDElementUtilEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IHasGates;
import org.omnetpp.ned.model.interfaces.IHasName;
import org.omnetpp.ned.model.interfaces.IHasParameters;
import org.omnetpp.ned.model.interfaces.IModelProvider;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.pojo.ConnectionsElement;
import org.omnetpp.ned.model.pojo.GateElement;
import org.omnetpp.ned.model.pojo.GatesElement;
import org.omnetpp.ned.model.pojo.ParamElement;
import org.omnetpp.ned.model.pojo.ParametersElement;
import org.omnetpp.ned.model.pojo.PropertyElement;
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
		Object contents = Clipboard.getDefault().getContents();
		return contents instanceof INEDElement[];
	}

	@Override @SuppressWarnings("unchecked")
	public void run() {
		Object contents = Clipboard.getDefault().getContents();
		if (!(contents instanceof INEDElement[]))
			return;

		// we'll paste a *duplicate* of the elements on the clipboard
		List<INEDElement> elements = new ArrayList<INEDElement>();
		for (INEDElement element : (INEDElement[])contents)
			elements.add(element.deepDup());

		// sort the collection so named elements will be pasted in dictionary
		// order, so that numbering after renaming the duplicates will be consistent
		// with the original order (i.e. pasting node1, node2, node3 will result in
		// node4, node5, node6 in *that* order)
		Collections.sort(elements, new Comparator<INEDElement>() {
			public int compare(INEDElement o1, INEDElement o2) {
				// do named ones in dictionary order
				if (o1 instanceof IHasName && o2 instanceof IHasName)
					return StringUtils.dictionaryCompare(((IHasName)o1).getName(), ((IHasName)o2).getName());
				if (o1 instanceof IHasName)
					return 1;
				if (o2 instanceof IHasName)
					return -1;
				return 0;
			}
		});

		// assemble compound command which effectively does the paste
		CompoundCommand compoundCommand = new CompoundCommand();
		List<INEDElement> pastedElements = new ArrayList<INEDElement>();
		pasteNedTypes(elements, compoundCommand, pastedElements);
		pasteSubmodulesAndConnections(elements, compoundCommand, pastedElements);
		pasteParametersAndProperties(elements, compoundCommand, pastedElements);
		pasteGates(elements, compoundCommand, pastedElements);

		// warn for elements that could not be pasted
		elements.removeAll(pastedElements);
		if (elements.size() > 0) {
			System.out.println("don't know how to paste: " + StringUtils.join(elements, ", "));
			Display.getCurrent().beep();
		}

		// execute the command
		compoundCommand.setLabel("Paste " + StringUtils.formatCounted(pastedElements.size(), "object"));
		execute(compoundCommand);

		// select newly pasted stuff in the editor
		GraphicalViewer graphicalViewer = getGraphicalViewer();
		graphicalViewer.getRootEditPart().refresh();
		getGraphicalViewer().deselectAll();
		Map<INEDElement,EditPart> editPartRegistry = graphicalViewer.getEditPartRegistry();
		for (INEDElement element : pastedElements) {
			EditPart editPart = editPartRegistry.get(element);
			if (editPart != null)
				graphicalViewer.appendSelection(editPart);
		}
	}

	protected void pasteNedTypes(List<INEDElement> elements, CompoundCommand compoundCommand, List<INEDElement> pastedElements) {
		boolean containsNedType = false;
		for (INEDElement element : elements)
			if (element instanceof INedTypeElement)
				containsNedType = true;
		if (!containsNedType)
			return;

		INEDElement parent;
		INEDElement beforeElement;
		String namePrefix;
		Set<String> usedNedTypeNames = new HashSet<String>();

		// find insertion point
		INEDElement primarySelectionElement = getPrimarySelectionElement();
		if (primarySelectionElement instanceof CompoundModuleElementEx) {
			// paste as inner type
			CompoundModuleElementEx compoundModule = (CompoundModuleElementEx)primarySelectionElement;
			parent = findOrCreateSection(compoundModule, NED_TYPES, compoundCommand);
			beforeElement = null;  // =append
			namePrefix = "";
			usedNedTypeNames.addAll(compoundModule.getNEDTypeInfo().getInnerTypes().keySet());
		}
		else {
			// paste as toplevel type into the file
			EditPart toplevelEditPart = getGraphicalViewer().getContents();
			parent = (INEDElement) toplevelEditPart.getModel();
			beforeElement = (primarySelectionElement != null && primarySelectionElement.getParent() == parent) ? primarySelectionElement : null;
            IProject project = NEDResourcesPlugin.getNEDResources().getNedFile(parent.getContainingNedFileElement()).getProject();
            namePrefix = parent.getContainingNedFileElement().getQNameAsPrefix();
			usedNedTypeNames.addAll(NEDResourcesPlugin.getNEDResources().getReservedQNames(project));
		}

		// insert stuff into parent
		for (INEDElement element : elements) {
			if (element instanceof INedTypeElement) {
				INedTypeElement typeElement = (INedTypeElement) element;

				// generate unique name
				String newName = NEDElementUtilEx.getUniqueNameFor(namePrefix + typeElement.getName(), usedNedTypeNames);
				typeElement.setName(newName.contains(".") ? StringUtils.substringAfterLast(newName, ".") : newName);
				usedNedTypeNames.add(newName);

				// paste it
				compoundCommand.add(new AddNEDElementCommand(parent, typeElement, beforeElement));
				pastedElements.add(typeElement);
			}
		}
	}

	protected void pasteSubmodulesAndConnections(List<INEDElement> elements, CompoundCommand compoundCommand, List<INEDElement> pastedElements) {
		CompoundModuleElementEx targetModule = getTargetCompoundModule();
		if (targetModule == null)
			return;

		Set<String> usedSubmoduleNames = new HashSet<String>();
		usedSubmoduleNames.addAll(targetModule.getNEDTypeInfo().getSubmodules().keySet());
		Map<String, String> submoduleNameMap = new HashMap<String, String>();

		// paste submodules
		SubmodulesElement submodulesSection = null;
		for (INEDElement element : elements) {
			if (element instanceof SubmoduleElementEx) {
				SubmoduleElementEx submodule = (SubmoduleElementEx) element;

				// generate unique name if needed
				String newName = NEDElementUtilEx.getUniqueNameFor(submodule.getName(), usedSubmoduleNames);
				usedSubmoduleNames.add(newName);
				if (!newName.equals(submodule.getName())) {
					submoduleNameMap.put(submodule.getName(), newName);
					submodule.setName(newName);
				}

				// insert
				if (submodulesSection == null)
					submodulesSection = (SubmodulesElement) findOrCreateSection(targetModule, NED_SUBMODULES, compoundCommand);
				compoundCommand.add(new AddNEDElementCommand(submodulesSection, submodule));
				pastedElements.add(submodule);
			}
		}

		// paste connections
		ConnectionsElement connectionsSection = null;
		for (INEDElement element : elements) {
			if (element instanceof ConnectionElementEx) {
				ConnectionElementEx connection = (ConnectionElementEx)element;

				// adjust src/dest submodule name if needed
				if (submoduleNameMap.containsKey(connection.getSrcModule()))
					connection.setSrcModule(submoduleNameMap.get(connection.getSrcModule()));
				if (submoduleNameMap.containsKey(connection.getDestModule()))
					connection.setDestModule(submoduleNameMap.get(connection.getDestModule()));

				// insert
				if (connectionsSection == null)
					connectionsSection = (ConnectionsElement) findOrCreateSection(targetModule, NED_CONNECTIONS, compoundCommand);
				compoundCommand.add(new AddNEDElementCommand(connectionsSection, connection));
				pastedElements.add(connection);
			}
		}
	}

	protected void pasteParametersAndProperties(List<INEDElement> elements, CompoundCommand compoundCommand, List<INEDElement> pastedElements) {
		INEDElement targetElement = getPrimarySelectionElement();
		if (targetElement == null || !(targetElement instanceof IHasParameters))
			return;

		ParametersElement parametersSection = null;
		for (INEDElement element : elements) {
			if (element instanceof ParamElement || element instanceof PropertyElement) {
				// insert
				if (parametersSection == null)
					parametersSection = (ParametersElement) findOrCreateSection(targetElement, NED_PARAMETERS, compoundCommand);
				compoundCommand.add(new AddNEDElementCommand(parametersSection, element));
				pastedElements.add(element);
			}
		}
	}

	protected void pasteGates(List<INEDElement> elements, CompoundCommand compoundCommand, List<INEDElement> pastedElements) {
		INEDElement targetElement = getPrimarySelectionElement();
		if (targetElement == null || !(targetElement instanceof IHasGates))
			return;

		GatesElement gatesSection = null;
		for (INEDElement element : elements) {
			if (element instanceof GateElement) {
				// insert
				if (gatesSection == null)
					gatesSection = (GatesElement) findOrCreateSection(targetElement, NED_GATES, compoundCommand);
				compoundCommand.add(new AddNEDElementCommand(gatesSection, element));
				pastedElements.add(element);
			}
		}
	}

	protected INEDElement findOrCreateSection(INEDElement parent, int tagcode, CompoundCommand compoundCommand) {
		INEDElement sectionElement = parent.getFirstChildWithTag(tagcode);
		if (sectionElement == null) {
			sectionElement = NEDElementFactoryEx.getInstance().createElement(tagcode);
			compoundCommand.add(new AddNEDElementCommand(parent, sectionElement));
		}
		return sectionElement;
	}

	@SuppressWarnings("unchecked")
	protected CompoundModuleElementEx getTargetCompoundModule() {
		// return the selected compound module or the compound module of the first selected submodule
		GraphicalViewer graphicalViewer = getGraphicalViewer();
		List selectedEditParts = graphicalViewer.getSelectedEditParts();
		for (Object editPart : selectedEditParts)
			if (editPart instanceof ModuleEditPart)
				return ((ModuleEditPart)editPart).getCompoundModulePart().getCompoundModuleModel();
		return null;
	}

	@SuppressWarnings("unchecked")
	protected INEDElement getPrimarySelectionElement() {
		// return the element from the primary selection
		GraphicalViewer graphicalViewer = getGraphicalViewer();
		List<EditPart> selectedEditParts = graphicalViewer.getSelectedEditParts();
		if (selectedEditParts.size() > 0 && selectedEditParts.get(0) instanceof IModelProvider)
			return ((IModelProvider)selectedEditParts.get(0)).getNedModel();
		else
			return null;
	}

	protected GraphicalViewer getGraphicalViewer() {
		GraphicalViewer viewer = (GraphicalViewer)getWorkbenchPart().getAdapter(GraphicalViewer.class);
		return viewer;
	}
}

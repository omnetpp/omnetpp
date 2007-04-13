package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;

/**
 * Inifile editor page to manage the sections in the file.
 * 
 * @author Andras
 */
//XXX let the user edit description= as well
//XXX show description= value as tooltip
//XXX double-click should go there in the inifile text (or in the Parameters page?)
public class SectionsPage extends FormPage {
	public static final String ICON_ERROR = "icons/full/obj16/Error.png"; //XXX find better place for it
	private TreeViewer treeViewer;
	
	static class SectionData {
		String sectionName;
		boolean hasError;
		
		public SectionData(String sectionName, boolean isUndefined) {
			this.sectionName = sectionName;
			this.hasError = isUndefined;
		}
		
		@Override
		public String toString() {
			return sectionName + (hasError ? " (base section does not exist)" : ""); //XXX not very elegant
		}
	}
	
	public SectionsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		
		// layout: 2x2 grid: (label, dummy) / (tree, buttonGroup)
		setLayout(new GridLayout(2,false));
		((GridLayout)getLayout()).marginRight = RIGHT_MARGIN/2;

		Composite titleArea = createTitleArea(this, "Sections");
		((GridData)titleArea.getLayoutData()).horizontalSpan = 2;

		// create section chain label
		Label label = new Label(this, SWT.NONE);
		label.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		((GridData)label.getLayoutData()).horizontalSpan = 2;
		label.setText("\nAdd/remove configuration sections, and edit the fallback sequence of parameter and configuration lookups.");

		treeViewer = createAndConfigureTreeViewer();
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		Composite buttonGroup = createButtons();
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));
		
		reread();
	}

	private TreeViewer createAndConfigureTreeViewer() {
		TreeViewer treeViewer = new TreeViewer(this, SWT.MULTI | SWT.BORDER);
		treeViewer.setLabelProvider(new GenericTreeLabelProvider(new LabelProvider() {
			@Override
			public Image getImage(Object element) {
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				if (element instanceof SectionData) {
					SectionData payload = (SectionData) element;
					return payload.hasError ? InifileEditorPlugin.getImage(ICON_ERROR) : ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER);
				}
				return null;
			}
		}));
		treeViewer.setContentProvider(new GenericTreeContentProvider());
		setupDragAndDropSupport(treeViewer);
		return treeViewer;
	}

	private void setupDragAndDropSupport(TreeViewer viewer) {
		int dndOperations = DND.DROP_COPY | DND.DROP_MOVE | DND.DROP_LINK;
		Transfer[] transfers = new Transfer[] { LocalTransfer.getInstance() };
		viewer.addDragSupport(dndOperations, transfers, new ViewerDragAdapter(viewer));
		viewer.addDropSupport(dndOperations, transfers, new DropTargetAdapter() {
			public void drop(DropTargetEvent event) {
				// note: in theory, the user can drag across different treeViewers
				// (i.e. from a different inifile editor, or even from a Scave editor!), 
				// so we have to be careful when looking at the dragged data 
				String[] draggedSections = getSectionNamesFromTreeSelection((IStructuredSelection) event.data);
				String targetSectionName = getSectionNameFromTreeNode(event.item==null ? null : event.item.getData());
				
				if (draggedSections.length != 0 && targetSectionName != null) {
			    	//System.out.println(draggedSections.length + " items dropped to: "+targetSectionName);
					IInifileDocument doc = getInifileDocument();
			    	for (String draggedSectionName : draggedSections)
			    		if (getInifileDocument().containsSection(draggedSectionName)) // might occur if it was dragged from a different editor's treeviewer...
			    			if (!InifileUtils.sectionChainContains(doc, targetSectionName, draggedSectionName)) // avoid circles
			    				setSectionExtendsKey(draggedSectionName, targetSectionName);
			    	reread();
			    }
			}
		});
	}

	@SuppressWarnings("unchecked")
	protected static String[] getSectionNamesFromTreeSelection(ISelection selection) {
		ArrayList list = new ArrayList();
		if (selection instanceof IStructuredSelection) {
			for (Object item : ((IStructuredSelection)selection).toArray()) {
				String payload = getSectionNameFromTreeNode(item);
				if (payload != null)
					list.add(payload);
			}
		}
		return (String[]) list.toArray(new String[]{});
	}

	protected static String getSectionNameFromTreeNode(Object data) {
		if (data instanceof GenericTreeNode)
			data = ((GenericTreeNode)data).getPayload();
		if (data instanceof SectionData)
				return ((SectionData) data).sectionName;
		return null;
	}
	
	protected void setSectionExtendsKey(String sectionName, String extendsSectionName) {
		IInifileDocument doc = getInifileDocument();
		if (extendsSectionName.equals(GENERAL))
			doc.removeKey(sectionName, "extends");
		else {
			String value = extendsSectionName.replaceAll("^Config +", "");
			if (doc.containsKey(sectionName, "extends"))
				doc.setValue(sectionName, "extends", value);
			else
				InifileUtils.addEntry(doc, sectionName, "extends", value, null);
		}
	}

	private Composite createButtons() {
		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayout(new GridLayout(1,false));
		
		Button addButton = createButton(buttonGroup, "Add...");
		Button renameButton = createButton(buttonGroup, "Rename...");
		Button removeButton = createButton(buttonGroup, "Remove");
		
		// in more than one places we'll need to check whether a string is acceptable as a new section's name
		final IInputValidator newSectionNameValidator = new IInputValidator() {
			public String isValid(String sectionName) {
				sectionName = sectionName.trim();
				if (!sectionName.equals(GENERAL) && !sectionName.startsWith("Config "))
					sectionName = "Config "+sectionName;
				if (getInifileDocument().containsSection(sectionName))
					return "Section ["+sectionName+"] already exists";
				return null;
			}
		};

		// configure "add section" button
		addButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				InputDialog dialog = new InputDialog(getShell(), "New Section", "Name for the new section:", "new", newSectionNameValidator);
				if (dialog.open()==Window.OK) {
					String sectionName = dialog.getValue().trim();
					if (!sectionName.equals(GENERAL) && !sectionName.startsWith("Config "))
						sectionName = "Config "+sectionName;
					InifileUtils.addSection(getInifileDocument(), sectionName);
					String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
					if (selection.length != 0)
						setSectionExtendsKey(sectionName, selection[0]);
					reread();
				}
			}
		});

		// configure "remove section" button
		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
				if (selection.length != 0) {
					for (String sectionName : selection) {
						getInifileDocument().removeSection(sectionName);
					}
					reread();
				}
			}
		});

		// configure "rename section" button
		renameButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				String[] selection = getSectionNamesFromTreeSelection(treeViewer.getSelection());
				if (selection.length == 0)
					return;
				String sectionName = selection[0];
				InputDialog dialog = new InputDialog(getShell(), "Rename Section", "New name for section ["+sectionName+"]:", sectionName.replaceFirst("^Config +", "")+"-1", newSectionNameValidator);
				if (dialog.open()==Window.OK) {
					String newSectionName = dialog.getValue().trim();
					if (!newSectionName.equals(GENERAL) && !newSectionName.startsWith("Config "))
						newSectionName = "Config "+newSectionName;
					InifileUtils.renameSection(getInifileDocument(), sectionName, newSectionName);
					reread();
				}
			}
		});

		return buttonGroup;
	}

	private Button createButton(Composite buttonGroup, String label) {
		Button button = new Button(buttonGroup, SWT.PUSH);
		button.setText(label);
		button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		return button;
	}
	
	@Override
	public void reread() {
		IInifileDocument doc = getInifileDocument();

		// create root node
		GenericTreeNode rootNode = new GenericTreeNode("root");
		GenericTreeNode generalSectionNode = new GenericTreeNode(new SectionData(GENERAL, false));
		rootNode.addChild(generalSectionNode);

		//FIXME detect circularity!!!!!
		
		// build tree
		HashMap<String,GenericTreeNode> nodes = new HashMap<String, GenericTreeNode>();
		for (String sectionName : doc.getSectionNames()) {
			if (sectionName.startsWith("Config ")) {
				GenericTreeNode node = getOrCreate(nodes, sectionName, false);
				String extendsName = doc.getValue(sectionName, "extends");
				if (extendsName == null) {
					// no "extends=...": falls back to [General]
					generalSectionNode.addChild(node);
				}
				else {
					// add as child to the section it extends
					String extendsSectionName = "Config "+extendsName;
					if (doc.containsSection(extendsSectionName)) {
						GenericTreeNode extendsSectionNode = getOrCreate(nodes, extendsSectionName, false);
						extendsSectionNode.addChild(node);
					}
					else {
						// "extends=...": is bogus, fall back to [General]
						((SectionData)(node.getPayload())).hasError = true;
						generalSectionNode.addChild(node);
					}
				}
			}
		}
		treeViewer.setInput(rootNode);
		treeViewer.expandAll();
	}

	private GenericTreeNode getOrCreate(HashMap<String, GenericTreeNode> nodes, String sectionName, boolean isUndefined) {
		GenericTreeNode node = nodes.get(sectionName);
		if (node==null) {
			node = new GenericTreeNode(new SectionData(sectionName, isUndefined));
			nodes.put(sectionName, node);
		}
		return node;
	}

	@Override
	public void commit() {
		//TODO
	}
}

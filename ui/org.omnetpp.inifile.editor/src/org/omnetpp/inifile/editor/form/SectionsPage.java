package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.GenericTreeUtils;
import org.omnetpp.common.ui.ITooltipProvider;
import org.omnetpp.common.ui.TooltipSupport;
import org.omnetpp.inifile.editor.IGotoInifile;
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
//XXX enable/disable buttons as tree selection changes
public class SectionsPage extends FormPage {
	private static final String ICON_ERROR = "icons/full/obj16/Error.png"; //XXX find better place for it
	private static final String CIRCLE_WARNING_TEXT = "NOTE: Sections that form circles (which is illegal) are not displayed here -- switch to text mode to fix them!";
	private static final String HINT_TEXT = "\nDrag&&drop sections to edit the fallback sequence of parameter and config lookups, or to add/remove/edit sections.";

	private Label label;
	private TreeViewer treeViewer;
	
	static class SectionData {
		String sectionName;
		boolean hasError;
		
		public SectionData(String sectionName, boolean isUndefined) {
			this.sectionName = sectionName;
			this.hasError = isUndefined;
		}
		
		/* label provider maps to this */
		@Override
		public String toString() {
			return sectionName + (hasError ? " (the section it extends does not exist)" : "");
		}
		
		/* needed for treeViewer.setSelection() to work */
		@Override
		public boolean equals(Object obj) {
			if (obj==null || getClass()!=obj.getClass())
				return false;
			return sectionName.equals(((SectionData)obj).sectionName);
		}
	}
	
	public SectionsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		
		// layout: 2x2 grid: (label, dummy) / (tree, buttonGroup)
		setLayout(new GridLayout(2,false));
		((GridLayout)getLayout()).marginRight = RIGHT_MARGIN/2;

		// create title
		Composite titleArea = createTitleArea(this, "Sections");
		((GridData)titleArea.getLayoutData()).horizontalSpan = 2;

		// create section chain label
		label = new Label(this, SWT.NONE);
		label.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		((GridData)label.getLayoutData()).horizontalSpan = 2;
		label.setText(HINT_TEXT);
		
		// create treeviewer and buttons
		treeViewer = createAndConfigureTreeViewer();
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		Composite buttonGroup = createButtons();
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));
		
		reread();
	}

	private TreeViewer createAndConfigureTreeViewer() {
		final TreeViewer treeViewer = new TreeViewer(this, SWT.MULTI | SWT.BORDER);
		treeViewer.setLabelProvider(new GenericTreeLabelProvider(new LabelProvider() {
			@Override
			public String getText(Object element) {
				String section = getSectionNameFromTreeNode(element);
				String description = getInifileDocument().getValue(section, CFGID_DESCRIPTION.getKey());
				return description==null ? section : (section+" -- "+description);
			}
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
		
		// drag and drop support
 		setupDragAndDropSupport(treeViewer);
		
		// on double-click, show section in the text editor
 		treeViewer.getTree().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent event) {
				String section = getSectionNameFromTreeNode(event.item==null ? null : event.item.getData());
				getEditorData().getInifileEditor().gotoSection(section, IGotoInifile.Mode.TEXT);
			}
		});

 		// export the tree's selection as editor selection
 		treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				IStructuredSelection sel = (IStructuredSelection) event.getSelection();
				String section = getSectionNameFromTreeNode(sel.getFirstElement());
				if (section!=null)
					setEditorSelection(section, null);
			}
 		});

 		// add tooltip support
 		TooltipSupport.adapt(treeViewer.getTree(), new ITooltipProvider() {
			public String getTooltipFor(Control control, int x, int y) {
				Item item = treeViewer.getTree().getItem(new Point(x,y));
				String section = getSectionNameFromTreeNode(item==null ? null : item.getData());
				return section==null ? null : InifileUtils.getSectionTooltip(section, getInifileDocument(), getInifileAnalyzer());
			}
 		});
 		
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
				if (sectionName.equals(""))
					return "Section name cannot be empty";
				if (!sectionName.equals(GENERAL) && !sectionName.startsWith(CONFIG_))
					sectionName = CONFIG_+sectionName;
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
					if (!sectionName.equals(GENERAL) && !sectionName.startsWith(CONFIG_))
						sectionName = CONFIG_+sectionName;
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
				if (!sectionName.equals(GENERAL)) { // [General] cannot be renamed
					InputDialog dialog = new InputDialog(getShell(), "Rename Section", "New name for section ["+sectionName+"]:", sectionName.replaceFirst("^Config +", "")+"-1", newSectionNameValidator);
					if (dialog.open()==Window.OK) {
						String newSectionName = dialog.getValue().trim();
						if (!newSectionName.equals(GENERAL) && !newSectionName.startsWith(CONFIG_))
							newSectionName = CONFIG_+newSectionName;
						InifileUtils.renameSection(getInifileDocument(), sectionName, newSectionName);
						reread();
					}
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
	public String getPageCategory() {
		return InifileFormEditor.SECTIONS_PAGE;
	}

	@Override
	public void reread() {
		IInifileDocument doc = getInifileDocument();

		// create root node
		GenericTreeNode rootNode = new GenericTreeNode("root");
		GenericTreeNode generalSectionNode = new GenericTreeNode(new SectionData(GENERAL, false));
		rootNode.addChild(generalSectionNode);

		// handling circles: they won't appear in the tree (property of the tree
		// building algorithm), and we warn the user (in the label text, see below)
		label.setText(!getInifileAnalyzer().containsSectionCircles() ? HINT_TEXT : HINT_TEXT + "\n" + CIRCLE_WARNING_TEXT);
		layout(true);  // number of lines in label may have changed
		
		// build tree
		HashMap<String,GenericTreeNode> nodes = new HashMap<String, GenericTreeNode>();
		for (String sectionName : doc.getSectionNames()) {
			if (sectionName.startsWith(CONFIG_)) {
				GenericTreeNode node = getOrCreate(nodes, sectionName, false);
				String extendsName = doc.getValue(sectionName, "extends");
				if (extendsName == null) {
					// no "extends=...": falls back to [General]
					generalSectionNode.addChild(node);
				}
				else {
					// add as child to the section it extends
					String extendsSectionName = CONFIG_+extendsName;
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

		// reduce flicker: only overwrite existing tree input if it's not the same as this one
		if (treeViewer.getInput()==null || !GenericTreeUtils.treeEquals(rootNode, (GenericTreeNode)treeViewer.getInput())) {
			treeViewer.setInput(rootNode);
			treeViewer.expandAll();
		}
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
		// nothing to do
	}

	@Override
	public void gotoSection(String section) {
		treeViewer.setSelection(new StructuredSelection(new GenericTreeNode(new SectionData(section, false))));
	}

	@Override
	public void gotoEntry(String section, String key) {
		gotoSection(section);
	}

//	protected static Item[] getAllTreeItems(Tree tree) {
//		List<Item> list = new ArrayList<Item>();
//		for (TreeItem item : tree.getItems())
//			collectTreeItems(item, list);
//		return list.toArray(new Item[]{});
//	}
//
//	private static void collectTreeItems(TreeItem item, List<Item> list) {
//		list.add(item);
//		for (TreeItem childItem : item.getItems())
//			collectTreeItems(childItem, list);
//	}

}

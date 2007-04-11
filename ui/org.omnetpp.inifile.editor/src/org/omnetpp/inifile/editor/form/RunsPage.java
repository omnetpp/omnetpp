package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;
import java.util.HashMap;

import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.jface.viewers.TreeViewer;
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

/**
 * Inifile editor page to manage the sections in the file.
 * 
 * @author Andras
 */
public class RunsPage extends FormPage {
	public static final String ICON_ERROR = "icons/full/obj16/Error.png"; //XXX find better place for it
	private TreeViewer treeViewer;
	
	static class Payload {
		String sectionName;
		boolean hasError;
		
		public Payload(String sectionName, boolean isUndefined) {
			this.sectionName = sectionName;
			this.hasError = isUndefined;
		}
		
		@Override
		public String toString() {
			return sectionName + (hasError ? " (base section does not exist)" : ""); //XXX not very elegant
		}
	}
	
	public RunsPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);
		
		// layout: 2x2 grid: (label, dummy) / (tree, buttonGroup)
		setLayout(new GridLayout(2,false));

		Label label = new Label(this, SWT.NONE);
		label.setText("Section Inheritance");
		new Label(this, SWT.NONE); // dummy, for row1col2

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
				if (element instanceof Payload) {
					Payload payload = (Payload) element;
					return payload.hasError ? InifileEditorPlugin.getImage(ICON_ERROR) : ImageFactory.getImage(ImageFactory.MODEL_IMAGE_FOLDER);
				}
				return null; //XXX
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
				Payload[] draggedSections = getPayloadsFrom(event.data);
				Payload targetSection = getPayloadFrom(event.item==null ? null : event.item.getData());

				if (draggedSections.length != 0 && targetSection != null) {
			    	//System.out.println(draggedSections.length + " items dropped to: "+targetSection);
			    	String newExtends = targetSection.sectionName.equals("General") ? null : targetSection.sectionName.replaceFirst("Config +", "");
			    	for (Payload draggedPayload : draggedSections)
			    		setSectionExtendsKey(draggedPayload.sectionName, newExtends);
			    	reread();
			    }
			}

			@SuppressWarnings("unchecked")
			private Payload[] getPayloadsFrom(Object data) {
				if (data instanceof TreeSelection) {
					ArrayList list = new ArrayList();
					for (Object item : ((TreeSelection) data).toArray()) {
						Payload payload = getPayloadFrom(item);
						if (payload != null)
							list.add(payload);
					}
					return (Payload[]) list.toArray(new Payload[]{});
				}
				return null;
			}

			private Payload getPayloadFrom(Object data) {
				if (data instanceof GenericTreeNode) {
					Object payload = ((GenericTreeNode)data).getPayload();
					if (payload instanceof Payload)
						return (Payload) payload;
				}
				return null;
			}
		});
	}
	
	private void setSectionExtendsKey(String sectionName, String newExtends) {
		IInifileDocument doc = getInifileDocument();
		if (newExtends == null)
			doc.removeKey(sectionName, "extends");
		else {
			if (doc.containsKey(sectionName, "extends")) {
				// overwrite
				doc.setValue(sectionName, "extends", newExtends);
			} 
			else {
				// insert at top
				String[] keys = doc.getKeys(sectionName);
				String firstKey = keys.length == 0 ? null : keys[0];
				doc.addEntry(sectionName, "extends", newExtends, "", firstKey);
			}
		}
	}

	private Composite createButtons() {
		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayout(new GridLayout(1,false));
		
		Button addButton = createButton(buttonGroup, "Add...");
		//Button editButton = createButton(buttonGroup, "Edit...");
		Button removeButton = createButton(buttonGroup, "Remove");
		//new Label(buttonGroup, SWT.NONE);
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		
		addButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				//XXX
			}
		});

		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
				for (Object o : sel.toArray()) {
					String sectionName = (String) o;  //XXX not exactly
					getInifileDocument().removeSection(sectionName);
				}
				reread();
			}
		});

		upButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				//XXX
			}
		});

		downButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				//XXX
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
		GenericTreeNode generalSectionNode = new GenericTreeNode(new Payload("General", false));
		rootNode.addChild(generalSectionNode);

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
						((Payload)(node.getPayload())).hasError = true;
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
			node = new GenericTreeNode(new Payload(sectionName, isUndefined));
			nodes.put(sectionName, node);
		}
		return node;
	}

	@Override
	public void commit() {
		//TODO
	}
}

package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_TYPE_NAME;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IMarker;
import org.eclipse.emf.edit.ui.dnd.LocalTransfer;
import org.eclipse.emf.edit.ui.dnd.ViewerDragAdapter;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.DropTargetAdapter;
import org.eclipse.swt.dnd.DropTargetEvent;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.events.KeyAdapter;
import org.eclipse.swt.events.KeyEvent;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.TreeColumn;
import org.omnetpp.common.Debug;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.SizeConstraint;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.actions.AddInifileKeysDialog;
import org.omnetpp.inifile.editor.contentassist.InifileParamKeyContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.SectionKey;

/**
 * For editing module parameters,  and type-name lines.
 *
 * @author Andras
 */
//XXX when adding params here, editor annotations become out of sync with markers
public class ParametersPage extends FormPage {
	private TreeViewer treeViewer;
	private Combo sectionsCombo;
	private Label networkNameLabel;
	private Label sectionChainLabel;
	private Label numUnassignedParamsLabel;
	private Button newButton;
	private Button removeButton;
	private Button addMissingButton;

	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// layout: 3x2 grid: (label, dummy) / (combobox-with-label, dummy) / (table, buttonGroup)
		setLayout(new GridLayout(2,false));
		((GridLayout)getLayout()).marginRight = RIGHT_MARGIN;

		Control titleArea = createTitleArea(this, "Parameter Assignments");
		((GridData)titleArea.getLayoutData()).horizontalSpan = 2;

		// create combobox
		Composite comboWithLabel = new Composite(this, SWT.NONE);
		comboWithLabel.setLayout(new GridLayout(2, false));
		//((GridLayout)comboWithLabel.getLayout()).marginWidth = 0;
		comboWithLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		new Label(this, SWT.NONE);  // dummy, to fill 2nd column

		Label comboLabel = new Label(comboWithLabel, SWT.NONE);
		comboLabel.setText("Configuration:");
		sectionsCombo = new Combo(comboWithLabel, SWT.BORDER | SWT.READ_ONLY);
		comboLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		sectionsCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

		// create network name and section chain labels
		networkNameLabel = createLabel(this, SWT.END, "Network name: n/a");
		sectionChainLabel = createLabel(this, SWT.END, "Section fallback chain: n/a");

		// create section chain label
		Label hintLabel = new Label(this, SWT.NONE);
		hintLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		((GridData)hintLabel.getLayoutData()).horizontalSpan = 2;
		hintLabel.setText("HINT: Drag the icons to change the order of entries.");

		// create table and buttons
		treeViewer = createAndConfigureTable();
		treeViewer.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		Composite buttonGroup = createButtons();
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));

		numUnassignedParamsLabel = createLabel(this, SWT.END, "0 unassigned parameters");

		sectionsCombo.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				// publish section as editor selection
				String selectedSection = sectionsCombo.getText();
				if (getInifileDocument().containsSection(selectedSection)) // because we may have lied that [General] existed
					setEditorSelection(selectedSection, null);

				// refresh table contents, etc.
				reread();
			}
		});

		reread();
	}

	private Label createLabel(Composite parent, int horizontalAlign, String text) {
		Label label = new Label(parent, SWT.NONE);
		label.setLayoutData(new GridData(horizontalAlign, SWT.BEGINNING, true, false));
		label.setText(text);
		new Label(this, SWT.NONE); // dummy, to fill 2nd column of GridLayout
		return label;
	}

	private TreeViewer createAndConfigureTable() {
		Tree tree = new Tree(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
		tree.setLinesVisible(true);
		tree.setHeaderVisible(true);
		addTreeColumn(tree, "Section/Key", 200);
		addTreeColumn(tree, "Value", 140);
		addTreeColumn(tree, "Comment", 80);

		// set up tableViewer, content and label providers
		final TreeViewer treeViewer = new TreeViewer(tree);
		treeViewer.setContentProvider(new GenericTreeContentProvider());
		treeViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
				element = ((GenericTreeNode)element).getPayload();
				if (element instanceof SectionKey) {
					SectionKey item = (SectionKey) element;
					switch (columnIndex) {
					case 0: return item.key;
					case 1: return nullToEmpty(getInifileDocument().getValue(item.section, item.key));
					case 2: return nullToEmpty(getInifileDocument().getComment(item.section, item.key));
					default: throw new IllegalArgumentException();
					}
				}
				else {
					return (columnIndex==0) ? element.toString() : "";
				}
			}
			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				element = ((GenericTreeNode)element).getPayload();
				if (element instanceof SectionKey) {
					if (columnIndex == 0) {
                        SectionKey item = (SectionKey) element;
                        return InifileUtils.getKeyImage(item.section, item.key, getInifileAnalyzer());
					}
					else if (columnIndex == 1) {
						SectionKey item = (SectionKey) element;
						IMarker[] markers = InifileUtils.getProblemMarkersFor(item.section, item.key, getInifileDocument());
						return FieldEditor.getProblemImage(markers, false, true);
					}
				}
				else {
					if (columnIndex==0)
						return InifileUtils.getSectionImage((String)element, getInifileAnalyzer());
				}
				return null;
			}
		});

		// set up hotkey support
		treeViewer.getTree().addKeyListener(new KeyAdapter() {
			public void keyPressed(KeyEvent e) {
				if (e.character == SWT.DEL)
					removeEntries();
				if (e.keyCode == 'A' && (e.stateMask & SWT.MOD1) != 0)
					treeViewer.getTree().selectAll(); //XXX this does not work, because text editor hotkey masks it
			}
		});

		// set up cell editing
		treeViewer.setColumnProperties(new String[] {"key", "value", "comment"});
		final TableTextCellEditor editors[] = new TableTextCellEditor[3];
		editors[0] = new TableTextCellEditor(treeViewer, 0);
		editors[1] = new TableTextCellEditor(treeViewer, 1);
		editors[2] = new TableTextCellEditor(treeViewer, 2);
		treeViewer.setCellEditors(editors);
		treeViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				return element instanceof SectionKey;
			}

			public Object getValue(Object element, String property) {
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				SectionKey item = (SectionKey) element;
				if (property.equals("key"))
					return item.key;
				else if (property.equals("value"))
					return nullToEmpty(getInifileDocument().getValue(item.section, item.key));
				else if (property.equals("comment"))
					return nullToEmpty(getInifileDocument().getComment(item.section, item.key));
				else
					return "-";
			}

			public void modify(Object element, String property, Object value) {
				if (element instanceof Item)
					element = ((Item) element).getData(); // workaround, see super's comment
				if (element instanceof GenericTreeNode)
					element = ((GenericTreeNode)element).getPayload();
				SectionKey item = (SectionKey) element;
				if (item==null)
					return;
				IInifileDocument doc = getInifileDocument();
				try {
					Debug.println("CellEditor committing the "+property+" column, value="+value);
					if (property.equals("key")) {
						String newKey = (String) value;
						if (!newKey.equals(item.key)) {
							if (!newKey.contains("."))
								throw new RuntimeException("Parameter key must contain at least one dot");
                            String suffix = StringUtils.substringAfterLast(newKey, ".");
                            if (suffix.contains("-") && !suffix.equals(CFGID_TYPE_NAME.getKey()))
                                throw new RuntimeException("Key should match a parameter, or end in \".type-name\"");
							// Note: all other validation is done within InifileDocument.changeKey()
							doc.renameKey(item.section, item.key, (String)value);
							reread(); // treeViewer.refresh() not enough, because input consists of keys
						}
					}
					else if (property.equals("value")) {
						if (!value.equals(doc.getValue(item.section, item.key))) {
							doc.setValue(item.section, item.key, (String)value);
							treeViewer.refresh();
						}
					}
					else if (property.equals("comment")) {
					    String comment = (String)value;
					    if (comment.equals(""))
					        comment = null;  // i.e. don't create empty comment, '#' followed by nothing
						if (!StringUtils.equals(comment, doc.getComment(item.section, item.key))) {
							doc.setComment(item.section, item.key, comment);
							treeViewer.refresh();
						}
					}
				}
				catch (RuntimeException e) {
					showErrorDialog(e);
				}
			}
		});

		// content assist for the Key column
		IContentProposalProvider proposalProvider = new InifileParamKeyContentProposalProvider(null, false, getInifileDocument(), getInifileAnalyzer()) {
			@Override
			public IContentProposal[] getProposals(String contents, int position) {
				Object element = ( (IStructuredSelection)treeViewer.getSelection()).getFirstElement();
				if (element instanceof GenericTreeNode)
				    element = ((GenericTreeNode)element).getPayload();
				if (element == null)
				    return new IContentProposal[0];
                SectionKey e = (SectionKey)element;
				configure(e.section, false); // set context for proposal calculation
				return super.getProposals(contents, position);
			}
		};
		ContentAssistUtil.configureTableColumnContentAssist(treeViewer, 0, proposalProvider, new char[]{'.'}, false);

		// content assist for the Value column
		IContentProposalProvider valueProposalProvider = new InifileValueContentProposalProvider(null, null, getInifileDocument(), getInifileAnalyzer(), false) {
			@Override
			public IContentProposal[] getProposals(String contents, int position) {
			    Object element = ( (IStructuredSelection)treeViewer.getSelection()).getFirstElement();
                if (element instanceof GenericTreeNode)
                    element = ((GenericTreeNode)element).getPayload();
                if (element == null)
                    return new IContentProposal[0];
                SectionKey e = (SectionKey)element;
				setInifileEntry(e.section, e.key); // set context for proposal calculation
				return super.getProposals(contents, position);
			}
		};
		ContentAssistUtil.configureTableColumnContentAssist(treeViewer, 1, valueProposalProvider, null, true);

		// on double-click, show entry in the text editor (not a good idea, commented out)
		//tableViewer.getTable().addSelectionListener(new SelectionAdapter() {
		//	@Override
		//	public void widgetDefaultSelected(SelectionEvent event) {
		//		SectionKey entry = (SectionKey) (event.item==null ? null : event.item.getData());
		//		getEditorData().getInifileEditor().gotoEntry(entry.section, entry.key, IGotoInifile.Mode.TEXT);
		//	}
		//});

		// export the table's selection as editor selection
		treeViewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				IStructuredSelection sel = (IStructuredSelection) event.getSelection();
				Object element =  sel.getFirstElement();
				element = element!=null ? ((GenericTreeNode)element).getPayload() : null;
				if (element instanceof SectionKey) {
					SectionKey entry = (SectionKey) element;
					setEditorSelection(entry.section, entry.key);
				}
				else if (element!=null) {
					setEditorSelection(element.toString(), null);
				}
				updateButtonStates();
			}
		});

		// add tooltip support
		addTooltipSupport(treeViewer.getTree(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, SizeConstraint outSizeConstraint) {
				Item item = treeViewer.getTree().getItem(new Point(x,y));
				Object element = item==null ? null : item.getData();
				element = element!=null ? ((GenericTreeNode)element).getPayload() : null;
				if (element instanceof SectionKey) {
					SectionKey entry = (SectionKey) element;
					return InifileHoverUtils.getEntryHoverText(entry.section, entry.key, getInifileDocument(), getInifileAnalyzer());
				}
				else if (element!=null) {
					return InifileHoverUtils.getSectionHoverText(element.toString(), getInifileDocument(), getInifileAnalyzer(), false);
				}
				return null;
			}
		});

		// drag and drop support
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
				SectionKey[] draggedEntries = getEntriesFromTreeSelection((IStructuredSelection) event.data);
				Object target = event.item==null ? null : event.item.getData();
				if (target instanceof GenericTreeNode)
					target = ((GenericTreeNode)target).getPayload();

				if (draggedEntries.length != 0 && target != null) {
					entriesDragged(draggedEntries, target);
				}
			}
		});
	}

	@SuppressWarnings("unchecked")
	protected static SectionKey[] getEntriesFromTreeSelection(ISelection selection) {
		ArrayList list = new ArrayList();
		if (selection instanceof IStructuredSelection) {
			for (Object item : ((IStructuredSelection)selection).toArray()) {
				SectionKey payload = getEntryFromTreeNode(item);
				if (payload != null)
					list.add(payload);
			}
		}
		return (SectionKey[]) list.toArray(new SectionKey[]{});
	}

	protected static SectionKey getEntryFromTreeNode(Object data) {
		if (data instanceof GenericTreeNode)
			data = ((GenericTreeNode)data).getPayload();
		if (data instanceof SectionKey)
			return (SectionKey) data;
		return null;
	}

	/**
	 * Invoked when the user selects a few sections, and drags them to another section.
	 */
	protected void entriesDragged(SectionKey[] draggedEntries, Object target) {
		try {
			Debug.println(draggedEntries.length + " items dropped to: "+target);
			IInifileDocument doc = getInifileDocument();

			int n = draggedEntries.length;
			String[] sections = new String[n];
			String[] keys = new String[n];
			String[] values = new String[n];
			String[] rawComments = new String[n];
			for (int i=0; i<n; i++) {
				sections[i] = draggedEntries[i].section;
				keys[i] = draggedEntries[i].key;
				values[i] = doc.getValue(draggedEntries[i].section, draggedEntries[i].key);
				rawComments[i] = doc.getRawComment(draggedEntries[i].section, draggedEntries[i].key);
			}

			doc.removeKeys(sections, keys);

			String section = target instanceof SectionKey ? ((SectionKey)target).section : (String)target;
			String beforeKey = target instanceof SectionKey ? ((SectionKey)target).key : null;
			// doc.getKeys(section).length>0 ? doc.getKeys(section)[0] : null;
			doc.addEntries(section, keys, values, rawComments, beforeKey);
			reread();
		}
		catch (RuntimeException e) {
			showErrorDialog(e);
		}
	}

	private static String nullToEmpty(String text) {
		return text == null ? "" : text;
	}

	protected static boolean nullSafeEquals(String first, String second) {
		return first==null ? second == null : first.equals(second);
	}

	private TreeColumn addTreeColumn(Tree tree, String label, int width) {
		TreeColumn column = new TreeColumn(tree, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
	}

	private Composite createButtons() {
		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayout(new GridLayout(1,false));

		newButton = createButton(buttonGroup, "New");
		removeButton = createButton(buttonGroup, "Remove");
		new Label(buttonGroup, SWT.NONE);
		addMissingButton = createButton(buttonGroup, "Add...");
		addMissingButton.setImage(InifileEditorPlugin.getCachedImage("icons/full/etool16/genkeys.png"));
		addMissingButton.setToolTipText("Add entries for unassigned parameters");

		newButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				addEntry();
			}
		});

		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				removeEntries();
			}
		});

		addMissingButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				addMissingKeys();
			}
		});

		return buttonGroup;
	}

	protected Button createButton(Composite buttonGroup, String label) {
		Button button = new Button(buttonGroup, SWT.PUSH);
		button.setText(label);
		button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, false));
		return button;
	}

	protected void updateButtonStates() {
		IStructuredSelection treeSelection = (IStructuredSelection) treeViewer.getSelection();
		String selectedSection = sectionsCombo.getText();
		removeButton.setEnabled(getEntriesFromTreeSelection(treeSelection).length>0);
		newButton.setEnabled(!treeSelection.isEmpty() || !"".equals(selectedSection));
	}

	protected void addEntry() {
		treeViewer.cancelEditing();

		// determine where to insert new element
		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		Object o = sel.getFirstElement();
		if (o instanceof GenericTreeNode)
			o = ((GenericTreeNode)o).getPayload();
		String section, beforeKey;
		if (o instanceof SectionKey) {
			SectionKey beforeSectionKey = (SectionKey) o;
			section = beforeSectionKey.section;
			beforeKey = beforeSectionKey.key;
		}
		else if (o instanceof String) { // a section is selected
			section = (String) o;
			beforeKey = null;
		}
		else {
			section = sectionsCombo.getText();
			beforeKey = null;
		}

		// generate unique key
		IInifileDocument doc = getInifileDocument();
		String newKey = "**.newKey";
		if (doc.containsKey(section, newKey)) {
			int i = 1;
			while (doc.containsKey(section, newKey+i)) i++;
			newKey = newKey+i;
		}

		try {
			// insert key and refresh table
			//XXX if all keys are from a readonly base section, one cannot add new keys AT ALL !!!!
			doc.addEntry(section, newKey, "", "", beforeKey);
			reread();
			GenericTreeNode newNode = new GenericTreeNode(new SectionKey(section, newKey));
            treeViewer.setSelection(new StructuredSelection(newNode), true);
			treeViewer.getTree().setFocus();
			treeViewer.editElement(newNode, 1);
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void removeEntries() {
		treeViewer.cancelEditing();

		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		try {
			for (Object o : sel.toArray()) {
				if (o instanceof GenericTreeNode)
					o = ((GenericTreeNode)o).getPayload();
				if (o instanceof SectionKey) {
					SectionKey item = (SectionKey) o;
					getInifileDocument().removeKey(item.section, item.key);
				}
			}
			treeViewer.getTree().deselectAll();
			reread();
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void addMissingKeys() {
		String selectedSection = sectionsCombo.getText();
		InifileAnalyzer analyzer = getInifileAnalyzer();
		IInifileDocument doc = analyzer.getDocument();

		// open the dialog
		AddInifileKeysDialog dialog = new AddInifileKeysDialog(getShell(), analyzer, selectedSection);
		if (dialog.open()==Dialog.OK) {
			// save selection
			IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();

			// add user-selected keys to the document
			String[] keys = dialog.getKeys();
			String section = dialog.getSection();
			try {
				doc.addEntries(section, keys, null, null, null);

				// refresh table and restore selection
				reread();
			}
			catch (RuntimeException ex) {
				showErrorDialog(ex);
			}
			treeViewer.setSelection(sel);
		}
	}

	@Override
	public void gotoSection(String section) {
		sectionsCombo.setText(section);
		reread(); // refresh page contents
	}

	@Override
	public void gotoEntry(String section, String key) {
		sectionsCombo.setText(section);
		reread(); // refresh page contents
		//XXX if key is a param key, select it in the table!
	}

	@Override
	public boolean setFocus() {
		return sectionsCombo.setFocus();
	}

	@Override
	public void reread() {
		super.reread();

		// refresh combo with the current section names, trying to preserve existing selection
		IInifileDocument doc = getInifileDocument();
		String selectedSection = sectionsCombo.getText();  // Note: "" if inifile is empty
		String[] sectionNames = doc.getSectionNames();
		sectionsCombo.setItems(sectionNames);
		sectionsCombo.setVisibleItemCount(Math.min(20, sectionsCombo.getItemCount()));
		int i = ArrayUtils.indexOf(sectionNames, selectedSection);
		sectionsCombo.select(i<0 ? 0 : i);
		selectedSection = sectionsCombo.getText();

 	    // compute fallback chain for selected section, and fill table with their contents
		GenericTreeNode rootNode = new GenericTreeNode("root");
		String[] sectionChain = "".equals(selectedSection) ? new String[0] : InifileUtils.resolveSectionChain(doc, selectedSection);
		for (String section : sectionChain) {
			GenericTreeNode sectionNode = new GenericTreeNode(section);
			rootNode.addChild(sectionNode);
			for (String key : doc.getKeys(section)) {
				if (key.contains(".")) { 
				    String suffix = StringUtils.substringAfterLast(key, ".");
				    if (!suffix.contains("-") || suffix.equals(CFGID_TYPE_NAME.getKey()))
				        sectionNode.addChild(new GenericTreeNode(new SectionKey(section, key)));
				}
			}
		}

		if (treeViewer.isCellEditorActive()) {
		    // refreshing the tree would close the cell editor, so don't do it.
		    // however, we'll need to do a refresh once the cell editor loses focus (commits or cancels).
		    // commit is OK, since it will directly invoke refresh(); we only have to deal with cancel.
		    // treeViewer.getColumnViewerEditor().addEditorActivationListener() doesn't seem to work
		    // (listener doesn't get called), so we just schedule another reread into the future.
            Debug.println("cell editor active -- postponing tree refresh");
		    delayedRereadJob.restartTimer();
		}
		else {
		    // refresh the tree
		    Debug.println("refreshing the tree");
		    treeViewer.setInput(rootNode);
		    treeViewer.expandAll();
		    treeViewer.refresh();
		}

		// update labels: "Network" and "Section fallback chain"
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		int numUnassigned = "".equals(selectedSection) ? 0 : getInifileAnalyzer().getUnassignedParams(selectedSection).length;
		networkNameLabel.setText("Network: "+(networkName==null ? "<not configured>" : networkName));
		sectionChainLabel.setText("Section fallback chain: "+(sectionChain.length==0 ? "<no sections>" : StringUtils.join(sectionChain, " > ")));
		numUnassignedParamsLabel.setText(numUnassigned+" unassigned parameter"+(numUnassigned!=1 ? "s" : ""));
		sectionChainLabel.getParent().layout();

		updateButtonStates();
		addMissingButton.setEnabled(selectedSection!=null);
	}

	@Override
	public String getPageCategory() {
		return InifileFormEditor.PARAMETERS_PAGE;
	}

	public List<ConfigKey> getSupportedKeys() {
		return new ArrayList<ConfigKey>();
	}
}

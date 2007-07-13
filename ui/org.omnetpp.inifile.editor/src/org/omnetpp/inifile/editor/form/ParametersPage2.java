package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.SWT;
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
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.actions.AddInifileKeysDialog;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.ConfigKey;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.SectionKey;
import org.omnetpp.inifile.editor.views.AbstractModuleView;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX set up cell editing
//XXX set up drag&drop for moving entries around (and remove Up/Down buttons!)
//XXX validation of keys and values! e.g. shouldn't allow empty key
//XXX comment handling (stripping/adding of "#")
public class ParametersPage2 extends FormPage {
	private TreeViewer treeViewer;
	private Combo sectionsCombo;
	private Label networkNameLabel;
	private Label sectionChainLabel;
	private Label numUnassignedParamsLabel;
	private Button newButton;
	private Button removeButton;
	private Button upButton;
	private Button downButton;
	private Button addMissingButton;

	public ParametersPage2(Composite parent, InifileEditor inifileEditor) {
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
					case 1: return getInifileDocument().getValue(item.section, item.key); 
					case 2: return getInifileDocument().getComment(item.section, item.key);
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
						return AbstractModuleView.ICON_INIPAR;
					}
					else if (columnIndex == 1) {
						SectionKey item = (SectionKey) element;
						IMarker[] markers = InifileUtils.getProblemMarkersFor(item.section, item.key, getInifileDocument());
						return FieldEditor.getProblemImage(markers, true);
					}
				}
				else {
					if (columnIndex==0)
						return SectionsPage.ICON_SECTION;
				}
				return null;
			}
		});

		//XXX see TreeEditor class comment on how to set up cell editing!
		
//		//XXX prefix comments with "#" after editing?
//		treeViewer.setColumnProperties(new String[] {"section", "key", "value", "comment"});
//		final TreeEditor editors[] = new TreeEditor[4];
//		editors[0] = null;
//		editors[1] = new TreeEditor(treeViewer.getTree());
//		editors[2] = new TreeEditor(treeViewer.getTree());
//		editors[3] = new TreeEditor(treeViewer.getTree());
//		treeViewer.setCellEditors(editors);
//		treeViewer.setCellModifier(new ICellModifier() {
//			public boolean canModify(Object element, String property) {
//				return !property.equals("section");
//			}
//
//			public Object getValue(Object element, String property) {
//				SectionKey item = (SectionKey) element;
//				if (property.equals("key"))
//					return item.key;
//				else if (property.equals("value"))
//					return nullToEmpty(getInifileDocument().getValue(item.section, item.key)); 
//				else if (property.equals("comment"))
//					return nullToEmpty(getInifileDocument().getComment(item.section, item.key));
//				else
//					return "-";
//			}
//
//			public void modify(Object element, String property, Object value) {
//				if (element instanceof Item)
//					element = ((Item) element).getData(); // workaround, see super's comment
//				SectionKey item = (SectionKey) element;
//				if (item==null)
//					return; //FIXME must be debugged how this can possibly happen
//				IInifileDocument doc = getInifileDocument();
//				try {
//					System.out.println("CellEditor committing the "+property+" column, value="+value);
//					if (property.equals("key")) {
//						String newKey = (String) value;
//						if (!newKey.equals(item.key)) {
//							String errorText = InifileUtils.validateParameterKey(doc, item.section, newKey);
//							if (errorText==null) { //XXX can't we validate in doc.changeKey()? 
//								doc.changeKey(item.section, item.key, (String)value);
//								reread(); // tableViewer.refresh() not enough, because input consists of keys
//							}
//							else {
//								MessageDialog.openError(getShell(), "Cannot Rename", "Cannot rename key: "+errorText);
//							}
//						}
//					}
//					else if (property.equals("value")) {
//						if (!value.equals(doc.getValue(item.section, item.key))) {
//							doc.setValue(item.section, item.key, (String)value);
//							treeViewer.refresh(); // if performance gets critical: refresh only if changed
//						}
//					}
//					else if (property.equals("comment")) {
//						if (value.equals("")) 
//							value = null; // no comment == null
//						if (!nullSafeEquals((String)value, doc.getComment(item.section, item.key))) {
//							doc.setComment(item.section, item.key, (String)value);
//							treeViewer.refresh(); // if performance gets critical: refresh only if changed
//						}
//					}
//				}
//				catch (RuntimeException e) {
//					showErrorDialog(e);
//				}
//			}
//		});
//
//		// content assist for the Key column
//		IContentProposalProvider proposalProvider = new InifileParamKeyContentProposalProvider(null, false, getInifileDocument(), getInifileAnalyzer()) {
//			@Override
//			public IContentProposal[] getProposals(String contents, int position) {
//				SectionKey e = (SectionKey)( (IStructuredSelection)treeViewer.getSelection()).getFirstElement();
//				configure(e.section, false); // set context for proposal calculation
//				return super.getProposals(contents, position);
//			}
//		};
//		ContentAssistUtil.configureTableColumnContentAssist(treeViewer, 1, proposalProvider, false);
//
//		// content assist for the Value column
//		IContentProposalProvider valueProposalProvider = new InifileValueContentProposalProvider(null, null, getInifileDocument(), getInifileAnalyzer(), false) {
//			@Override
//			public IContentProposal[] getProposals(String contents, int position) {
//				SectionKey e = (SectionKey)( (IStructuredSelection)treeViewer.getSelection()).getFirstElement();
//				setInifileEntry(e.section, e.key); // set context for proposal calculation
//				return super.getProposals(contents, position);
//			}
//		};
//		ContentAssistUtil.configureTableColumnContentAssist(treeViewer, 2, valueProposalProvider, true);

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
			}
		});

		// add tooltip support
		addTooltipSupport(treeViewer.getTree(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y, Point outPreferredSize) {
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

		return treeViewer;
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
		upButton = createButton(buttonGroup, "Up");
		downButton = createButton(buttonGroup, "Down");
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

		upButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				moveEntriesUp();
			}
		});

		downButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				moveEntriesDown();
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

	protected void addEntry() {
		// determine where to insert new element
		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		SectionKey beforeSectionKey = sel.isEmpty() ? null : (SectionKey) sel.getFirstElement();
		String section = beforeSectionKey == null ? sectionsCombo.getText() : beforeSectionKey.section;
		String beforeKey = beforeSectionKey == null ? null : beforeSectionKey.key;

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
			doc.addEntry(section, newKey, "", null, beforeKey);
			reread();
			treeViewer.setSelection(new StructuredSelection(new SectionKey(section, newKey)), true);
			treeViewer.getTree().setFocus();
			treeViewer.editElement(newKey, 1); //XXX does not seem to work
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void removeEntries() {
		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		try {
			for (Object o : sel.toArray()) {
				SectionKey item = (SectionKey) o;
				getInifileDocument().removeKey(item.section, item.key);
			}
			treeViewer.getTree().deselectAll();
			reread();
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void moveEntriesUp() {
		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		SectionKey[] items = (SectionKey[]) treeViewer.getInput();
		try {
			//XXX does not work properly
			for (Object o : sel.toArray()) {
				SectionKey item = (SectionKey) o;
				int pos = ArrayUtils.indexOf(items, item);
				Assert.isTrue(pos != -1);
				if (pos == 0) 
					break; // hit the top
				getInifileDocument().moveKey(item.section, item.key, items[pos-1].key);
				SectionKey tmp = items[pos-1]; items[pos-1] = items[pos]; items[pos] = tmp;
			}
			reread();
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
		treeViewer.setSelection(sel);
	}

	protected void moveEntriesDown() {
		IStructuredSelection sel = (IStructuredSelection) treeViewer.getSelection();
		Object[] array = sel.toArray();
		SectionKey[] items = (SectionKey[]) treeViewer.getInput();
		try {
			ArrayUtils.reverse(array);  // we must iterate in reverse order
			//XXX does not work properly
			for (Object o : array) {
				SectionKey item = (SectionKey) o;
				int pos = ArrayUtils.indexOf(items, item);
				Assert.isTrue(pos != -1);
				if (pos == items.length-1) 
					break; // hit the bottom
				getInifileDocument().moveKey(item.section, item.key, pos==items.length-2 ? null : items[pos+2].key);
				SectionKey tmp = items[pos+1]; items[pos+1] = items[pos]; items[pos] = tmp;
			}
			reread();
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
		treeViewer.setSelection(sel);
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

			// add user-selected keys to the document, and also **.apply-default if chosen by the user
			String[] keys = dialog.getKeys();
			String section = dialog.getSection();
			try {
				doc.addEntries(section, keys, null, null, null);
				if (dialog.getAddApplyDefault())
					InifileUtils.addEntry(doc, section, "**.apply-default", "true", null);

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
		String selectedSection = sectionsCombo.getText(); 
		String[] sectionNames = doc.getSectionNames();
		if (sectionNames.length==0) 
			sectionNames = new String[] {"General"};  //XXX we lie that [General] exists
		sectionsCombo.setItems(sectionNames);
		sectionsCombo.setVisibleItemCount(Math.min(20, sectionsCombo.getItemCount()));
		int i = ArrayUtils.indexOf(sectionNames, selectedSection);
		sectionsCombo.select(i<0 ? 0 : i);
		selectedSection = sectionsCombo.getText(); 

 	    // compute fallback chain for selected section, and fill table with their contents
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, selectedSection);
		GenericTreeNode rootNode = new GenericTreeNode("root");
		for (String section : sectionChain) {
			GenericTreeNode sectionNode = new GenericTreeNode(section);
			rootNode.addChild(sectionNode);
			for (String key : doc.getKeys(section))
				if (key.contains("."))
					sectionNode.addChild(new GenericTreeNode(new SectionKey(section, key)));
		}
		treeViewer.setInput(rootNode);
		treeViewer.expandAll();
		treeViewer.refresh();

		// update labels: "Network" and "Section fallback chain"
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		int numUnassigned = getInifileAnalyzer().getUnassignedParams(selectedSection).length;
		networkNameLabel.setText("Network: "+(networkName==null ? "<not configured>" : networkName));
		sectionChainLabel.setText("Section fallback chain: "+(sectionChain.length==0 ? "<no sections>" : StringUtils.join(sectionChain, " > ")));
		numUnassignedParamsLabel.setText(numUnassigned+" unassigned parameter"+(numUnassigned>1 ? "s" : ""));
		sectionChainLabel.getParent().layout();
	}

	@Override
	public String getPageCategory() {
		return InifileFormEditor.PARAMETERS_PAGE;
	}

	public List<ConfigKey> getSupportedKeys() {
		return new ArrayList<ConfigKey>();
	}
}

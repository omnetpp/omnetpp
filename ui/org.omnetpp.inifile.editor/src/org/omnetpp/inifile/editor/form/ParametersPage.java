package org.omnetpp.inifile.editor.form;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
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
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentAssistUtil;
import org.omnetpp.common.ui.IHoverTextProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.actions.AddInifileKeysDialog;
import org.omnetpp.inifile.editor.contentassist.InifileParamKeyContentProposalProvider;
import org.omnetpp.inifile.editor.contentassist.InifileValueContentProposalProvider;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileHoverUtils;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.SectionKey;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX validation of keys and values! e.g. shouldn't allow empty key
//XXX comment handling (stripping/adding of "#")
//XXX add status label below the table: "34 unassigned parameters"
public class ParametersPage extends FormPage {
	private TableViewer tableViewer;
	private Combo sectionsCombo;
	private Label networkNameLabel;
	private Label sectionChainLabel;
	private Button addButton;
	private Button removeButton;
	private Button upButton;
	private Button downButton;
	private Button addMissingButton;

	public ParametersPage(Composite parent, InifileEditor inifileEditor) {
		super(parent, inifileEditor);

		// layout: 3x2 grid: (label, dummy) / (combobox-with-label, dummy) / (table, buttonGroup)
		setLayout(new GridLayout(2,false));
		((GridLayout)getLayout()).marginRight = RIGHT_MARGIN;

		Composite titleArea = createTitleArea(this, "Parameter Assignments");
		((GridData)titleArea.getLayoutData()).horizontalSpan = 2;

		// create combobox
		Composite comboWithLabel = new Composite(this, SWT.NONE);
		comboWithLabel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		new Label(this, SWT.NONE);  // dummy, to fill 2nd column

		Label comboLabel = new Label(comboWithLabel, SWT.NONE);
		comboLabel.setText("Configuration:");
		sectionsCombo = new Combo(comboWithLabel, SWT.BORDER | SWT.READ_ONLY);
		comboWithLabel.setLayout(new GridLayout(2, false));
		comboLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		sectionsCombo.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, false));

		// create network name and section chain labels
		networkNameLabel = new Label(this, SWT.NONE);
		networkNameLabel.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		networkNameLabel.setText("Network name: n/a");
		new Label(this, SWT.NONE); // dummy, to fill 2nd column

		sectionChainLabel = new Label(this, SWT.NONE);
		sectionChainLabel.setLayoutData(new GridData(SWT.END, SWT.BEGINNING, true, false));
		sectionChainLabel.setText("Section fallback chain: n/a");
		new Label(this, SWT.NONE); // dummy, to fill 2nd column

		// create table and buttons
		tableViewer = createAndConfigureTable();
		tableViewer.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		Composite buttonGroup = createButtons();
		buttonGroup.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, false, true));

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

	private TableViewer createAndConfigureTable() {
		Table table = new Table(this, SWT.BORDER | SWT.MULTI | SWT.FULL_SELECTION | SWT.VIRTUAL);
		table.setLinesVisible(true);
		table.setHeaderVisible(true);
		addTableColumn(table, "Section", 80);
		addTableColumn(table, "Key", 180);
		addTableColumn(table, "Value", 120);
		addTableColumn(table, "Comment", 80);

		// set up tableViewer, content and label providers
		final TableViewer tableViewer = new TableViewer(table);
		tableViewer.setContentProvider(new ArrayContentProvider());
		tableViewer.setLabelProvider(new TableLabelProvider() {
			@Override
			public String getColumnText(Object element, int columnIndex) {
				SectionKey item = (SectionKey) element;
				switch (columnIndex) {
				case 0: return item.section;
				case 1: return item.key;
				case 2: return getInifileDocument().getValue(item.section, item.key); 
				case 3: return getInifileDocument().getComment(item.section, item.key);
				default: throw new IllegalArgumentException();
				}
			}
			@Override
			public Image getColumnImage(Object element, int columnIndex) {
				if (columnIndex == 2) {
					SectionKey item = (SectionKey) element;
					IMarker[] markers = InifileUtils.getProblemMarkersFor(item.section, item.key, getInifileDocument());
					return FieldEditor.getProblemImage(markers, true);
				}
				return null;
			}
		});

		//XXX prefix comments with "#" after editing?
		tableViewer.setColumnProperties(new String[] {"section", "key", "value", "comment"});
		final TableTextCellEditor editors[] = new TableTextCellEditor[4];
		editors[0] = null;
		editors[1] = new TableTextCellEditor(tableViewer, 1);
		editors[2] = new TableTextCellEditor(tableViewer, 2);
		editors[3] = new TableTextCellEditor(tableViewer, 3);
		tableViewer.setCellEditors(editors);
		tableViewer.setCellModifier(new ICellModifier() {
			public boolean canModify(Object element, String property) {
				return !property.equals("section");
			}

			public Object getValue(Object element, String property) {
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
				SectionKey item = (SectionKey) element;
				if (item == null)
					return; // shit happens when user selects from content assist window... 
				IInifileDocument doc = getInifileDocument();
				try {
					if (property.equals("key")) {
						String newKey = (String) value;
						if (!newKey.equals(item.key)) {
							String errorText = InifileUtils.validateParameterKey(doc, item.section, newKey);
							if (errorText==null) { //XXX can't we validate in doc.changeKey()? 
								doc.changeKey(item.section, item.key, (String)value);
								reread(); // tableViewer.refresh() not enough, because input consists of keys
							}
							else {
								MessageDialog.openError(null, "Cannot Rename", "Cannot rename key: "+errorText);
							}
						}
					}
					else if (property.equals("value")) {
						if (!value.equals(doc.getValue(item.section, item.key))) {
							doc.setValue(item.section, item.key, (String)value);
							tableViewer.refresh(); // if performance gets critical: refresh only if changed
						}
					}
					else if (property.equals("comment")) {
						if (value.equals("")) 
							value = null; // no comment == null
						if (!nullSafeEquals((String)value, doc.getComment(item.section, item.key))) {
							doc.setComment(item.section, item.key, (String)value);
							tableViewer.refresh(); // if performance gets critical: refresh only if changed
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
				SectionKey e = (SectionKey)( (IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				configure(e.section, false); // set context for proposal calculation
				return super.getProposals(contents, position);
			}
		};
		ContentAssistUtil.configureTableColumnContentAssist(tableViewer, 1, proposalProvider, false);
		
		//new ContentAssistCommandAdapter(editors[1].getText(), new TextContentAdapter(), proposalProvider, 
		//	ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);

		// content assist for the Value column
		IContentProposalProvider valueProposalProvider = new InifileValueContentProposalProvider(null, null, getInifileDocument(), getInifileAnalyzer(), false) {
			@Override
			public IContentProposal[] getProposals(String contents, int position) {
				SectionKey e = (SectionKey)( (IStructuredSelection)tableViewer.getSelection()).getFirstElement();
				setInifileEntry(e.section, e.key); // set context for proposal calculation
				return super.getProposals(contents, position);
			}
		};
		ContentAssistUtil.configureTableColumnContentAssist(tableViewer, 2, valueProposalProvider, true);
		
		//new ContentAssistCommandAdapter(editors[2].getText(), new TextContentAdapter(), valueProposalProvider, 
		//	ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, null, true);

		// on double-click, show entry in the text editor
		tableViewer.getTable().addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetDefaultSelected(SelectionEvent event) {
				SectionKey entry = (SectionKey) (event.item==null ? null : event.item.getData());
				getEditorData().getInifileEditor().gotoEntry(entry.section, entry.key, IGotoInifile.Mode.TEXT);
			}
		});

		// export the table's selection as editor selection
		tableViewer.addSelectionChangedListener(new ISelectionChangedListener() {
			public void selectionChanged(SelectionChangedEvent event) {
				IStructuredSelection sel = (IStructuredSelection) event.getSelection();
				SectionKey entry = (SectionKey) sel.getFirstElement();
				if (entry!=null)
					setEditorSelection(entry.section, entry.key);
			}
		});

		// add tooltip support
		addTooltipSupport(tableViewer.getTable(), new IHoverTextProvider() {
			public String getHoverTextFor(Control control, int x, int y) {
				Item item = tableViewer.getTable().getItem(new Point(x,y));
				SectionKey entry = (SectionKey) (item==null ? null : item.getData());
				return entry==null ? null : InifileHoverUtils.getEntryHoverText(entry.section, entry.key, getInifileDocument(), getInifileAnalyzer());
			}
		});

		return tableViewer;
	}

	private static String nullToEmpty(String text) {
		return text == null ? "" : text;
	}

	protected static boolean nullSafeEquals(String first, String second) {
		return first==null ? second == null : first.equals(second);
	}
	
	private TableColumn addTableColumn(Table table, String label, int width) {
		TableColumn column = new TableColumn(table, SWT.NONE);
		column.setText(label);
		column.setWidth(width);
		return column;
	}

	private Composite createButtons() {
		Composite buttonGroup = new Composite(this, SWT.NONE);
		buttonGroup.setLayout(new GridLayout(1,false));

		addButton = createButton(buttonGroup, "Add");
		removeButton = createButton(buttonGroup, "Remove");
		upButton = createButton(buttonGroup, "Up");
		downButton = createButton(buttonGroup, "Down");
		new Label(buttonGroup, SWT.NONE);
		addMissingButton = createButton(buttonGroup, "Add missing...");
		
		addButton.addSelectionListener(new SelectionAdapter() {
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
		IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
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
			tableViewer.getTable().setFocus();
			tableViewer.setSelection(new StructuredSelection(new SectionKey(section, newKey)), true);
			tableViewer.editElement(newKey, 1); //XXX does not seem to work
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void removeEntries() {
		IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
		try {
			for (Object o : sel.toArray()) {
				SectionKey item = (SectionKey) o;
				getInifileDocument().removeKey(item.section, item.key);
			}
			tableViewer.getTable().deselectAll();
			reread();
		}
		catch (RuntimeException ex) {
			showErrorDialog(ex);
		}
	}

	protected void moveEntriesUp() {
		IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
		SectionKey[] items = (SectionKey[]) tableViewer.getInput();
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
		tableViewer.setSelection(sel);
	}

	protected void moveEntriesDown() {
		IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
		Object[] array = sel.toArray();
		SectionKey[] items = (SectionKey[]) tableViewer.getInput();
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
		tableViewer.setSelection(sel);
	}

	protected void addMissingKeys() {
		String selectedSection = sectionsCombo.getText();
		InifileAnalyzer analyzer = getInifileAnalyzer();
		IInifileDocument doc = analyzer.getDocument();

		// open the dialog
		AddInifileKeysDialog dialog = new AddInifileKeysDialog(getShell(), analyzer, selectedSection);
		if (dialog.open()==Dialog.OK) {
			// save selection
			IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();

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
			tableViewer.setSelection(sel);
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
		ArrayList<SectionKey> list = new ArrayList<SectionKey>();
		for (String section : sectionChain)
			for (String key : doc.getKeys(section))
				if (key.contains("."))
					list.add(new SectionKey(section, key));

		tableViewer.setInput(list.toArray(new SectionKey[list.size()]));
		tableViewer.refresh();

		// update labels: "Network" and "Section fallback chain"
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		networkNameLabel.setText("Network: "+(networkName==null ? "<not configured>" : networkName)+"  ");
		sectionChainLabel.setText("Section fallback chain: "+(sectionChain.length==0 ? "<no sections>" : StringUtils.join(sectionChain, " > ")));
		sectionChainLabel.getParent().layout();
	}

	@Override
	public String getPageCategory() {
		return InifileFormEditor.PARAMETERS_PAGE;
	}

}

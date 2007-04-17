package org.omnetpp.inifile.editor.form;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
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
import org.omnetpp.common.ui.ITooltipProvider;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.ui.TooltipSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.IGotoInifile;
import org.omnetpp.inifile.editor.editors.InifileEditor;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.SectionKey;

/**
 * For editing module parameters.
 * 
 * @author Andras
 */
//XXX validation of keys and values! e.g. shouldn't allow empty key
//XXX comment handling (stripping/adding of "#")
//XXX publish selection as editor selection
public class ParametersPage extends FormPage {
	private TableViewer tableViewer;
	private Combo sectionsCombo;
	private Label sectionChainLabel;

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

		// create section chain label
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
		});

		//XXX set up content assist. See: addCellEditors() method, ChangeParametersControl.class in JDT
		//XXX prefix comments with "#" after editing?
		tableViewer.setColumnProperties(new String[] {"section", "key", "value", "comment"});
		final CellEditor editors[] = new CellEditor[4];
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
				IInifileDocument doc = getInifileDocument();
				if (property.equals("key")) {
					String newKey = (String) value;
					if (!newKey.equals(item.key)) {
						String errorText = InifileUtils.validateParameterKey(doc, item.section, newKey);
						if (errorText==null) {
							doc.changeKey(item.section, item.key, (String)value);
							reread(); // tableViewer.refresh() not enough, because input consists of keys
						}
						else {
							MessageDialog.openError(null, "Cannot Rename", "Cannot rename key: "+errorText);
						}
					}
				}
				else if (property.equals("value")) {
					doc.setValue(item.section, item.key, (String)value);
					tableViewer.refresh(); // if performance gets critical: refresh only if changed
				}
				else if (property.equals("comment")) {
					if (value.equals("")) value = null; // no comment == null
					doc.setComment(item.section, item.key, (String)value);
					tableViewer.refresh();// if performance gets critical: refresh only if changed
				}
			}
		});

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
 		TooltipSupport.adapt(tableViewer.getTable(), new ITooltipProvider() {
			public String getTooltipFor(Control control, int x, int y) {
				Item item = tableViewer.getTable().getItem(new Point(x,y));
				SectionKey entry = (SectionKey) (item==null ? null : item.getData());
				return entry==null ? null : InifileUtils.getEntryTooltip(entry.section, entry.key, getInifileDocument(), getInifileAnalyzer());
			}
 		});
		
		return tableViewer;
	}

	private static String nullToEmpty(String text) {
		return text == null ? "" : text;
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
		
		Button addButton = createButton(buttonGroup, "Add...");
		Button removeButton = createButton(buttonGroup, "Remove");
		//new Label(buttonGroup, SWT.NONE);
		Button upButton = createButton(buttonGroup, "Up");
		Button downButton = createButton(buttonGroup, "Down");
		
		addButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
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
				
				// insert key and refresh table
				doc.addEntry(section, newKey, "", null, beforeKey); //XXX what if no such section
				reread();
				tableViewer.editElement(newKey, 1);
			}
		});

		removeButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				for (Object o : sel.toArray()) {
					SectionKey item = (SectionKey) o;
					getInifileDocument().removeKey(item.section, item.key);
				}
				tableViewer.getTable().deselectAll();
				reread();
			}
		});

		upButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				SectionKey[] items = (SectionKey[]) tableViewer.getInput();
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
				tableViewer.setSelection(sel);
			}
		});

		downButton.addSelectionListener(new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				IStructuredSelection sel = (IStructuredSelection) tableViewer.getSelection();
				Object[] array = sel.toArray();
				SectionKey[] items = (SectionKey[]) tableViewer.getInput();
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
				tableViewer.setSelection(sel);
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

		// update "Section fallback chain" label
		sectionChainLabel.setText("Section fallback chain: "+(sectionChain.length==0 ? "<no sections>" : StringUtils.join(sectionChain, " > ")));
		sectionChainLabel.getParent().layout();
	}

	@Override
	public void commit() {
		//XXX todo
	}

	@Override
	public String getPageCategory() {
		return InifileFormEditor.PARAMETERS_PAGE;
	}
}

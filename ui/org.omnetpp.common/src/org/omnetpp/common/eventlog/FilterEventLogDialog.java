package org.omnetpp.common.eventlog;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.math.BigDecimal;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.PStringVector;

public class FilterEventLogDialog extends TitleAreaDialog {

	private EventLogInput eventLogInput;

	private EventLogFilterParameters filterParameters;

	private Button enableTraceFilter;

	private Button enableEventNumberFilter;

	private Button enableSimulationTimeFilter;

	private Button enableModuleFilter;

	private Button enableMessageFilter;

	@SuppressWarnings("unused")
	private Button enableMessageIdFilter;

	@SuppressWarnings("unused")
	private Button enableMessageTreeIdFilter;

	@SuppressWarnings("unused")
	private Button enableMessageEncapsulationIdFilter;

	@SuppressWarnings("unused")
	private Button enableMessageEncapsulationTreeIdFilter;

	private Text lowerEventNumberLimit;

	private Text upperEventNumberLimit;
	
	private Text lowerSimulationTimeLimit;
	
	private Text upperSimulationTimeLimit;

	private Text tracedEventNumber;

	private Button traceCauses;

	private Button traceConsequences;

	private Button traceMessageReuses;

	private Button traceSelfMessages;
	
	private Text causeEventNumberDelta;

	private Text consequenceEventNumberDelta;

	private Text causeSimulationTimeDelta;

	private Text consequenceSimulationTimeDelta;

	private Text moduleFilterExpression;

	private TabFolder moduleTabFolder;

	private CheckboxTableViewer moduleClassNames;

	private ModuleTreeViewer moduleNameIds;

	private CheckboxTableViewer moduleIds;

	private TabFolder messageTabFolder;

	private Text messageFilterExpression;

	private CheckboxTableViewer messageClassNames;

	private CheckboxTableViewer messageNames;

	@SuppressWarnings("unused")
	private EditableList messageIds;

	@SuppressWarnings("unused")
	private EditableList messageTreeIds;

	@SuppressWarnings("unused")
	private EditableList messageEncapsulationIds;

	@SuppressWarnings("unused")
	private EditableList messageEncapsulationTreeIds;

	public FilterEventLogDialog(Shell parentShell, EventLogInput eventLogInput, EventLogFilterParameters filterParameters) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.MAX | SWT.RESIZE);
		this.eventLogInput = eventLogInput;
		this.filterParameters = filterParameters;
	}

	private void unparseFilterParameters() {
		try {
			Class<EventLogFilterParameters> clazz = EventLogFilterParameters.class;
			
			for (Field parameterField : clazz.getDeclaredFields()) {
				if ((parameterField.getModifiers() & Modifier.PUBLIC) != 0) {
					Class<?> parameterFieldType = parameterField.getType();
					Field guiField = getClass().getDeclaredField(parameterField.getName());
	
					if (parameterFieldType == boolean.class)
						unparseBoolean((Button)guiField.get(this), parameterField.getBoolean(filterParameters));
					else if (parameterFieldType == int.class)
						unparseInt((Text)guiField.get(this), parameterField.getInt(filterParameters));
					else if (parameterFieldType == BigDecimal.class)
						unparseBigDecimal((Text)guiField.get(this), (BigDecimal)parameterField.get(filterParameters));
					else if (parameterFieldType == String.class)
						unparseString((Text)guiField.get(this), (String)parameterField.get(filterParameters));
					else if (parameterFieldType == int[].class) {
						Object guiControl = guiField.get(this);
						
						if (guiControl instanceof EditableList)
							unparseIntArray((EditableList)guiControl, (int[])parameterField.get(filterParameters));
						else if (guiControl instanceof CheckboxTableViewer)
							unparseIntArray((CheckboxTableViewer)guiControl, (int[])parameterField.get(filterParameters));
						else if (guiControl instanceof ModuleTreeViewer)
							unparseModuleNameIdArray((ModuleTreeViewer)guiControl, (int[])parameterField.get(filterParameters));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else if (parameterFieldType == String[].class) {
						Object guiControl = guiField.get(this);
						
						if (guiControl instanceof EditableList)
							unparseStringArray((EditableList)guiField.get(this), (String[])parameterField.get(filterParameters));
						else if (guiControl instanceof CheckboxTableViewer)
							unparseStringArray((CheckboxTableViewer)guiControl, (String[])parameterField.get(filterParameters));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else
						throw new RuntimeException("Unknown parameter field type");
				}
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	private void unparseBoolean(Button button, boolean value) {
		button.setSelection(value);
	}
	
	private void unparseInt(Text text, int value) {
		if (value != -1)
			text.setText(String.valueOf(value));
	}
	
	private void unparseBigDecimal(Text text, BigDecimal value) {
		if (value != null)
			text.setText(value.toString());
	}
	
	private void unparseString(Text text, String value) {
		if (value != null)
			text.setText(value);
	}

	private void unparseIntArray(EditableList editableList, int[] values) {
		if (values != null) {
			String[] stringValues = new String[values.length];
			for (int i = 0; i < values.length; i++)
				stringValues[i] = String.valueOf(values[i]);
			editableList.getList().setItems(stringValues);
		}
	}

	private void unparseIntArray(CheckboxTableViewer checkboxTableViewer, int[] values) {
		if (values != null) {
			Integer[] integerValues = new Integer[values.length];

			for (int i = 0; i < values.length; i++)
				integerValues[i] = values[i];

			checkboxTableViewer.setCheckedElements(integerValues);
		}
	}

	private void unparseStringArray(EditableList editableList, String[] values) {
		if (values != null)
			editableList.getList().setItems(values);
	}

	private void unparseModuleNameIdArray(ModuleTreeViewer moduleTreeViewer, int[] values) {
		if (values != null) {
			ModuleTreeItem[] moduleTreeItems = new ModuleTreeItem[values.length];
	
			for (int i = 0; i < values.length; i++)
				moduleTreeItems[i] = eventLogInput.getModuleTreeRoot().findDescendantModule(values[i]);
	
			moduleTreeViewer.setCheckedElements(moduleTreeItems);
		}
	}

	private void unparseStringArray(CheckboxTableViewer checkboxTableViewer, String[] values) {
		if (values != null)
			checkboxTableViewer.setCheckedElements(values);
	}

	public void parseFilterParameters() {
		try {
			Class<EventLogFilterParameters> clazz = EventLogFilterParameters.class;
			
			for (Field parameterField : clazz.getDeclaredFields()) {
				if ((parameterField.getModifiers() & Modifier.PUBLIC) != 0) {
					Class<?> parameterFieldType = parameterField.getType();
					Field guiField = getClass().getDeclaredField(parameterField.getName());
	
					if (parameterFieldType == boolean.class)
						parameterField.setBoolean(filterParameters, parseBoolean((Button)guiField.get(this)));
					else if (parameterFieldType == int.class)
						parameterField.setInt(filterParameters, parseInt((Text)guiField.get(this)));
					else if (parameterFieldType == BigDecimal.class)
						parameterField.set(filterParameters, parseBigDecimal((Text)guiField.get(this)));
					else if (parameterFieldType == String.class)
						parameterField.set(filterParameters, parseString((Text)guiField.get(this)));
					else if (parameterFieldType == int[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof EditableList)
							parameterField.set(filterParameters, parseIntArray((EditableList)guiControl));
						else if (guiControl instanceof ModuleTreeViewer)
							parameterField.set(filterParameters, parseModuleNameIdArray((ModuleTreeViewer)guiField.get(this)));
						else if (guiControl instanceof CheckboxTableViewer)
							parameterField.set(filterParameters, parseIntArray((CheckboxTableViewer)guiField.get(this)));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else if (parameterFieldType == String[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof EditableList)
							parameterField.set(filterParameters, parseStringArray((EditableList)guiField.get(this)));
						else if (guiControl instanceof CheckboxTableViewer)
							parameterField.set(filterParameters, parseModuleClassNameArray((CheckboxTableViewer)guiField.get(this)));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else
						throw new RuntimeException("Unknown parameter field type");
				}
			}
		}
		catch (Exception e) {
			throw new RuntimeException(e);
		}
	}

	private boolean parseBoolean(Button button) {
		return button.getSelection();
	}
	
	private int parseInt(Text text) {
		if (text.getText().length() != 0)
			return Integer.parseInt(text.getText());
		else
			return -1;
	}
	
	private BigDecimal parseBigDecimal(Text text) {
		if (text.getText().length() != 0)
			return new BigDecimal(text.getText());
		else
			return null;
	}
	
	private String parseString(Text text) {
		return text.getText();
	}
	
	private int[] parseIntArray(EditableList editableList) {
		String[] stringValues = editableList.getList().getItems();
		int[] intValues = new int[editableList.getList().getItems().length];

		for (int i = 0; i < stringValues.length; i++)
			intValues[i] = Integer.parseInt(stringValues[i]);

		return intValues;
	}
	
	private int[] parseIntArray(CheckboxTableViewer checkBoxTableViewer) {
		Object[] elements = checkBoxTableViewer.getCheckedElements();
		int[] values = new int[elements.length];
		
		for (int i = 0; i < elements.length; i++)
			values[i] = (Integer)elements[i];
		
		return values;
	}

	private String[] parseStringArray(EditableList editableList) {
		return editableList.getList().getItems();
	}
	
	private int[] parseModuleNameIdArray(ModuleTreeViewer moduleTreeViewer) {
		Object[] treeItems = moduleTreeViewer.getCheckedElements();
		int[] values = new int[treeItems.length];

		for (int i = 0; i < values.length; i++)
			values[i] = ((ModuleTreeItem)treeItems[i]).getModuleId();
		
		return values;
	}

	private String[] parseModuleClassNameArray(CheckboxTableViewer checkBoxTableViewer) {
		Object[] elements = checkBoxTableViewer.getCheckedElements();
		String[] moduleClassNames = new String[elements.length];
		
		for (int i = 0; i < elements.length; i++)
			moduleClassNames[i] = (String)elements[i];
		
		return moduleClassNames;
	}
	
	@Override
	protected Control createDialogArea(Composite parent) {
		Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
		container.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		container.setLayout(new GridLayout());

		setHelpAvailable(false);
		setTitle("Select filter criteria");
		setMessage("The event log will be filtered for events that match all criteria");

		TabFolder tabFolder = new TabFolder(container, SWT.NONE);
		tabFolder.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

		createGeneralTabItem(tabFolder);
		createModuleTabItem(tabFolder);
		createMessageTabItem(tabFolder);
		createEventTraceTabItem(tabFolder);

		unparseFilterParameters();
		
		enableEventNumberFilter.notifyListeners(SWT.Selection, null);
		enableSimulationTimeFilter.notifyListeners(SWT.Selection, null);
        enableModuleFilter.notifyListeners(SWT.Selection, null);
		enableMessageFilter.notifyListeners(SWT.Selection, null);
		enableMessageIdFilter.notifyListeners(SWT.Selection, null);
		enableMessageTreeIdFilter.notifyListeners(SWT.Selection, null);
		enableMessageEncapsulationIdFilter.notifyListeners(SWT.Selection, null);
		enableMessageEncapsulationTreeIdFilter.notifyListeners(SWT.Selection, null);
		enableTraceFilter.notifyListeners(SWT.Selection, null);

		return container;
	}

	@Override
	protected void configureShell(Shell newShell) {
		newShell.setText("Filter event log");
		super.configureShell(newShell);
	}
	
	@Override
	protected void okPressed() {
		parseFilterParameters();
		super.okPressed();
	}
	
	private Composite createTabItem(TabFolder tabFolder, String tabText) {
		TabItem tabItem = new TabItem(tabFolder, SWT.NONE);
		tabItem.setText(tabText);
		Composite panel = new Composite(tabFolder, SWT.NONE);
		panel.setLayout(new GridLayout(2, false));
		tabItem.setControl(panel);
		
		return panel;
	}

	private void createGeneralTabItem(TabFolder tabFolder) {
		Composite panel = createTabItem(tabFolder, "General filter");
		
		Label label = new Label(panel, SWT.NONE);
		label.setText("The following limits if specified filter out ranges of events from the result");
		label.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		// event number limits
		Group group = new Group(panel, SWT.NONE);
		group.setText("Event number limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		enableEventNumberFilter = new Button(group, SWT.CHECK);
		enableEventNumberFilter.setText("Limit event log by event numbers");
		enableEventNumberFilter.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));
		enableEventNumberFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				boolean selection = enableEventNumberFilter.getSelection();
				lowerEventNumberLimit.setEnabled(selection);
				upperEventNumberLimit.setEnabled(selection);
			}
		});

		label = new Label(group, SWT.NONE);
		label.setText("Lower event number limit");
		label.setToolTipText("Events with event number less than this will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		lowerEventNumberLimit = new Text(group, SWT.BORDER);
		lowerEventNumberLimit.setToolTipText(label.getToolTipText());
		lowerEventNumberLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Upper event number limit");
		label.setToolTipText("Events with event number greater than this will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		upperEventNumberLimit = new Text(group, SWT.BORDER);
		upperEventNumberLimit.setToolTipText(label.getToolTipText());
		upperEventNumberLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// simulation time limits
		group = new Group(panel, SWT.NONE);
		group.setText("Simulation time limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		enableSimulationTimeFilter = new Button(group, SWT.CHECK);
		enableSimulationTimeFilter.setText("Limit event log by simulation time");
		enableSimulationTimeFilter.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));
		enableSimulationTimeFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				boolean selection = enableSimulationTimeFilter.getSelection();
				lowerSimulationTimeLimit.setEnabled(selection);
				upperSimulationTimeLimit.setEnabled(selection);
			}
		});
		
		label = new Label(group, SWT.NONE);
		label.setText("Lower simulation time limit in seconds");
		label.setToolTipText("Events occured before this simulation time will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		lowerSimulationTimeLimit = new Text(group, SWT.BORDER);
		lowerSimulationTimeLimit.setToolTipText(label.getToolTipText());
		lowerSimulationTimeLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Upper simulation time limit in seconds");
		label.setToolTipText("Events occured after this simulation time will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		upperSimulationTimeLimit = new Text(group, SWT.BORDER);
		upperSimulationTimeLimit.setToolTipText(label.getToolTipText());
		upperSimulationTimeLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}

	private void createModuleTabItem(TabFolder tabFolder) {
		Composite panel = createTabItem(tabFolder, "Module filter");

		enableModuleFilter = new Button(panel, SWT.CHECK);
		enableModuleFilter.setText("Filter for events occured in any of the selected modules");
		enableModuleFilter.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
        enableModuleFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				boolean selection = enableModuleFilter.getSelection();
				moduleFilterExpression.setEnabled(selection);
				moduleTabFolder.setEnabled(selection);
				moduleNameIds.getTree().setEnabled(selection);
				moduleClassNames.getTable().setEnabled(selection);
				moduleIds.getTable().setEnabled(selection);
			}
		});
		
		Label label = new Label(panel, SWT.NONE);
		label.setText("Module filter expression");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		moduleFilterExpression = new Text(panel, SWT.BORDER);
		moduleFilterExpression.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// tab folder
		panel = new Composite(panel, SWT.NONE);
		panel.setLayout(new FillLayout());
		panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		moduleTabFolder = new TabFolder(panel, SWT.NONE);
		IEventLog eventLog = eventLogInput.getEventLog();

		// module class name filter
		panel = createTabItem(moduleTabFolder, "Filter by class name");
		int count = eventLog.getNumModuleCreatedEntries();
		Set<String> moduleClassNameSet = new HashSet<String>();
		for (int i = 0; i < count; i++) {
			ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(i);
			if (moduleCreatedEntry != null)
				moduleClassNameSet.add(moduleCreatedEntry.getModuleClassName());
		}

		String[] moduleClassNamesAsStrings = (String[])moduleClassNameSet.toArray(new String[0]);
		Arrays.sort(moduleClassNamesAsStrings);
		moduleClassNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		moduleClassNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		for (String moduleClassName : moduleClassNamesAsStrings)
			moduleClassNames.add(moduleClassName);

		// module name filter
		panel = createTabItem(moduleTabFolder, "Filter by name");

		moduleNameIds = new ModuleTreeViewer(panel, eventLogInput.getModuleTreeRoot());
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1);
        gridData.widthHint = 500;
        gridData.heightHint = 400;
        moduleNameIds.getTree().setLayoutData(gridData);
        
        // module id filter
        panel = createTabItem(moduleTabFolder, "Filter by id");
		moduleIds = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		moduleIds.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		moduleIds.setLabelProvider(new LabelProvider() {
			public String getText(Object element) {
				ModuleTreeItem moduleTreeItem = eventLogInput.getModuleTreeRoot().findDescendantModule((Integer)element);
				
				return "(id = " + moduleTreeItem.getModuleId() + ") " + moduleTreeItem.getModuleFullPath();
			}
		});
		for (int i = 0; i < eventLog.getNumModuleCreatedEntries(); i++) {
			ModuleCreatedEntry moduleCreatedEntry = eventLog.getModuleCreatedEntry(i);
			
			if (moduleCreatedEntry != null)
				moduleIds.add(moduleCreatedEntry.getModuleId());
		}
	}

	private void createMessageTabItem(TabFolder tabFolder) {
		Composite panel = createTabItem(tabFolder, "Message filter");

		enableMessageFilter = new Button(panel, SWT.CHECK);
		enableMessageFilter.setText("Filter for events processing a message where any of the following criteria holds");
		enableMessageFilter.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, 2, 1));
		enableMessageFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				boolean selection = enableMessageFilter.getSelection();
				messageFilterExpression.setEnabled(selection);
				messageTabFolder.setEnabled(selection);
				messageClassNames.getTable().setEnabled(selection);
				messageNames.getTable().setEnabled(selection);
			}
		});
		
		Label label = new Label(panel, SWT.NONE);
		label.setText("Message filter expression");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		messageFilterExpression = new Text(panel, SWT.BORDER);
		messageFilterExpression.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// tab folder
		panel = new Composite(panel, SWT.NONE);
		panel.setLayout(new FillLayout());
		panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		messageTabFolder = new TabFolder(panel, SWT.NONE);
		IEventLog eventLog = eventLogInput.getEventLog();

		// message class name filter
		panel = createTabItem(messageTabFolder, "Filter by class name");
		label = new Label(panel, SWT.NONE);
		label.setText("The following message class names have been encountered so far");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
		messageClassNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageClassNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		PStringVector names = eventLog.getMessageClassNames().keys();
		for (int i = 0; i < names.size(); i++)
			messageClassNames.add(names.get(i));

		// message name filter
		panel = createTabItem(messageTabFolder, "Filter by name");
		label = new Label(panel, SWT.NONE);
		label.setText("The following message names have been encountered so far");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
		messageNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		names = eventLog.getMessageNames().keys();
		for (int i = 0; i < names.size(); i++)
			messageNames.add(names.get(i));

		// message id filter
		panel = createTabItem(messageTabFolder, "Filter by id");
		
		Object[] values = createEditableList(panel, "Trace messages with ids");
		enableMessageIdFilter = (Button)values[0];
		messageIds = (EditableList)values[1];
		
		values = createEditableList(panel, "Trace messages with tree ids");
		enableMessageTreeIdFilter = (Button)values[0];
		messageTreeIds = (EditableList)values[1];

		values = createEditableList(panel, "Trace messages with encapsulation ids");
		enableMessageEncapsulationIdFilter = (Button)values[0];
		messageEncapsulationIds = (EditableList)values[1];
		
		values = createEditableList(panel, "Trace messages with encapsulation tree ids");
		enableMessageEncapsulationTreeIdFilter = (Button)values[0];
		messageEncapsulationTreeIds = (EditableList)values[1];
	}

	private Object[] createEditableList(Composite panel, String label) {
		final Button enableButton = new Button(panel, SWT.CHECK);
		enableButton.setText(label);
		enableButton.setSelection(true);
		enableButton.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		final EditableList editableList = new EditableList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		GridData gridData = new GridData(SWT.FILL, SWT.BEGINNING, true, false);
		gridData.widthHint = 200;
		gridData.heightHint = 100;
		editableList.setLayoutData(gridData);

		enableButton.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				editableList.setEnabled(enableMessageFilter.getSelection() && enableButton.getSelection());
			}
		});
		
		enableMessageFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				editableList.setEnabled(enableMessageFilter.getSelection() && enableButton.getSelection());
				enableButton.setEnabled(enableMessageFilter.getSelection());
			}
		});

		return new Object[] {enableButton, editableList};
	}

	private void createEventTraceTabItem(TabFolder tabFolder) {
		Composite panel = createTabItem(tabFolder, "Cause/consequence filter");

		enableTraceFilter = new Button(panel, SWT.CHECK);
		enableTraceFilter.setText("Trace causes and/or consequences for a particular event");
		enableTraceFilter.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));
		enableTraceFilter.addSelectionListener(new SelectionAdapter() {
			@Override
			public void widgetSelected(SelectionEvent e) {
				boolean selection = enableTraceFilter.getSelection();
				tracedEventNumber.setEnabled(selection);
				traceCauses.setEnabled(selection);
				traceConsequences.setEnabled(selection);
				traceMessageReuses.setEnabled(selection);
				traceSelfMessages.setEnabled(selection);
				causeEventNumberDelta.setEnabled(selection);
				consequenceEventNumberDelta.setEnabled(selection);
				causeSimulationTimeDelta.setEnabled(selection);
				consequenceSimulationTimeDelta.setEnabled(selection);
			}
		});

		Label label = new Label(panel, SWT.NONE);
		label.setText("Event number to be traced");
		label.setToolTipText("An event which is neither cause nor consequence of this event will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		tracedEventNumber = new Text(panel, SWT.BORDER);
		tracedEventNumber.setToolTipText(label.getToolTipText());
		tracedEventNumber.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		traceCauses = new Button(panel, SWT.CHECK);
		traceCauses.setText("Include cause events");
		traceCauses.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		traceConsequences = new Button(panel, SWT.CHECK);
		traceConsequences.setText("Include consequence events");
		traceConsequences.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		traceMessageReuses = new Button(panel, SWT.CHECK);
		traceMessageReuses.setText("Follow message reuse dependencies");
		traceMessageReuses.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		traceSelfMessages = new Button(panel, SWT.CHECK);
		traceSelfMessages.setText("Follow self message dependencies");
		traceSelfMessages.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		// event number limits
		Group group = new Group(panel, SWT.NONE);
		group.setText("Event number limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		label = new Label(group, SWT.NONE);
		label.setText("Cause event number delta limit");
		label.setToolTipText("Cause events with event number delta greater than this will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		causeEventNumberDelta = new Text(group, SWT.BORDER);
		causeEventNumberDelta.setText("1000");
		causeEventNumberDelta.setToolTipText(label.getToolTipText());
		causeEventNumberDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Consequence event number delta limit");
		label.setToolTipText("Consequence events with event number delta greater than this will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		consequenceEventNumberDelta = new Text(group, SWT.BORDER);
		consequenceEventNumberDelta.setText("1000");
		consequenceEventNumberDelta.setToolTipText(label.getToolTipText());
		consequenceEventNumberDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// simulation time limits
		group = new Group(panel, SWT.NONE);
		group.setText("Simulation time limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		label = new Label(group, SWT.NONE);
		label.setText("Cause simulation time delta limit in seconds");
		label.setToolTipText("Cause events occured before this simulation time delta will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		causeSimulationTimeDelta = new Text(group, SWT.BORDER);
		causeSimulationTimeDelta.setToolTipText(label.getToolTipText());
		causeSimulationTimeDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Consequence simulation time delta limit in seconds");
		label.setToolTipText("Consequence events occured after this simulation time delta will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		consequenceSimulationTimeDelta = new Text(group, SWT.BORDER);
		consequenceSimulationTimeDelta.setToolTipText(label.getToolTipText());
		consequenceSimulationTimeDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
	}
}

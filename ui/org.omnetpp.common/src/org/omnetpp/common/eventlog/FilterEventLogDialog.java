package org.omnetpp.common.eventlog;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ITreeSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.common.ui.EditableList;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.eventlog.engine.IEventLog;
import org.omnetpp.eventlog.engine.ModuleCreatedEntry;
import org.omnetpp.eventlog.engine.ModuleCreatedEntryList;
import org.omnetpp.eventlog.engine.PStringVector;

@SuppressWarnings("unused")
public class FilterEventLogDialog
    extends TitleAreaDialog
{
	private EventLogInput eventLogInput;

	private EventLogFilterParameters filterParameters;
	
	private CheckboxTreeViewer panelCheckboxTree;

    private FilterDialogTreeNode enableCollectionLimits;

    private FilterDialogTreeNode enableEventNumberFilter;

	private FilterDialogTreeNode enableSimulationTimeFilter;

	private FilterDialogTreeNode enableModuleFilter;

    private FilterDialogTreeNode enableModuleExpressionFilter;

    private FilterDialogTreeNode enableModuleClassNameFilter;

    private FilterDialogTreeNode enableModuleNameFilter;

    private FilterDialogTreeNode enableModuleIdFilter;

    private FilterDialogTreeNode enableMessageFilter;
	
	private FilterDialogTreeNode enableMessageExpressionFilter;

    private FilterDialogTreeNode enableMessageClassNameFilter;

    private FilterDialogTreeNode enableMessageNameFilter;

    private FilterDialogTreeNode enableMessageIdFilter;

	private FilterDialogTreeNode enableMessageTreeIdFilter;

	private FilterDialogTreeNode enableMessageEncapsulationIdFilter;

	private FilterDialogTreeNode enableMessageEncapsulationTreeIdFilter;

    private FilterDialogTreeNode enableTraceFilter;

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

	private CheckboxTableViewer moduleClassNames;

	private ModuleTreeViewer moduleNameIds;

	private CheckboxTableViewer moduleIds;

	private Text messageFilterExpression;

	private CheckboxTableViewer messageClassNames;

	private CheckboxTableViewer messageNames;

	private EditableList messageIds;

	private EditableList messageTreeIds;

	private EditableList messageEncapsulationIds;

	private EditableList messageEncapsulationTreeIds;

    private Button collectMessageReuses;

    private Text maximumNumberOfMessageDependencies;

    private Text maximumDepthOfMessageDependencies;

    private String initialTreeNodeName;

	public FilterEventLogDialog(Shell parentShell, EventLogInput eventLogInput, EventLogFilterParameters filterParameters) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
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
	
					if (parameterFieldType == boolean.class) {
					    Object guiFieldValue = guiField.get(this);
					    boolean value = parameterField.getBoolean(filterParameters);
                        if (guiFieldValue instanceof Button)
                            unparseBoolean((Button)guiFieldValue, value);
                        else if (guiFieldValue instanceof FilterDialogTreeNode)
					        unparseBoolean((FilterDialogTreeNode)guiFieldValue, value);
                        else
                            throw new RuntimeException("Unknown gui field type");
					}
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
	
    private void unparseBoolean(FilterDialogTreeNode treeNode, boolean value) {
        panelCheckboxTree.setChecked(treeNode, value);
        treeNode.checkStateChanged(value);
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
			ArrayList<ModuleTreeItem> moduleTreeItems = new ArrayList<ModuleTreeItem>();
	
			for (int i = 0; i < values.length; i++) {
			    ModuleTreeItem item = eventLogInput.getModuleTreeRoot().findDescendantModule(values[i]);
			    
			    if (item != null)
			        moduleTreeItems.add(item);
			}
	
			moduleTreeViewer.setCheckedElements(moduleTreeItems.toArray());
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
	
					if (parameterFieldType == boolean.class) {
					    Object guiFieldValue = guiField.get(this);
					    
					    if (guiFieldValue instanceof Button)
					        parameterField.setBoolean(filterParameters, parseBoolean((Button)guiFieldValue));
					    else if (guiFieldValue instanceof FilterDialogTreeNode)
	                        parameterField.setBoolean(filterParameters, parseBoolean((FilterDialogTreeNode)guiFieldValue));
                        else
                            throw new RuntimeException("Unknown gui field type");
					}
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
	
    private boolean parseBoolean(FilterDialogTreeNode treeNode) {
        return panelCheckboxTree.getChecked(treeNode);
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
	
	public int open(String initialTreeNodeName) {
	    this.initialTreeNodeName = initialTreeNodeName;
	    return super.open();
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.heightHint = 400;
		container.setLayoutData(gridData);
		container.setLayout(new GridLayout(2, false));

		setHelpAvailable(false);
		setTitle("Select filter criteria");
		setMessage("The event log will be filtered for events that match all criteria");

		// create left hand side tree viewer
		panelCheckboxTree = new CheckboxTreeViewer(container);

		// create right hand side panel container
        final Composite panelContainer = new Composite(container, SWT.NONE);
        panelContainer.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        final StackLayout stackLayout = new StackLayout();
        panelContainer.setLayout(stackLayout);
        final Label defaultPanel = new Label(panelContainer, SWT.None);
        defaultPanel.setText("Please select an option from the tree on the left");
        stackLayout.topControl = defaultPanel;

        // create tree
        GenericTreeNode treeRoot = new GenericTreeNode("root");
        treeRoot.addChild(createCollectionLimitsTreeNode(panelContainer));
        treeRoot.addChild(createGeneralFilterTreeNode(panelContainer));
        treeRoot.addChild(createModuleFilterTreeNode(panelContainer));
        treeRoot.addChild(createMessageFilterTreeNode(panelContainer));
        treeRoot.addChild(createEventTraceFilterTreeNode(panelContainer));

        panelCheckboxTree.setContentProvider(new GenericTreeContentProvider());
        panelCheckboxTree.setInput(treeRoot);
        panelCheckboxTree.expandAll();
        panelCheckboxTree.getTree().setLayoutData(new GridData(SWT.BEGINNING, SWT.FILL, false, true));
        
        panelCheckboxTree.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                Object firstSelection = ((ITreeSelection)event.getSelection()).getFirstElement();
                
                if (firstSelection != null) {
                    FilterDialogTreeNode treeNode = (FilterDialogTreeNode)firstSelection;
                    
                    if (treeNode.getPanel() != null)
                        stackLayout.topControl = treeNode.getPanel();
                    else
                        stackLayout.topControl = defaultPanel;

                    panelContainer.layout();
                }
            }
        });
        
        panelCheckboxTree.addCheckStateListener(new ICheckStateListener() {
            public void checkStateChanged(CheckStateChangedEvent event) {
                GenericTreeNode treeNode = (GenericTreeNode)event.getElement();

                if (event.getChecked()) {
                    if (treeNode.getChildCount() != 0)
                        panelCheckboxTree.setChecked(treeNode, false);
                    else {
                        while (treeNode != null) {
                            if (treeNode instanceof FilterDialogTreeNode)
                                ((FilterDialogTreeNode)treeNode).checkStateChanged(true);
                            panelCheckboxTree.setChecked(treeNode, true);
                            treeNode = treeNode.getParent();
                        }
                    }
                }
                else
                    treeNodeDeselected(new GenericTreeNode[] {treeNode});
            }
            
            private void treeNodeDeselected(GenericTreeNode[] treeNodes) {
                for (GenericTreeNode treeNode : treeNodes) {
                    ((FilterDialogTreeNode)treeNode).checkStateChanged(false);
                    panelCheckboxTree.setChecked(treeNode, false);
                    treeNodeDeselected(treeNode.getChildren());
                }
            }
        });

        if (initialTreeNodeName != null) {
            for (GenericTreeNode treeNode : treeRoot.getChildren())
                if (treeNode.getPayload().equals(initialTreeNodeName))
                    panelCheckboxTree.setSelection(new TreeSelection(new TreePath(new Object[] {treeRoot, treeNode})));
        }

		unparseFilterParameters();

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

	private GenericTreeNode createCollectionLimitsTreeNode(Composite parent) {
        // depth and number limits
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableCollectionLimits = new FilterDialogTreeNode("Collection limits", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                collectMessageReuses.setEnabled(checked);
                maximumNumberOfMessageDependencies.setEnabled(checked);
                maximumDepthOfMessageDependencies.setEnabled(checked);
            }
        };

        collectMessageReuses = new Button(panel, SWT.CHECK);
        collectMessageReuses.setText("Collect message reuse dependencies");
        collectMessageReuses.setToolTipText("Message reuses will be followed when collecting dependencies between events far away on the consequence chain");
        collectMessageReuses.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

        Label label = new Label(panel, SWT.NONE);
        label.setText("Maximum number of message dependencies:");
        label.setToolTipText("Collecting message dependencies will stop at this limit for each event");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
        
        maximumNumberOfMessageDependencies = new Text(panel, SWT.BORDER);
        maximumNumberOfMessageDependencies.setToolTipText(label.getToolTipText());
        maximumNumberOfMessageDependencies.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

        label = new Label(panel, SWT.NONE);
        label.setText("Maximum depth of message dependencies:");
        label.setToolTipText("Collecting message dependencies will not look deeper into the cause/consequence chain than this limit");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

        maximumDepthOfMessageDependencies = new Text(panel, SWT.BORDER);
        maximumDepthOfMessageDependencies.setToolTipText(label.getToolTipText());
        maximumDepthOfMessageDependencies.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

        return enableCollectionLimits;
	}
	
	private GenericTreeNode createGeneralFilterTreeNode(Composite parent) {
	    // generic filter
	    Label label = new Label(parent, SWT.NONE);
        label.setText("Filter for a range of events");
	    FilterDialogTreeNode generalFilter = new FilterDialogTreeNode("General filter", label);

		// event number filter
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));

        generalFilter.addChild(enableEventNumberFilter = new FilterDialogTreeNode("by event numbers", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                lowerEventNumberLimit.setEnabled(checked);
                upperEventNumberLimit.setEnabled(checked);
            }
        });

		label = new Label(panel, SWT.NONE);
		label.setText("Lower event number limit:");
		label.setToolTipText("Events with event number less than the provided will be filtered out");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		lowerEventNumberLimit = new Text(panel, SWT.BORDER);
		lowerEventNumberLimit.setToolTipText(label.getToolTipText());
		lowerEventNumberLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(panel, SWT.NONE);
		label.setText("Upper event number limit:");
		label.setToolTipText("Events with event number greater than the provided will be filtered out");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		upperEventNumberLimit = new Text(panel, SWT.BORDER);
		upperEventNumberLimit.setToolTipText(label.getToolTipText());
		upperEventNumberLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// simulation time filter
		panel = new Composite(parent, SWT.NONE);
		panel.setLayout(new GridLayout(2, false));
        generalFilter.addChild(enableSimulationTimeFilter = new FilterDialogTreeNode("by simulation time", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                lowerSimulationTimeLimit.setEnabled(checked);
                upperSimulationTimeLimit.setEnabled(checked);
            }
        });
		
		label = new Label(panel, SWT.NONE);
		label.setText("Lower simulation time limit in seconds:");
		label.setToolTipText("Events occured before this simulation time will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		lowerSimulationTimeLimit = new Text(panel, SWT.BORDER);
		lowerSimulationTimeLimit.setToolTipText(label.getToolTipText());
		lowerSimulationTimeLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(panel, SWT.NONE);
		label.setText("Upper simulation time limit in seconds:");
		label.setToolTipText("Events occured after this simulation time will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		upperSimulationTimeLimit = new Text(panel, SWT.BORDER);
		upperSimulationTimeLimit.setToolTipText(label.getToolTipText());
		upperSimulationTimeLimit.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		return generalFilter;
	}

	private FilterDialogTreeNode createModuleFilterTreeNode(Composite parent) {
	    // synchorinze tree state first
        eventLogInput.synchronizeModuleTree();

        // module filter 
        Label label = new Label(parent, SWT.NONE);
        label.setText("Filter for events occured in any of the selected modules");
        enableModuleFilter = new FilterDialogTreeNode("Module filter", label);

        // expression filter
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableModuleFilter.addChild(enableModuleExpressionFilter = new FilterDialogTreeNode("by expression", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleFilterExpression.setEnabled(checked);
            }
        });

		label = new Label(panel, SWT.NONE);
		label.setText("Module filter expression:");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		moduleFilterExpression = new Text(panel, SWT.BORDER);
		moduleFilterExpression.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// module class name filter
        IEventLog eventLog = eventLogInput.getEventLog();
        panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableModuleFilter.addChild(enableModuleClassNameFilter = new FilterDialogTreeNode("by class name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleClassNames.getTable().setEnabled(checked);
            }
        });

        ModuleCreatedEntryList moduleCreatedEntryList = eventLog.getModuleCreatedEntries();
		Set<String> moduleClassNameSet = new HashSet<String>();
		for (int i = 0; i < moduleCreatedEntryList.size(); i++) {
			ModuleCreatedEntry moduleCreatedEntry = moduleCreatedEntryList.get(i);
			if (moduleCreatedEntry != null)
				moduleClassNameSet.add(moduleCreatedEntry.getModuleClassName());
		}

		String[] moduleClassNamesAsStrings = (String[])moduleClassNameSet.toArray(new String[0]);
		Collections.sort(Arrays.asList(moduleClassNamesAsStrings), StringUtils.dictionaryComparator);
		moduleClassNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		moduleClassNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		moduleClassNames.add(moduleClassNamesAsStrings);

		// module name filter
		panel = new Composite(parent, SWT.NONE);
		panel.setLayout(new GridLayout(2, false));
        enableModuleFilter.addChild(enableModuleNameFilter = new FilterDialogTreeNode("by name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleNameIds.getTree().setEnabled(checked);
            }
        });

		moduleNameIds = new ModuleTreeViewer(panel, eventLogInput.getModuleTreeRoot());
        moduleNameIds.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
        
        // module id filter
        panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableModuleFilter.addChild(enableModuleIdFilter = new FilterDialogTreeNode("by id", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleIds.getTable().setEnabled(checked);
            }
        });

        moduleIds = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		moduleIds.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		moduleIds.setLabelProvider(new LabelProvider() {
			public String getText(Object element) {
				ModuleTreeItem moduleTreeItem = eventLogInput.getModuleTreeRoot().findDescendantModule((Integer)element);
				
				return "(id = " + moduleTreeItem.getModuleId() + ") " + moduleTreeItem.getModuleFullPath();
			}
		});
		ModuleCreatedEntryList moduleCreatedEntries = eventLog.getModuleCreatedEntries();
		for (int i = 0; i < moduleCreatedEntries.size(); i++) {
			ModuleCreatedEntry moduleCreatedEntry = moduleCreatedEntries.get(i);
			
			if (moduleCreatedEntry != null)
				moduleIds.add(moduleCreatedEntry.getModuleId());
		}
		
		return enableModuleFilter;
	}

	private FilterDialogTreeNode createMessageFilterTreeNode(Composite parent) {
        // message filter 
        Label label = new Label(parent, SWT.NONE);
        label.setText("Filter for events processing a message where any of the following criteria holds");
        enableMessageFilter = new FilterDialogTreeNode("Message filter", label);

		// expression filter
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableMessageFilter.addChild(enableMessageExpressionFilter = new FilterDialogTreeNode("by expression", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageFilterExpression.setEnabled(checked);
            }
        });

        label = new Label(panel, SWT.NONE);
		label.setText("Message filter expression:");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		messageFilterExpression = new Text(panel, SWT.BORDER);
		messageFilterExpression.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		// message class name filter
        IEventLog eventLog = eventLogInput.getEventLog();
        panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableMessageFilter.addChild(enableMessageClassNameFilter = new FilterDialogTreeNode("by class name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageClassNames.getTable().setEnabled(checked);
            }
        });

		label = new Label(panel, SWT.NONE);
		label.setText("The following message class names have been encountered so far:");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));

        PStringVector names = eventLog.getMessageClassNames().keys();
        String[] messageClassNamesAsStrings = new String[(int)names.size()];
        for (int i = 0; i < names.size(); i++)
            messageClassNamesAsStrings[i] = names.get(i);
        Collections.sort(Arrays.asList(messageClassNamesAsStrings), StringUtils.dictionaryComparator);

        messageClassNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageClassNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		messageClassNames.add(messageClassNamesAsStrings);

		// message name filter
        panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableMessageFilter.addChild(enableMessageNameFilter = new FilterDialogTreeNode("by name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageNames.getTable().setEnabled(checked);
            }            
        });

        label = new Label(panel, SWT.NONE);
		label.setText("The following message names have been encountered so far:");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, 2, 1));
        
		names = eventLog.getMessageNames().keys();
		String[] messageNamesAsStrings = new String[(int)names.size()];
        for (int i = 0; i < names.size(); i++)
            messageNamesAsStrings[i] = names.get(i);
        Collections.sort(Arrays.asList(messageNamesAsStrings), StringUtils.dictionaryComparator);

        messageNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		messageNames.add(messageNamesAsStrings);

		// message id filter
		Object[] values = createEditableList(parent, "by id");
		enableMessageIdFilter = (FilterDialogTreeNode)values[0];
		messageIds = (EditableList)values[1];
		
		values = createEditableList(parent, "by tree id");
		enableMessageTreeIdFilter = (FilterDialogTreeNode)values[0];
		messageTreeIds = (EditableList)values[1];

		values = createEditableList(parent, "by encapsulation id");
		enableMessageEncapsulationIdFilter = (FilterDialogTreeNode)values[0];
		messageEncapsulationIds = (EditableList)values[1];
		
		values = createEditableList(parent, "by encapsulation tree id");
		enableMessageEncapsulationTreeIdFilter = (FilterDialogTreeNode)values[0];
		messageEncapsulationTreeIds = (EditableList)values[1];
		
		return enableMessageFilter;
	}

	private Object[] createEditableList(Composite parent, String label) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));

        final EditableList editableList = new EditableList(panel, SWT.NONE);
        editableList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));

        FilterDialogTreeNode treeNode = new FilterDialogTreeNode(label, panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                editableList.setEnabled(checked);
            }
        };
        enableMessageFilter.addChild(treeNode);
        
		return new Object[] {treeNode, editableList};
	}

	private FilterDialogTreeNode createEventTraceFilterTreeNode(Composite parent) {
        // trace filter 
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayout(new GridLayout(2, false));
        enableTraceFilter = new FilterDialogTreeNode("Cause/consequence filter", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                tracedEventNumber.setEnabled(checked);
                traceCauses.setEnabled(checked);
                traceConsequences.setEnabled(checked);
                traceMessageReuses.setEnabled(checked);
                traceSelfMessages.setEnabled(checked);
                causeEventNumberDelta.setEnabled(checked);
                consequenceEventNumberDelta.setEnabled(checked);
                causeSimulationTimeDelta.setEnabled(checked);
                consequenceSimulationTimeDelta.setEnabled(checked);
            }
        };


        Label label = new Label(panel, SWT.NONE);
        label.setText("Trace causes and/or consequences for a particular event");
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.BEGINNING, true, false, 2, 1));

		label = new Label(panel, SWT.NONE);
		label.setText("Event number to be traced:");
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
		label.setText("Cause event number delta limit:");
		label.setToolTipText("Cause events with event number delta greater than this will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		causeEventNumberDelta = new Text(group, SWT.BORDER);
		causeEventNumberDelta.setText("1000");
		causeEventNumberDelta.setToolTipText(label.getToolTipText());
		causeEventNumberDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Consequence event number delta limit:");
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
		label.setText("Cause simulation time delta limit in seconds:");
		label.setToolTipText("Cause events occured before this simulation time delta will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));
		
		causeSimulationTimeDelta = new Text(group, SWT.BORDER);
		causeSimulationTimeDelta.setToolTipText(label.getToolTipText());
		causeSimulationTimeDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));

		label = new Label(group, SWT.NONE);
		label.setText("Consequence simulation time delta limit in seconds:");
		label.setToolTipText("Consequence events occured after this simulation time delta will be filtered out from the result");
		label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false));

		consequenceSimulationTimeDelta = new Text(group, SWT.BORDER);
		consequenceSimulationTimeDelta.setToolTipText(label.getToolTipText());
		consequenceSimulationTimeDelta.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		
		return enableTraceFilter;
	}
	
	private class FilterDialogTreeNode extends GenericTreeNode {
	    private Control panel;

	    public FilterDialogTreeNode(Object payload, Control panel) {
            super(payload);
            this.panel = panel;
        }
	    
	    public Control getPanel() {
            return panel;
        }
	    
	    public void checkStateChanged(boolean checked) {
	    }
	}
}

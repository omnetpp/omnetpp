package org.omnetpp.common.eventlog;

import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.jface.dialogs.IInputValidator;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ITreeSelection;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.TreeSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.ControlAdapter;
import org.eclipse.swt.events.ControlEvent;
import org.eclipse.swt.events.ModifyEvent;
import org.eclipse.swt.events.ModifyListener;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.ScrollBar;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.eventlog.EventLogFilterParameters.EnabledInt;
import org.omnetpp.common.ui.AbstractEditableList;
import org.omnetpp.common.ui.EditableCheckboxList;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.eventlog.engine.BeginSendEntry;
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

    private FilterDialogTreeNode enableRangeFilter;

    private FilterDialogTreeNode enableEventNumberFilter;

	private FilterDialogTreeNode enableSimulationTimeFilter;

	private FilterDialogTreeNode enableModuleFilter;

    private FilterDialogTreeNode enableModuleExpressionFilter;

    private FilterDialogTreeNode enableModuleNEDTypeNameFilter;

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

	private CheckboxTableViewer moduleNEDTypeNames;

	private ModuleTreeViewer moduleNameIds;

	private CheckboxTableViewer moduleIds;

	private Text messageFilterExpression;

	private CheckboxTableViewer messageClassNames;

	private CheckboxTableViewer messageNames;

	private AbstractEditableList messageIds;

	private AbstractEditableList messageTreeIds;

	private AbstractEditableList messageEncapsulationIds;

	private AbstractEditableList messageEncapsulationTreeIds;

    private Button collectMessageReuses;

    private Text maximumDepthOfMessageDependencies;

    private Text maximumNumberOfMessageDependencies;

    private Text maximumMessageDependencyCollectionTime;

    private Text filterDescription;

    private String initialTreeNodeName;

	public FilterEventLogDialog(Shell parentShell, EventLogInput eventLogInput, EventLogFilterParameters filterParameters) {
		super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
		this.eventLogInput = eventLogInput;
		this.filterParameters = filterParameters;
	}

	private void unparseFilterParameters(EventLogFilterParameters filterParameters) {
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
                    else if (parameterFieldType == long.class)
                        unparseLong((Text)guiField.get(this), parameterField.getLong(filterParameters));
					else if (parameterFieldType == BigDecimal.class)
						unparseBigDecimal((Text)guiField.get(this), (BigDecimal)parameterField.get(filterParameters));
					else if (parameterFieldType == String.class)
						unparseString((Text)guiField.get(this), (String)parameterField.get(filterParameters));
					else if (parameterFieldType == int[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof AbstractEditableList)
							unparseIntArray((AbstractEditableList)guiControl, (int[])parameterField.get(filterParameters));
						else if (guiControl instanceof CheckboxTableViewer)
							unparseIntArray((CheckboxTableViewer)guiControl, (int[])parameterField.get(filterParameters));
						else if (guiControl instanceof AbstractEditableList)
                            unparseLongArray((AbstractEditableList)guiControl, (long[])parameterField.get(filterParameters));
                        else if (guiControl instanceof CheckboxTableViewer)
                            unparseLongArray((CheckboxTableViewer)guiControl, (long[])parameterField.get(filterParameters));
						else if (guiControl instanceof ModuleTreeViewer)
							unparseModuleNameIdArray((ModuleTreeViewer)guiControl, (int[])parameterField.get(filterParameters));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else if (parameterFieldType == String[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof AbstractEditableList)
							unparseStringArray((AbstractEditableList)guiField.get(this), (String[])parameterField.get(filterParameters));
						else if (guiControl instanceof CheckboxTableViewer)
							unparseStringArray((CheckboxTableViewer)guiControl, (String[])parameterField.get(filterParameters));
						else
							throw new RuntimeException("Unknown gui field type");
					}
                    else if (parameterFieldType == EnabledInt[].class) {
                        unparseEnabledIntArray((EditableCheckboxList)guiField.get(this), (EnabledInt[])parameterField.get(filterParameters));
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
	
    private void unparseLong(Text text, long value) {
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

	private void unparseIntArray(AbstractEditableList editableList, int[] values) {
		if (values != null) {
			String[] stringValues = new String[values.length];

			for (int i = 0; i < values.length; i++)
				stringValues[i] = String.valueOf(values[i]);
			
			editableList.setItems(stringValues);
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

    private void unparseLongArray(AbstractEditableList editableList, long[] values) {
        if (values != null) {
            String[] stringValues = new String[values.length];

            for (int i = 0; i < values.length; i++)
                stringValues[i] = String.valueOf(values[i]);
            
            editableList.setItems(stringValues);
        }
    }

    private void unparseLongArray(CheckboxTableViewer checkboxTableViewer, long[] values) {
        if (values != null) {
            Long[] longValues = new Long[values.length];

            for (int i = 0; i < values.length; i++)
                longValues[i] = values[i];

            checkboxTableViewer.setCheckedElements(longValues);
        }
    }

    private void unparseEnabledIntArray(EditableCheckboxList editableCheckboxList, EnabledInt[] enabledInts) {
        if (enabledInts != null) {
            for (EnabledInt enabledInt : enabledInts) {
                String value = String.valueOf(enabledInt.value);
                editableCheckboxList.add(value);
                editableCheckboxList.setChecked(value, enabledInt.enabled);
            }
        }
    }

	private void unparseStringArray(AbstractEditableList editableList, String[] values) {
		if (values != null)
			editableList.setItems(values);
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

	public void parseFilterParameters(EventLogFilterParameters filterParameters) {
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
                    else if (parameterFieldType == long.class)
                        parameterField.setLong(filterParameters, parseLong((Text)guiField.get(this)));
					else if (parameterFieldType == BigDecimal.class)
						parameterField.set(filterParameters, parseBigDecimal((Text)guiField.get(this)));
					else if (parameterFieldType == String.class)
						parameterField.set(filterParameters, parseString((Text)guiField.get(this)));
					else if (parameterFieldType == int[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof AbstractEditableList)
							parameterField.set(filterParameters, parseIntArray((AbstractEditableList)guiControl));
                        else if (guiControl instanceof CheckboxTableViewer)
                            parameterField.set(filterParameters, parseIntArray((CheckboxTableViewer)guiField.get(this)));
                        else if (guiControl instanceof AbstractEditableList)
                            parameterField.set(filterParameters, parseLongArray((AbstractEditableList)guiControl));
                        else if (guiControl instanceof CheckboxTableViewer)
                            parameterField.set(filterParameters, parseLongArray((CheckboxTableViewer)guiField.get(this)));
						else if (guiControl instanceof ModuleTreeViewer)
							parameterField.set(filterParameters, parseModuleNameIdArray((ModuleTreeViewer)guiField.get(this)));
						else
							throw new RuntimeException("Unknown gui field type");
					}
					else if (parameterFieldType == String[].class) {
						Object guiControl = guiField.get(this);

						if (guiControl instanceof AbstractEditableList)
							parameterField.set(filterParameters, parseStringArray((AbstractEditableList)guiField.get(this)));
						else if (guiControl instanceof CheckboxTableViewer)
							parameterField.set(filterParameters, parseModuleNEDTypeNameArray((CheckboxTableViewer)guiField.get(this)));
						else
							throw new RuntimeException("Unknown gui field type");
					}
                    else if (parameterFieldType == EnabledInt[].class) {
                        parameterField.set(filterParameters, parseEnabledIntArray((EditableCheckboxList)guiField.get(this)));
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
	
    private long parseLong(Text text) {
        if (text.getText().length() != 0)
            return Long.parseLong(text.getText());
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
	
	private int[] parseIntArray(AbstractEditableList editableList) {
		String[] stringValues = editableList.getItems();
		int[] intValues = new int[stringValues.length];

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

    private long[] parseLongArray(AbstractEditableList editableList) {
        String[] stringValues = editableList.getItems();
        long[] longValues = new long[stringValues.length];

        for (int i = 0; i < stringValues.length; i++)
            longValues[i] = Long.parseLong(stringValues[i]);

        return longValues;
    }
    
    private long[] parseLongArray(CheckboxTableViewer checkBoxTableViewer) {
        Object[] elements = checkBoxTableViewer.getCheckedElements();
        long[] values = new long[elements.length];
        
        for (int i = 0; i < elements.length; i++)
            values[i] = (Long)elements[i];
        
        return values;
    }

    private EnabledInt[] parseEnabledIntArray(EditableCheckboxList editableCheckboxList) {
        String[] stringValues = editableCheckboxList.getItems();
        Collections.sort(Arrays.asList(stringValues), StringUtils.dictionaryComparator);
        EnabledInt[] enabledInts = new EnabledInt[stringValues.length];

        for (int i = 0; i < stringValues.length; i++) {
            String stringValue = stringValues[i];
            enabledInts[i] = new EnabledInt(editableCheckboxList.getChecked(stringValue), Integer.parseInt(stringValue));
        }

        return enabledInts;
    }

	private String[] parseStringArray(AbstractEditableList editableList) {
		return editableList.getItems();
	}
	
	private int[] parseModuleNameIdArray(ModuleTreeViewer moduleTreeViewer) {
		Object[] treeItems = moduleTreeViewer.getCheckedElements();
		int[] values = new int[treeItems.length];

		for (int i = 0; i < values.length; i++)
			values[i] = ((ModuleTreeItem)treeItems[i]).getModuleId();
		
		return values;
	}

	private String[] parseModuleNEDTypeNameArray(CheckboxTableViewer checkBoxTableViewer) {
		Object[] elements = checkBoxTableViewer.getCheckedElements();
		String[] moduleNEDTypeNames = new String[elements.length];
		
		for (int i = 0; i < elements.length; i++)
			moduleNEDTypeNames[i] = (String)elements[i];
		
		return moduleNEDTypeNames;
	}
	
	public int open(String initialTreeNodeName) {
	    this.initialTreeNodeName = initialTreeNodeName;
	    return super.open();
	}

	@Override
	protected Control createDialogArea(Composite parent) {
		final Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
		GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
		gridData.heightHint = 400;
		container.setLayoutData(gridData);
		container.setLayout(new GridLayout(2, false));

		setHelpAvailable(false);
		setTitle("Select filter criteria");
		setMessage("The event log will be filtered for events that match the following criteria");

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
        treeRoot.addChild(createRangeFilterTreeNode(panelContainer));
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
                GenericTreeNode clickedTreeNode = (GenericTreeNode)event.getElement();

                if (event.getChecked()) {
                    GenericTreeNode treeNode = clickedTreeNode; 

                    // ignore non leave nodes
                    if (treeNode.getChildCount() != 0)
                        panelCheckboxTree.setChecked(treeNode, false);
                    else {
                        // ensure path to root is checked
                        while (treeNode != null) {
                            if (treeNode instanceof FilterDialogTreeNode)
                                ((FilterDialogTreeNode)treeNode).checkStateChanged(true);
                            panelCheckboxTree.setChecked(treeNode, true);
                            treeNode = treeNode.getParent();
                        }
                    }
                }
                else {
                    // uncheck subtree
                    GenericTreeNode treeNode = clickedTreeNode; 
                    treeNodeUnchecked(new GenericTreeNode[] {treeNode});

                    // uncheck path to root if no other siblings are checked
                    while (treeNode != null) {
                        boolean found = false;
                        for (GenericTreeNode childNode : treeNode.getChildren()) { 
                            if (panelCheckboxTree.getChecked(childNode)) {
                                found = true;
                                break;
                            }
                        }

                        if (!found)
                            panelCheckboxTree.setChecked(treeNode, false);

                        treeNode = treeNode.getParent();
                    }
                }

                // also, select the clicked node and display its page
                panelCheckboxTree.setSelection(new StructuredSelection(event.getElement()));
                if (clickedTreeNode instanceof FilterDialogTreeNode && ((FilterDialogTreeNode)clickedTreeNode).getPanel() != null) {
                    stackLayout.topControl = ((FilterDialogTreeNode)clickedTreeNode).getPanel();
                    panelContainer.layout();
                }
                
                updateFilterDescription();
            }
            
            private void treeNodeUnchecked(GenericTreeNode[] treeNodes) {
                for (GenericTreeNode treeNode : treeNodes) {
                    ((FilterDialogTreeNode)treeNode).checkStateChanged(false);
                    panelCheckboxTree.setChecked(treeNode, false);
                    treeNodeUnchecked(treeNode.getChildren());
                }
            }
        });

        if (initialTreeNodeName != null) {
            for (GenericTreeNode treeNode : treeRoot.getChildren())
                if (treeNode.getPayload().equals(initialTreeNodeName))
                    panelCheckboxTree.setSelection(new TreeSelection(new TreePath(new Object[] {treeRoot, treeNode})));
        }

		unparseFilterParameters(filterParameters);

		filterDescription = new Text(container, SWT.WRAP | SWT.V_SCROLL);
        final GridData filterDescriptionGridData = new GridData(SWT.BEGINNING, SWT.BEGINNING, false, false, 2, 1);
        filterDescriptionGridData.widthHint = 0;
        filterDescriptionGridData.heightHint = JFaceResources.getDefaultFont().getFontData()[0].getHeight() * 8;
        filterDescription.setLayoutData(filterDescriptionGridData);
        filterDescription.setEditable(false);
        filterDescription.setText(filterParameters.toString());

        container.addControlListener(new ControlAdapter() {
            public void controlResized(ControlEvent e) {
                ScrollBar verticalBar = filterDescription.getVerticalBar();
                int scrollBarWidth = verticalBar == null ? 0 : verticalBar.getSize().x;
                int marginWidth = ((GridLayout)(container.getLayout())).marginWidth;
                filterDescriptionGridData.widthHint = container.getSize().x - marginWidth * 3 - scrollBarWidth;
                container.layout(true);
            }
        });

        return container;
	}

	@Override
	protected void configureShell(Shell newShell) {
		newShell.setText("Filter event log");
		super.configureShell(newShell);
	}
	
	@Override
	protected void okPressed() {
		parseFilterParameters(filterParameters);
		super.okPressed();
	}
	
	private void addFilterDescriptionListener(Text text) {
        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                EventLogFilterParameters filterParameters = new EventLogFilterParameters(eventLogInput);
                unparseFilterParameters(filterParameters);
                filterDescription.setText(filterParameters.toString());
            }
        });
	}

	private GenericTreeNode createCollectionLimitsTreeNode(Composite parent) {
        // depth and number limits
        Composite panel = createPanel(parent, "Collection Limits", "Here you can specify limits when searching for message dependencies.", 2);
        enableCollectionLimits = new FilterDialogTreeNode("Collection limits", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                collectMessageReuses.setEnabled(checked);
                maximumDepthOfMessageDependencies.setEnabled(checked);
                maximumNumberOfMessageDependencies.setEnabled(checked);
                maximumMessageDependencyCollectionTime.setEnabled(checked);
            }
        };

        collectMessageReuses = createCheckbox(panel, "Collect message reuse dependencies", "Message reuses will be followed when collecting dependencies between events far away on the consequence chain", 2);

        Label label = createLabel(panel, "Maximum depth of message dependencies:", "Collecting message dependencies will not look deeper into the cause/consequence chain than this limit (separately for each event)", 1);
        maximumDepthOfMessageDependencies = createText(panel, label.getToolTipText(), 1);
        
        label = createLabel(panel, "Maximum number of message dependencies:", "Collecting message dependencies will stop at this limit for each event", 1);
        maximumNumberOfMessageDependencies = createText(panel, label.getToolTipText(), 1);

        label = createLabel(panel, "Maximum message dependency collection time:", "Collecting message dependencies will stop after the specified amount of time (in milliseconds) for each event", 1);
        maximumMessageDependencyCollectionTime = createText(panel, label.getToolTipText(), 1);

        return enableCollectionLimits;
	}
	
	private GenericTreeNode createRangeFilterTreeNode(Composite parent) {
	    // generic filter
	    Composite panel0 = createPanel(parent, "Range", "Choose subcategories to limit the eventlog to a range of event numbers or simulation times.", 1);
	    enableRangeFilter = new FilterDialogTreeNode("Range", panel0);

		// event number filter
        Composite panel = createPanel(parent, "Event Number Range", "When enabled, events within the given event number range will be considered.", 2);

        enableRangeFilter.addChild(enableEventNumberFilter = new FilterDialogTreeNode("by event numbers", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                lowerEventNumberLimit.setEnabled(checked);
                upperEventNumberLimit.setEnabled(checked);
            }
        });

		Label label = createLabel(panel, "Lower event number limit:", "Events with event number less than the provided will be filtered out", 1);
		lowerEventNumberLimit = createText(panel, label.getToolTipText(), 1);

		label = createLabel(panel, "Upper event number limit:", "Events with event number greater than the provided will be filtered out", 1);
		upperEventNumberLimit = createText(panel, label.getToolTipText(), 1);

		// simulation time filter
		panel = createPanel(parent, "Simulation Time Range", "When enabled, events within the given simulation time range will be considered.", 2);
		enableRangeFilter.addChild(enableSimulationTimeFilter = new FilterDialogTreeNode("by simulation time", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                lowerSimulationTimeLimit.setEnabled(checked);
                upperSimulationTimeLimit.setEnabled(checked);
            }
        });
		
		label = createLabel(panel, "Lower simulation time limit in seconds:", "Events occured before this simulation time will be filtered out from the result", 1);
		lowerSimulationTimeLimit = createText(panel, label.getToolTipText(), 1);

		label = createLabel(panel, "Upper simulation time limit in seconds:", "Events occured after this simulation time will be filtered out from the result", 1);
		upperSimulationTimeLimit = createText(panel, label.getToolTipText(), 1);
		
		return enableRangeFilter;
	}

	private FilterDialogTreeNode createModuleFilterTreeNode(Composite parent) {
	    // synchronize module tree state first
        eventLogInput.synchronizeModuleTree();

        // module filter 
        Composite panel1 = createPanel(parent, "Module Filter", "Choose subcategories to filter to events that occurred in any of the selected modules.", 1);
        enableModuleFilter = new FilterDialogTreeNode("Module filter", panel1);

        // expression filter
        Composite panel = createPanel(parent, "Module Filter Expression", "When enabled, events in modules that match the expression will be considered.", 2);
        enableModuleFilter.addChild(enableModuleExpressionFilter = new FilterDialogTreeNode("by expression", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleFilterExpression.setEnabled(checked);
            }
        });

		Label label = createLabel(panel, "Expression:", null, 1);
		moduleFilterExpression = createTextWithProposals(panel, "A Match Expression for the Raw data present in the eventlog lines (MC) where modules are created", 1, ModuleCreatedEntry.class);

		// module class name filter
        IEventLog eventLog = eventLogInput.getEventLog();
        panel = createPanel(parent, "Filter by Module Type", "When enabled, modules with the selected NED types will be considered.", 2);
        enableModuleFilter.addChild(enableModuleNEDTypeNameFilter = new FilterDialogTreeNode("by NED type", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleNEDTypeNames.getTable().setEnabled(checked);
            }
        });

        ModuleCreatedEntryList moduleCreatedEntryList = eventLog.getModuleCreatedEntries();
		Set<String> moduleNEDTypeNameSet = new HashSet<String>();
		for (int i = 0; i < moduleCreatedEntryList.size(); i++) {
			ModuleCreatedEntry moduleCreatedEntry = moduleCreatedEntryList.get(i);
			if (moduleCreatedEntry != null)
				moduleNEDTypeNameSet.add(moduleCreatedEntry.getNedTypeName());
		}

		String[] moduleNEDTypeNamesAsStrings = (String[])moduleNEDTypeNameSet.toArray(new String[0]);
		Collections.sort(Arrays.asList(moduleNEDTypeNamesAsStrings), StringUtils.dictionaryComparator);
		moduleNEDTypeNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		moduleNEDTypeNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		moduleNEDTypeNames.add(moduleNEDTypeNamesAsStrings);
		moduleNEDTypeNames.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateFilterDescription();
            }
		});

		// module name filter
		panel = createPanel(parent, "Filter by Module Name", "When enabled, modules with the selected names will be considered.", 2);
        enableModuleFilter.addChild(enableModuleNameFilter = new FilterDialogTreeNode("by name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                moduleNameIds.getTree().setEnabled(checked);
            }
        });

		moduleNameIds = new ModuleTreeViewer(panel, eventLogInput.getModuleTreeRoot());
        moduleNameIds.getTree().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
        moduleNameIds.addCheckStateListener(new ICheckStateListener() {
            public void checkStateChanged(CheckStateChangedEvent event) {
                updateFilterDescription();
            }
        });
        
        // module id filter
        panel = createPanel(parent, "Filter by Module IDs", "When enabled, modules with the selected IDs will be considered.", 2);
        enableModuleFilter.addChild(enableModuleIdFilter = new FilterDialogTreeNode("by ID", panel) {
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
        moduleIds.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateFilterDescription();
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
        Composite panel0 = createPanel(parent, "Message Filter", "Choose subcategories to filter to events processing or sending any of the selected messages.",  1);
        enableMessageFilter = new FilterDialogTreeNode("Message filter", panel0);

		// expression filter
        Composite panel = createPanel(parent, "Message Filter Exression", "When enabled, messages that match the filter expression will be considered.", 2);
        enableMessageFilter.addChild(enableMessageExpressionFilter = new FilterDialogTreeNode("by expression", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageFilterExpression.setEnabled(checked);
            }
        });

        Label label = createLabel(panel, "Expression:", null, 1);
		messageFilterExpression = createTextWithProposals(panel, "A Match Expression for the Raw data present in the eventlog lines (BS) where messages are sent", 1, BeginSendEntry.class);

		// message class name filter
        IEventLog eventLog = eventLogInput.getEventLog();
        panel = createPanel(parent, "Filter by Message Class", "When enabled, messages of the selected classes will be considered.", 2);
        enableMessageFilter.addChild(enableMessageClassNameFilter = new FilterDialogTreeNode("by class name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageClassNames.getTable().setEnabled(checked);
            }
        });

		label = createLabel(panel, "Message classes encountered so far:", null, 2);

        PStringVector names = eventLog.getMessageClassNames().keys();
        String[] messageClassNamesAsStrings = new String[(int)names.size()];
        for (int i = 0; i < names.size(); i++)
            messageClassNamesAsStrings[i] = names.get(i);
        Collections.sort(Arrays.asList(messageClassNamesAsStrings), StringUtils.dictionaryComparator);

        messageClassNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageClassNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		messageClassNames.add(messageClassNamesAsStrings);
        messageClassNames.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateFilterDescription();
            }
        });

		// message name filter
        panel = createPanel(parent, "Filter by Message Name", "When enabled, messages with the selected names will be considered.", 2);
        enableMessageFilter.addChild(enableMessageNameFilter = new FilterDialogTreeNode("by name", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
                messageNames.getTable().setEnabled(checked);
            }            
        });

        label = createLabel(panel, "Message names encountered so far:", null, 2);
        
		names = eventLog.getMessageNames().keys();
		String[] messageNamesAsStrings = new String[(int)names.size()];
        for (int i = 0; i < names.size(); i++)
            messageNamesAsStrings[i] = names.get(i);
        Collections.sort(Arrays.asList(messageNamesAsStrings), StringUtils.dictionaryComparator);

        messageNames = CheckboxTableViewer.newCheckList(panel, SWT.MULTI | SWT.BORDER | SWT.V_SCROLL);
		messageNames.getTable().setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 2, 1));
		messageNames.add(messageNamesAsStrings);
        messageNames.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateFilterDescription();
            }
        });

		// message id filter
		Object[] values = createPanelWithEditableList(parent, "ID");
		enableMessageIdFilter = (FilterDialogTreeNode)values[0];
		messageIds = (AbstractEditableList)values[1];
		
		values = createPanelWithEditableList(parent, "tree ID");
		enableMessageTreeIdFilter = (FilterDialogTreeNode)values[0];
		messageTreeIds = (AbstractEditableList)values[1];

		values = createPanelWithEditableList(parent, "encapsulation ID");
		enableMessageEncapsulationIdFilter = (FilterDialogTreeNode)values[0];
		messageEncapsulationIds = (AbstractEditableList)values[1];
		
		values = createPanelWithEditableList(parent, "encapsulation tree ID");
		enableMessageEncapsulationTreeIdFilter = (FilterDialogTreeNode)values[0];
		messageEncapsulationTreeIds = (AbstractEditableList)values[1];
		
		return enableMessageFilter;
	}

	private Object[] createPanelWithEditableList(Composite parent, String label) {
        Composite panel = createPanel(parent, "Filter by message " + label, "When enabled, messages with the selected " + label + "s will be considered.", 2);

        final EditableCheckboxList editableList = new EditableCheckboxList(panel, SWT.NONE);
        editableList.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        editableList.setAddDialogMessage("Please enter the ID");
        editableList.setInputValidator(new IInputValidator() {
            public String isValid(String newText) {
                try {
                    Integer.parseInt(newText);
                    return null;
                }
                catch (NumberFormatException e) {
                    return "Not a valid integer";
                }
            }
        });
        editableList.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                updateFilterDescription();
            }
        });

        FilterDialogTreeNode treeNode = new FilterDialogTreeNode("by " + label, panel) {
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
        Composite panel = createPanel(parent, "Cause/Consequence Filter", "When enabled, only the selected event and its causes/consequences will be considered.", 2);
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


		Label label = createLabel(panel, "Event number to be traced:", "An event which is neither cause nor consequence of this event will be filtered out from the result", 1);
		tracedEventNumber = createText(panel, label.getToolTipText(), 1);
		traceCauses = createCheckbox(panel, "Include cause events", null, 2);
		traceConsequences = createCheckbox(panel, "Include consequence events", null, 2);
		traceMessageReuses = createCheckbox(panel, "Follow message reuse dependencies", null, 2);
		traceSelfMessages = createCheckbox(panel, "Follow self-message dependencies", null, 2);

		// event number limits
		Group group = new Group(panel, SWT.NONE);
		group.setText("Event number limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		label = createLabel(group, "Cause event number delta limit:", "Cause events with event number delta greater than this will be filtered out from the result", 1);
		causeEventNumberDelta = createText(group, label.getToolTipText(), 1);
        causeEventNumberDelta.setText("1000");

		label = createLabel(group, "Consequence event number delta limit:", "Consequence events with event number delta greater than this will be filtered out from the result", 1);
		consequenceEventNumberDelta = createText(group, label.getToolTipText(), 1);
		consequenceEventNumberDelta.setText("1000");

		// simulation time limits
		group = new Group(panel, SWT.NONE);
		group.setText("Simulation time limits");
		group.setLayout(new GridLayout(2, false));
		group.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, 2, 1));

		label = createLabel(group, "Cause simulation time delta limit in seconds:", "Cause events occured before this simulation time delta will be filtered out from the result", 1);
		causeSimulationTimeDelta = createText(group, label.getToolTipText(), 1);

		label = createLabel(group, "Consequence simulation time delta limit in seconds:", "Consequence events occured after this simulation time delta will be filtered out from the result", 1);
		consequenceSimulationTimeDelta = createText(group, label.getToolTipText(), 1);
		
		return enableTraceFilter;
	}

    protected Composite createPanel(Composite parent, String title, String description, int numColumns) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        panel.setLayout(new GridLayout(numColumns, false));
        
        Label titleLabel = new Label(panel, SWT.NONE);
        titleLabel.setFont(JFaceResources.getBannerFont());
        titleLabel.setText(title);
        titleLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false, numColumns, 1));

        if (description != null) {
            Label descriptionLabel = new Label(panel, SWT.WRAP);
            descriptionLabel.setText(description);
            descriptionLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false, numColumns, 1));
        }

        Label separator = new Label(panel, SWT.HORIZONTAL | SWT.SEPARATOR);
        separator.setLayoutData(new GridData(SWT.FILL, SWT.CENTER, true, false, numColumns, 1));

        return panel;
    }

	protected Label createLabel(Composite parent, String text, String tooltip, int hspan) {
        Label label = new Label(parent, SWT.NONE);
        label.setText(text);
        label.setToolTipText(tooltip);
        label.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        return label;
	}
	
	protected Button createCheckbox(Composite parent, String text, String tooltip, int hspan) {
	    Button checkbox = new Button(parent, SWT.CHECK);
	    checkbox.setText(text);
	    checkbox.setToolTipText(tooltip);
	    checkbox.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, false, false, hspan, 1));
        checkbox.addSelectionListener(new SelectionListener() {
            public void widgetDefaultSelected(SelectionEvent e) {
            }

            public void widgetSelected(SelectionEvent e) {
                updateFilterDescription();
            }
        });

	    return checkbox;
	}

	protected Text createText(Composite parent, String tooltip, int hspan) {
	    Text text = new Text(parent, SWT.BORDER);
	    text.setToolTipText(tooltip);
	    text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, hspan, 1));
        text.addModifyListener(new ModifyListener() {
            public void modifyText(ModifyEvent e) {
                updateFilterDescription();
            }
        });

	    return text;
	}
	
    protected Text createTextWithProposals(Composite parent, String tooltip, int hspan, Class<?> clazz) {
        final Text text = createText(parent, tooltip, hspan);
        ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(text, new TextContentAdapter(), new EventLogEntryProposalProvider(clazz),
            ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, "( ".toCharArray(), true);
        commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
        commandAdapter.addContentProposalListener(new IContentProposalListener() {
            public void proposalAccepted(IContentProposal proposal) {
                ContentProposal contentProposal = (ContentProposal)proposal;
                int start = contentProposal.getStartIndex();
                int end =contentProposal.getEndIndex();
                int cursorPosition = contentProposal.getCursorPosition();
                String content = contentProposal.getContent();
                text.setSelection(start, end);
                text.insert(content);
                text.setSelection(start + cursorPosition, start + cursorPosition);
            }
        });
        return text;
    }
    
    protected void updateFilterDescription() {
        if (filterDescription != null) {
            EventLogFilterParameters filterParameters = new EventLogFilterParameters(eventLogInput);
            parseFilterParameters(filterParameters);
            filterDescription.setText(filterParameters.toString());
        }
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

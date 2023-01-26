package org.omnetpp.scave.preferences;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPreferencePage;
import org.omnetpp.common.engine.QuantityFormatter;
import org.omnetpp.common.ui.TableLabelProvider;
import org.omnetpp.common.ui.TableTextCellEditor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ui.QuantityFormatterOptionsConfigurationDialog;
import org.omnetpp.scave.editors.ui.QuantityFormatterRegistry;
import org.omnetpp.scave.editors.ui.QuantityFormatterRegistry.Mapping;

/**
 * Preference page for Scave.
 */
public class QuantityFormattersPreferencePage extends PreferencePage implements IWorkbenchPreferencePage {
    private static final int BUTTON_TOP         = 0;
    private static final int BUTTON_UP          = 1;
    private static final int BUTTON_DOWN        = 2;
    private static final int BUTTON_BOTTOM      = 3;
    private static final int BUTTON_ADD         = 4;
    private static final int BUTTON_COPY        = 5;
    private static final int BUTTON_REMOVE      = 6;
    private static final int BUTTON_EDIT        = 7;

    private List<QuantityFormatterRegistry.Mapping> originalMappings;
    private List<QuantityFormatterRegistry.Mapping> mappings;
    private IStructuredContentProvider contentProvider;
    private TableViewer tableViewer;
    private int widthInChars = 150;
    private int heightInChars = 15;

    public QuantityFormattersPreferencePage() {
        setPreferenceStore(ScavePlugin.getDefault().getPreferenceStore());
        setTitle("OMNeT++/Number Formats");
        setDescription("Number Formats for the OMNeT++ Result Analysis editor.");
    }

    @Override
    public void init(IWorkbench workbench) {
        setMappings(QuantityFormatterRegistry.getInstance().getMappings());
    }

    protected void setMappings(List<QuantityFormatterRegistry.Mapping> originalMappings) {
        this.originalMappings = originalMappings;
        mappings = new ArrayList<QuantityFormatterRegistry.Mapping>();
        for (QuantityFormatterRegistry.Mapping mapping : originalMappings)
            mappings.add(new QuantityFormatterRegistry.Mapping(mapping.expression, new QuantityFormatter.Options(mapping.options), mapping.testInput));
    }

    @Override
    protected Control createContents(Composite parent) {
        setTitle("Modify quantity formatting configuration");
        setMessage("Please update the ordered list of quantity formatting options.\nThe first matching options will be used when formatting a quantity.");

// TODO This expression is matched against the context object related to the quantity that is being formatted. Available fields are: display, column, unit, module, name, file, run, config, experiment, measurement, replication, etc. Available columns are: value, count, mean, min, max, stddev, variance, etc.
//        Composite panel = createPanel(parent, "Filter expression for quantity formatting options", "Here you can specify the filter expression for the selected quantity formatting options.\nYou can use object field names, boolean operators (and/or/not), and optional wildcards.\nFor example, 'unit=~bps and column=~min and module=~*.transmitter'.\nSee the tooltip on the text field for the complete list of available fields.", 2);

        Composite tableArea = new Composite(parent, SWT.NONE);
        tableArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(2, false);
        layout.marginWidth = layout.marginHeight = 0;
        tableArea.setLayout(layout);

        tableViewer = new TableViewer(tableArea, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER);
        Table table = tableViewer.getTable();
        table.setLinesVisible(true);
        table.setHeaderVisible(true);
        addTableColumn(table, "Filter expression", 600);
        addTableColumn(table, "Test input", 160);
        addTableColumn(table, "Test output", 160);
        contentProvider = new ArrayContentProvider();
        tableViewer.setContentProvider(contentProvider);

        tableViewer.setColumnProperties(new String[] {"expression", "input", "output"});
        final TableTextCellEditor[] cellEditors = new TableTextCellEditor[] {new TableTextCellEditor(tableViewer, 0), new TableTextCellEditor(tableViewer, 1), null};
        tableViewer.setCellEditors(cellEditors);

        tableViewer.setCellModifier(new ICellModifier() {
            public boolean canModify(Object element, String property) {
                return property.equals("expression") || property.equals("input");
            }

            public Object getValue(Object element, String property) {
                QuantityFormatterRegistry.Mapping mapping = (QuantityFormatterRegistry.Mapping)element;
                if (property.equals("expression"))
                    return mapping.expression;
                else if (property.equals("input"))
                    return mapping.testInput;
                else
                    throw new IllegalArgumentException();
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment
                QuantityFormatterRegistry.Mapping mapping = (QuantityFormatterRegistry.Mapping)element;
                if (property.equals("expression"))
                    mapping.setExpression(value.toString());
                else if (property.equals("input"))
                    mapping.testInput = value.toString();
                else
                    throw new IllegalArgumentException();
                tableViewer.refresh();
            }
        });

//        IContentProposalProvider valueProposalProvider = new MatchExpressionContentProposalProvider() {
//            @Override
//            protected List<IContentProposal> getFieldNameProposals(String prefix) {
//                return null;
//            }
//
//            @Override
//            protected List<IContentProposal> getDefaultFieldValueProposals(String prefix) {
//                return null;
//            }
//
//            @Override
//            protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix) {
//                return null;
//            }
//        };
//        ContentAssistUtil.configureTableColumnContentAssist(tableViewer, 0, valueProposalProvider, null, true);

        tableViewer.setContentProvider(contentProvider);
        tableViewer.setLabelProvider(new TableLabelProvider() {
            @Override
            public String getColumnText(Object element, int columnIndex) {
                QuantityFormatterRegistry.Mapping mapping = (QuantityFormatterRegistry.Mapping)element;
                switch (columnIndex) {
                    case 0:
                        return mapping.expression;
                    case 1:
                        return StringUtils.nullToEmpty(mapping.testInput);
                    case 2:
                        return StringUtils.nullToEmpty(mapping.computeTestOutput());
                    default:
                        throw new RuntimeException();
                }
            }
        });

        tableViewer.setInput(mappings);
        GridData gd = new GridData(GridData.FILL_BOTH);
        gd.heightHint = convertHeightInCharsToPixels(heightInChars);
        gd.widthHint = convertWidthInCharsToPixels(widthInChars);
        table.setLayoutData(gd);
        table.setFont(parent.getFont());

        // buttons
        Composite buttonsArea = new Composite(tableArea, SWT.NONE);
        buttonsArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        buttonsArea.setLayout(new GridLayout(1, false));

        createSideButton(buttonsArea, BUTTON_TOP, "&Top");
        createSideButton(buttonsArea, BUTTON_UP, "&Up");
        createSideButton(buttonsArea, BUTTON_DOWN, "&Down");
        createSideButton(buttonsArea, BUTTON_BOTTOM, "&Bottom");
        new Label(buttonsArea, SWT.NONE);
        createSideButton(buttonsArea, BUTTON_ADD, "&Add");
        createSideButton(buttonsArea, BUTTON_COPY, "&Copy");
        createSideButton(buttonsArea, BUTTON_REMOVE, "&Remove");
        createSideButton(buttonsArea, BUTTON_EDIT, "&Edit");

        return parent;
    }

    private Button createSideButton(Composite parent, final int buttonID, String label) {
        Button button = new Button(parent, SWT.PUSH);
        button.setText(label);
        button.setFont(JFaceResources.getDialogFont());
        button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        button.addSelectionListener(new SelectionAdapter() {
            @Override
            public void widgetSelected(SelectionEvent event) {
                buttonPressed(buttonID);
            }
        });
        return button;
    }

    protected Composite createPanel(Composite parent, String title, String description, int numColumns) {
        Composite panel = new Composite(parent, SWT.NONE);
        panel.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        panel.setLayout(new GridLayout(numColumns, false));

        if (title != null) {
            Label titleLabel = new Label(panel, SWT.NONE);
            titleLabel.setFont(JFaceResources.getBannerFont());
            titleLabel.setText(title);
            titleLabel.setLayoutData(new GridData(SWT.BEGINNING, SWT.CENTER, true, false, numColumns, 1));
        }

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
        return checkbox;
    }

    protected Text createText(Composite parent, String tooltip, int hspan) {
        Text text = new Text(parent, SWT.BORDER);
        text.setToolTipText(tooltip);
        text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, hspan, 1));
        return text;
    }

    @Override
    public boolean performOk() {
        originalMappings.clear();
        for (Mapping mapping : mappings)
            originalMappings.add(mapping);
        QuantityFormatterRegistry.getInstance().save(getPreferenceStore());
        return true;
    }

    protected void buttonPressed(int buttonId) {
        StructuredSelection selection = (StructuredSelection)tableViewer.getSelection();
        List<?> selectionAsList = selection.toList();
        Object firstSelectionElement = selection.isEmpty() ? null : selectionAsList.get(0);
        Object lastSelectionElement = selection.isEmpty() ? null : selectionAsList.get(selectionAsList.size() - 1);
        switch (buttonId) {
            case BUTTON_TOP:
                if (!selection.isEmpty())
                    move(selectionAsList, -mappings.indexOf(firstSelectionElement));
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_UP:
                if (!selection.isEmpty() && mappings.indexOf(firstSelectionElement) > 0)
                    move(selectionAsList, -1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_DOWN:
                if (!selection.isEmpty() && mappings.indexOf(lastSelectionElement) < mappings.size() - 1)
                    move(selectionAsList, 1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_BOTTOM:
                if (!selection.isEmpty())
                    move(selectionAsList, mappings.size() - mappings.indexOf(lastSelectionElement) - 1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_REMOVE:
                int index = mappings.indexOf(selection.getFirstElement());
                for (var selected : selectionAsList)
                    mappings.remove(selected);
                if (index != -1 && !mappings.isEmpty())
                    tableViewer.setSelection(new StructuredSelection(mappings.get(Math.max(0, Math.min(index, mappings.size() - 1)))));
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_COPY: {
                if (selection.size() == 1) {
                    QuantityFormatterRegistry.Mapping selectedMapping = ((QuantityFormatterRegistry.Mapping)selection.getFirstElement());
                    QuantityFormatterRegistry.Mapping mapping = new QuantityFormatterRegistry.Mapping(selectedMapping.expression, new QuantityFormatter.Options(selectedMapping.options), selectedMapping.testInput);
                    mappings.add(mapping);
                    tableViewer.refresh();
                    tableViewer.setSelection(new StructuredSelection(mapping), true);
                }
                break;
            }
            case BUTTON_ADD: {
                QuantityFormatterRegistry.Mapping mapping = new QuantityFormatterRegistry.Mapping("*", new QuantityFormatter.Options(), null);
                mappings.add(mapping);
                tableViewer.refresh();
                tableViewer.setSelection(new StructuredSelection(mapping), true);
                break;
            }
            case BUTTON_EDIT:
                if (selection.size() == 1) {
                    QuantityFormatterOptionsConfigurationDialog dialog = new QuantityFormatterOptionsConfigurationDialog(Display.getCurrent().getActiveShell());
                    QuantityFormatterRegistry.Mapping mapping = ((QuantityFormatterRegistry.Mapping)selection.getFirstElement());
                    dialog.setOptions(mapping.options);
                    dialog.open();
                }
                break;
        }
    }

    private void move(List<?> selection, int delta) {
        QuantityFormatterRegistry.Mapping[] movedCurrentAxisModuleOrder = new QuantityFormatterRegistry.Mapping[mappings.size()];

        for (int i = 0; i < mappings.size(); i++) {
            QuantityFormatterRegistry.Mapping element = mappings.get(i);

            if (selection.contains(element))
                movedCurrentAxisModuleOrder[i + delta] = element;
        }

        for (QuantityFormatterRegistry.Mapping element : mappings) {
            if (!selection.contains(element)) {
                for (int j = 0; j < movedCurrentAxisModuleOrder.length; j++) {
                    if (movedCurrentAxisModuleOrder[j] == null) {
                        movedCurrentAxisModuleOrder[j] = element;
                        break;
                    }
                }
            }
        }

        mappings.clear();
        mappings.addAll(Arrays.asList(movedCurrentAxisModuleOrder));

        tableViewer.refresh();

        if (delta < 0)
            tableViewer.reveal(selection.get(0));
        else if (delta > 0)
            tableViewer.reveal(selection.get(selection.size() - 1));
    }

    protected TableColumn addTableColumn(Table table, String label, int width) {
        TableColumn column = new TableColumn(table, SWT.NONE);
        column.setText(label);
        column.pack();
        if (column.getWidth() < width)
            column.setWidth(width);
        return column;
    }


}
package org.omnetpp.scave.preferences;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.preference.PreferencePage;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTableViewer;
import org.eclipse.jface.viewers.ICellModifier;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ICheckStateProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.StructuredSelection;
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
import org.omnetpp.scave.editors.ui.QuantityFormattingRule;
import org.omnetpp.scave.editors.ui.QuantityFormatterOptionsConfigurationDialog;
import org.omnetpp.scave.editors.ui.QuantityFormatterRegistry;

/**
 * Preference page for Scave.
 */
public class QuantityFormattersPreferencePage extends PreferencePage implements IWorkbenchPreferencePage {
    private static final String P_TESTER = "tester";
    private static final String P_EXPRESSION = "expression";
    private static final String P_NAME = "name";
    private static final String DEFAULT_NAME = "New";
    private static final String DEFAULT_MATCHEXPRESSION = "*";
    private static final String DEFAULT_TESTINPUT = "1234.5678ms";

    private static final int BUTTON_TOP         = 0;
    private static final int BUTTON_UP          = 1;
    private static final int BUTTON_DOWN        = 2;
    private static final int BUTTON_BOTTOM      = 3;
    private static final int BUTTON_ADD         = 4;
    private static final int BUTTON_COPY        = 5;
    private static final int BUTTON_REMOVE      = 6;
    private static final int BUTTON_EDIT        = 7;

    private List<QuantityFormattingRule> rules;
    private IStructuredContentProvider contentProvider;
    private CheckboxTableViewer tableViewer;
    //private int widthInChars = 150;
    private int heightInChars = 15;

    public QuantityFormattersPreferencePage() {
        setPreferenceStore(ScavePlugin.getDefault().getPreferenceStore());
        setTitle("Number Formats for the OMNeT++ Result Analysis editor");
        setDescription("Multiple formatting rules can be specified, and the first one whose filter matches the item will be used to format it.");
    }

    @Override
    public void init(IWorkbench workbench) {
        setRules(QuantityFormatterRegistry.getInstance().getRules());
    }

    protected void setRules(List<QuantityFormattingRule> originalRules) {
        rules = new ArrayList<QuantityFormattingRule>();
        for (QuantityFormattingRule rule : originalRules)
            rules.add(rule.getCopy());
    }

    @Override
    protected Control createContents(Composite parent) {
        Composite tableArea = new Composite(parent, SWT.NONE);
        tableArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(2, false);
        layout.marginWidth = layout.marginHeight = 0;
        tableArea.setLayout(layout);

        Table table = new Table(tableArea, SWT.CHECK | SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER);
        tableViewer = new CheckboxTableViewer(table);
        table.setLinesVisible(true);
        table.setHeaderVisible(true);
        addTableColumn(table, "Enabled", 20);
        addTableColumn(table, "Name", 200);
        addTableColumn(table, "Applies where", 200);
        addTableColumn(table, "Example output", 200);
        contentProvider = new ArrayContentProvider();
        tableViewer.setContentProvider(contentProvider);

        tableViewer.setColumnProperties(new String[] {"", P_NAME, P_EXPRESSION, P_TESTER});
        final TableTextCellEditor[] cellEditors = new TableTextCellEditor[] {
                null,
                new TableTextCellEditor(tableViewer, 1),
                new TableTextCellEditor(tableViewer, 2),
                new TableTextCellEditor(tableViewer, 3),
                null
        };
        tableViewer.setCellEditors(cellEditors);

        tableViewer.setCellModifier(new ICellModifier() {
            public boolean canModify(Object element, String property) {
                return property.equals(P_NAME) || property.equals(P_EXPRESSION) || property.equals(P_TESTER);
            }

            public Object getValue(Object element, String property) {
                QuantityFormattingRule rule = (QuantityFormattingRule)element;
                if (property.equals(P_NAME))
                    return rule.name;
                else if (property.equals(P_EXPRESSION))
                    return rule.expression;
                else if (property.equals(P_TESTER))
                    return rule.testInput;
                else
                    throw new IllegalArgumentException();
            }

            public void modify(Object element, String property, Object value) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment
                QuantityFormattingRule rule = (QuantityFormattingRule)element;
                if (property.equals(P_NAME))
                    rule.name = value.toString();
                else if (property.equals(P_EXPRESSION))
                    rule.setExpression(value.toString());
                else if (property.equals(P_TESTER))
                    rule.testInput = value.toString();
                else
                    throw new IllegalArgumentException();
                tableViewer.refresh();
            }
        });

        tableViewer.setCheckStateProvider(new ICheckStateProvider() {
            @Override
            public boolean isGrayed(Object element) {
                return false;
            }

            @Override
            public boolean isChecked(Object element) {
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment
                QuantityFormattingRule rule = (QuantityFormattingRule)element;
                return rule.enabled;
            }
        });

        tableViewer.addCheckStateListener(new ICheckStateListener() {
            @Override
            public void checkStateChanged(CheckStateChangedEvent event) {
                Object element = event.getElement();
                if (element instanceof Item)
                    element = ((Item) element).getData(); // workaround, see super's comment
                QuantityFormattingRule rule = (QuantityFormattingRule)element;
                rule.enabled = event.getChecked();
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
                QuantityFormattingRule rule = (QuantityFormattingRule)element;
                switch (columnIndex) {
                    case 0:
                        return ""; // the dummy "enabled" column
                    case 1:
                        return StringUtils.nullToEmpty(rule.name);
                    case 2:
                        return rule.expression;
                    case 3:
                        return StringUtils.nullToEmpty(rule.computeTestOutput());
                    default:
                        throw new RuntimeException();
                }
            }
        });

        tableViewer.setInput(rules);
        GridData gd = new GridData(GridData.FILL_BOTH);
        gd.heightHint = convertHeightInCharsToPixels(heightInChars);
        gd.widthHint = 600 + convertWidthInCharsToPixels(10); // sum of column widths and then some
        table.setLayoutData(gd);
        table.setFont(parent.getFont());

        // buttons
        Composite buttonsArea = new Composite(tableArea, SWT.NONE);
        buttonsArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        buttonsArea.setLayout(new GridLayout(1, false));

        createSideButton(buttonsArea, BUTTON_ADD, "&Add");
        createSideButton(buttonsArea, BUTTON_EDIT, "&Edit");
        createSideButton(buttonsArea, BUTTON_COPY, "&Copy");
        createSideButton(buttonsArea, BUTTON_REMOVE, "&Remove");
        new Label(buttonsArea, SWT.NONE);
        //createSideButton(buttonsArea, BUTTON_TOP, "&Top");
        createSideButton(buttonsArea, BUTTON_UP, "&Up");
        createSideButton(buttonsArea, BUTTON_DOWN, "&Down");
        //createSideButton(buttonsArea, BUTTON_BOTTOM, "&Bottom");

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
        int catchAllIndex = findFirstCatchAll(rules);
        if (catchAllIndex == -1) {
            MessageDialog.openInformation(getShell(), "Note", "Rules saved. Note that a catch-all rule was added at the last position.");
            rules.add(new QuantityFormattingRule("Default", true, "", new QuantityFormatter.Options(), DEFAULT_TESTINPUT));
            tableViewer.refresh(); // in case we are invoked on Apply
        }
        else if (catchAllIndex < rules.size()-1) {
            MessageDialog.openInformation(getShell(), "Note", "Rules saved. Note that rules that follow the catch-all rule "
                    + "\"" + rules.get(catchAllIndex).name + "\" will never be used.");
        }

        QuantityFormatterRegistry.getInstance().setRules(rules);
        QuantityFormatterRegistry.getInstance().save(getPreferenceStore());
        return true;
    }

    private static boolean isCatchAll(QuantityFormattingRule rule) {
        return rule.enabled && (StringUtils.isEmpty(rule.expression) || rule.expression.equals("*"));
    }

    private static int findFirstCatchAll(List<QuantityFormattingRule> rules) {
        for (int i = 0; i < rules.size(); i++)
            if (isCatchAll(rules.get(i)))
                return i;
        return -1;
    }

    protected void buttonPressed(int buttonId) {
        StructuredSelection selection = (StructuredSelection)tableViewer.getSelection();
        List<?> selectionAsList = selection.toList();
        Object firstSelectionElement = selection.isEmpty() ? null : selectionAsList.get(0);
        Object lastSelectionElement = selection.isEmpty() ? null : selectionAsList.get(selectionAsList.size() - 1);
        switch (buttonId) {
            case BUTTON_TOP:
                if (!selection.isEmpty())
                    move(selectionAsList, -rules.indexOf(firstSelectionElement));
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_UP:
                if (!selection.isEmpty() && rules.indexOf(firstSelectionElement) > 0)
                    move(selectionAsList, -1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_DOWN:
                if (!selection.isEmpty() && rules.indexOf(lastSelectionElement) < rules.size() - 1)
                    move(selectionAsList, 1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_BOTTOM:
                if (!selection.isEmpty())
                    move(selectionAsList, rules.size() - rules.indexOf(lastSelectionElement) - 1);
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_REMOVE:
                int index = rules.indexOf(selection.getFirstElement());
                for (var selected : selectionAsList)
                    rules.remove(selected);
                if (index != -1 && !rules.isEmpty())
                    tableViewer.setSelection(new StructuredSelection(rules.get(Math.max(0, Math.min(index, rules.size() - 1)))));
                tableViewer.getTable().setFocus();
                tableViewer.refresh();
                break;
            case BUTTON_COPY: {
                if (selection.size() == 1) {
                    QuantityFormattingRule selectedRule = ((QuantityFormattingRule)selection.getFirstElement());
                    QuantityFormattingRule rule = selectedRule.getCopy();
                    rules.add(rule);
                    tableViewer.refresh();
                    tableViewer.setSelection(new StructuredSelection(rule), true);
                }
                break;
            }
            case BUTTON_ADD: {
                QuantityFormattingRule rule = new QuantityFormattingRule(DEFAULT_NAME, true, DEFAULT_MATCHEXPRESSION, new QuantityFormatter.Options(), DEFAULT_TESTINPUT);
                rules.add(rule);
                tableViewer.refresh();
                tableViewer.setSelection(new StructuredSelection(rule), true);
                break;
            }
            case BUTTON_EDIT:
                if (selection.size() == 1) {
                    QuantityFormatterOptionsConfigurationDialog dialog = new QuantityFormatterOptionsConfigurationDialog(Display.getCurrent().getActiveShell());
                    QuantityFormattingRule rule = ((QuantityFormattingRule)selection.getFirstElement());
                    dialog.setOptions(rule);
                    dialog.open();
                    tableViewer.refresh();
                }
                break;
        }
    }

    private void move(List<?> selection, int delta) {
        QuantityFormattingRule[] movedRules = new QuantityFormattingRule[rules.size()];

        for (int i = 0; i < rules.size(); i++) {
            QuantityFormattingRule element = rules.get(i);

            if (selection.contains(element))
                movedRules[i + delta] = element;
        }

        for (QuantityFormattingRule element : rules) {
            if (!selection.contains(element)) {
                for (int j = 0; j < movedRules.length; j++) {
                    if (movedRules[j] == null) {
                        movedRules[j] = element;
                        break;
                    }
                }
            }
        }

        rules.clear();
        rules.addAll(Arrays.asList(movedRules));

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
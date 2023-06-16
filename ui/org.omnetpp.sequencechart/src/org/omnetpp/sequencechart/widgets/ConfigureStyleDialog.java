/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.TitleAreaDialog;
import org.eclipse.jface.fieldassist.ContentProposalAdapter;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalListener;
import org.eclipse.jface.fieldassist.TextContentAdapter;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.jface.viewers.CheckStateChangedEvent;
import org.eclipse.jface.viewers.CheckboxTreeViewer;
import org.eclipse.jface.viewers.ColumnLabelProvider;
import org.eclipse.jface.viewers.EditingSupport;
import org.eclipse.jface.viewers.ICheckStateListener;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ITreeSelection;
import org.eclipse.jface.viewers.SelectionChangedEvent;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.TableViewerColumn;
import org.eclipse.jface.viewers.TextCellEditor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StackLayout;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.fieldassist.ContentAssistCommandAdapter;
import org.eclipse.ui.texteditor.ITextEditorActionDefinitionIds;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.eventlog.EventLogEntryProposalProvider;
import org.omnetpp.common.ui.GenericTreeContentProvider;
import org.omnetpp.common.ui.GenericTreeLabelProvider;
import org.omnetpp.common.ui.GenericTreeNode;
import org.omnetpp.common.ui.ListContentProvider;
import org.omnetpp.common.util.Pair;
import org.omnetpp.common.util.UIUtils;
import org.omnetpp.eventlog.EventLogEntry;

public class ConfigureStyleDialog
    extends TitleAreaDialog
{
    protected SequenceChartSettings sequenceChartSettings;

    protected CheckboxTreeViewer panelCheckboxTree;

    protected DialogTreeNode colorFilter;

    protected DialogTreeNode axesColorFilter;

    protected DialogTreeNode axesHeaderColorFilter;

    protected DialogTreeNode eventColorFilter;

    protected DialogTreeNode selfMessageEventColorFilter;

    protected DialogTreeNode messageSendColorFilter;

    protected DialogTreeNode componentMethodCallColorFilter;

    protected TableViewer axesColor;

    protected TableViewer axesHeaderColor;

    protected TableViewer eventColor;

    protected TableViewer selfMessageEventColor;

    protected TableViewer messageSendColor;

    protected TableViewer componentMethodCallColor;

    public ConfigureStyleDialog(Shell parentShell, SequenceChartSettings sequenceChartSettings) {
        super(parentShell);
        setShellStyle(getShellStyle() | SWT.RESIZE);
        this.sequenceChartSettings = sequenceChartSettings;
    }

    public SequenceChartSettings getSequenceChartSettings() {
        return sequenceChartSettings;
    }

    @Override
    protected IDialogSettings getDialogBoundsSettings() {
        return UIUtils.getDialogSettings(CommonPlugin.getDefault(), getClass().getName());
    }

    public int open(String initialTreeNodeName) {
        return super.open();
    }

    @Override
    protected Control createDialogArea(Composite parent) {
        final Composite container = new Composite((Composite)super.createDialogArea(parent), SWT.NONE);
        GridData gridData = new GridData(SWT.FILL, SWT.FILL, true, true);
        gridData.widthHint = 800;
        gridData.heightHint = 600;
        container.setLayoutData(gridData);
        container.setLayout(new GridLayout(2, false));

        setHelpAvailable(false);
        setTitle("Configure style");
        setMessage("The sequence chart content will be stylized based on the following filter criteria");

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

        GenericTreeNode treeRoot = new GenericTreeNode("root");
        treeRoot.addChild(createColorTreeNode(panelContainer));

        panelCheckboxTree.setContentProvider(new GenericTreeContentProvider());
        panelCheckboxTree.setLabelProvider(new GenericTreeLabelProvider());
        panelCheckboxTree.setInput(treeRoot);
        panelCheckboxTree.expandAll();
        panelCheckboxTree.getTree().setLayoutData(new GridData(SWT.BEGINNING, SWT.FILL, false, true));


        panelCheckboxTree.addSelectionChangedListener(new ISelectionChangedListener() {
            public void selectionChanged(SelectionChangedEvent event) {
                Object firstSelection = ((ITreeSelection)event.getSelection()).getFirstElement();

                if (firstSelection != null) {
                    DialogTreeNode treeNode = (DialogTreeNode)firstSelection;

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
                            if (treeNode instanceof DialogTreeNode)
                                ((DialogTreeNode)treeNode).checkStateChanged(true);
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
                if (clickedTreeNode instanceof DialogTreeNode && ((DialogTreeNode)clickedTreeNode).getPanel() != null) {
                    stackLayout.topControl = ((DialogTreeNode)clickedTreeNode).getPanel();
                    panelContainer.layout();
                }

//                updateFilterDescription();
            }

            protected void treeNodeUnchecked(GenericTreeNode[] treeNodes) {
                for (GenericTreeNode treeNode : treeNodes) {
                    ((DialogTreeNode)treeNode).checkStateChanged(false);
                    panelCheckboxTree.setChecked(treeNode, false);
                    treeNodeUnchecked(treeNode.getChildren());
                }
            }
        });

        unparseStyleParameters(sequenceChartSettings);

        return container;
    }

    @Override
    protected void configureShell(Shell newShell) {
        newShell.setText("Configure style");
        super.configureShell(newShell);
    }

    @Override
    protected void okPressed() {
        parseStyleParameters(sequenceChartSettings);
        super.okPressed();
    }

    protected void unparseStyleParameters(SequenceChartSettings sequenceChartSettings) {
        axesColor.setInput(unparseColorFallback(sequenceChartSettings.axesColorFallback));
        axesHeaderColor.setInput(unparseColorFallback(sequenceChartSettings.axesHeaderColorFallback));
        eventColor.setInput(unparseColorFallback(sequenceChartSettings.eventColorFallback));
        selfMessageEventColor.setInput(unparseColorFallback(sequenceChartSettings.selfMessageEventColorFallback));
        messageSendColor.setInput(unparseColorFallback(sequenceChartSettings.messageSendColorFallback));
        componentMethodCallColor.setInput(unparseColorFallback(sequenceChartSettings.componentMethodCallColorFallback));
    }

    protected void unparseBoolean(DialogTreeNode treeNode, boolean value) {
        panelCheckboxTree.setChecked(treeNode, value);
        treeNode.checkStateChanged(value);
    }

    protected ArrayList<Pair<String, String>> unparseColorFallback(Pair<String, String[]>[] fallback) {
        ArrayList<Pair<String, String>> input = new ArrayList<Pair<String, String>>();
        if (fallback != null) {
            for (Pair<String, String[]> pair : fallback) {
                StringBuilder colors = new StringBuilder();
                for (String color : pair.second) {
                    if (colors.length() != 0)
                        colors.append(", ");
                    colors.append(color);
                }
                input.add(new Pair<String, String>(pair.first, colors.toString()));
            }
        }
        return input;
    }

    protected void parseStyleParameters(SequenceChartSettings sequenceChartSettings) {
        sequenceChartSettings.axesColorFallback = parseColorFallback(axesColor);
        sequenceChartSettings.axesHeaderColorFallback = parseColorFallback(axesHeaderColor);
        sequenceChartSettings.eventColorFallback = parseColorFallback(eventColor);
        sequenceChartSettings.selfMessageEventColorFallback = parseColorFallback(selfMessageEventColor);
        sequenceChartSettings.messageSendColorFallback = parseColorFallback(messageSendColor);
        sequenceChartSettings.componentMethodCallColorFallback = parseColorFallback(componentMethodCallColor);
    }

    @SuppressWarnings("unchecked")
    private Pair<String, String[]>[] parseColorFallback(TableViewer tableViewer) {
        ArrayList<Pair<String, String>> input = (ArrayList<Pair<String, String>>)tableViewer.getInput();
        Pair<String, String[]>[] messageSendColorFallback = new Pair[input.size()];
        for (int i = 0; i < input.size(); i++) {
            Pair<String, String> element = input.get(i);
            messageSendColorFallback[i] = new Pair<String, String[]>(null, null);
            messageSendColorFallback[i].first = element.first;
            messageSendColorFallback[i].second = element.second.replaceAll(" ", "").split(",");
        }
        return messageSendColorFallback;
    }

    protected boolean parseBoolean(DialogTreeNode treeNode) {
        return panelCheckboxTree.getChecked(treeNode);
    }

    protected GenericTreeNode createColorTreeNode(Composite parent) {
        Composite panel0 = createPanel(parent, "Color filter", "Choose subcategories to colorize the content of the sequence chart.", 1);
        colorFilter = new DialogTreeNode("Coloring", panel0);

        Composite panel = createPanel(parent, "Axes", "When enabled, axes are colorized.", 2);
        colorFilter.addChild(axesColorFilter = new DialogTreeNode("Axes", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        axesColor = createFallback(panel);

        panel = createPanel(parent, "Axis headers", "When enabled, axes headers are colorized.", 2);
        colorFilter.addChild(axesHeaderColorFilter = new DialogTreeNode("Axis headers", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        axesHeaderColor = createFallback(panel);

        panel = createPanel(parent, "Events", "When enabled, events are colorized.", 2);
        colorFilter.addChild(eventColorFilter = new DialogTreeNode("Events", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        eventColor = createFallback(panel);

        panel = createPanel(parent, "Self events", "When enabled, self message events are colorized.", 2);
        colorFilter.addChild(selfMessageEventColorFilter = new DialogTreeNode("Self events", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        selfMessageEventColor = createFallback(panel);

        panel = createPanel(parent, "Message sends", "When enabled, message sends are colorized.", 2);
        colorFilter.addChild(messageSendColorFilter = new DialogTreeNode("Message sends", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        messageSendColor = createFallback(panel);

        panel = createPanel(parent, "Method calls", "When enabled, method calls are colorized.", 2);
        colorFilter.addChild(componentMethodCallColorFilter = new DialogTreeNode("Method calls", panel) {
            @Override
            public void checkStateChanged(boolean checked) {
            }
        });
        componentMethodCallColor = createFallback(panel);

        return colorFilter;
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

    protected TableViewer createFallback(Composite parent) {
        TableViewer tableViewer = new TableViewer(parent, SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER);
        tableViewer.setContentProvider(new ListContentProvider());
        TableViewerColumn expressionColumn = createTableViewerColumn(tableViewer, "Expression", 300);
        expressionColumn.setLabelProvider(new ColumnLabelProvider() {
            @SuppressWarnings("unchecked")
            @Override
            public String getText(Object element) {
                return ((Pair<String, String>)element).first;
            }
        });
        expressionColumn.setEditingSupport(new EditingSupport(tableViewer) {
            @SuppressWarnings("unchecked")
            @Override
            protected void setValue(Object element, Object value) {
                ((Pair<String, String>)element).first = value.toString();
                getViewer().update(element, null);
            }

            @SuppressWarnings("unchecked")
            @Override
            protected Object getValue(Object element) {
                return ((Pair<String, String>)element).first;
            }

            @Override
            protected CellEditor getCellEditor(Object element) {
                return new TextCellEditor((Composite)getViewer().getControl());
            }

            @Override
            protected boolean canEdit(Object element) {
                return true;
            }
        });
        TableViewerColumn colorsColumn = createTableViewerColumn(tableViewer, "Colors", 200);
        colorsColumn.setLabelProvider(new ColumnLabelProvider() {
            @SuppressWarnings("unchecked")
            @Override
            public String getText(Object element) {
                return ((Pair<String, String>)element).second;
            }
        });
        colorsColumn.setEditingSupport(new EditingSupport(tableViewer) {
            @SuppressWarnings("unchecked")
            @Override
            protected void setValue(Object element, Object value) {
                ((Pair<String, String>)element).second = value.toString();
                getViewer().update(element, null);
            }

            @SuppressWarnings("unchecked")
            @Override
            protected Object getValue(Object element) {
                return ((Pair<String, String>)element).second;
            }

            @Override
            protected CellEditor getCellEditor(Object element) {
                return new TextCellEditor((Composite)getViewer().getControl());
            }

            @Override
            protected boolean canEdit(Object element) {
                return true;
            }
        });
        Table table = tableViewer.getTable();
        table.setHeaderVisible(true);
        table.setLinesVisible(true);
        table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true, 1, 1));
        Composite buttons = new Composite(parent, SWT.NONE);
        buttons.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, false, 1, 1));
        buttons.setLayout(new GridLayout(1, false));
        Button button = createButton(buttons, "&Add");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                Pair<String, String> element = new Pair<String, String>("", "");
                input.add(element);
                tableViewer.refresh();
                tableViewer.setSelection(new StructuredSelection(element), true);
            }
        });
        button = createButton(buttons, "&Remove");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                int i = tableViewer.getTable().getSelectionIndex();
                if (i != -1) {
                    List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                    input.remove(i);
                    tableViewer.refresh();
                }
            }
        });
        button = createButton(buttons, "&Top");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                int i = tableViewer.getTable().getSelectionIndex();
                if (i != -1) {
                    List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                    input.add(0, input.remove(i));
                    tableViewer.refresh();
                }
            }
        });
        button = createButton(buttons, "&Up");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                int i = tableViewer.getTable().getSelectionIndex();
                if (i > 0) {
                    List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                    input.add(i - 1, input.remove(i));
                    tableViewer.refresh();
                }
            }
        });
        button = createButton(buttons, "&Down");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                int i = tableViewer.getTable().getSelectionIndex();
                List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                if (i != -1 && i < input.size() - 1) {
                    input.add(i + 1, input.remove(i));
                    tableViewer.refresh();
                }
            }
        });
        button = createButton(buttons, "&Bottom");
        button.addSelectionListener(new SelectionAdapter() {
            @SuppressWarnings("unchecked")
            @Override
            public void widgetSelected(SelectionEvent event) {
                int i = tableViewer.getTable().getSelectionIndex();
                if (i != -1) {
                    List<Pair<String, String>> input = (List<Pair<String, String>>)tableViewer.getInput();
                    input.add(input.remove(i));
                    tableViewer.refresh();
                }
            }
        });
        return tableViewer;
    }

    protected TableViewerColumn createTableViewerColumn(TableViewer tableViewer, String title, int bound) {
        TableViewerColumn viewerColumn = new TableViewerColumn(tableViewer, SWT.NONE);
        TableColumn column = viewerColumn.getColumn();
        column.setText(title);
        column.setWidth(bound);
        return viewerColumn;

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

    protected Button createButton(Composite parent, String label) {
        Button button = new Button(parent, SWT.PUSH);
        button.setText(label);
        button.setFont(JFaceResources.getDialogFont());
        button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        return button;
    }

    protected Text createText(Composite parent, String tooltip, int hspan) {
        Text text = new Text(parent, SWT.BORDER);
        text.setToolTipText(tooltip);
        text.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false, hspan, 1));
        return text;
    }

    protected Text createTextWithProposals(Composite parent, String tooltip, int hspan, EventLogEntry prototype) {
        final Text text = createText(parent, tooltip, hspan);
        ContentAssistCommandAdapter commandAdapter = new ContentAssistCommandAdapter(text, new TextContentAdapter(), new EventLogEntryProposalProvider(prototype),
            ITextEditorActionDefinitionIds.CONTENT_ASSIST_PROPOSALS, "( ".toCharArray(), true);
        commandAdapter.setProposalAcceptanceStyle(ContentProposalAdapter.PROPOSAL_IGNORE);
        commandAdapter.addContentProposalListener(new IContentProposalListener() {
            public void proposalAccepted(IContentProposal proposal) {
                ContentProposalEx contentProposal = (ContentProposalEx)proposal;
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

    protected class DialogTreeNode extends GenericTreeNode {
        protected Control panel;

        public DialogTreeNode(Object payload, Control panel) {
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

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.resource.JFaceResources;
import org.eclipse.jface.viewers.DoubleClickEvent;
import org.eclipse.jface.viewers.IDoubleClickListener;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
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
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Table;
import org.eclipse.ui.dialogs.SelectionDialog;

/**
 * List dialog for in-place ordering of a list.
 *
 * @author Andras
 */
@SuppressWarnings({"rawtypes", "unchecked"})
public class ListReorderDialog extends SelectionDialog {
    private static final int BUTTON_TOP         = IDialogConstants.CLIENT_ID;
    private static final int BUTTON_UP          = IDialogConstants.CLIENT_ID + 1;
    private static final int BUTTON_DOWN        = IDialogConstants.CLIENT_ID + 2;
    private static final int BUTTON_BOTTOM      = IDialogConstants.CLIENT_ID + 3;
    private static final int BUTTON_REVERSE     = IDialogConstants.CLIENT_ID + 4;
    private static final int BUTTON_USE_CURRENT = IDialogConstants.CLIENT_ID + 5;

    private Object[] originalOrder;
    private List listToReorder;

    private IStructuredContentProvider contentProvider;
    private ILabelProvider labelProvider;
    private TableViewer tableViewer;
    private int widthInChars = 40;
    private int heightInChars = 15;

    public ListReorderDialog(Shell parent) {
        super(parent);
    }

    public void setContentProvider(IStructuredContentProvider sp) {
        contentProvider = sp;
    }

    public void setLabelProvider(ILabelProvider lp) {
        labelProvider = lp;
    }

    /**
     * Set the items to reorder in place. Must be a mutable List object.
     */
    public void setInput(List listToReorder) {
        this.listToReorder = listToReorder;
    }

    /**
     * This is a reference ordering to which the "Use Current" button resets ordering
     */
    public void setOriginalOrder(Object[] originalOrder) {
        this.originalOrder = originalOrder;
    }

    @Override
    public boolean isHelpAvailable() {
        return false;
    }

    @Override
    protected int getShellStyle() {
        return super.getShellStyle() | SWT.RESIZE;
    }

    protected int getTableStyle() {
        return SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER;
    }

    public TableViewer getTableViewer() {
        return tableViewer;
    }

    protected Control createDialogArea(Composite container) {
        Composite parent = (Composite) super.createDialogArea(container);
        createMessageArea(parent);

        Composite tableArea = new Composite(parent, SWT.NONE);
        tableArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
        GridLayout layout = new GridLayout(2, false);
        layout.marginWidth = layout.marginHeight = 0;
        tableArea.setLayout(layout);


        // table
        tableViewer = new TableViewer(tableArea, getTableStyle());
        tableViewer.setContentProvider(contentProvider);
        tableViewer.setLabelProvider(labelProvider);
        tableViewer.setInput(listToReorder);
        tableViewer.addDoubleClickListener(new IDoubleClickListener() {
            public void doubleClick(DoubleClickEvent event) {
                okPressed();
            }
        });
        List initialSelection = getInitialElementSelections();
        if (initialSelection != null) {
            tableViewer.setSelection(new StructuredSelection(initialSelection));
        }
        GridData gd = new GridData(GridData.FILL_BOTH);
        gd.heightHint = convertHeightInCharsToPixels(heightInChars);
        gd.widthHint = convertWidthInCharsToPixels(widthInChars);
        Table table = tableViewer.getTable();
        table.setLayoutData(gd);
        table.setFont(container.getFont());

        // buttons
        Composite buttonsArea = new Composite(tableArea, SWT.NONE);
        buttonsArea.setLayoutData(new GridData(SWT.FILL, SWT.FILL, false, true));
        buttonsArea.setLayout(new GridLayout(1, false));

        createSideButton(buttonsArea, BUTTON_TOP, "&Top");
        createSideButton(buttonsArea, BUTTON_UP, "&Up");
        createSideButton(buttonsArea, BUTTON_DOWN, "&Down");
        createSideButton(buttonsArea, BUTTON_BOTTOM, "&Bottom");
        new Label(buttonsArea, SWT.NONE);
        createSideButton(buttonsArea, BUTTON_REVERSE, "&Reverse");
        if (originalOrder != null)
            createSideButton(buttonsArea, BUTTON_USE_CURRENT, "Use &Current");

        return parent;
    }

    private Button createSideButton(Composite parent, final int buttonID, String label) {
        Button button = new Button(parent, SWT.PUSH);
        button.setText(label);
        button.setFont(JFaceResources.getDialogFont());
        button.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
        button.addSelectionListener(new SelectionAdapter() {
            public void widgetSelected(SelectionEvent event) {
                buttonPressed(buttonID);
            }
        });
        return button;
    }

    /*
     * Overrides method from Dialog
     */
    protected void okPressed() {
        // Build a list of selected children.
        IStructuredSelection selection = (IStructuredSelection) tableViewer.getSelection();
        setResult(selection.toList());
        super.okPressed();
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (buttonId < IDialogConstants.CLIENT_ID) {
            super.buttonPressed(buttonId);
        }
        else {
            StructuredSelection selection = (StructuredSelection)getTableViewer().getSelection();

            if (buttonId == BUTTON_USE_CURRENT) {
                Collections.sort(listToReorder, new Comparator<Object>() {
                    public int compare(Object o1, Object o2) {
                        return ArrayUtils.indexOf(originalOrder, o1) - ArrayUtils.indexOf(originalOrder, o2);
                    }
                });
            }
            else if (buttonId == BUTTON_REVERSE) {
                List selectionAsList = selection.toList();
                List list = selectionAsList.size()>=2 ? selectionAsList : new ArrayList(listToReorder);
                for (int i = 0; i < list.size() / 2; i++)
                    swap(list.get(i), list.get(list.size() - i - 1));
            }
            else if (selection.size() != 0) {
                List selectionAsList = selection.toList();
                Object firstSelectionElement = selectionAsList.get(0);
                Object lastSelectionElement = selectionAsList.get(selectionAsList.size() - 1);

                switch (buttonId) {
                    case BUTTON_TOP:
                        move(selectionAsList, -listToReorder.indexOf(firstSelectionElement));
                        break;
                    case BUTTON_UP:
                        if (listToReorder.indexOf(firstSelectionElement) > 0)
                            move(selectionAsList, -1);
                        break;
                    case BUTTON_DOWN:
                        if (listToReorder.indexOf(lastSelectionElement) < listToReorder.size() - 1)
                            move(selectionAsList, 1);
                        break;
                    case BUTTON_BOTTOM:
                        move(selectionAsList, listToReorder.size() - listToReorder.indexOf(lastSelectionElement) - 1);
                        break;
                }
            }

            getTableViewer().getTable().setFocus();
            getTableViewer().refresh();
        }
    }

    private void move(List<?> selection, int delta) {
        Object[] movedCurrentAxisModuleOrder = new Object[listToReorder.size()];

        for (int i = 0; i < listToReorder.size(); i++) {
            Object axisModule = listToReorder.get(i);

            if (selection.contains(axisModule))
                movedCurrentAxisModuleOrder[i + delta] = axisModule;
        }

        for (Object axisModule : listToReorder) {
            if (!selection.contains(axisModule)) {
                for (int j = 0; j < movedCurrentAxisModuleOrder.length; j++) {
                    if (movedCurrentAxisModuleOrder[j] == null) {
                        movedCurrentAxisModuleOrder[j] = axisModule;
                        break;
                    }
                }
            }
        }

        listToReorder.clear();
        listToReorder.addAll(Arrays.asList(movedCurrentAxisModuleOrder));

        getTableViewer().refresh();

        if (delta < 0)
            getTableViewer().reveal(selection.get(0));
        else if (delta > 0)
            getTableViewer().reveal(selection.get(selection.size() - 1));
    }

    private void swap(Object module1, Object module2) {
        int index1 = listToReorder.indexOf(module1);
        int index2 = listToReorder.indexOf(module2);
        listToReorder.set(index1, module2);
        listToReorder.set(index2, module1);
    }

    /**
     * Returns the initial height of the dialog in number of characters.
     */
    public int getHeightInChars() {
        return heightInChars;
    }

    /**
     * Returns the initial width of the dialog in number of characters.
     */
    public int getWidthInChars() {
        return widthInChars;
    }

    /**
     * Sets the initial height of the dialog in number of characters.
     */
    public void setHeightInChars(int heightInChars) {
        this.heightInChars = heightInChars;
    }

    /**
     * Sets the initial width of the dialog in number of characters.
     */
    public void setWidthInChars(int widthInChars) {
        this.widthInChars = widthInChars;
    }
}

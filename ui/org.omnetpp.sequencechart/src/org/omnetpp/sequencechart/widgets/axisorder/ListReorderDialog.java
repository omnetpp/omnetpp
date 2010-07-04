/**
 * 
 */
package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ListDialog;

/**
 * List dialog for in-place ordering of a list.
 * 
 * @author Andras
 */
@SuppressWarnings("unchecked")
public class ListReorderDialog extends ListDialog {
    private static final int BUTTON_TOP         = IDialogConstants.CLIENT_ID;
    private static final int BUTTON_UP          = IDialogConstants.CLIENT_ID + 1;
    private static final int BUTTON_DOWN        = IDialogConstants.CLIENT_ID + 2;
    private static final int BUTTON_BOTTOM      = IDialogConstants.CLIENT_ID + 3;
    private static final int BUTTON_REVERSE     = IDialogConstants.CLIENT_ID + 4;
    private static final int BUTTON_USE_CURRENT = IDialogConstants.CLIENT_ID + 5;

    private Object[] originalOrder;
    private List listToReorder;

    public ListReorderDialog(Shell parent) {
        super(parent);
    }

    /**
     * Set the items to reorder in place. Must be a mutable List object.
     */
    @Override
    public void setInput(Object input) {
        super.setInput(input);
        this.listToReorder = (List) input;
    }
    
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
    
    @Override
    protected int getTableStyle() {
        return SWT.MULTI | SWT.H_SCROLL | SWT.V_SCROLL | SWT.BORDER;
    }
    
    @Override
    protected void createButtonsForButtonBar(Composite parent) {
        
        createButton(parent, BUTTON_TOP, "Top", false);
    	createButton(parent, BUTTON_UP, "Up", false);
    	createButton(parent, BUTTON_DOWN, "Down", false);
        createButton(parent, BUTTON_BOTTOM, "Bottom", false);
        createButton(parent, BUTTON_REVERSE, "Reverse", false);
    	createButton(parent, BUTTON_USE_CURRENT, "Use Current", false);
    	super.createButtonsForButtonBar(parent);
    }

    @Override
    protected void buttonPressed(int buttonId) {
        if (buttonId >= IDialogConstants.CLIENT_ID) {
            StructuredSelection selection = (StructuredSelection)getTableViewer().getSelection();

            if (buttonId == BUTTON_USE_CURRENT) {
                Collections.sort(listToReorder, new Comparator<Object>() {
                    public int compare(Object o1, Object o2) {
                        return ArrayUtils.indexOf(originalOrder, o1) - ArrayUtils.indexOf(originalOrder, o2);
                    }
                });
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
                    case BUTTON_REVERSE:
                        for (int i = 0; i < selectionAsList.size() / 2; i++)
                            swap(selectionAsList.get(i), selectionAsList.get(selectionAsList.size() - i - 1));
                        break;
    			}

                getTableViewer().getTable().setFocus();
            }

            getTableViewer().refresh();
        }
        else
            super.buttonPressed(buttonId);
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
}

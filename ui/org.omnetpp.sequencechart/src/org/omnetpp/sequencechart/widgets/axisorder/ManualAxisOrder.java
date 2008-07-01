package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.eventlog.ModuleTreeItem;

public class ManualAxisOrder {
    private static final int BUTTON_TOP         = IDialogConstants.CLIENT_ID;
    private static final int BUTTON_UP          = IDialogConstants.CLIENT_ID + 1;
    private static final int BUTTON_DOWN        = IDialogConstants.CLIENT_ID + 2;
    private static final int BUTTON_BOTTOM      = IDialogConstants.CLIENT_ID + 3;
    private static final int BUTTON_REVERSE     = IDialogConstants.CLIENT_ID + 4;
    private static final int BUTTON_USE_CURRENT = IDialogConstants.CLIENT_ID + 5;

    // this is a total ordering among all modules seen so far
    private ArrayList<ModuleTreeItem> lastAxisModuleOrder;
    
    public ManualAxisOrder() {
        setAxisOrder(new ArrayList<ModuleTreeItem>());
    }

    public ManualAxisOrder(ArrayList<ModuleTreeItem> axisModuleOrder) {
        setAxisOrder(axisModuleOrder);
    }
    
    public void setAxisOrder(ArrayList<ModuleTreeItem> axisModuleOrder) {
        this.lastAxisModuleOrder = axisModuleOrder;
    }
    
    public ArrayList<ModuleTreeItem> getAxisOrder() {
        return lastAxisModuleOrder;
    }

    private ArrayList<ModuleTreeItem> getCurrentAxisModuleOrder(ModuleTreeItem[] selectedAxisModules) {
        // ensure all modules are known in the stored ordering
        for (ModuleTreeItem axisModule : selectedAxisModules)
            if (!lastAxisModuleOrder.contains(axisModule))
                lastAxisModuleOrder.add(axisModule);

        // add selected modules in the last known order
        ArrayList<ModuleTreeItem> currentAxisModuleOrder = new ArrayList<ModuleTreeItem>();
        for (ModuleTreeItem axisModule : lastAxisModuleOrder)
            if (ArrayUtils.contains(selectedAxisModules, axisModule))
                currentAxisModuleOrder.add(axisModule);
        return currentAxisModuleOrder;
    }

    private void updateLastAxisModuleOrder(ArrayList<ModuleTreeItem> currentAxisModuleOrder, ArrayList<ModuleTreeItem> updatedCurrentAxisModuleOrder) {
        ArrayList<ModuleTreeItem> updatedLastAxisModuleOrder = new ArrayList<ModuleTreeItem>(lastAxisModuleOrder);

        for (ModuleTreeItem axisModule : currentAxisModuleOrder) {
            ModuleTreeItem targetAxisModule = currentAxisModuleOrder.get(updatedCurrentAxisModuleOrder.indexOf(axisModule));
            updatedLastAxisModuleOrder.set(lastAxisModuleOrder.indexOf(targetAxisModule), axisModule);
        }
        
        lastAxisModuleOrder = updatedLastAxisModuleOrder;
    }

	public int[] calculateOrdering(ModuleTreeItem[] selectedAxisModules) {
        int numberOfAxes = selectedAxisModules.length;
        int[] axisPositions = new int[numberOfAxes];
	    ArrayList<ModuleTreeItem> currentAxisModuleOrder = getCurrentAxisModuleOrder(selectedAxisModules);
		
        for (int i = 0; i < selectedAxisModules.length; i++)
            axisPositions[i] = currentAxisModuleOrder.indexOf(selectedAxisModules[i]);
        
        return axisPositions;
	}
    
    public int showManualOrderDialog(final ModuleTreeItem[] selectedAxisModules) {
        ArrayList<ModuleTreeItem> currentAxisModuleOrder = getCurrentAxisModuleOrder(selectedAxisModules);
        final ArrayList<ModuleTreeItem> updatedCurrentAxisModuleOrder = new ArrayList<ModuleTreeItem>(currentAxisModuleOrder);

        ListDialog dialog = new ListDialog(null) {
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
                        Collections.sort(updatedCurrentAxisModuleOrder, new Comparator<Object>() {
                            public int compare(Object o1, Object o2) {
                                return ArrayUtils.indexOf(selectedAxisModules, o1) - ArrayUtils.indexOf(selectedAxisModules, o2);
                            }
                        });
                    }
                    else if (selection.size() != 0) {
                        List<?> selectionAsList = selection.toList();
                        Object firstSelectionElement = selectionAsList.get(0);
                        Object lastSelectionElement = selectionAsList.get(selectionAsList.size() - 1);

                        switch (buttonId) {
        					case BUTTON_TOP:
        					    move(selectionAsList, -updatedCurrentAxisModuleOrder.indexOf(firstSelectionElement));
        						break;
                            case BUTTON_UP:
                                if (updatedCurrentAxisModuleOrder.indexOf(firstSelectionElement) > 0)
                                    move(selectionAsList, -1);
                                break;
        					case BUTTON_DOWN:
        					    if (updatedCurrentAxisModuleOrder.indexOf(lastSelectionElement) < updatedCurrentAxisModuleOrder.size() - 1)
        					        move(selectionAsList, 1);
        						break;
                            case BUTTON_BOTTOM:
                                move(selectionAsList, updatedCurrentAxisModuleOrder.size() - updatedCurrentAxisModuleOrder.indexOf(lastSelectionElement) - 1);
                                break;
                            case BUTTON_REVERSE:
                                for (int i = 0; i < selectionAsList.size() / 2; i++)
                                    swap((ModuleTreeItem)selectionAsList.get(i), (ModuleTreeItem)selectionAsList.get(selectionAsList.size() - i - 1));
                                break;
        				}
                        
                        getTableViewer().getTable().setFocus();
                    }
                    
                    getTableViewer().refresh();
                }
                else
                    super.buttonPressed(buttonId);
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
			
			private void move(List<?> selection, int delta) {
			    ModuleTreeItem[] movedCurrentAxisModuleOrder = new ModuleTreeItem[updatedCurrentAxisModuleOrder.size()];

			    for (int i = 0; i < updatedCurrentAxisModuleOrder.size(); i++) {
			        ModuleTreeItem axisModule = updatedCurrentAxisModuleOrder.get(i);

			        if (selection.contains(axisModule))
			            movedCurrentAxisModuleOrder[i + delta] = axisModule;
			    }
			    
			    for (ModuleTreeItem axisModule : updatedCurrentAxisModuleOrder) {
                    if (!selection.contains(axisModule)) {
                        for (int j = 0; j < movedCurrentAxisModuleOrder.length; j++) {
                            if (movedCurrentAxisModuleOrder[j] == null) {
                                movedCurrentAxisModuleOrder[j] = axisModule;
                                break;
                            }
                        }
                    }
			    }
			    
			    updatedCurrentAxisModuleOrder.clear();
			    updatedCurrentAxisModuleOrder.addAll(Arrays.asList(movedCurrentAxisModuleOrder));
			    
			    getTableViewer().refresh();

			    if (delta < 0)
			        getTableViewer().reveal(selection.get(0));
			    else if (delta > 0)
			        getTableViewer().reveal(selection.get(selection.size() - 1));
			}
			
			private void swap(ModuleTreeItem module1, ModuleTreeItem module2) {
			    int index1 = updatedCurrentAxisModuleOrder.indexOf(module1);
                int index2 = updatedCurrentAxisModuleOrder.indexOf(module2);
                updatedCurrentAxisModuleOrder.set(index1, module2);
                updatedCurrentAxisModuleOrder.set(index2, module1);
			}
		};

		dialog.setContentProvider(new ArrayContentProvider());
		dialog.setLabelProvider(new LabelProvider() {
			@Override
			public String getText(Object element) {
				ModuleTreeItem moduleTreeItem = (ModuleTreeItem)element;
				return moduleTreeItem.getModuleFullPath();
			}
		});

		dialog.setInput(updatedCurrentAxisModuleOrder);
		dialog.setTitle("Manual axis reordering");
		dialog.open();

		if (dialog.getReturnCode() == Window.OK)
            updateLastAxisModuleOrder(currentAxisModuleOrder, updatedCurrentAxisModuleOrder);
		
		return dialog.getReturnCode();
    }
}

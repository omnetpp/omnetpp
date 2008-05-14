package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;

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

public class ManualAxisOrder implements IAxisOrder {
    // this is a total ordering among all modules seen so far
    private ArrayList<ModuleTreeItem> lastAxisModuleOrder = new ArrayList<ModuleTreeItem>();

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

	public void calculateOrdering(ModuleTreeItem[] selectedAxisModules, int[] axisPositions) {
	    ArrayList<ModuleTreeItem> currentAxisModuleOrder = getCurrentAxisModuleOrder(selectedAxisModules);
		
        for (int i = 0; i < selectedAxisModules.length; i++)
            axisPositions[i] = currentAxisModuleOrder.indexOf(selectedAxisModules[i]);
	}
    
    public void showManualOrderDialog(ModuleTreeItem[] selectedAxisModules) {
        ArrayList<ModuleTreeItem> currentAxisModuleOrder = getCurrentAxisModuleOrder(selectedAxisModules);
        final ArrayList<ModuleTreeItem> updatedCurrentAxisModuleOrder = new ArrayList<ModuleTreeItem>(currentAxisModuleOrder);

        ListDialog dialog = new ListDialog(null) {
			@Override
		    protected void createButtonsForButtonBar(Composite parent) {
				createButton(parent, IDialogConstants.CLIENT_ID, "Up", false);
				createButton(parent, IDialogConstants.CLIENT_ID + 1, "Down", false);
				super.createButtonsForButtonBar(parent);
		    }

			@Override
			protected void buttonPressed(int buttonId) {
				int index = -1;
				StructuredSelection selection = (StructuredSelection)getTableViewer().getSelection();
				ModuleTreeItem moduleTreeItem = (ModuleTreeItem)selection.getFirstElement();

				if (buttonId >= IDialogConstants.CLIENT_ID) {
					moduleTreeItem = (ModuleTreeItem)selection.getFirstElement();
					index = updatedCurrentAxisModuleOrder.indexOf(moduleTreeItem);
				}

				switch (buttonId) {
					case IDialogConstants.CLIENT_ID:
						swap(updatedCurrentAxisModuleOrder, index - 1, index);
						break;
					case IDialogConstants.CLIENT_ID + 1:
						swap(updatedCurrentAxisModuleOrder, index, index + 1);
						break;
					default:
						super.buttonPressed(buttonId);
						break;
				}

				if (buttonId >= IDialogConstants.CLIENT_ID)
					getTableViewer().refresh();
			}
			
			@Override
			public boolean isHelpAvailable() {
			    return false;
			}
			
			@Override
			protected int getShellStyle() {
			    return super.getShellStyle() | SWT.RESIZE;
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
    }
	
	private void swap(ArrayList<ModuleTreeItem> list, int index1, int index2) {
		if (index1 >= 0 && index2 < list.size())
			list.add(index2, list.remove(index1));
	}
}

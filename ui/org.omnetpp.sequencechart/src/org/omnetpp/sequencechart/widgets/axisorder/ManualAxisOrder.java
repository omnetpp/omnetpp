package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.eventlog.ModuleTreeItem;

public class ManualAxisOrder implements IAxisOrder {
	public void calculateOrdering(ModuleTreeItem[] axisModules, int[] axisPositions) {
		final ArrayList<ModuleTreeItem> orderedAxisModules = new ArrayList<ModuleTreeItem>();

		for (int i = 0; i < axisModules.length; i++)
			orderedAxisModules.add(axisModules[ArrayUtils.indexOf(axisPositions, i)]);
		
		// TODO: show dialog with current positions and let the user to update it
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
					index = orderedAxisModules.indexOf(moduleTreeItem);
				}

				switch (buttonId) {
					case IDialogConstants.CLIENT_ID:
						swap(orderedAxisModules, index - 1, index);
						break;
					case IDialogConstants.CLIENT_ID + 1:
						swap(orderedAxisModules, index, index + 1);
						break;
					default:
						super.buttonPressed(buttonId);
						break;
				}

				if (buttonId >= IDialogConstants.CLIENT_ID)
					getTableViewer().refresh();
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

		dialog.setInput(orderedAxisModules);
		dialog.setTitle("Manual axis reordering");
		dialog.open();
		
		if (dialog.getReturnCode() == Window.OK)
			for (int i = 0; i < axisModules.length; i++)
				axisPositions[i] = orderedAxisModules.indexOf(axisModules[i]);
	}
	
	private void swap(ArrayList<ModuleTreeItem> list, int index1, int index2) {
		if (index1 >= 0 && index2 < list.size())
			list.add(index2, list.remove(index1));
	}
}

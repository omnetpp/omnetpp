package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.jface.dialogs.IDialogConstants;
import org.eclipse.jface.viewers.IStructuredContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.common.eventlog.ModuleTreeItem;

public class ManualAxisOrder implements IAxisOrder {
	public void calculateOrdering(ModuleTreeItem[] axisModules, int[] axisPositions) {
		ArrayList<ModuleTreeItem> orderedAxisModules = new ArrayList<ModuleTreeItem>();
		
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
				switch (buttonId) {
					case IDialogConstants.CLIENT_ID:
						Object o = getResult();
						break;
					case IDialogConstants.CLIENT_ID + 1:
						break;
					default:
						super.buttonPressed(buttonId);
						break;
				}
			}
		};
		dialog.setContentProvider(new IStructuredContentProvider() {
			ArrayList<ModuleTreeItem> axisModules;

			public Object[] getElements(Object inputElement) {
				return axisModules.toArray();
			}

			public void dispose() {
			}

			@SuppressWarnings("unchecked")
			public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
				axisModules = (ArrayList<ModuleTreeItem>)newInput;
			}
		});
		dialog.setLabelProvider(new LabelProvider() {
			@Override
			public String getText(Object element) {
				ModuleTreeItem moduleTreeItem = (ModuleTreeItem)element;
				return moduleTreeItem.getModuleFullPath();
			}
		});
		dialog.setInput(orderedAxisModules);
		dialog.open();
		
		if (dialog.getReturnCode() == SWT.OK) {
			for (int i = 0; i < axisModules.length; i++)
				axisPositions[i] = orderedAxisModules.indexOf(axisModules[i]);
		}
	}
}

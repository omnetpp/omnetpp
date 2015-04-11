/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.sequencechart.widgets.axisorder;

import java.util.ArrayList;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.eventlog.ModuleTreeItem;

public class ManualAxisOrder {
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

    public ArrayList<ModuleTreeItem> getCurrentAxisModuleOrder(ModuleTreeItem[] selectedAxisModules) {
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
        ArrayList<ModuleTreeItem> updatedCurrentAxisModuleOrder = new ArrayList<ModuleTreeItem>(currentAxisModuleOrder);

        ListReorderDialog dialog = new ListReorderDialog(Display.getCurrent().getActiveShell());

        dialog.setContentProvider(new ArrayContentProvider());
        dialog.setLabelProvider(new LabelProvider() {
            @Override
            public String getText(Object element) {
                ModuleTreeItem moduleTreeItem = (ModuleTreeItem)element;
                return moduleTreeItem.getModuleFullPath() + " (id = " + moduleTreeItem.getModuleId() + ")";
            }
        });

        dialog.setTitle("Manual Axis Ordering");
        dialog.setMessage("Reorder chart axes:");
        dialog.setInput(updatedCurrentAxisModuleOrder);
        dialog.setOriginalOrder(selectedAxisModules);
        dialog.open();

        if (dialog.getReturnCode() == Window.OK)
            updateLastAxisModuleOrder(currentAxisModuleOrder, updatedCurrentAxisModuleOrder);

        return dialog.getReturnCode();
    }
}

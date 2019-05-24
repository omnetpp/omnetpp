/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Copy model objects to the clipboard.
 */
public class PasteAction extends AbstractScaveAction {
    public PasteAction() {
        setText("Paste from Clipboard");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_PASTE));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {

        Clipboard clipboard = new Clipboard(Display.getCurrent());
        Object content = clipboard.getContents(LocalTransfer.getInstance());
        clipboard.dispose();

        if (content instanceof Object[]) {
            Object[] objects = (Object[])content;

            CompoundCommand command = new CompoundCommand("Paste objects");

            List<AnalysisItem> charts = editor.getAnalysis().getCharts().getCharts();
            int pasteIndex = -1;

            for (Object o : asStructuredOrEmpty(selection).toArray())
                if (o instanceof AnalysisItem)
                    pasteIndex = charts.indexOf(o) + 1;

            if (pasteIndex < 0)
                pasteIndex = charts.size();

            List<String> existingNames = new ArrayList<String>();

            for (AnalysisItem i : editor.getAnalysis().getCharts().getCharts())
                existingNames.add(i.getName());

            List<Object> toSelect = new ArrayList<Object>();
            for (Object o : objects) {
                if (o instanceof AnalysisItem) {
                    AnalysisItem itemToPaste = (AnalysisItem)((AnalysisItem)o).dup();
                    itemToPaste.assignNewId();
                    String newName = ScaveModelUtil.makeItemCopyName(existingNames, itemToPaste.getName());
                    itemToPaste.setName(newName);
                    existingNames.add(newName);
                    command.append(new AddChartCommand(editor.getAnalysis(), itemToPaste, pasteIndex));
                    toSelect.add(itemToPaste);
                    pasteIndex += 1;
                }
            }

            editor.getChartsPage().getCommandStack().execute(command);
            editor.getChartsPage().getViewer().setSelection(new StructuredSelection(toSelect));
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
//        BrowseDataPage browseDataPage = editor.getBrowseDataPage();
//        return browseDataPage != null && browseDataPage.getActivePanel() != null;
        return true;
    }
}

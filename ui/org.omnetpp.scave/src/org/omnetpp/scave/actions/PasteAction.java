/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.List;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.commands.AddChartCommand;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.RemoveChartCommand;

/**
 * Copy model objects to the clipboard.
 */
public class PasteAction extends AbstractScaveAction {
    public PasteAction() {
        setText("Paste from Clipboard");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_PASTE));
    }

    @Override
    protected void doRun(ScaveEditor editor, IStructuredSelection selection) {

        Clipboard clipboard = new Clipboard(Display.getCurrent());
        Object content = clipboard.getContents(LocalTransfer.getInstance());
        clipboard.dispose();

        if (content instanceof Object[]) {
            Object[] objects = (Object[])content;

            CompoundCommand command = new CompoundCommand("Paste objects");

            List<AnalysisItem> charts = editor.getAnalysis().getCharts().getCharts();
            int pasteIndex = -1;

            for (Object o : selection.toArray()) {
                if (o instanceof AnalysisItem) {
                    command.append(new RemoveChartCommand((AnalysisItem)o));
                    if (pasteIndex < 0)
                        pasteIndex = charts.indexOf(o);
                }
            }

            if (pasteIndex < 0)
                pasteIndex = charts.size();

            for (Object o : objects)
                if (o instanceof AnalysisItem) {
                    command.append(new AddChartCommand(editor.getAnalysis(), (AnalysisItem)o, pasteIndex));
                    pasteIndex += 1;
                }
            editor.getCommandStack().execute(command);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
//        BrowseDataPage browseDataPage = editor.getBrowseDataPage();
//        return browseDataPage != null && browseDataPage.getActivePanel() != null;
        return true;
    }
}

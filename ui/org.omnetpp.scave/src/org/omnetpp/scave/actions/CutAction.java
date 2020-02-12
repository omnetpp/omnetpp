/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.common.ui.LocalTransfer;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.AnalysisItem;
import org.omnetpp.scave.model.commands.CompoundCommand;
import org.omnetpp.scave.model.commands.RemoveChartCommand;

/**
 * Copy model objects to the clipboard.
 */
public class CutAction extends AbstractScaveAction {
    public CutAction() {
        setText("Cut");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_TOOL_CUT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {

        CompoundCommand command = new CompoundCommand("Cut objects");

        Object[] objects = asStructuredOrEmpty(selection).toArray();
        for (int i = 0; i < objects.length; ++i)
            if (objects[i] instanceof AnalysisItem)
                command.append(new RemoveChartCommand((AnalysisItem)objects[i]));
        // TODO filter out non-AnalysisObject objects
        Clipboard clipboard = new Clipboard(Display.getCurrent());
        clipboard.setContents(new Object[] { objects }, new Transfer[] {LocalTransfer.getInstance()});
        clipboard.dispose();

        editor.getChartsPage().getCommandStack().execute(command);

    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return !selection.isEmpty(); // TODO check selected content
    }
}

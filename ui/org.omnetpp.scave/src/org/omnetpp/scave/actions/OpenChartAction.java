/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;

/**
 * Opens the selected chart(s) or chart sheet(s) in the editor.
 */
public class OpenChartAction extends AbstractScaveAction {
    private int counter = 0;

    public OpenChartAction() {
        setText("Open");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOT));
        setToolTipText("Open chart(s)");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        // collect charts and chartsheets from the selection
        List<Chart> charts = new ArrayList<>();
        for (Object element : asStructuredOrEmpty(selection).toArray()) {
            if (element instanceof Chart)
                charts.add((Chart)element);
        }

        for (Chart c : charts)
            scaveEditor.openPage(c);
    }

    @Override
    public boolean isApplicable(ScaveEditor editor, ISelection selection) {
        for (Object element : asStructuredOrEmpty(selection).toArray())
            if (element instanceof Chart)
                return true;
        return false;
    }
}

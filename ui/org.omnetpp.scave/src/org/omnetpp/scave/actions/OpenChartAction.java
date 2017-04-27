/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;

/**
 * Opens the selected chart(s) in the editor.
 */
public class OpenChartAction extends AbstractScaveAction {
    public OpenChartAction() {
        setText("Open");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOT));
        setToolTipText("Open chart(s)");
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        for (Object element : selection.toArray()) {
            Object object = AdapterFactoryEditingDomain.unwrap(element);
            if (object instanceof Chart) {
                Chart chart = (Chart)object;
                scaveEditor.openChart(chart);
            }
        }
    }

    @Override
    public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        for (Object element : selection.toArray())
            if (element instanceof Chart)
                return true;
        return false;
    }
}

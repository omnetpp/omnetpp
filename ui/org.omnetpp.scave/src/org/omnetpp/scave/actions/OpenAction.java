/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import static org.omnetpp.common.image.ImageFactory.TOOLBAR_IMAGE_OPEN;

import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.image.ImageFactory;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;

/**
 * Opens the selected chart in the editor.
 */
public class OpenAction extends AbstractScaveAction {
    public OpenAction() {
        setText("Open");
        setImageDescriptor(ImageFactory.global().getDescriptor(TOOLBAR_IMAGE_OPEN));
        setToolTipText("Open item in a separate page");
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

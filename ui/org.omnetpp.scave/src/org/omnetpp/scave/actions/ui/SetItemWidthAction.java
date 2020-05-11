/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.ui;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.ui.IconGridViewer;
import org.omnetpp.common.ui.IconGridViewer.ViewMode;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.ScaveEditor;

/**
 * Increase the width of items in the Charts page until it makes sense, then wrap around.
 *
 * @author andras
 */
public class SetItemWidthAction extends AbstractScaveAction {

    public SetItemWidthAction() {
        setText("Increase Item Width");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_SETWIDTH));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        IconGridViewer viewer = scaveEditor.getChartsPage().getViewer();
        if (viewer.getViewMode() == ViewMode.ICONS) {
            int n = viewer.getNumItemsPerRow();
            if (n >= 3)
                viewer.setNumItemsPerRow(n-1);
            else
                viewer.setItemWidth(60);
        }
        else if (viewer.getViewMode() == ViewMode.MULTICOLUMN_LIST) {
            int n = viewer.getNumColumns();
            if (n >= 3)
                viewer.setNumColumns(n-1);
            else
                viewer.setColumnWidth(200);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}

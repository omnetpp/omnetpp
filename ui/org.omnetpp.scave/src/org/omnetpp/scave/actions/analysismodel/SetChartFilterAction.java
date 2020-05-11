/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.common.ui.TimeTriggeredProgressMonitorDialog2;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;
import org.omnetpp.scave.model2.ResultSelectionFilterGenerator;

public class SetChartFilterAction extends AbstractScaveAction {
    private Chart chart;

    public SetChartFilterAction(Chart c) {
        this.chart = c;

        setText(StringUtils.defaultIfEmpty(c.getName(), "<Unnamed>"));

        String iconPath = c.getIconPath();

        if (iconPath == null || iconPath.isEmpty())
            switch (c.getType()) {
                case BAR:        iconPath = ScaveImages.IMG_OBJ_BARCHART;        break;
                case HISTOGRAM:  iconPath = ScaveImages.IMG_OBJ_HISTOGRAMCHART;  break;
                case LINE:       iconPath = ScaveImages.IMG_OBJ_LINECHART;       break;
                case MATPLOTLIB: iconPath = ScaveImages.IMG_OBJ_MATPLOTLIBCHART; break;
            }
        setImageDescriptor(ImageDescriptor.createFromImageData(ScavePlugin.getImageDescriptor(iconPath).getImageData().scaledTo(16, 16)));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IDList all = editor.getBrowseDataPage().getActivePanel().getIDList();
        IDList target = ((IDListSelection)selection).getIDList();
        ResultFileManager rfm = editor.getResultFileManager();
        String[] result = new String[1];
        TimeTriggeredProgressMonitorDialog2.runWithDialog("Generating filter expression", (monitor) -> {
            result[0] = ResultSelectionFilterGenerator.getFilter(target, all, rfm, monitor);
        });
        if (result[0] != null) {
            String filter = result[0];
            SetChartPropertyCommand command = new SetChartPropertyCommand(chart, "filter", filter);
            editor.getChartsPage().getCommandStack().execute(command);
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}

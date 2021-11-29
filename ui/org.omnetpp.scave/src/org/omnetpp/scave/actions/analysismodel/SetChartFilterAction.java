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
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.actions.AbstractScaveAction;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Folder;
import org.omnetpp.scave.model.commands.SetChartPropertyCommand;
import org.omnetpp.scave.model2.ResultSelectionFilterGenerator;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class SetChartFilterAction extends AbstractScaveAction {
    private Chart chart;

    public SetChartFilterAction(Chart chart) {
        this.chart = chart;

        setText(ScaveModelUtil.getDisplayFullPath(chart, " / "));

        String iconPath = chart.getIconPath();

        if (iconPath == null || iconPath.isEmpty())
            switch (chart.getType()) {
                case BAR:        iconPath = ScaveImages.IMG_OBJ_BARCHART;        break;
                case HISTOGRAM:  iconPath = ScaveImages.IMG_OBJ_HISTOGRAMCHART;  break;
                case LINE:       iconPath = ScaveImages.IMG_OBJ_LINECHART;       break;
                case MATPLOTLIB: iconPath = ScaveImages.IMG_OBJ_MATPLOTLIBCHART; break;
            }
        setImageDescriptor(ImageDescriptor.createFromImageData(ScavePlugin.getImageDescriptor(iconPath).getImageData().scaledTo(16, 16)));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IDListSelection idListSelection = (IDListSelection)selection;
        String filter = ResultSelectionFilterGenerator.makeFilterForIDListSelection(idListSelection);
        int types = idListSelection.getSource().getType().getItemTypes();
        editor.getFilterCache().putFilterResult(types, filter, editor.getBrowseDataPage().getScalarsPanel().getShowFieldsAsScalars(), idListSelection.getIDList());

        SetChartPropertyCommand command = new SetChartPropertyCommand(chart, "filter", filter);
        editor.getChartsPage().getCommandStack().execute(command);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}

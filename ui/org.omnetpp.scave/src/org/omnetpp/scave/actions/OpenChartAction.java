/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.emf.common.command.BasicCommandStack;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CommandStack;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ScaveModelPackage;
import org.omnetpp.scave.model2.ScaveModelUtil;

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
    protected void doRun(ScaveEditor scaveEditor, IStructuredSelection selection) {
        // collect charts and chartsheets from the selection
        List<Chart> charts = new ArrayList<>();
        List<ChartSheet> chartsheets = new ArrayList<>();
        for (Object element : selection.toArray()) {
            Object object = AdapterFactoryEditingDomain.unwrap(element);
            if (object instanceof Chart)
                charts.add((Chart)object);
            else if (object instanceof ChartSheet) {
                chartsheets.add((ChartSheet)object);
            }
        }

        if (charts.size() == 1)
            scaveEditor.openChart(charts.get(0));
        else if (charts.size() > 1) {
            // make temporary chart sheet for them
            // note: code is similar to that it CreateTempChartAction
            String title = "Sheet" + (++counter);
            ChartSheet chartsheet = ScaveModelUtil.createChartSheet(title);
            chartsheet.setTemporary(true);
            chartsheet.getCharts().addAll(charts);
            chartsheets.add(chartsheet);
        }
        for (ChartSheet chartsheet : chartsheets)
            scaveEditor.openChartsheet(chartsheet);
    }

    @Override
    public boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        for (Object element : selection.toArray())
            if (element instanceof Chart || element instanceof ChartSheet)
                return true;
        return false;
    }
}

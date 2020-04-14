/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Scave;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartTemplate;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model2.ResultSelectionFilterGenerator;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, automatically
 * choosing the template to use, then opens it.
 */
public class PlotAction extends AbstractScaveAction {
    public PlotAction() {
        setText("Plot");
        setToolTipText("Plot");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        IDList idList = ((IDListSelection)selection).getIDList();
        ResultFileManager manager = editor.getResultFileManager();

        ChartTemplateRegistry templateRegistry = editor.getChartTemplateRegistry();
        List<ChartTemplate> templates = templateRegistry.getChartTemplatesForResultTypes(idList.getItemTypes());
        Assert.isTrue(!templates.isEmpty());

        Chart chart = ScaveModelUtil.createChartFromTemplate(templates.get(0));

        String[] filterFields = new String[] { Scave.EXPERIMENT, Scave.MEASUREMENT, Scave.REPLICATION,
                Scave.MODULE, Scave.NAME };
        String viewFilter = editor.getBrowseDataPage().getActivePanel().getFilter();
        String filter = ResultFileManager.callWithReadLock(manager, () -> { return ResultSelectionFilterGenerator.getIDListAsFilterExpression(idList, filterFields, viewFilter, manager, null); });

        Property filterProperty = chart.lookupProperty("filter");
        if (filterProperty != null)
            filterProperty.setValue(filter);
        else
            chart.addProperty(new Property("filter", filter));

        chart.setTemporary(true);
        editor.openPage(chart);
    }

    ChartTemplate getTemplateForResults(ScaveEditor editor, ISelection selection) {
        if (!(selection instanceof IDListSelection) || selection.isEmpty())
            return null;
        ChartTemplateRegistry templateRegistry = editor.getChartTemplateRegistry();
        IDList idList = ((IDListSelection)selection).getIDList();

        List<ChartTemplate> templates = templateRegistry.getChartTemplatesForResultTypes(idList.getItemTypes());

        return templates.isEmpty() ? null : templates.get(0);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return getTemplateForResults(editor, selection) != null;
    }
}

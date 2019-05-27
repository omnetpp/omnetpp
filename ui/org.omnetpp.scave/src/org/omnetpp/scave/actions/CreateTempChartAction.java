/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charttemplates.ChartTemplateRegistry;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, and opens it.
 */
// TODO merge with CreateTempMatplotlibChartAction (common base?)
public class CreateTempChartAction extends AbstractScaveAction {
    public CreateTempChartAction() {
        setText("Plot");
        setToolTipText("Plot");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) {
        IDListSelection idListSelection = (IDListSelection)selection;
        ResultFileManager manager = idListSelection.getResultFileManager();
        if (idListSelection.getScalarsCount() != 0) {
            IDList idList = idListSelection.getScalarIDs();
            openChart(editor, manager, ResultType.SCALAR, idList);
        }
        if (idListSelection.getVectorsCount() != 0) {
            IDList idList = idListSelection.getVectorIDs();
            openChart(editor, manager, ResultType.VECTOR, idList);
        }
        if (idListSelection.getHistogramsCount() != 0) {
            IDList idList = idListSelection.getHistogramIDs();
            openChart(editor, manager, ResultType.HISTOGRAM, idList);
        }
    }

    protected void openChart(ScaveEditor editor, final ResultFileManager manager, final ResultType type, final IDList idList) {
        String[] filterFields = new String[] { RunAttribute.EXPERIMENT, RunAttribute.MEASUREMENT, RunAttribute.REPLICATION,
                ResultItemField.MODULE, ResultItemField.NAME };
        ChartTemplateRegistry templateRegistry = editor.getChartTemplateRegistry();

        Chart chart = null;
        switch (type) {
            case HISTOGRAM: chart = ScaveModelUtil.createChartFromTemplate(templateRegistry, "histogram"); break;
            case SCALAR: chart = ScaveModelUtil.createChartFromTemplate(templateRegistry, "barchart"); break;
            case VECTOR: chart = ScaveModelUtil.createChartFromTemplate(templateRegistry, "linechart"); break;
            case STATISTICS: Assert.isLegal(false, "unsupported result type"); break;
            default: Assert.isLegal(false, "invalid enum value"); break;
        }

        ResultType resultType = editor.getBrowseDataPage().getActivePanelType();
        String viewFilter = editor.getBrowseDataPage().getActivePanel().getFilter().getFilterPattern();
        String filter = ResultFileManager.callWithReadLock(manager, () -> { return ScaveModelUtil.getIDListAsFilterExpression(idList, filterFields, resultType, viewFilter, manager); });
        Property property = new Property("filter", filter);
        chart.addProperty(property);

        chart.setTemporary(true);
        editor.openPage(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty() && ((IDListSelection)selection).getStatisticsCount() == 0;
    }
}

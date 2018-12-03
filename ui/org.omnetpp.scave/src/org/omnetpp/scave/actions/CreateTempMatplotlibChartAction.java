/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.model.MatplotlibChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.python.ChartScriptGenerator;
import org.omnetpp.scave.script.ScriptEngine;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, and opens it.
 */
public class CreateTempMatplotlibChartAction extends AbstractScaveAction {
    static int counter = 0;

    public CreateTempMatplotlibChartAction() {
        setText("Plot with Matplotlib");
        setToolTipText("Plot with Matplotlib");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOTMATPLOTLIB));
    }

    @Override
    protected void doRun(ScaveEditor editor, final IStructuredSelection selection) {
        IDListSelection idListSelection = (IDListSelection)selection;
        ResultFileManager manager = idListSelection.getResultFileManager();
        if (idListSelection.getScalarsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getScalarIDs());
            openMatplotlibChart(editor, manager, ResultType.SCALAR_LITERAL, idList);
        }
        if (idListSelection.getVectorsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getVectorIDs());
            openMatplotlibChart(editor, manager, ResultType.VECTOR_LITERAL, idList);
        }
        if (idListSelection.getHistogramsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getHistogramIDs());
            openMatplotlibChart(editor, manager, ResultType.HISTOGRAM_LITERAL, idList);
        }
    }

    protected void openMatplotlibChart(final ScaveEditor editor, final ResultFileManager manager, final ResultType type, final IDList idList) {

        //String[] filterFields = new String[] { ResultItemField.FILE, RunAttribute.CONFIGNAME, RunAttribute.RUNNUMBER };
        String[] filterFields = new String[] { ResultItemField.RUN };

        String name = "MatplotlibChart" + (++counter);

        String title = ResultFileManager.callWithReadLock(manager, () -> {
            return StringUtils.defaultIfEmpty(ScriptEngine.defaultTitle(ScaveModelUtil.getResultItems(idList, manager)), name);
        });

        String input = ChartScriptGenerator.makeMatplotlibChartScript(title, idList, filterFields, type, manager);

        MatplotlibChart chart = ScaveModelUtil.createMatplotlibChart(name);

        Property property = ScaveModelFactory.eINSTANCE.createProperty();
        property.setName(ChartProperties.PROP_GRAPH_TITLE);
        property.setValue(title);
        chart.getProperties().add(property);

        chart.setScript(input);
        chart.setTemporary(true);

        editor.openChart(chart);
    }


    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}

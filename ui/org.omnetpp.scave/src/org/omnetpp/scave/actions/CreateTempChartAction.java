/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model2.ScaveModelUtil;
import org.omnetpp.scave.script.ScriptEngine;

/**
 * Creates a temporary chart from the selection on the BrowseDataPage, and opens it.
 */
public class CreateTempChartAction extends AbstractScaveAction {
    static int counter = 0;

    public CreateTempChartAction() {
        setText("Plot");
        setToolTipText("Plot");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_PLOT));
    }

    @Override
    protected void doRun(ScaveEditor editor, final IStructuredSelection selection) {
        IDListSelection idListSelection = (IDListSelection)selection;
        ResultFileManager manager = idListSelection.getResultFileManager();
        if (idListSelection.getScalarsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getScalarIDs());
            openChart(editor, manager, ResultType.SCALAR_LITERAL, idList);
        }
        if (idListSelection.getVectorsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getVectorIDs());
            openChart(editor, manager, ResultType.VECTOR_LITERAL, idList);
        }
        if (idListSelection.getHistogramsCount() != 0) {
            IDList idList = IDList.fromArray(idListSelection.getHistogramIDs());
            openChart(editor, manager, ResultType.HISTOGRAM_LITERAL, idList);
        }
    }

    protected void openChart(final ScaveEditor editor, final ResultFileManager manager, final ResultType type, final IDList idList) {
        Chart chart = ResultFileManager.callWithReadLock(manager, new Callable<Chart>() {
            @Override
            public Chart call() {
                //String[] filterFields = new String[] { ResultItemField.FILE, RunAttribute.CONFIGNAME, RunAttribute.RUNNUMBER };
                String[] filterFields = new String[] { RunAttribute.EXPERIMENT, RunAttribute.MEASUREMENT, RunAttribute.REPLICATION,
                        ResultItemField.MODULE, ResultItemField.NAME };

                String typeName = type.getName();
                typeName = typeName.substring(0, 1).toUpperCase() + typeName.substring(1) + "s";


                StringBuilder inputBuilder = new StringBuilder();

                inputBuilder.append("import results\n\n");
                inputBuilder.append("# This expression selects the results (you might be able to logically simplify it)\n");
                inputBuilder.append("filter_expression = \"\"\"\n" + ScaveModelUtil.getIDListAsChartInput(idList, filterFields, manager) + "\"\"\"\n\n");

                inputBuilder.append("# The data is returned as a Pandas DataFrame\n");
                inputBuilder.append("df = results.get" + typeName + "(filter_expression)\n\n");

                if (type == ResultType.SCALAR_LITERAL) {
                    inputBuilder.append("# The scalars are transformed into a much simpler format\n");
                    inputBuilder.append("df = results.pivotScalars(df, columns=[\"module\"], index=[\"name\", \"measurement\"])\n\n");
                }

                inputBuilder.append("# You can perform any transformations on the data here\n\n");

                inputBuilder.append("# Finally, the results are plotted\n");
                inputBuilder.append("chart.plot" + typeName + "(df)\n");

                String name = "Chart" + (++counter);
                String title = StringUtils.defaultIfEmpty(ScriptEngine.defaultTitle(ScaveModelUtil.getResultItems(idList, manager)), name);


                Chart chart = ScaveModelUtil.createChart(name, title, type);
                chart.setScript(inputBuilder.toString());
                chart.setTemporary(true);
                //TODO cache the IDs
                // IDList cachedIDs = new IDList();
                // cachedIDs.set(idList);;
                // add.setCachedIDs(cachedIDs);  //TODO see Add.cachedIDs field in scave_old
                return chart;
            }
        });

        editor.openChart(chart);
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, IStructuredSelection selection) {
        return selection instanceof IDListSelection && !selection.isEmpty();
    }
}

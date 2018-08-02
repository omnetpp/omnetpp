/*--------------------------------------------------------------*

  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.util.concurrent.Callable;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.editors.IDListSelection;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.MatplotlibChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.ScaveModelUtil;
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

    // TODO: move this into ScaveModuleUtil ?
    // copied from ScaveModuleUtil.getIDListAsChartInput
    String getIDListAsScript(String title, IDList ids, String[] runidFields, ResultType type, ResultFileManager manager) {
        Assert.isNotNull(runidFields);
        String[] filterFields = ScaveModelUtil.getFilterFieldsFor(runidFields);

        String typeName = type.getName();
        typeName = typeName.substring(0, 1).toUpperCase() + typeName.substring(1) + "s";

        StringBuilder sb = new StringBuilder("df = results.get" + typeName + "(\"\"\"\n");

        sb.append(ScaveModelUtil.getIDListAsChartInput(ids, filterFields, manager));

        sb.append("\n\"\"\")\n");

        if (type.getName().equals("vector")) {
            sb.append("df = df[df.type == 'vector']\n");

            sb.append("plt.xlabel('Simulation time (s)')\n");
            sb.append("plt.ylabel('Vector value')\n");

            sb.append("for t in df[['vectime', 'vecvalue', 'module', 'name']].itertuples(index=False):\n");
            sb.append("    plt.plot(t[0], t[1], label=(t[2] + ':' + t[3])[:-7], drawstyle='steps-post')\n");

        } else if (type.getName().equals("scalar")) {
            sb.append("df = results.pivotScalars(df)\n");
            sb.append("df.plot(kind='bar')\n");
        } else if (type.getName().equals("histogram")) {
            sb.append("df = df[df.type == 'histogram']\n");

            //sb.append("plt.xlabel('Simulation time (s)')\n");
            //sb.append("plt.ylabel('Vector value')\n");

            sb.append("for t in df[['binedges', 'binvalues', 'module', 'name']].itertuples(index=False):\n");
            sb.append("    plt.hist(bins=t[0][1:], x=t[0][1:-1], weights=t[1][1:-1], label=t[2] + ':' + t[3], histtype='step')\n");
        }

        sb.append("plt.legend()\n");
        sb.append("plt.title(\"\"\"" + title + "\"\"\")\n");
        sb.append("plt.tight_layout()\n");

        //sb.append("cursor = mpl.widgets.Cursor(plt.gca(), useblit=True, color='red', linewidth=2)\n");

        // System.out.println(sb.toString());
        return sb.toString();
    }

    protected void openMatplotlibChart(final ScaveEditor editor, final ResultFileManager manager, final ResultType type, final IDList idList) {
        Chart chart = ResultFileManager.callWithReadLock(manager, new Callable<Chart>() {
            @Override
            public Chart call() {
                //String[] filterFields = new String[] { ResultItemField.FILE, RunAttribute.CONFIGNAME, RunAttribute.RUNNUMBER };
                String[] filterFields = new String[] { ResultItemField.RUN };

                String name = "MatplotlibChart" + (++counter);
                String title = StringUtils.defaultIfEmpty(ScriptEngine.defaultTitle(ScaveModelUtil.getResultItems(idList, manager)), name);
                String input = getIDListAsScript(title, idList, filterFields, type, manager);
                //Chart chart = ScaveModelUtil.createChart(name, title, type);


                MatplotlibChart chart = ScaveModelUtil.createMatplotlibChart(name);

                Property property = ScaveModelFactory.eINSTANCE.createProperty();
                property.setName(ChartProperties.PROP_GRAPH_TITLE);
                property.setValue(title);
                chart.getProperties().add(property);


                chart.setScript(input);
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

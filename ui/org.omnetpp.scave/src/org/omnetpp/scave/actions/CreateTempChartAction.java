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
                String[] filterFields = new String[] { ResultItemField.RUN };
                String input = ScaveModelUtil.getIDListAsChartInput(idList, filterFields, manager);
                String name = "Chart" + (++counter);
                String title = StringUtils.defaultIfEmpty(ScriptEngine.defaultTitle(ScaveModelUtil.getResultItems(idList, manager)), name);
                Chart chart = ScaveModelUtil.createChart(name, title, type);
                chart.setInput(input);
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

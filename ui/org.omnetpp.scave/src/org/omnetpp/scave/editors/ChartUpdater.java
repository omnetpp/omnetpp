/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocumentListener;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.model.commands.ICommand;
import org.omnetpp.scave.model.commands.SetChartScriptCommand;

/**
 * This class listens on changes of the edited text document, propagating the edits
 * into the model by setting the script of the chart with a command.
 * This class also listens on changes in the model, and refreshes the chart accordingly
 * by asking the editor to execute the script again.
 */
public class ChartUpdater implements IModelChangeListener, IDocumentListener {
    private static final int CHART_SCRIPT_TYPING_DELAY_MS = 1500;
    private static final int CHART_SCRIPT_EXECUTION_DELAY_MS = 100;

    private final Chart chart;
    private final ChartScriptEditor editor;

    private boolean autoUpdateChart = true;

    private final DelayedJob setChartScriptJob = new DelayedJob(CHART_SCRIPT_TYPING_DELAY_MS) {
        @Override
        public void run() {
            String newCode = editor.getDocument().get();
            if (!newCode.equals(chart.getScript())) {
                ICommand command = new SetChartScriptCommand(chart, newCode);
                editor.getScaveEditor().executeCommand(command);
            }
        }
    };

    private final DelayedJob rerunChartScriptJob = new DelayedJob(CHART_SCRIPT_EXECUTION_DELAY_MS) {
        @Override
        public void run() {
            editor.refreshChart();
        }
    };

    public ChartUpdater(Chart chart, ChartScriptEditor editor) {
        this.chart = chart;
        this.editor = editor;
    }

    @Override
    public void documentAboutToBeChanged(DocumentEvent event) {
        // no-op
    }

    @Override
    public void documentChanged(DocumentEvent event) {
            setChartScriptJob.restartTimer();
    }

    @Override
    public void modelChanged(ModelChangeEvent event) {
        if (autoUpdateChart)
            rerunChartScriptJob.restartTimer();
    }

    public boolean isAutoUpdateChart() {
        return autoUpdateChart;
    }

    public void setAutoUpdateChart(boolean autoUpdateChart) {
        if (this.autoUpdateChart != autoUpdateChart) {
            this.autoUpdateChart = autoUpdateChart;
            if (autoUpdateChart)
                rerunChartScriptJob.restartTimer();
        }
    }
}

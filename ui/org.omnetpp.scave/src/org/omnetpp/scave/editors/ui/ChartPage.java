package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ModelChangeEvent;

public class ChartPage extends FormEditorPage {

    protected ChartScriptEditor chartScriptEditor;

    public ChartPage(Composite parent, int style, ScaveEditor scaveEditor, ChartScriptEditor chartScriptEditor) {
        super(parent, style, scaveEditor);
        this.chartScriptEditor = chartScriptEditor;
    }

    public ChartScriptEditor getChartScriptEditor() {
        return chartScriptEditor;
    }

    @Override
    public PlotBase getActivePlot() {
        Control viewer = chartScriptEditor.getChartViewer().getWidget();
        return viewer instanceof PlotBase ? (PlotBase)viewer : null;
    }

    @Override
    public void updatePage(ModelChangeEvent event) {
        // TODO update description
        setPageTitle(chartScriptEditor.getChartDisplayName());
        setFormTitle(chartScriptEditor.getChartDisplayName());
    }
}

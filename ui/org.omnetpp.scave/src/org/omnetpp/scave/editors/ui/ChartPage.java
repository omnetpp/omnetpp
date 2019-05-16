package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.omnetpp.scave.charting.ChartViewer;
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
    public ChartViewer getActiveChartViewer() {
        Control viewer = chartScriptEditor.getChartViewer().getWidget();
        return viewer instanceof ChartViewer ? (ChartViewer)viewer : null;
    }

    @Override
    public void updatePage(ModelChangeEvent event) {
        // TODO update description
        setPageTitle(chartScriptEditor.getChartName());
        setFormTitle(chartScriptEditor.getChartName());
    }
}

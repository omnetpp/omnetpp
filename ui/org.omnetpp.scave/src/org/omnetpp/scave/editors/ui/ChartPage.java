package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.widgets.Composite;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;

public class ChartPage extends FormEditorPage {

    protected ChartScriptEditor chartScriptEditor;

    public ChartPage(Composite parent, int style, ScaveEditor scaveEditor, ChartScriptEditor chartScriptEditor) {
        super(parent, style, scaveEditor);
        this.chartScriptEditor = chartScriptEditor;
    }

    public ChartScriptEditor getChartScriptEditor() {
        return chartScriptEditor;
    }
}

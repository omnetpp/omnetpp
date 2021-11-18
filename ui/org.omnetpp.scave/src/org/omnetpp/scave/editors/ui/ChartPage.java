/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors.ui;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IMemento;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.python.ChartViewerBase;

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
    public ChartViewerBase getActiveChartViewer() {
        return chartScriptEditor.getChartViewer();
    }

    @Override
    public void modelChanged(ModelChangeEvent event) {
        setPageTitle(chartScriptEditor.getChartDisplayName());
        setFormTitle(chartScriptEditor.getChartDisplayName());
    }

    @Override
    public void saveState(IMemento memento) {
        chartScriptEditor.saveState(memento);
    }

    @Override
    public void restoreState(IMemento memento) {
        //TODO getChartScriptEditor().restoreState(memento);
    }

}

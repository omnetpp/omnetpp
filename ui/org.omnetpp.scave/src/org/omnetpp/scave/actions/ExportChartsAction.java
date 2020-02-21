package org.omnetpp.scave.actions;

import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ExportChartsDialog;
import org.omnetpp.scave.jobs.ChartExport;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class ExportChartsAction extends AbstractScaveAction {

    public ExportChartsAction() {
        setText("Export Charts...");
        setToolTipText("Export charts to image files");
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        // we'll display the list of all charts, and check the selected one(s) and those under selected ones
        final List<Chart> allCharts = ScaveModelUtil.collectCharts(editor.getAnalysis().getCharts().getCharts());
        List<Chart> initialSelection = ScaveModelUtil.getChartsFromSelection(asStructuredOrEmpty(selection));

        ExportChartsDialog dialog = new ExportChartsDialog(editor.getSite().getShell(), allCharts, initialSelection);
        int result = dialog.open();
        if (result == Window.OK) {
            try {
                List<Chart> selectedCharts = dialog.getSelectedCharts();
                String fileFormat = dialog.getFileFormat();
                IContainer targetFolder = dialog.getTargetFolder();
                if (!selectedCharts.isEmpty())
                    ChartExport.exportChartImages(selectedCharts, targetFolder, fileFormat, editor.getAnfFile().getParent().getLocation().toFile(), editor.getResultFileManager());
            }
            catch (OperationCanceledException e) {
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        List<Chart> charts = ScaveModelUtil.collectCharts(editor.getAnalysis().getCharts().getCharts());
        return !charts.isEmpty();
    }
}

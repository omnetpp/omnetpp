package org.omnetpp.scave.actions;

import java.io.File;
import java.util.HashMap;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ExportChartsDialog;
import org.omnetpp.scave.jobs.ChartExport;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Batch export of chart image and/or data
 */
public class ExportChartsAction extends AbstractScaveAction {

    public ExportChartsAction() {
        setText("Export Charts...");
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        // we'll display the list of all charts, and check the selected one(s) and those under selected ones
        final List<Chart> allCharts = ScaveModelUtil.collectCharts(editor.getAnalysis().getCharts().getCharts());
        List<Chart> initialSelection = ScaveModelUtil.getChartsFromSelection(asStructuredOrEmpty(selection));

        ExportChartsDialog dialog = new ExportChartsDialog(editor.getSite().getShell(), allCharts, initialSelection, editor.getAnfFile());
        if (dialog.open() == Window.OK) {
            try {
                ExportChartsDialog.Result result = dialog.getResult();
                if (!result.selectedCharts.isEmpty()) {
                    File chartsDir = editor.getAnfFile().getParent().getLocation().toFile();
                    HashMap<String,String> extraProperties = new HashMap<>();
                    if (result.exportImages)
                        extraProperties.putAll(ChartExport.makeExtraPropertiesForImageExport(result.imageTargetFolder, result.imageFormat, result.imageDpi));
                    if (result.exportData)
                        extraProperties.putAll(ChartExport.makeExtraPropertiesForDataExport(result.dataTargetFolder));
                    ChartExport.exportCharts(result.selectedCharts, extraProperties, chartsDir, editor.getResultFileManager(), result.stopOnError, result.numConcurrentProcesses);
                }
            }
            catch (OperationCanceledException e) {
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return true;
    }
}

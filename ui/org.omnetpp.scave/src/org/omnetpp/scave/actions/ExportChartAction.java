package org.omnetpp.scave.actions;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.OperationCanceledException;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.window.Window;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.editors.ui.ExportChartsDialog;
import org.omnetpp.scave.jobs.ChartExport;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.python.ChartViewerBase;

/**
 * Export of chart image and/or data
 */
public class ExportChartAction extends AbstractScaveAction {

    public ExportChartAction() {
        setText("Export Chart...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EXPORT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        ChartViewerBase chartViewer = editor.getActiveChartViewer();
        Chart chart = chartViewer != null ? chartViewer.getChart() : null;
        if (chart == null)
            return;
        List<Chart> tmp = new ArrayList<Chart>();
        tmp.add(chart);

        ExportChartsDialog dialog = new ExportChartsDialog(editor.getSite().getShell(), chart, editor.getAnfFile());
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
                    ChartExport.exportChart(chart, extraProperties, chartsDir, editor.getResultFileManager());
                }
            }
            catch (OperationCanceledException e) {
            }
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActiveChartViewer() != null;
    }
}

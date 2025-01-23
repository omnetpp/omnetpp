/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.io.File;
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
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * Batch export of chart image and/or data
 */
public class ExportChartsAction extends AbstractScaveAction {

    public ExportChartsAction() {
        setText("Export Chart Graphics/Data...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EXPORT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        // we'll display the list of all charts, and check the selected one(s) and those under selected ones
        final List<Chart> allCharts = ScaveModelUtil.collectCharts(editor.getAnalysis().getRootFolder());
        List<Chart> initialSelection = selection.isEmpty() ? null /*reuse last setting*/ : ScaveModelUtil.getChartsFromSelection(asStructuredOrEmpty(selection));

        ExportChartsDialog dialog = new ExportChartsDialog(editor.getSite().getShell(), allCharts, initialSelection, editor.getAnfFile(), editor.getActiveCommandStack());
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
                    ChartExport.exportCharts(result.selectedCharts, extraProperties, chartsDir,
                            editor.getResultFileManager(), editor.getMemoizationCache(), editor.getFilterCache(),
                            result.stopOnError, result.numConcurrentProcesses);
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

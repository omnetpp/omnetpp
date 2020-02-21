/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.python.ChartViewerBase;
import org.omnetpp.scave.python.MatplotlibChartViewer;

/**
 * Save the plot's image as currently visible in the active chart viewer.
 */
public class SaveImageAction extends AbstractScaveAction {
    public SaveImageAction() {
        setText("Save Image...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_SAVEIMAGE));
    }

    @Override
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Shell parentShell = Display.getCurrent().getActiveShell();
        ChartScriptEditor chartEditor = scaveEditor.getActiveChartScriptEditor();
        ChartViewerBase chartViewer = chartEditor == null ? null : chartEditor.getChartViewer();
        if (chartViewer == null)
            return;
        if (chartViewer instanceof MatplotlibChartViewer && !((MatplotlibChartViewer)chartViewer).isSaveImagePossible())
            return;

        String defaultFileName = chartEditor.getChart().getName() + ".svg";
        IContainer defaultFolder = scaveEditor.getAnfFile().getParent();

        String fileName = askFileName(parentShell, defaultFolder, defaultFileName);
        if (fileName == null)
            return; // cancelled

        try {
            String finalFileName = fileName;
            BusyIndicator.showWhile(Display.getDefault(), () -> chartViewer.saveImage(finalFileName));
        }
        catch (Exception e) {
            MessageDialog.openError(parentShell, "Error", "Image save failed: " + e.getMessage());
        }

    }

    protected String askFileName(Shell parent, IContainer defaultFolder, String defaultFileName) {
        SaveAsDialog dialog = new SaveAsDialog(parent) {
            @Override
            public void create() {
                super.create();
                setTitle("Save Image"); // note: these calls won't work when called before the dialog has been created...
                setMessage("Saves the chart's current view. Popular vector and raster formats are accepted.");
            }
        };
        dialog.setOriginalFile(defaultFolder.getFile(new Path(defaultFileName)));
        if (dialog.open() != Dialog.OK)
            return null;
        IPath path = dialog.getResult();
        IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(path);
        return file.getLocation().toOSString();
    }

    @Override
    protected boolean isApplicable(ScaveEditor scaveEditor, ISelection selection) {
        ChartScriptEditor chartEditor = scaveEditor.getActiveChartScriptEditor();
        if (chartEditor == null)
            return false;
        if (chartEditor.getNativeChartViewer() != null)
            return true;
        if (chartEditor.getMatplotlibChartViewer() != null && chartEditor.getMatplotlibChartViewer().isSaveImagePossible())
            return true;
        return false;
    }


}

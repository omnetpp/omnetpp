/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.io.File;
import java.util.ArrayList;
import java.util.Map;

import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.MessageBox;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.part.FileEditorInput;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.charting.PlotBase;
import org.omnetpp.scave.editors.ChartScriptEditor;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.python.ChartViewerBase;
import org.omnetpp.scave.python.MatplotlibChartViewer;
import org.omnetpp.scave.python.NativeChartViewer;

/**
 * Save the plot's image as currently visible in the active chart viewer.
 */
public class SaveImageAction extends AbstractScaveAction {
    public SaveImageAction() {
        setText("Save Image...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_SAVEIMAGE));
    }

    @Override
    protected void doRun(final ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        ChartScriptEditor chartEditor = scaveEditor.getActiveChartScriptEditor();
        ChartViewerBase chartViewer = chartEditor == null ? null : chartEditor.getChartViewer();

        if (chartViewer instanceof MatplotlibChartViewer) {
            MatplotlibChartViewer matplotlibChartViewer = (MatplotlibChartViewer)chartViewer;
            if (matplotlibChartViewer != null) {
                if (matplotlibChartViewer.isSaveImagePossible())
                    mplAskFilenameAndSaveImage(matplotlibChartViewer, chartEditor.getChart().getName()); //TODO make the two branches similar
            }
        }
        else if (chartViewer instanceof NativeChartViewer) {
            final PlotBase plot = ((NativeChartViewer)chartViewer).getPlot();
            String fileName = askFileName(scaveEditor);
            if (fileName != null)
                BusyIndicator.showWhile(Display.getDefault(), () -> plot.saveImage(fileName));
        }
    }

    private void mplAskFilenameAndSaveImage(MatplotlibChartViewer viewer, String defaultName) {
        Map<String, ArrayList<String>> types = viewer.getSupportedFiletypes();

        String defaultType = viewer.getDefaultFiletype();
        int filterIndex = 0;

        String[] names = new String[types.keySet().size()];
        Object[] ks = types.keySet().toArray();
        for (int i = 0; i < ks.length; ++i)
            names[i] = (String) ks[i];

        String[] extensions = new String[names.length];

        for (int i = 0; i < names.length; ++i) {
            String filter = "";

            ArrayList<String> exts = types.get(names[i]);
            for (String e : exts) {
                if (e.equals(defaultType))
                    filterIndex = i;
                filter += ("*." + e + ";");
            }

            extensions[i] = filter.substring(0, filter.length() - 1);
            names[i] += " (" + extensions[i].replace(";", ", ") + ")";
        }

        String filename = mplAskFileName(names, extensions, filterIndex, defaultName, Display.getCurrent().getActiveShell());

        if (filename != null) {
            try {
                viewer.saveImage(filename);
            }
            catch (Exception e) {
                MessageBox messageBox = new MessageBox(Display.getCurrent().getActiveShell(), SWT.OK | SWT.APPLICATION_MODAL | SWT.ICON_ERROR);
                messageBox.setText("Error");
                messageBox.setMessage("Image save failed: " + e.getMessage());
                messageBox.open();
            }
        }
    }

    //TODO merge the two askFileName() methods

    private String mplAskFileName(String[] filterNames, String[] filterExtensions, int filterIndex, String fileName, Shell shell) {

        FileDialog dialog = new FileDialog(shell, SWT.SAVE);
        dialog.setFilterNames(filterNames);
        dialog.setFilterExtensions(filterExtensions);
        dialog.setFilterIndex(filterIndex);
        dialog.setFileName(fileName);

        String filename = dialog.open();

        if (filename != null) {
            File file = new File(filename);
            if (file.exists()) {
                MessageBox messageBox = new MessageBox(shell,
                        SWT.YES | SWT.NO | SWT.APPLICATION_MODAL | SWT.ICON_WARNING);
                messageBox.setText("File already exists");
                messageBox.setMessage("The file " + filename
                        + " already exists and will be overwritten. Do you want to continue the operation?");
                if (messageBox.open() == SWT.NO)
                    filename = null;
            }
        }

        return filename;
    }


    private String askFileName(ScaveEditor editor) {
        Shell activeShell = Display.getCurrent().getActiveShell();
        FileDialog fileDialog = new FileDialog(activeShell, SWT.SAVE);
        IEditorInput editorInput = editor.getEditorInput();
        if (editorInput instanceof FileEditorInput) {
            IPath location = ((FileEditorInput)editorInput).getFile().getLocation().makeAbsolute();
            fileDialog.setFileName(location.removeFileExtension().addFileExtension("svg").lastSegment());
            fileDialog.setFilterPath(location.removeLastSegments(1).toOSString());
        }
        String fileName = fileDialog.open();
        if (fileName != null) {
            File file = new File(fileName);
            if (file.exists()) {
                MessageBox messageBox = new MessageBox(activeShell, SWT.OK | SWT.CANCEL | SWT.APPLICATION_MODAL | SWT.ICON_WARNING);
                messageBox.setText("File already exists");
                messageBox.setMessage("The file " + fileName + " already exists and will be overwritten. Do you want to continue the operation?");
                if (messageBox.open() == SWT.CANCEL)
                    fileName = null;
            }
        }
        return fileName;
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

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
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.BusyIndicator;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.SaveAsDialog;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
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
    protected void doRun(ScaveEditor scaveEditor, ISelection selection) throws CoreException {
        Shell parentShell = Display.getCurrent().getActiveShell();
        ChartScriptEditor chartEditor = scaveEditor.getActiveChartScriptEditor();
        ChartViewerBase chartViewer = chartEditor == null ? null : chartEditor.getChartViewer();
        if (chartViewer == null)
            return;

        String defaultFileName = chartEditor.getChart().getName();
        String defaultLocation = scaveEditor.getAnfFile().getParent().getLocation().toOSString();

        String fileName = null;

        if (chartViewer instanceof MatplotlibChartViewer) {
            MatplotlibChartViewer matplotlibChartViewer = (MatplotlibChartViewer)chartViewer;
            if (!matplotlibChartViewer.isSaveImagePossible())
                return;
            FileTypes filter = toFileDialogFilter(matplotlibChartViewer.getSupportedFiletypes(), matplotlibChartViewer.getDefaultFiletype());
            fileName = askFileName(parentShell, defaultLocation, defaultFileName, filter.names, filter.extensions, filter.defaultIndex);
        }
        else if (chartViewer instanceof NativeChartViewer) {
            // NativeChartViewer currently only supports SVG
            fileName = askFileName(parentShell, defaultLocation, defaultFileName, new String[] {"Scalable Vector Graphics"}, new String[] {"svg"}, 0);
        }

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

    static class FileTypes {
        public String[] names;
        public String[] extensions;
        public int defaultIndex;
    }

    private static FileTypes toFileDialogFilter(Map<String, ArrayList<String>> types, String defaultType) {
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

        FileTypes f = new FileTypes();
        f.extensions = extensions;
        f.names = names;
        f.defaultIndex = filterIndex;
        return f;
    }

    private String askFileName(Shell parent, String defaultLocation, String defaultFileName, String[] filterNames, String[] filterExtensions, int filterIndex) {
        // Issue: The filter combo in the dialog does NOT change the extension of the entered file name,
        // so it's essentially useless. Choosing e.g. PNG in it won't result in a PNG file being saved,
        // the user manually needs to enter the file extension for that. Note: getFilterIndex() will return
        // the file type of the filename, NOT the combo state!
        FileDialog dialog = new FileDialog(parent, SWT.SAVE);
        dialog.setFileName(defaultFileName);
        dialog.setFilterNames(filterNames);
        dialog.setFilterExtensions(filterExtensions);
        dialog.setFilterIndex(filterIndex);
        dialog.setFilterPath(defaultLocation);
        dialog.setOverwrite(true); // ask for confirmation
        return dialog.open();
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

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.io.File;

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
import org.omnetpp.scave.editors.ScaveEditor;

public class ExportToSVGAction extends AbstractScaveAction {
    public ExportToSVGAction() {
        setText("Export to SVG...");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_EXPORTTOSVG));
        setToolTipText("Export char to SVG format");
    }

    @Override
    protected void doRun(final ScaveEditor editor, ISelection selection) {
        final PlotBase plot = editor.getActivePlot();
        if (plot != null) {
            BusyIndicator.showWhile(Display.getDefault(), new Runnable() {
                public void run() {
                    String fileName = askFileName(editor);
                    if (fileName != null)
                        plot.exportToSVG(fileName);
                }
            });
        }
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return editor.getActivePlot() != null;
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
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.eclipse.jface.action.Action;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.FileDialog;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;

import org.omnetpp.scave.editors.DatasetEditor;
import org.omnetpp.scave.editors.ScalarChartEditor;
import org.omnetpp.scave.editors.ScalarDatasetEditor;
import org.omnetpp.scave.editors.VectorChartEditor;
import org.omnetpp.scave.editors.VectorDatasetEditor;


public class OpenResultFileAction extends Action {

    // File type descriptions for the Open dialog
    private static final String[] SCA_FILTER_NAMES = {"Scalar files (*.sca)", "All Files (*.*)"};
    private static final String[] VEC_FILTER_NAMES = {"Vector files (*.vec)", "All Files (*.*)"};

    // These filter extensions are used to filter which files are displayed.
    private static final String[] SCA_FILTER_EXTS = { "*.sca", "*.*"};
    private static final String[] VEC_FILTER_EXTS = { "*.vec", "*.*"};

    private final IWorkbenchWindow window;

    public OpenResultFileAction(IWorkbenchWindow window) {
        this.window = window;
        // The id is used to refer to the action in a menu or toolbar
        setId(ICommandIds.CMD_OPEN_RESULT_FILE);

        // Associate the action with a pre-defined command, to allow key bindings.
        // TODO this would probably need a command definition in plugin.xml? (Otherwise we get "undefined command" exception in the log)
        //setActionDefinitionId(ICommandIds.CMD_OPEN_RESULT_FILE);

        // Set other properties
        setText("Load Scalar/Vector File...");
        setImageDescriptor(PlatformUI.getWorkbench().getSharedImages().getImageDescriptor(IDE.SharedImages.IMG_OBJ_PROJECT)); // FIXME icon
        //setImageDescriptor(ScavePlugin.getImageDescriptor("/icons/sample.gif")); --THIS FILE DOESN'T EXIST
        setToolTipText("Load scalar/vector file and add to current dataset");
    }

    public void run() {
        // ask file name
        IEditorPart editor = window.getActivePage().getActiveEditor();
        FileDialog dlg = new FileDialog(window.getShell(), SWT.OPEN);
        if (editor instanceof ScalarDatasetEditor || editor instanceof ScalarChartEditor) {
            dlg.setText("Open Scalar File");
            dlg.setFilterNames(SCA_FILTER_NAMES);
            dlg.setFilterExtensions(SCA_FILTER_EXTS);
        }
        else if (editor instanceof VectorDatasetEditor || editor instanceof VectorChartEditor) {
            dlg.setText("Open Vector File");
            dlg.setFilterNames(VEC_FILTER_NAMES);
            dlg.setFilterExtensions(VEC_FILTER_EXTS);
        }
        String fileName = dlg.open();

        // load file & put contents into the editor
        if (fileName!=null) {
            DatasetEditor dsEditor = (DatasetEditor)editor;
            dsEditor.loadFileAndAddContentsToDataset(fileName);
        }
    }
}

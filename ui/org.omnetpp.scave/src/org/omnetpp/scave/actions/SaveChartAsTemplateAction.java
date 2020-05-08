/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import java.io.IOException;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.jface.dialogs.Dialog;
import org.eclipse.jface.dialogs.InputDialog;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.ISharedImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model2.ScaveModelUtil;

public class SaveChartAsTemplateAction extends AbstractScaveAction {
    public SaveChartAsTemplateAction() {
        setText("Save as Template");
        setImageDescriptor(ScavePlugin.getSharedImageDescriptor(ISharedImages.IMG_ETOOL_SAVEAS_EDIT));
    }

    @Override
    protected void doRun(ScaveEditor editor, ISelection selection) throws CoreException {
        Chart chart = ScaveModelUtil.getChartFromSingleSelection(selection);

        IProject currentProject = editor.getAnfFile().getProject();
        IFolder templatesFolder = currentProject.getFolder("charttemplates");

        InputDialog dialog = new InputDialog(Display.getCurrent().getActiveShell(),
                "Save Chart as Template",
                "Enter filename base below. Files will be created in the analysis file's project, " +
                "under " + templatesFolder.getFullPath().toString()+ ".",
                chart.getTemplateID() + "_" + chart.getId(), (String newText) -> {
                    IStatus status = templatesFolder.getWorkspace().validateName(newText, IResource.FILE);
                    return status.isOK() ? null : status.getMessage();
                });
        if (dialog.open() != Dialog.OK)
            return;

        String newTemplateId = dialog.getValue();

        try {
            ScaveModelUtil.saveChartAsTemplate(chart, templatesFolder.getLocation().toFile(), newTemplateId);
            templatesFolder.refreshLocal(IFile.DEPTH_ONE, null);
        } catch (IOException e) {
            throw ScavePlugin.wrapIntoCoreException(e);
        }

//        MessageDialog.openInformation(editor.getSite().getShell(), "Chart saved as template",
//                "The selected chart was exported as a template, generating the following files in the \"" +
//                templatesDir.getAbsolutePath() + "\" directory:\n\n" + filenames + "\n\n" +
//                "You can create new charts from it using the \"New\" submenu of the context menu on the \"Charts\" page."
//                );
    }

    @Override
    protected boolean isApplicable(ScaveEditor editor, ISelection selection) {
        return ScaveModelUtil.getChartFromSingleSelection(selection) != null;
    }
}

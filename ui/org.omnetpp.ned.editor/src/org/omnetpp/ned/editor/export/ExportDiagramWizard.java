/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.gef.EditPart;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.wizard.Wizard;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IExportWizard;
import org.eclipse.ui.IPathEditorInput;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.omnetpp.ned.editor.NedEditorPlugin;

public class ExportDiagramWizard extends Wizard implements IExportWizard
{
    private IWorkbench workbench;
    private ExportDiagramWizardPage page;
    private List<GraphicalEditPart> selectedEditParts;
    private List<Integer> selectionStates;
    private IPath diagramPath;

    public ExportDiagramWizard()
    {
        setNeedsProgressMonitor(true);
        setWindowTitle("Export Current GEF Diagram as Image");
        ImageDescriptor pageImageDescriptor = AbstractUIPlugin.imageDescriptorFromPlugin(NedEditorPlugin.PLUGIN_ID, "icons/export-diagram-image-banner.png");
        setDefaultPageImageDescriptor(pageImageDescriptor);
        initDialogSettings();
    }

    private void initDialogSettings()
    {
        IDialogSettings dialogSettings = NedEditorPlugin.getDefault().getDialogSettings();
        IDialogSettings thisWizardSection = dialogSettings.getSection("ExportDiagramWizard");
        if (thisWizardSection == null)
            thisWizardSection = dialogSettings.addNewSection("ExportDiagramWizard");
        setDialogSettings(thisWizardSection);
    }

    public void init(IWorkbench iworkbench, IStructuredSelection s)
    {
        workbench = iworkbench;
        selectedEditParts = new ArrayList<GraphicalEditPart>();
        IEditorPart currentEditor = workbench.getActiveWorkbenchWindow().getActivePage().getActiveEditor();
        Object input= currentEditor.getEditorInput();
        if (input instanceof IPathEditorInput)
            diagramPath = ((IPathEditorInput) input).getPath();
        if (!s.isEmpty())
            for (Object selectedObject : s.toList())
                selectedEditParts.add((GraphicalEditPart) selectedObject);
        else {
            GraphicalEditPart rootEditPart = (GraphicalEditPart) currentEditor.getAdapter(EditPart.class);
            selectedEditParts.add((GraphicalEditPart) rootEditPart.getViewer().getContents());
        }
        unselectEditParts();
    }

    public void addPages()
    {
        page = new ExportDiagramWizardPage(diagramPath, selectedEditParts);
        addPage(page);
    }

    private void unselectEditParts()
    {
        selectionStates = new ArrayList<Integer>(selectedEditParts.size());
        for (GraphicalEditPart editPart : selectedEditParts) {
            selectionStates.add(editPart.getSelected());
            editPart.setSelected(EditPart.SELECTED_NONE);
        }
    }

    private void reselectEditParts()
    {
        for (int i=0; i<selectedEditParts.size(); ++i)
            selectedEditParts.get(i).setSelected(selectionStates.get(i));
    }

    @Override
    public boolean performCancel()
    {
        reselectEditParts();
        return super.performCancel();
    }

    public boolean performFinish()
    {
        boolean finished = true;
        try {
            List<GraphicalEditPart> editPartsToExport = page.getEditPartsToExport();
            page.finish();
            finished = executeExportOperation(new ExportDiagramOperation(diagramPath, editPartsToExport, page.getImageExporter(), new File(page.getOutputFileName()), page.isAntialias(), page));
        }
        catch (InterruptedException e) {
            finished = false;
        }
        catch (InvocationTargetException e) {
            MessageDialog.openError(getShell(), "Error", e.toString() + "(" + e.getCause().getMessage() + ")");
            finished = false;
        }
        finally {
            reselectEditParts();
        }
        return finished;
    }

    protected boolean executeExportOperation(ExportDiagramOperation operation) throws InvocationTargetException, InterruptedException
    {
        getContainer().run(false, true, operation);
        IStatus status = operation.getStatus();
        if (!status.isOK()) {
            ErrorDialog.openError(getContainer().getShell(), "Diagram Export Errors", null, status);
            return false;
        }
        return true;
    }
}

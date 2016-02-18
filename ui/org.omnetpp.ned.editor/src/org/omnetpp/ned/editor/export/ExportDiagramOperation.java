/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.text.MessageFormat;
import java.util.List;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.draw2d.IFigure;
import org.eclipse.gef.GraphicalEditPart;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.ui.dialogs.IOverwriteQuery;
import org.omnetpp.ned.editor.NedEditorPlugin;

public class ExportDiagramOperation implements IRunnableWithProgress
{
    private IPath diagramPath;
    private List<GraphicalEditPart> editParts;
    private DiagramExporter diagramExporter;
    private File file;
    private boolean antialias;
    private IOverwriteQuery overwriteQuery;
    private MultiStatus status = new MultiStatus(NedEditorPlugin.PLUGIN_ID, IStatus.OK, "Errors while exporting diagrams", null);

    public ExportDiagramOperation(IPath diagramPath, List<GraphicalEditPart> editParts, DiagramExporter diagramExporter, File file, boolean antialias, IOverwriteQuery overwriteQuery)
    {
        this.diagramPath = diagramPath;
        this.editParts = editParts;
        this.diagramExporter = diagramExporter;
        this.file = file;
        this.antialias = antialias;
        this.overwriteQuery = overwriteQuery;
    }

    public void run(IProgressMonitor monitor) throws InterruptedException
    {
        try {
            monitor.beginTask("Exporting diagram...", editParts.size());
            exportDiagram(monitor);
        }
        finally {
            monitor.done();
        }
    }

    protected void exportDiagram(IProgressMonitor monitor) throws InterruptedException
    {
        monitor.beginTask("Exporting diagram...", editParts.size());
        if (file.exists()) {
            if (!file.canWrite()) {
                String msg = "Image file '{0}' is not writable.";
                addStatus(IStatus.ERROR, MessageFormat.format(msg, file.getAbsolutePath()), null);
                return;
            }
            String overwriteAnswer = overwriteQuery.queryOverwrite(file.getAbsolutePath());
            if (overwriteAnswer.equals(IOverwriteQuery.CANCEL))
                throw new InterruptedException();
            else if (overwriteAnswer.equals(IOverwriteQuery.NO))
                return;
            else if (overwriteAnswer.equals(IOverwriteQuery.NO_ALL))
                return;
        }
        exportImage();
        try {
            IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
            for (IContainer container : workspaceRoot.findContainersForLocation(new Path(file.getParent())))
                container.refreshLocal(IResource.DEPTH_ONE, monitor);
        }
        catch (Exception e) {
            // Nothing to do about it.
        }
    }

    private void exportImage()
    {
        try {
            NedFigureProvider figureProvider = new NedFigureProvider();
            List<IFigure> figures = figureProvider.getDiagramFigures(editParts);
            if (figures != null)
                diagramExporter.exportDiagram(figures, file, antialias);
            else {
                String msg = "{0} failed to provide a figure for '{1}'.";
                addStatus(IStatus.ERROR, MessageFormat.format(msg, figureProvider, diagramPath.toOSString()), null);
            }
        }
        catch (Throwable e) {
            String msg = "Error while exporting '{0}'.";
            addStatus(IStatus.ERROR, MessageFormat.format(msg, diagramPath.toOSString()), null);
        }
    }

    public IStatus getStatus()
    {
        return status;
    }

    public void addStatus(int severity, String message, Throwable e)
    {
        status.add(new Status(severity, NedEditorPlugin.PLUGIN_ID, 0, message, e));
    }
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.export;

import java.io.File;
import java.text.MessageFormat;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.MultiStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.draw2d.IFigure;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.ui.dialogs.IOverwriteQuery;
import org.omnetpp.ned.editor.NedEditorPlugin;

public class ExportDiagramFilesOperation implements IRunnableWithProgress
{
    public enum OverwriteMode
    {
        ASK_USER,
        NONE,
        ALL
    }

    private List<IFile> diagramFiles;
    private DiagramExporter diagramExporter;
    private boolean antialias;
    private IOverwriteQuery overwriteQuery;
    private OverwriteMode overwriteMode = OverwriteMode.ASK_USER;
    private MultiStatus status = new MultiStatus(NedEditorPlugin.PLUGIN_ID, IStatus.OK, "Errors while exporting diagrams", null);

    public ExportDiagramFilesOperation(List<IFile> diagramFiles, DiagramExporter diagramExporter, boolean antialias, IOverwriteQuery overwriteQuery)
    {
        this.diagramFiles = diagramFiles;
        this.diagramExporter = diagramExporter;
        this.antialias = antialias;
        this.overwriteQuery = overwriteQuery;
    }

    public void setOverwriteMode(OverwriteMode overwriteMode)
    {
        this.overwriteMode = overwriteMode;
    }

    public void run(IProgressMonitor monitor) throws InterruptedException
    {
        try {
            monitor.beginTask("Exporting diagrams...", diagramFiles.size());
            exportDiagramFiles(monitor);
        }
        finally {
            monitor.done();
        }
    }

    protected void exportDiagramFiles(IProgressMonitor monitor) throws InterruptedException
    {
        monitor.beginTask("Exporting diagrams...", diagramFiles.size());
        for (IFile diagramFile : diagramFiles) {
            if (!diagramFile.isAccessible())
                addStatus(IStatus.WARNING, MessageFormat.format("Cannot read '{0}'.", diagramFile.getFullPath()), null);
            else {
                exportDiagramFile(diagramFile, monitor);
                monitor.worked(1);
            }
        }
    }

    protected void exportDiagramFile(IFile diagramFile, IProgressMonitor monitor) throws InterruptedException
    {
        monitor.subTask(diagramFile.getFullPath().toString());
        try {
            exportDiagramFile(diagramFile, new NedFigureProvider(), monitor);
        }
        catch (Throwable e) {
            addStatus(IStatus.ERROR, MessageFormat.format("Error while exporting '{0}'.", diagramFile.getFullPath()), e);
        }
    }

    private void exportDiagramFile(IFile diagramFile, NedFigureProvider figureProvider, IProgressMonitor monitor) throws InterruptedException
    {
        Map<IFigure, String> providedFiguresAndNames = figureProvider.getDiagramFigures(diagramFile);
        Set<IFigure> figures = providedFiguresAndNames.keySet();
        if (figures != null && !figures.isEmpty()) {
            for (IFigure figure : figures) {
                String imageFileBaseName = providedFiguresAndNames.get(figure);
                IFile imageIFile = getImageIFile(diagramFile, imageFileBaseName);
                File imageFile = new File(imageIFile.getRawLocation().toString());
                if (isAllowedToWrite(imageIFile)) {
                    diagramExporter.exportDiagram(Collections.singletonList(figure), imageFile, antialias);
                    refreshContainer(imageIFile.getParent(), monitor);
                }
            }
        }
        else
            addStatus(IStatus.ERROR, MessageFormat.format("{0} failed to provide a figure for '{1}'.", figureProvider, diagramFile.getFullPath()), null);
    }

    protected void refreshContainer(IContainer container, IProgressMonitor monitor)
    {
        try {
            container.refreshLocal(IResource.DEPTH_ONE, monitor);
        }
        catch (CoreException e) {
            // Nothing to do about it I guess.
        }
    }

    protected boolean isAllowedToWrite(IFile imageFile) throws InterruptedException
    {
        if (imageFile.exists()) {
            if (imageFile.isReadOnly()) {
                addStatus(IStatus.ERROR, MessageFormat.format("Image file '{0}' is not writable.", imageFile.getFullPath()), null);
                return false;
            }

            if (overwriteMode == OverwriteMode.NONE)
                return false;

            if (overwriteMode != OverwriteMode.ALL) {
                String overwriteAnswer = overwriteQuery.queryOverwrite(imageFile.getFullPath().toString());

                if (overwriteAnswer.equals(IOverwriteQuery.CANCEL))
                    throw new InterruptedException("User cancelled export operation.");
                else if (overwriteAnswer.equals(IOverwriteQuery.NO))
                    return false;
                else if (overwriteAnswer.equals(IOverwriteQuery.NO_ALL)) {
                    overwriteMode = OverwriteMode.NONE;
                    return false;
                }
                else if (overwriteAnswer.equals(IOverwriteQuery.ALL))
                    overwriteMode = OverwriteMode.ALL;
            }
        }
        return true;
    }

    private IFile getImageIFile(IResource diagramFile, String imageFileBaseName)
    {
        IPath imagePath = new Path(imageFileBaseName + "." + diagramExporter.getFileExtension());
        IFile imageFile = diagramFile.getParent().getFile(imagePath);
        if (imageFile.equals(diagramFile)) {
            imagePath = imagePath.addFileExtension(diagramExporter.getFileExtension());
            imageFile = diagramFile.getParent().getFile(imagePath);
        }
        return imageFile;
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

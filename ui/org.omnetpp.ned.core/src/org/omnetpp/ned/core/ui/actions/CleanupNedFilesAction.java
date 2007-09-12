package org.omnetpp.ned.core.ui.actions;

import java.io.ByteArrayInputStream;
import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.action.IAction;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.IWorkbenchWindowActionDelegate;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.dialogs.ContainerSelectionDialog;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.ned.core.NEDResources;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.core.refactoring.RefactoringTools;
import org.omnetpp.ned.model.ex.NedFileElementEx;

/**
 * Fixes package declarations, organizes imports, and reformats
 * source code in the selected directories.
 *  
 * @author Andras
 */
public class CleanupNedFilesAction implements IWorkbenchWindowActionDelegate {
    public void init(IWorkbenchWindow window) {
    }

    public void dispose() {
    }
    
    public void run(IAction action) {
        //FIXME tell user: "Please save all files and close all editors" etc.
        Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        ContainerSelectionDialog dialog = new ContainerSelectionDialog(shell, ResourcesPlugin.getWorkspace().getRoot(), false, "Clean up NED files in the following folder:");
        if (dialog.open() == ListDialog.OK) {
            IPath path = (IPath) dialog.getResult()[0];
            final IContainer container = (IContainer) ResourcesPlugin.getWorkspace().getRoot().findMember(path);

            try {
                IRunnableWithProgress op = new IRunnableWithProgress() {
                    public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                        cleanupNedFilesIn(container, monitor);
                    }};
                new ProgressMonitorDialog(shell).run(true, true, op);
            } 
            catch (InvocationTargetException e) {
                NEDResourcesPlugin.logError(e);
                ErrorDialog.openError(shell, "Error", "Error during cleaning up NED files", new Status(IMarker.SEVERITY_ERROR, NEDResourcesPlugin.PLUGIN_ID, e.getMessage(), e));
            } catch (InterruptedException e) { 
                // nothing to do 
            }
        }
    }

    protected void cleanupNedFilesIn(IContainer container, final IProgressMonitor monitor) {
        try {
            NEDResourcesPlugin.getNEDResources().fireBeginChangeEvent();
            container.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) throws CoreException {
                    if (NEDResourcesPlugin.getNEDResources().isNEDFile(resource)) {
                        monitor.subTask(resource.getFullPath().toString());
                        cleanupNedFile((IFile)resource);
                        monitor.worked(1);
                    }
                    if (monitor.isCanceled())
                        return false;
                    return true;
                }
            });
            monitor.done();
        }
        catch (CoreException e) {
            NEDResourcesPlugin.logError(e);
            Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
            ErrorDialog.openError(shell, "Error", "An error occurred during cleaning up NED files. Not all of the selected files have been processed.", e.getStatus());
        }
        finally {
            NEDResourcesPlugin.getNEDResources().fireEndChangeEvent();
        }
    }

    protected void cleanupNedFile(IFile file) throws CoreException {
        NEDResources res = NEDResourcesPlugin.getNEDResources();
        NedFileElementEx nedFileElement = res.getNedFileElement(file);
        
        // clean up
        RefactoringTools.cleanupTree(nedFileElement);
        RefactoringTools.fixupPackageDeclaration(nedFileElement);
        RefactoringTools.organizeImports(nedFileElement);
        
        // save the file
        String source = nedFileElement.getNEDSource();
        
        // save it
        file.setContents(new ByteArrayInputStream(source.getBytes()), IFile.FORCE, null);
    }

    public void selectionChanged(IAction action, ISelection selection) {
    }

}

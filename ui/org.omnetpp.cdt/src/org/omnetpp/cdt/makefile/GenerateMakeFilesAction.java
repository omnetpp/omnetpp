package org.omnetpp.cdt.makefile;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
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
import org.omnetpp.cdt.Activator;

/**
 * Scans all C++ files, etc....
 * TODO
 *  
 * @author Andras
 */
public class GenerateMakeFilesAction implements IWorkbenchWindowActionDelegate {
    public void init(IWorkbenchWindow window) {
    }

    public void dispose() {
    }
    
    public void run(IAction action) {
        Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        ContainerSelectionDialog dialog = new ContainerSelectionDialog(shell, ResourcesPlugin.getWorkspace().getRoot(), false, "Select project to generate Makefiles for:");
        if (dialog.open() == ListDialog.OK) {
            IPath path = (IPath) dialog.getResult()[0];
            final IContainer container = (IContainer) ResourcesPlugin.getWorkspace().getRoot().findMember(path);

            try {
                IRunnableWithProgress op = new IRunnableWithProgress() {
                    public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                        processFilesIn(container, monitor);
                    }};
                new ProgressMonitorDialog(shell).run(true, true, op);
            } 
            catch (InvocationTargetException e) {
                Activator.logError(e);
                ErrorDialog.openError(shell, "Error", "Error during processing C++ files", new Status(IMarker.SEVERITY_ERROR, Activator.PLUGIN_ID, e.getMessage(), e));
            } catch (InterruptedException e) { 
                // nothing to do 
            }
        }
    }

    protected void processFilesIn(IContainer container, final IProgressMonitor monitor) {
        try {
            CppTools.generateMakefiles(container, monitor);
            monitor.done();
        }
        catch (CoreException e) {
            Activator.logError(e);
            Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
            ErrorDialog.openError(shell, "Error", "An error occurred during scanning C++ files.", e.getStatus());
        }
    }

    public void selectionChanged(IAction action, ISelection selection) {
    }

}

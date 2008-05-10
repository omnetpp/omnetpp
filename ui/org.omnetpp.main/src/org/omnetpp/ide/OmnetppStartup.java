package org.omnetpp.ide;

import java.lang.reflect.InvocationTargetException;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Status;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.project.ProjectUtils;

/**
 * Performs various tasks when the workbench starts up.
 * 
 * @author Andras
 */
public class OmnetppStartup implements IStartup {

    public static final String SAMPLES_DIR = "samples";

    public void earlyStartup() {
        // call importSampleProjects() in the UI thread
        final IWorkbench workbench = PlatformUI.getWorkbench();
        workbench.getDisplay().asyncExec(new Runnable() {
            public void run() {
                if (workbench.getActiveWorkbenchWindow() != null && shouldImportSampleProjects())
                    importSampleProjects();
            }
        });
    }

    /**
     * Determines whether we want to import sample projects into the workspace.
     * Typically we only want to do that on the very first, "fresh" IDE startup.
     * We use the following condition: " if the workspace location points to the 
     * OMNeT++ "samples" directory, and there are no projects in the workspace yet 
     * (none have been created or imported yet). 
     */
    protected boolean shouldImportSampleProjects() {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return root.getLocation().lastSegment().equals(SAMPLES_DIR) && root.getProjects().length == 0;
    }

    /**
     * Import sample projects.
     */
    protected void importSampleProjects() {
        Shell shell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
        try {
            IRunnableWithProgress op = new IRunnableWithProgress() {
                public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                    // note: we don't open them, because CDT's autobuild would immediately start and the whole process would last forever...
                    ProjectUtils.importAllProjectsFromWorkspaceDirectory(false, monitor);
                }
            };
            new ProgressMonitorDialog(shell).run(true, true, op);
        } 
        catch (InvocationTargetException e) {
            OmnetppMainPlugin.logError(e);
            ErrorDialog.openError(shell, "Error", "Error during importing sample projects into the workspace", new Status(IMarker.SEVERITY_ERROR, OmnetppMainPlugin.PLUGIN_ID, e.getMessage(), e));
        } catch (InterruptedException e) { 
            // nothing to do 
        }
    }
}

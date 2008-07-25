package org.omnetpp.ide;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.MalformedURLException;
import java.net.URL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.jface.operation.IRunnableWithProgress;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.IConstants;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.ide.views.NewsView;

/**
 * Performs various tasks when the workbench starts up.
 * 
 * @author Andras
 */
public class OmnetppStartup implements IStartup {

    private static final String VERSION = "4.0.0b3";
    private static final String VERSIONS_URL = NewsView.BASE_URL + "versions/";
	private static final String LATESTVERSION_URL = VERSIONS_URL + "latest/";
	public static final String SAMPLES_DIR = "samples";

    /*
     * Method declared on IStartup.
     */
    public void earlyStartup() {

    	checkForNewVersion();
    	
        final IWorkbench workbench = PlatformUI.getWorkbench();
        workbench.getDisplay().asyncExec(new Runnable() {
            public void run() {
                if (isInitialDefaultStartup()) {
                    // We need to turn off "build automatically", otherwise it'll start 
                    // building during the import process and will take forever.
                    // Also, CDT is a pain with autobuild on.
                    disableAutoBuild();
                    importSampleProjects(true);
                }
            }
        });
    }

	/**
	 * The current version string returned by the version check URL or NULL if 
	 * there is an error, no network present etc.
	 */
	private String getCurrentVersion() {
		try {
			byte buffer[] = new byte[1024];
			URL url = new URL(LATESTVERSION_URL);
			url.openStream().read(buffer);
			return new String(buffer).trim();
		} catch (MalformedURLException e) {
		} catch (IOException e) {
		}
		return null;
	}
	
	private void checkForNewVersion() {
		Job versioncheckJob = new Job ("Checking for newer versions") {
			@Override
			protected IStatus run(IProgressMonitor monitor) {
				String newestVersion = getCurrentVersion(); 
		    	if (newestVersion != null && !newestVersion.equals(VERSION)) {
		    		Display.getDefault().asyncExec(new Runnable() {
						public void run() {
			    			try {
			    				IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
			    				IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
			    				if (workbenchPage != null) {
			    					NewsView newsView = (NewsView)workbenchPage.showView(IConstants.NEWS_VIEW_ID);
		    						newsView.setURL(VERSIONS_URL);
			    				}
			    			} 
			    			catch (PartInitException e) {
			    				CommonPlugin.logError(e);
			    			}
						}
		    		});
		    	}
				return Status.OK_STATUS;
			}
    	};
    	versioncheckJob.setSystem(true);
    	versioncheckJob.schedule();
	}
    /**
     * Determines whether this is the first IDE startup after the installation, with the 
     * default workspace (the "samples" directory). We check two things:
     * - the workspace location points to the OMNeT++ "samples" directory
     * - there are no projects in the workspace yet (none have been created or imported) 
     */
    protected boolean isInitialDefaultStartup() {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return root.getLocation().lastSegment().equals(SAMPLES_DIR) && root.getProjects().length == 0;
    }

    /**
     * Import sample projects.
     */
    protected void importSampleProjects(final boolean open) {
        IWorkbenchWindow workbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        if (workbenchWindow == null)
            return;
        Shell shell = workbenchWindow.getShell();
        try {
            IRunnableWithProgress op = new IRunnableWithProgress() {
                public void run(IProgressMonitor monitor) throws InvocationTargetException, InterruptedException {
                    ProjectUtils.importAllProjectsFromWorkspaceDirectory(open, monitor);
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
    
    /**
     * Turns off the "Build automatically" option.
     */
    protected void disableAutoBuild() {
        try {
            IWorkspace ws = ResourcesPlugin.getWorkspace();
            IWorkspaceDescription desc = ws.getDescription();
            desc.setAutoBuilding(false);
            ws.setDescription(desc);
        }
        catch (CoreException e) {
            OmnetppMainPlugin.logError("Could not turn off 'Build automatically' option", e);
        }
    }
}

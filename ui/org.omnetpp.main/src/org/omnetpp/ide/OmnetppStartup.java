package org.omnetpp.ide;

import java.io.IOException;
import java.net.URL;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.swt.widgets.Display;
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
    public static final String SAMPLES_DIR = "samples";
    
    protected long VERSIONCHECK_INTERVAL_MILLIS = 3*24*3600*1000L;  // 3 days

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
                    importSampleProjects(false);
                }
            }
        });
    }

    private void checkForNewVersion() {
        if (System.getProperty("com.simulcraft.test.running") != null)
            return;

        // skip this when version check was done recently
        long lastCheckMillis = OmnetppMainPlugin.getDefault().getPluginPreferences().getLong("lastCheck");
        if (System.currentTimeMillis() - lastCheckMillis < VERSIONCHECK_INTERVAL_MILLIS)
            return;
        
        // Show the version check URL in the News view if it's not empty -- the page should
        // contain download information etc.
        //
        // NOTE: web page will decide whether there is a new version, by checking 
        // the version number we send to it; it may also return a page specific 
        // to the installed version.
        //
        Display.getDefault().asyncExec(new Runnable() {
            public void run() {
                try {
                    IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
                    if (workbenchPage != null) {
                        String versionCheckURL = NewsView.VERSIONCHECK_URL + "?v=" + OmnetppMainPlugin.getVersion() + "," + OmnetppMainPlugin.getInstallDate();
                        if (isWebPageNotBlank(versionCheckURL)) {
                            NewsView view = (NewsView)workbenchPage.showView(IConstants.NEWS_VIEW_ID);
                            view.showURL(versionCheckURL);
                        }
                        OmnetppMainPlugin.getDefault().getPluginPreferences().setValue("lastCheck", System.currentTimeMillis());
                    }
                } 
                catch (PartInitException e) {
                    CommonPlugin.logError(e);
                }
            }
        });
    }

    /**
     * Checks whether the given web page is available and contains something (i.e. is not empty).
     */
    public boolean isWebPageNotBlank(String url) {
        try {
            byte[] buf = new byte[10];
            new URL(url).openStream().read(buf); // probe it by reading a few bytes
            return new String(buf).trim().length() > 0;
        } 
        catch (IOException e) {
            return false;
        }
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
        WorkspaceJob job = new WorkspaceJob("Importing sample projects") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                ProjectUtils.importAllProjectsFromWorkspaceDirectory(open, monitor);
                return Status.OK_STATUS;
            }
        };
        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.LONG);
        job.schedule();
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

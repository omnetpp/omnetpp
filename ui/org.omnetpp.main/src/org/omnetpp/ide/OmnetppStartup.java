package org.omnetpp.ide;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.URL;

import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Status;
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
                    importSampleProjects(true);
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
            new URL(url).openStream().read(new byte[10]); // probe it by reading a few bytes
            return true;
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

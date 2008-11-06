package org.omnetpp.ide;

import java.io.IOException;

import org.apache.commons.httpclient.HostConfiguration;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.UsernamePasswordCredentials;
import org.apache.commons.httpclient.auth.AuthScope;
import org.apache.commons.httpclient.methods.GetMethod;
import org.eclipse.core.internal.net.ProxyManager;
import org.eclipse.core.net.proxy.IProxyData;
import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Platform;
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
        long lastCheckMillis = OmnetppMainPlugin.getDefault().getConfigurationPreferenceStore().getLong("lastCheck");
        if (System.currentTimeMillis() - lastCheckMillis < VERSIONCHECK_INTERVAL_MILLIS)
            return;
        
        // Show the version check URL in the News view if it's not empty -- the page should
        // contain download information etc.
        //
        // NOTE: web page will decide whether there is a new version, by checking 
        // the version number we send to it; it may also return a page specific 
        // to the installed version.
        //
        Job job = new Job("Version check") { 
        	public IStatus run(IProgressMonitor pm) {
        		final String versionCheckURL = NewsView.VERSIONCHECK_URL + "?v=" + OmnetppMainPlugin.getVersion() + "," + OmnetppMainPlugin.getInstallDate()+","+Platform.getOS();
        		if (isWebPageNotBlank(versionCheckURL)) {
        			Display.getDefault().asyncExec(new Runnable() {
        				public void run() {
        					try {
        						IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        						IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
        						if (workbenchPage != null) {
        							NewsView view = (NewsView)workbenchPage.showView(IConstants.NEWS_VIEW_ID);
        							view.showURL(versionCheckURL);
        						}
        					} 
        					catch (PartInitException e) {
        						CommonPlugin.logError(e);
        					}
        				}});
        		}
        		OmnetppMainPlugin.getDefault().getConfigurationPreferenceStore().setValue("lastCheck", System.currentTimeMillis());
        		return Status.OK_STATUS;
        	}
        };
        job.setSystem(true);
        job.schedule();
    }

    /**
     * 
     * @param url
     * @return
     */
//    public boolean isWebPageNotBlank(final String url) {
//    	final boolean result[] = new boolean[1];
//    	Display.getDefault().syncExec(new Runnable() {
//			@Override
//			public void run() {
//				final Shell activeShell = Display.getDefault().getActiveShell();
//				final Browser browser = new Browser(activeShell,SWT.NONE);  // FIXME getActiveShell can be null
//				browser.setUrl("http://omnetppblsdsldlsl.org/noonsadn");
//				System.out.println("setural");
//				browser.addProgressListener(new ProgressAdapter() {
//					@Override
//					public void completed(ProgressEvent event) {
//						System.out.println("**** completed");
//						System.out.println("text="+browser.getText());
//						System.out.println("display:" +Display.getCurrent());
//						System.out.println("activeShel:"+activeShell);
//						System.out.println("new active shell:"+Display.getDefault().getActiveShell());
//					}
//					
//				});
//			}
//    		
//    	});
//    	return true;
//    }
    
//    /**
//     * Checks whether the given web page is available and contains something (i.e. is not empty).
//     */
//    public boolean isWebPageNotBlankJDK(String url) {
//        try {
//            byte[] buf = new byte[10];
//            new URL(url).openStream().read(buf); // probe it by reading a few bytes
//            return new String(buf).trim().length() > 0;
//        } 
//        catch (IOException e) {
//            return false;
//        }
//    }

    /**
     * Checks whether the given web page is available and contains something (i.e. is not empty).
     */
    public boolean isWebPageNotBlank(String url) {
    	HttpClient client = new HttpClient();
    	client.getParams().setSoTimeout(10000);

    	IProxyData proxyData = ProxyManager.getProxyManager().getProxyDataForHost("omnetpp.org", IProxyData.HTTP_PROXY_TYPE );
    	if (proxyData != null) {
    		client.getState().setProxyCredentials(
    				new AuthScope(proxyData.getHost(), proxyData.getPort()),
					new UsernamePasswordCredentials(proxyData.getUserId(), proxyData.getPassword()));
    		HostConfiguration hc = new HostConfiguration();
    		hc.setProxy(proxyData.getHost(), proxyData.getPort());
    	}

    	GetMethod get = new GetMethod(url);
    	get.setDoAuthentication( true );

        try {
            int status = client.executeMethod( get );
            
            String responseBody = get.getResponseBodyAsString();
			return responseBody.trim().length() > 0 && status == HttpStatus.SC_OK;
        } catch (HttpException e) {
        	return false;
		} catch (IOException e) {
        	return false;
		} finally {
            get.releaseConnection();
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

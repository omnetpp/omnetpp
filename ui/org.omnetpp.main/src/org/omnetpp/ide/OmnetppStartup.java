package org.omnetpp.ide;

import java.io.File;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.logging.Level;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.httpclient.HostConfiguration;
import org.apache.commons.httpclient.HttpClient;
import org.apache.commons.httpclient.HttpException;
import org.apache.commons.httpclient.HttpMethod;
import org.apache.commons.httpclient.HttpMethodRetryHandler;
import org.apache.commons.httpclient.HttpStatus;
import org.apache.commons.httpclient.UsernamePasswordCredentials;
import org.apache.commons.httpclient.auth.AuthScope;
import org.apache.commons.httpclient.auth.BasicScheme;
import org.apache.commons.httpclient.methods.GetMethod;
import org.apache.commons.httpclient.params.HttpMethodParams;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.logging.impl.Jdk14Logger;
import org.eclipse.core.internal.net.ProxyData;
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
import org.omnetpp.common.util.FileUtils;
import org.omnetpp.ide.views.NewVersionView;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;

/**
 * Performs various tasks when the workbench starts up.
 * 
 * @author Andras
 */
@SuppressWarnings("restriction")
public class OmnetppStartup implements IStartup {
	protected long VERSIONCHECK_INTERVAL_MILLIS = 3*24*3600*1000L;  // 3 days
    public static final String SAMPLES_DIR = "samples";
    public static final String BASE_URL = "http://omnetpp.org/ide/" + (IConstants.IS_COMMERCIAL ? "omnest" : "omnetpp");
	public static final String VERSIONCHECK_URL = BASE_URL + "/versioncheck/"; // used in OmnetppStartup

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

	/**
	 * For use in the version check URL.
	 */
	protected String getInstallDate() {
	    String installDate = CommonPlugin.getConfigurationPreferenceStore().getString("installDate");
	    if (StringUtils.isEmpty(installDate)) {
	        installDate = new SimpleDateFormat("yyyyMMddHHmmss").format(new Date());
	        CommonPlugin.getConfigurationPreferenceStore().setValue("installDate", installDate);
	        try {
	        	CommonPlugin.getConfigurationPreferenceStore().save();
			} catch (IOException e) {
				OmnetppMainPlugin.logError("Cannot store installDate preference", e);
			}
	        
	    }
	    return installDate;
	}

	private void checkForNewVersion() {
        if (System.getProperty("com.simulcraft.test.running") != null)
            return;

        // skip this when version check was done recently
        long lastCheckMillis = CommonPlugin.getConfigurationPreferenceStore().getLong("lastCheck");
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
        		String versionCheckURL = OmnetppStartup.VERSIONCHECK_URL + "?v=" + OmnetppMainPlugin.getVersion() + "&d=" + getInstallDate()+"&o="+Platform.getOS()+"."+Platform.getOSArch();
        		final String content = getWebPage(versionCheckURL); 
        		if (StringUtils.isNotBlank(content)) {
        			Display.getDefault().asyncExec(new Runnable() {
        				public void run() {
        					try {
        						IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        						IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
        						if (workbenchPage != null) {
        							NewVersionView view = (NewVersionView)workbenchPage.showView(IConstants.NEW_VERSION_VIEW_ID);
        							view.setText(content);
        						}
        					} 
        					catch (PartInitException e) {
        						CommonPlugin.logError(e);
        					}
        				}});
        		}
        		CommonPlugin.getConfigurationPreferenceStore().setValue("lastCheck", System.currentTimeMillis());
        		try {
        			CommonPlugin.getConfigurationPreferenceStore().save();
				} catch (IOException e) {
					OmnetppMainPlugin.logError("Cannot store lastCheck preference", e);
				}
        		return Status.OK_STATUS;
        	}
        };
        job.setSystem(true);
        job.schedule();
    }

//  /**
//  * Checks whether the given web page is available and contains something (i.e. is not empty).
//  */
// public boolean isWebPageNotBlank_plain(String url) {
//     try {
//         byte[] buf = new byte[10];
//         new URL(url).openStream().read(buf); // probe it by reading a few bytes
//         return new String(buf).trim().length() > 0;
//     } 
//     catch (IOException e) {
//         return false;
//     }
// }

//  /**
//  * Checks whether the given web page is available and contains something (i.e. is not empty).
//  */
//    public boolean isWebPageNotBlank_browser(final String url) {
//    	final boolean result[] = new boolean[1];
//    	Display.getDefault().syncExec(new Runnable() {
//			@Override
//			public void run() {
//				final Shell activeShell = Display.getDefault().getActiveShell();
//				final Browser browser = new Browser(activeShell,SWT.NONE);  // FIXME getActiveShell can be null
//				browser.setUrl("http://omnetppblsdsldlsl.org/noonsadn");
//				Debug.println("setural");
//				browser.addProgressListener(new ProgressAdapter() {
//					@Override
//					public void completed(ProgressEvent event) {
//						Debug.println("**** completed");
//						Debug.println("text="+browser.getText());
//						Debug.println("display:" +Display.getCurrent());
//						Debug.println("activeShel:"+activeShell);
//						Debug.println("new active shell:"+Display.getDefault().getActiveShell());
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
//    public boolean isWebPageNotBlank(String url) {
//    	HttpClient client = new HttpClient();
//    	client.getParams().setSoTimeout(10000);
//
//    	IProxyData proxyData = ProxyManager.getProxyManager().getProxyDataForHost("omnetpp.org", IProxyData.HTTP_PROXY_TYPE );
//    	if (proxyData != null) {
//    		client.getState().setProxyCredentials(
//    				new AuthScope(proxyData.getHost(), proxyData.getPort()),
//					new UsernamePasswordCredentials(proxyData.getUserId(), proxyData.getPassword()));
//    		HostConfiguration hc = new HostConfiguration();
//    		hc.setProxy(proxyData.getHost(), proxyData.getPort());
//    	}
//
//    	GetMethod method = new GetMethod(url);
//    	method.setDoAuthentication(true);
//
//        try {
//            int status = client.executeMethod(method);
//            String responseBody = method.getResponseBodyAsString();
//			return responseBody.trim().length() > 0 && status == HttpStatus.SC_OK;
//        } catch (HttpException e) {
//        	return false;
//		} catch (IOException e) {
//        	return false;
//		} finally {
//            method.releaseConnection();
//        }
//    }

    
    /**
     * Downloads the given web page and returns it. Returns NULL if cannot download.
     * Proxy detection is a royal pain here.
     */
    public String getWebPage(String url) {
        // try with Eclipse settings
        IProxyData proxyData = ProxyManager.getProxyManager().getProxyDataForHost("omnetpp.org", IProxyData.HTTP_PROXY_TYPE);
        String content = getPageContent(url, proxyData);
        if (content != null)
            return content;

        // try without proxy as well (in case settings in Eclipse are wrong)
        if (proxyData != null && proxyData.getHost() != null) {
            content = getPageContent(url, null);
            if (content != null)
                return content;
        }

        // try with "http_proxy" environment variable (there's also http_user and http_passwd)
        String http_proxy = System.getenv("http_proxy");
        if (http_proxy != null) {
            try {
                proxyData = parseHttpProxyURL(http_proxy);
                if (proxyData != null && proxyData.getHost() != null) {
                    content = getPageContent(url, proxyData);
                    if (content != null)
                        return content;

                    if (System.getenv("http_user") != null) {
                        proxyData.setUserid(System.getenv("http_user"));
                        proxyData.setPassword(System.getenv("http_passwd"));
                        content = getPageContent(url, proxyData);
                        if (content != null)
                            return content;
                    }
                }
            } 
            catch (Exception e) {
            }
        }
        
        // try with Firefox proxy settings
        String HOME = System.getenv("HOME");
        if (HOME != null) {
            File[] members = new File(HOME + "/.mozilla/firefox").listFiles();
            if (members != null) {
                for (File member : members) {
                    File prefsFile = new File(member.getPath()+"/prefs.js");
                    if (prefsFile.isFile()) {
                        try {
                            String prefsText = FileUtils.readTextFile(prefsFile);
                            if (prefsText != null) {
                                // user_pref("network.proxy.http", "someproxy.edu");
                                // user_pref("network.proxy.http_port", 9999);
                                proxyData = new ProxyData(IProxyData.HTTP_PROXY_TYPE);
                                Matcher matcher = Pattern.compile("(?m)^ *user_pref\\(\"network\\.proxy\\.http\", *\"(.*?)\"\\);").matcher(prefsText);
                                if (matcher.find())
                                    proxyData.setHost(matcher.group(1));
                                matcher = Pattern.compile("(?m)^ *user_pref\\(\"network\\.proxy\\.http_port\", *([0-9]+)\\);").matcher(prefsText);
                                if (matcher.find())
                                    proxyData.setPort(Integer.parseInt(matcher.group(1)));
                                if (proxyData.getHost() != null) {
                                    content = getPageContent(url, proxyData);
                                    if (content != null)
                                        return content;
                                }
                                // user_pref("network.proxy.autoconfig_url", "http://someproxy.edu/proxy.pac");
                                matcher = Pattern.compile("(?m)^ *user_pref\\(\"network\\.proxy\\.autoconfig_url\", *\"(.*?)\"\\);").matcher(prefsText);
                                if (matcher.find()) {
                                    String proxyPacUrl = matcher.group(1);
                                    IProxyData[] proxies = detectPotentialProxies(proxyPacUrl);
                                    for (IProxyData proxy : proxies) {
                                        content = getPageContent(url, proxy);
                                        if (content != null)
                                            return content;
                                    }
                                }
                            }
                        }
                        catch (Exception e) {
                        }
                    }
                }
            }
        }

        // try with GNOME proxy settings
        if (HOME != null) {
            File gconfFile = new File(HOME + "/.gconf/system/http_proxy/%gconf.xml");
            if (gconfFile.isFile()) {
                try {
                    DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
                    Document doc = docBuilder.parse(gconfFile);
                    Element root = doc.getDocumentElement();
                    NodeList entries = root.getElementsByTagName("entry");
                    proxyData = new ProxyData(IProxyData.HTTP_PROXY_TYPE);
                    for (int i=0; i<entries.getLength(); i++) {
                        Element e = (Element)entries.item(i);
                        if (StringUtils.equals(e.getAttribute("name"), "host"))
                            proxyData.setHost(e.getElementsByTagName("stringvalue").item(0).getTextContent().trim());
                        if (StringUtils.equals(e.getAttribute("name"), "port"))
                            proxyData.setPort(Integer.parseInt(e.getAttribute("value").trim()));
                        if (StringUtils.equals(e.getAttribute("name"), "authentication_user"))
                            proxyData.setUserid(e.getElementsByTagName("stringvalue").item(0).getTextContent().trim());
                        if (StringUtils.equals(e.getAttribute("name"), "authentication_password"))
                            proxyData.setPassword(e.getElementsByTagName("stringvalue").item(0).getTextContent().trim());
                    }
                    if (proxyData.getHost() != null) {
                        content = getPageContent(url, proxyData);
                        if (content != null)
                            return content;
                    }
                } 
                catch (Exception e) { 
                }
            }

            gconfFile = new File(HOME + "/.gconf/system/proxy/%gconf.xml");
            if (gconfFile.isFile()) {
                try {
                    DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
                    Document doc = docBuilder.parse(gconfFile);
                    Element root = doc.getDocumentElement();
                    NodeList entries = root.getElementsByTagName("entry");
                    for (int i=0; i<entries.getLength(); i++) {
                        Element e = (Element)entries.item(i);
                        if (StringUtils.equals(e.getAttribute("name"), "autoconfig_url")) {
                            String proxyPacUrl = e.getElementsByTagName("stringvalue").item(0).getTextContent().trim();
                            if (!StringUtils.isEmpty(proxyPacUrl)) {
                                IProxyData[] proxies = detectPotentialProxies(proxyPacUrl);
                                for (IProxyData proxy : proxies) {
                                    content = getPageContent(url, proxy);
                                    if (content != null)
                                        return content;
                                }
                            }
                        }
                    }
                } 
                catch (Exception e) { 
                }
            }

        }

        // try with KDE proxy settings
        if (HOME != null) {
            File kioslavercFile = new File(HOME + "/.kde/share/config/kioslaverc");
            if (kioslavercFile.isFile()) {
                try {
                    String kioslavercText = FileUtils.readTextFile(kioslavercFile);
                    if (kioslavercText != null) {
                        // httpProxy=http://127.0.0.1:3128
                        proxyData = new ProxyData(IProxyData.HTTP_PROXY_TYPE);
                        Matcher matcher = Pattern.compile("(?m)^httpProxy=(.*)").matcher(kioslavercText);
                        if (matcher.find()) {
                            String httpProxy = matcher.group(1).trim();
                            proxyData = parseHttpProxyURL(httpProxy);
                            if (proxyData != null && proxyData.getHost() != null) {
                                content = getPageContent(url, proxyData);
                                if (content != null)
                                    return content;
                            }
                        }
                    }
                }
                catch (Exception e) {
                }
            }
        }
        return null;
    }

    /**
     * Download "proxy.pac" from the given URL, and try to find proxy settings in it.
     */
    protected IProxyData[] detectPotentialProxies(String proxyPacUrl) {
        String proxyPacText = getPageContent(proxyPacUrl, null);
        if (proxyPacText == null)
            return new IProxyData[0];
        // find lines like this in it: PROXY www.somesite.edu:8080
        List<IProxyData> list = new ArrayList<IProxyData>();
        Matcher matcher = Pattern.compile("(?m)PROXY +([^: \"]+):([0-9]+)").matcher(proxyPacText);
        while (matcher.find()) {
            IProxyData proxyData = new ProxyData(IProxyData.HTTP_PROXY_TYPE);
            proxyData.setHost(matcher.group(1));
            proxyData.setPort(Integer.parseInt(matcher.group(2)));
            list.add(proxyData);
        }
        return list.toArray(new IProxyData[]{});
    }
    
    /**
     * Parse URL of the syntax http://host:port/ or http://username:password@host:port/,
     * being a little flexible (allow http:// to be missing, trailing slash to be missing, etc)
     */
    protected IProxyData parseHttpProxyURL(String url) {
        Matcher matcher = Pattern.compile("(http:/*)?((.+):(.+)@)?([^:]+)(:([0-9]+))?/?").matcher(url);
        if (matcher.matches()) {
            IProxyData proxyData = new ProxyData(IProxyData.HTTP_PROXY_TYPE);
            proxyData.setHost(matcher.group(5));
            proxyData.setPort(Integer.parseInt(StringUtils.defaultIfEmpty(matcher.group(7),"-1")));
            proxyData.setUserid(matcher.group(3));
            proxyData.setPassword(matcher.group(4));
            return proxyData;
        }
        return null;
    }

    /**
     * Returns null on failure, otherwise the page content.
     */
    protected String getPageContent(String url, IProxyData proxyData) {
        // turn off log messages from Apache HttpClient if we can...
        Log log = LogFactory.getLog("org.apache.commons.httpclient");
        if (log instanceof Jdk14Logger)
            ((Jdk14Logger)log).getLogger().setLevel(Level.OFF);
        
        HttpClient client = new HttpClient();
        client.getParams().setSoTimeout(10000);       
        if (proxyData != null && !StringUtils.isEmpty(proxyData.getHost())) {
            if (!StringUtils.isEmpty(proxyData.getUserId()) && !StringUtils.isEmpty(proxyData.getPassword()))
                client.getState().setProxyCredentials(
                        new AuthScope(proxyData.getHost(), proxyData.getPort()),
                        new UsernamePasswordCredentials(proxyData.getUserId(), proxyData.getPassword()));
            HostConfiguration hc = new HostConfiguration();
            hc.setProxy(proxyData.getHost(), proxyData.getPort());
            client.setHostConfiguration(hc);
        }

        // do not retry 
        HttpMethodRetryHandler noRetryhandler = new HttpMethodRetryHandler() {
            public boolean retryMethod(final HttpMethod method,final IOException exception, int executionCount) {
                return false;
            }
        };

        GetMethod method = new GetMethod(url);
        method.getProxyAuthState().setAuthScheme(new BasicScheme());
        method.getParams().setParameter(HttpMethodParams.RETRY_HANDLER, noRetryhandler);
        method.setDoAuthentication(true);
        
        try {
            int status = client.executeMethod(method);
            String responseBody = method.getResponseBodyAsString();
            return status == HttpStatus.SC_OK ? responseBody : null;
        } catch (HttpException e) {
            return null;
        } catch (IOException e) {
            return null;
        } finally {
            method.releaseConnection();
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

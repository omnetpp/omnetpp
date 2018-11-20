package org.omnetpp.common.editor;

import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.LinkedHashMap;
import java.util.Map;

import org.eclipse.core.filesystem.EFS;
import org.eclipse.core.filesystem.IFileStore;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.Path;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IPageLayout;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.FileStoreEditorInput;
import org.eclipse.ui.navigator.CommonNavigator;
import org.eclipse.ui.part.FileEditorInput;
import org.eclipse.ui.part.ISetSelectionTarget;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.Debug;

/**
 * This class extends the standard Eclipse HTML editor with various actions
 * that can be run by providing an action query parameter to HTML anchors.
 * The supported actions are the following:
 *  - openEditor
 *  - openView
 *  - launchRun
 *  - launchDebug
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class WebBrowserEditor extends org.eclipse.ui.internal.browser.WebBrowserEditor {
    protected static Map<String, IActionHandler> handlers = new LinkedHashMap<String, IActionHandler>();

    public interface IActionHandler {
        public void executeAction(URL url, IResource resource, Map<String, String> parameters) throws Exception;
    }

    public static void registerActionHandler(String action, IActionHandler handler) {
        handlers.put(action, handler);
    }

    static {
        registerActionHandler("openEditor", new IActionHandler() {
            @Override
            public void executeAction(URL url, IResource resource, Map<String, String> parameters) throws CoreException, URISyntaxException {
                IWorkbench workbench = PlatformUI.getWorkbench();
                IWorkbenchPage workbenchPage = workbench.getActiveWorkbenchWindow().getActivePage();
                if (resource != null) {
                    if (resource instanceof IFile) {
                        IFile file = (IFile)resource;
                        IEditorDescriptor editorDescriptor = workbench.getEditorRegistry().getDefaultEditor(file.getName());
                        workbenchPage.openEditor(new FileEditorInput(file), editorDescriptor.getId());
                    }
                }
                else {
                    URI uri = url.toURI();
                    // NOTE: workaround the fact that the query part is not removed from the file name by getStore
                    IFileStore fileStore = EFS.getStore(new URI(uri.getScheme(), uri.getHost(), uri.getPath(), uri.getFragment()));
                    FileStoreEditorInput editorInput = new FileStoreEditorInput(fileStore);
                    IEditorDescriptor editorDescriptor = workbench.getEditorRegistry().getDefaultEditor(url.getPath());
                    workbenchPage.openEditor(editorInput, editorDescriptor.getId());
                }
            }
        });
        registerActionHandler("openView", new IActionHandler() {
            @Override
            public void executeAction(URL url, IResource resource, Map<String, String> parameters) throws PartInitException {
                IViewPart viewer = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().showView(parameters.get("id"));
                if (viewer instanceof CommonNavigator) {
                    CommonNavigator commonNavigator = (CommonNavigator)viewer;
                    commonNavigator.selectReveal(new StructuredSelection(resource));
                }
            }
        });
        registerActionHandler("reveal", new IActionHandler() {
            @Override
            public void executeAction(URL url, IResource resource, Map<String, String> parameters) throws CoreException {
                if (resource != null) {
                    if (resource != null && resource.getProject() != null && !resource.getProject().isOpen())
                        resource.getProject().open(null);
                    IWorkbench workbench = PlatformUI.getWorkbench();
                    IWorkbenchPage workbenchPage = workbench.getActiveWorkbenchWindow().getActivePage();
                    IViewPart projectExplorerPart = workbenchPage.showView(IPageLayout.ID_PROJECT_EXPLORER);
                    ISetSelectionTarget projectExplorer = (ISetSelectionTarget)projectExplorerPart;

                    if (!(resource instanceof IProject)) {
                        // workaround: a freshly opened project often stays collapsed in Project Explorer
                        // when we try to reveal a file/folder in it; we need to reveal one real file or folder first
                        IProject project = resource.getProject();
                        IResource[] members = project.members();
                        for (int i = 0; i < members.length; i++) {
                            if (!members[i].getName().startsWith(".") && !members[i].isVirtual() && !members[i].isHidden() && !members[i].isPhantom() && !members[i].isDerived()) {
                                projectExplorer.selectReveal(new StructuredSelection(members[i]));
                                break;
                            }
                        }
                    }

                    projectExplorer.selectReveal(new StructuredSelection(resource));
                }
            }
        });
    }

    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);

        if (webBrowser == null || webBrowser.getBrowser() == null) {
            Debug.print("Web browser missing in WebBrowserEditor.");
            return;
        }

        webBrowser.getBrowser().addLocationListener(new LocationListener() {
            @Override
            public void changing(LocationEvent event) {
                String action = null;
                try {
                    String location = event.location.replaceFirst("^workspace:", "file:");
                    URL url = new URL(location);
                    if (url.getQuery() != null) {
                        Map<String, String> parameters = getParameters(url);
                        action = parameters.get("action");
                        if (action != null) {
                            event.doit = false;
                            IActionHandler actionHandler = handlers.get(action);
                            if (actionHandler == null)
                                throw new RuntimeException("Unknown action: " + action);
                            IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
                            if (event.location.startsWith("workspace:")) {
                                IResource resource = findResource(root, new Path(url.getPath()));
                                if (resource != null)
                                    executeAction(actionHandler, url, resource, parameters);
                                else
                                    throw new RuntimeException("No such resource: " + url.getPath());
                            }
                            else {
                                URI uri = url.toURI();
                                IFile[] files = root.findFilesForLocationURI(uri);
                                if (files.length == 0)
                                    executeAction(actionHandler, url, null, parameters);
                                else if (files.length == 1)
                                    executeAction(actionHandler, url, files[0], parameters);
                                else
                                    throw new RuntimeException("More than one matching resource found for: " + uri);
                            }
                        }
                    }
                }
                catch (Exception e) {
                    CommonPlugin.logError("Error executing '" + action + "' action on '" + event.location, e);
                    MessageDialog.openError(null, "Error", "Error executing '" + action + "' action on '" + event.location + "' : " + e.getMessage());
                }
            }

            protected IResource findResource(IWorkspaceRoot root, Path path) throws CoreException {
                IResource resource = root.findMember(path);
                if (resource == null) {
                    // try harder: maybe it's inside a closed project
                    IProject project = root.getProject(path.segment(0));
                    if (project.exists() && !project.isOpen())
                        project.open(null);
                    resource = root.findMember(path);
                }
                return resource;
            }

            @Override
            public void changed(LocationEvent event) {
            }
        });
    }

    protected void executeAction(IActionHandler actionHandler, URL url, IResource resource, Map<String, String> parameters) throws Exception {
        if (resource != null && resource.getProject() != null && !resource.getProject().isOpen())
            resource.getProject().open(null);
        actionHandler.executeAction(url, resource, parameters);
    }

    protected Map<String, String> getParameters(URL url) throws UnsupportedEncodingException {
        Map<String, String> parameters = new LinkedHashMap<String, String>();
        String query = url.getQuery();
        String[] pairs = query.split("&");
        for (String pair : pairs) {
            int index = pair.indexOf("=");
            parameters.put(URLDecoder.decode(pair.substring(0, index), "UTF-8"), URLDecoder.decode(pair.substring(index + 1), "UTF-8"));
        }
        return parameters;
    }
}
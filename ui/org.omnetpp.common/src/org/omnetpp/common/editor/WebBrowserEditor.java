package org.omnetpp.common.editor;

import java.io.UnsupportedEncodingException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.LinkedHashMap;
import java.util.Map;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.navigator.CommonNavigator;
import org.eclipse.ui.part.FileEditorInput;

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
        public void executeAction(IResource resource, Map<String, String> parameters) throws CoreException;
    }

    public static void registerActionHandler(String action, IActionHandler handler) {
        handlers.put(action, handler);
    }

    static {
        registerActionHandler("openEditor", new IActionHandler() {
            @Override
            public void executeAction(IResource resource, Map<String, String> parameters) throws PartInitException {
                IFile file = (IFile)resource;
                IWorkbench workbench = PlatformUI.getWorkbench();
                IWorkbenchPage workbenchPage = workbench.getActiveWorkbenchWindow().getActivePage();
                IEditorDescriptor editorDescriptor = workbench.getEditorRegistry().getDefaultEditor(file.getName());
                workbenchPage.openEditor(new FileEditorInput(file), editorDescriptor.getId());
            }
        });
        registerActionHandler("openView", new IActionHandler() {
            @Override
            public void executeAction(IResource resource, Map<String, String> parameters) throws PartInitException {
                IViewPart viewer = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().showView(parameters.get("id"));
                if (viewer instanceof CommonNavigator) {
                    CommonNavigator commonNavigator = (CommonNavigator)viewer;
                    commonNavigator.selectReveal(new StructuredSelection(resource));
                }
            }
        });
    }

    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        webBrowser.getBrowser().addLocationListener(new LocationListener() {
            @Override
            public void changing(LocationEvent event) {
                String action = null;
                try {
                    URL url = new URL(event.location);
                    if (url.getQuery() != null) {
                        Map<String, String> parameters = getParameters(url);
                        action = parameters.get("action");
                        if (action != null) {
                            event.doit = false;
                            IActionHandler actionHandler = handlers.get(action);
                            if (actionHandler == null)
                                throw new RuntimeException("Unknown action: " + action);
                            for (IFile file : ResourcesPlugin.getWorkspace().getRoot().findFilesForLocationURI(url.toURI())) {
                                if (!file.getProject().isOpen())
                                    file.getProject().open(null);
                                actionHandler.executeAction(file, parameters);
                            }
                        }
                    }
                }
                catch (Exception e) {
                    MessageDialog.openError(null, "Error", "Cannot execute " + action + " action on '" + event.location + "' : " + e.getMessage());
                }
            }

            @Override
            public void changed(LocationEvent event) {
            }
        });
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
package org.omnetpp.common.editor;

import java.io.IOException;
import java.net.URL;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.internal.browser.WebBrowserEditorInput;
import org.osgi.framework.Bundle;

/**
 * Opens a file from a plugin in a WebBrowserEditor. The command should have "pluginId" and "path" parameters.
 *
 * @author Andras
 */
public class OpenWebBrowserHandler extends AbstractHandler {
    @Override
    public Object execute(ExecutionEvent event) throws ExecutionException {
        IWorkbenchPage page = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
        try {
            String pluginId = event.getParameter("pluginId"); // e.g. "org.omnetpp.doc"
            String path = event.getParameter("path"); // e.g. "/content/samples/samples.html"
            if (pluginId==null || path==null)
                throw new RuntimeException("missing command parameter");
            openFileFromPlugin(page, pluginId, path);
            return null;
        }
        catch (Exception e) {
            throw new ExecutionException("Cannot open requested page", e);
        }
    }

    protected IEditorPart openFileFromPlugin(IWorkbenchPage page, String pluginId, String path) throws IOException, PartInitException {
        Bundle docBundle = Platform.getBundle(pluginId);
        URL[] urls = FileLocator.findEntries(docBundle, new Path(path));
        Assert.isTrue(urls.length > 0, path + " is missing from plugin " + pluginId);
        URL url = FileLocator.resolve(urls[0]);
        int style = 0; //BrowserViewer.BUTTON_BAR | BrowserViewer.LOCATION_BAR;
        return IDE.openEditor(page, new WebBrowserEditorInput(url, style), "org.omnetpp.common.editor.WebBrowserEditor");
    }
}

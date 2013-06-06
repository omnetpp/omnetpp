package org.omnetpp.common.editor;

import java.net.URL;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.browser.LocationEvent;
import org.eclipse.swt.browser.LocationListener;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IEditorDescriptor;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.part.FileEditorInput;

/**
 * This class extends the standard Eclipse HTML editor.
 * The extra feature is that is will open files in new editors within Eclipse
 * if the URL contains the 'open=editor' parameter.
 *
 * @author levy
 */
@SuppressWarnings("restriction")
public class WebBrowserEditor extends org.eclipse.ui.internal.browser.WebBrowserEditor {
    @Override
    public void createPartControl(Composite parent) {
        super.createPartControl(parent);
        webBrowser.getBrowser().addLocationListener(new LocationListener() {
            @Override
            public void changing(LocationEvent event) {
                try {
                    URL url = new URL(event.location);
                    String query = url.getQuery();
                    if (query != null && query.contains("open=editor")) {
                        event.doit = false;
                        IWorkbench workbench = PlatformUI.getWorkbench();
                        IWorkbenchPage workbenchPage = workbench.getActiveWorkbenchWindow().getActivePage();
                        for (IFile file : ResourcesPlugin.getWorkspace().getRoot().findFilesForLocationURI(url.toURI())) {
                            IEditorDescriptor editorDescriptor = workbench.getEditorRegistry().getDefaultEditor(file.getName());
                            workbenchPage.openEditor(new FileEditorInput(file), editorDescriptor.getId());
                        }
                    }
                }
                catch (Exception e) {
                    MessageDialog.openError(null, "Open Failed", "Cannot open '" + event.location + "' : " + e.getMessage());
                }
            }

            @Override
            public void changed(LocationEvent event) {
            }
        });
    }
}
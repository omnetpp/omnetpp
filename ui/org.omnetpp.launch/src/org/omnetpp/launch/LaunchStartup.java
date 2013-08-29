package org.omnetpp.launch;

import java.util.Map;

import org.eclipse.core.resources.IResource;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.PartInitException;
import org.omnetpp.common.editor.WebBrowserEditor;
import org.omnetpp.common.editor.WebBrowserEditor.IActionHandler;

public class LaunchStartup implements IStartup {
    @Override
    public void earlyStartup() {
        WebBrowserEditor.registerActionHandler("launchRun", new IActionHandler() {
            @Override
            public void executeAction(IResource resource, Map<String, String> parameters) throws PartInitException {
                SimulationLaunchShortcut launchShortcut = new SimulationLaunchShortcut();
                launchShortcut.launch(new StructuredSelection(resource), ILaunchManager.RUN_MODE);
            }
        });
        WebBrowserEditor.registerActionHandler("launchDebug", new IActionHandler() {
            @Override
            public void executeAction(IResource resource, Map<String, String> parameters) throws PartInitException {
                SimulationLaunchShortcut launchShortcut = new SimulationLaunchShortcut();
                launchShortcut.launch(new StructuredSelection(resource), ILaunchManager.DEBUG_MODE);
            }
        });
    }
}

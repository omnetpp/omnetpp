package org.omnetpp.launch;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.IPath;
import org.eclipse.debug.core.DebugPlugin;
import org.eclipse.debug.core.ILaunchConfiguration;
import org.eclipse.debug.core.ILaunchManager;
import org.eclipse.debug.ui.DebugUITools;
import org.eclipse.ui.IEditorLauncher;

/**
 * Launch the configurations in .launch files 
 * @author rhornig
 */
public class ConfigurationLauncher implements IEditorLauncher {
    public void open(IPath path) {
        IFile file = ResourcesPlugin.getWorkspace().getRoot().getFileForLocation(path);
        ILaunchManager launchManager = DebugPlugin.getDefault().getLaunchManager();
        ILaunchConfiguration launchConfiguration = launchManager.getLaunchConfiguration(file);
        DebugUITools.launch(launchConfiguration, ILaunchManager.RUN_MODE);
    }

}

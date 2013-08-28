/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;

import java.net.URL;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.CommandlineUtils;
import org.omnetpp.ide.installer.FirstStepsDialog;
import org.omnetpp.ide.installer.OnClosingWelcomeView;
import org.osgi.framework.Bundle;

/**
 * Performs various tasks when the workbench starts up.
 *
 * @author Andras
 */
public class OmnetppStartup implements IStartup {
    public static final String SAMPLES_DIR = "samples";

    /*
     * Method declared on IStartup.
     */
    public void earlyStartup() {
        checkForNewVersion();

        IWorkbench workbench = PlatformUI.getWorkbench();
        workbench.getDisplay().asyncExec(new Runnable() {
            public void run() {
                CommandlineUtils.autoimportAndOpenFilesOnCommandLine();
                if (true /*TODO: if workspace is empty; or isInitialDefaultStartup()*/) {
                    new OnClosingWelcomeView(new Runnable() {
                        @Override
                        public void run() {
                            openGlancePage();
                            new FirstStepsDialog(null).open();
                        }
                    }).hook();
                }
            }
        });
    }

    protected void openGlancePage() {
        try {
            String settingName = "glancePageHasBeenShown";
            IDialogSettings dialogSettings = OmnetppMainPlugin.getDefault().getDialogSettings();
            if (!dialogSettings.getBoolean(settingName)) {
                Bundle docBundle = Platform.getBundle("org.omnetpp.doc");
                URL glanceURL = FileLocator.resolve(FileLocator.findEntries(docBundle, new Path("/content/ide-glance/glance.html"))[0]);
                PlatformUI.getWorkbench().getBrowserSupport().createBrowser("glance-page").openURL(glanceURL);
                dialogSettings.put(settingName, true);
            }
        }
        catch (Exception e) {
            OmnetppMainPlugin.logError("Cannot open glance page", e);
        }
    }

    /**
     * Check for new version
     */
    protected void checkForNewVersion() {
        new VersionChecker().checkForNewVersion();
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

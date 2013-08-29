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
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.internal.ViewIntroAdapterPart;
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

        final IWorkbench workbench = PlatformUI.getWorkbench();
        workbench.getDisplay().asyncExec(new Runnable() {
            public void run() {
                CommandlineUtils.autoimportAndOpenFilesOnCommandLine();
                if (isWorkspaceEmpty()) {
                    IWorkbenchWindow activeWorkbenchWindow = workbench.getActiveWorkbenchWindow();
                    IWorkbenchPage workbenchPage = activeWorkbenchWindow == null ? null : activeWorkbenchWindow.getActivePage();
                    if (workbenchPage != null && workbenchPage.getActivePart() instanceof ViewIntroAdapterPart) {
                        new OnClosingWelcomeView(new Runnable() {
                            @Override
                            public void run() {
                                openGlancePage();
                                new FirstStepsDialog(null).open();
                            }
                        }).hook();
                    }
                    else
                        new FirstStepsDialog(null).open();
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
     * Determines if there are no projects in the workspace yet (none have been created or imported).
     */
    protected boolean isWorkspaceEmpty() {
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        return root.getProjects().length == 0;
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

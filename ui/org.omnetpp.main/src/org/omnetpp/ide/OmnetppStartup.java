/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ide;

import java.io.IOException;
import java.net.URL;

import org.eclipse.core.resources.IWorkspace;
import org.eclipse.core.resources.IWorkspaceDescription;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.help.ui.internal.IHelpUIConstants;
import org.eclipse.help.ui.internal.views.HelpView;
import org.eclipse.help.ui.internal.views.ReusableHelpPart;
import org.eclipse.jface.dialogs.IDialogSettings;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IEditorPart;
import org.eclipse.ui.IStartup;
import org.eclipse.ui.IWorkbench;
import org.eclipse.ui.IWorkbenchPage;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.PlatformUI;
import org.eclipse.ui.ide.IDE;
import org.eclipse.ui.internal.ViewIntroAdapterPart;
import org.eclipse.ui.internal.browser.WebBrowserEditorInput;
import org.eclipse.ui.internal.help.WorkbenchHelpSystem;
import org.omnetpp.common.util.CommandlineUtils;
import org.omnetpp.common.util.ReflectionUtils;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ide.installer.FirstStepsDialog;
import org.omnetpp.ide.installer.OnClosingWelcomeView;
import org.omnetpp.scave.ScavePlugin;
import org.osgi.framework.Bundle;

/**
 * Performs various tasks when the workbench starts up.
 *
 * @author Andras
 */
@SuppressWarnings("restriction")
public class OmnetppStartup implements IStartup {
    private static final String AT_A_GLANCE_HTML = "/content/at-a-glance/at-a-glance.html";
    private static final String GETTINGSTARTED_HTML = "/content/ide-getting-started/getting-started.html";
    private static final String SAMPLES_HTML = "/content/samples/samples.html";

    /*
     * Method declared on IStartup.
     */
    public void earlyStartup() {
    	// check if the environment is properly set up
    	// if the IDE is started by directly executing the ide/opp_ide executable
    	// from a file manager, or by executing a pinned icon on the task bar
    	// the required environment variables are not properly set.
    	// We must force the IDE to exit and warn the user to execute the IDE
    	// by either running the bin/opp_ide script from the GUI or the command line
    	// and avoid pinning the application to the taskbar.
    	if (StringUtils.nullToEmpty(System.getenv("__omnetpp_root_dir")).equals("")) {
    		PlatformUI.getWorkbench().getDisplay().asyncExec(new Runnable() {
    		    public void run() {
    		        Shell activeShell = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getShell();
    		        MessageDialog.openError(activeShell, "Error Starting Simulation IDE", 
    		        		"The IDE was not started properly, exiting.\n\n"
                            + "Environment variables have not been set properly, preventing normal operation.\n\n"
                            + "The IDE should be started with the 'omnetpp' command or via the provided "
    		        		+ "launcher shortcuts in your desktop environment. "
    		        		+ "Do not pin the IDE to the taskbar, because it bypasses the launcher script.");
    		        PlatformUI.getWorkbench().close();
    		    }
    		});
    	}
    	
        // FIXME do version check only if the user agreed to it.
        // as we currently do not have a consent dialog, we just disable it for now
        // checkForNewVersion();

        ScavePlugin.ensurePyDevInterpreterConfigured();

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
                                FirstStepsDialog dialog = new FirstStepsDialog(null);
                                dialog.open();
                                openInitialPages(dialog.isImportSamplesRequested());
                            }
                        }).hook();
                    }
                    else
                        new FirstStepsDialog(null).open();
                }
            }
        });
    }

    protected void openInitialPages(boolean importSamplesRequested) {
        try {
            String settingName = "initialPagesHaveBeenShown";
            IDialogSettings dialogSettings = OmnetppMainPlugin.getDefault().getDialogSettings();
            if (!dialogSettings.getBoolean(settingName)) {
                dialogSettings.put(settingName, true);
                IWorkbenchPage workbenchPage = PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage();
                IEditorPart howToEditorPart = openURL(workbenchPage, GETTINGSTARTED_HTML);
                openURL(workbenchPage, AT_A_GLANCE_HTML);
                if (importSamplesRequested)
                    openURL(workbenchPage, SAMPLES_HTML);
                workbenchPage.activate(howToEditorPart);
                WorkbenchHelpSystem.getInstance().displayDynamicHelp(); //TODO: show the "OMNeT++ / Getting Started" folder within the ToC
                HelpView helpView = (HelpView)PlatformUI.getWorkbench().getActiveWorkbenchWindow().getActivePage().showView("org.eclipse.help.ui.HelpView");
                ReusableHelpPart reusableHelpPart = (ReusableHelpPart)ReflectionUtils.getFieldValue(helpView, "reusableHelpPart");
                reusableHelpPart.showPage(IHelpUIConstants.HV_ALL_TOPICS_PAGE);
            }
        }
        catch (Exception e) {
            OmnetppMainPlugin.logError("Error during opening initial pages", e);
        }
    }

    protected IEditorPart openURL(IWorkbenchPage page, String path) throws IOException, PartInitException {
        Bundle docBundle = Platform.getBundle("org.omnetpp.doc");
        URL[] urls = FileLocator.findEntries(docBundle, new Path(path));
        Assert.isTrue(urls.length > 0, path + " is missing from org.omnetpp.doc, needs to be copied from doc/");
        URL url = FileLocator.resolve(urls[0]);
        int style = 0; //BrowserViewer.BUTTON_BAR | BrowserViewer.LOCATION_BAR;
        return IDE.openEditor(page, new WebBrowserEditorInput(url, style), "org.omnetpp.common.editor.WebBrowserEditor");
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

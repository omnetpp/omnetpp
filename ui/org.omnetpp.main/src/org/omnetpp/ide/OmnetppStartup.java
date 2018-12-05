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

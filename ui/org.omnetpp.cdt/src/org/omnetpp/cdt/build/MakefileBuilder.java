/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.build;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICConfigurationDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.managedbuilder.core.IConfiguration;
import org.eclipse.cdt.managedbuilder.core.IManagedBuildInfo;
import org.eclipse.cdt.managedbuilder.core.IToolChain;
import org.eclipse.cdt.managedbuilder.core.ManagedBuildManager;
import org.eclipse.core.resources.ICommand;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.eclipse.jface.dialogs.ErrorDialog;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.IWorkbenchWindow;
import org.eclipse.ui.PlatformUI;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.build.ProjectFeaturesManager.Problem;
import org.omnetpp.common.Debug;
import org.omnetpp.common.OmnetppDirs;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.ui.ProblemsMessageDialog;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.common.util.UIUtils;

/**
 * Keeps makefiles up to date.
 *
 * @author Andras
 */
public class MakefileBuilder extends IncrementalProjectBuilder {
    public static final String BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";
    public static final String MARKER_ID = "org.omnetpp.cdt.makefileproblem"; //XXX this is shared with DependencyCache

    private BuildSpecification buildSpec = null;  // re-read for each build
    private ProblemMarkerSynchronizer markerSynchronizer = null; // new instance for each build

    /**
     * Method declared on IncrementalProjectBuilder. Main entry point.
     */
    @Override @SuppressWarnings({ "rawtypes" })
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) {
        try {
            checkProjectFeatures();
            checkOrderOfProjectBuilders();
            checkActiveCDTConfiguration();

            markerSynchronizer = new ProblemMarkerSynchronizer(MARKER_ID);
            buildSpec = BuildSpecification.readBuildSpecFile(getProject());
            if (buildSpec == null)
                buildSpec = BuildSpecification.createBlank(getProject());

            // refresh makefiles
            generateMakefiles(monitor);
        }
        catch (final CoreException e) {
            // A serious error occurred during Makefile generation. Add it as marker,
            // and also pop up a dialog so that the error is obvious to the user.
            // Note: we cannot let the CoreException propagate, because Eclipse would
            // disable the builder completely! (for the duration of the session)
            Activator.logError(e);
            addMarker(getProject(), IMarker.SEVERITY_ERROR, "Error refreshing Makefiles: " + e.getMessage());
            runInUIThread(new Runnable() {
                public void run() {
                    ErrorDialog.openError(getActiveShell(), "Error", "Error refreshing Makefiles", e.getStatus());
                }
            });
        }
        finally {
            markerSynchronizer.runAsWorkspaceJob();
            markerSynchronizer = null;
            buildSpec = null;
        }

        try {
            return ProjectUtils.getAllReferencedProjects(getProject());
        }
        catch (CoreException e) {
            Activator.logError(e);
            return new IProject[0];
        }
    }

    @Override
    protected void clean(IProgressMonitor monitor) throws CoreException {
        super.clean(monitor);
        Activator.getIncludeFoldersCache().clean(getProject());
        getProject().deleteMarkers(MARKER_ID, true, IResource.DEPTH_INFINITE);
    }

    /**
     * Checks whether the project's CDT configuration corresponds to the set of enabled project
     * features (see ProjectFeatureManager).
     */
    protected void checkProjectFeatures() {
        try {
            // load feature description from file
            final ProjectFeaturesManager features = new ProjectFeaturesManager(getProject());
            if (features.loadFeaturesFile()) {
                // check that CDT and NED state corresponds to the feature selection
                final List<Problem> problems = features.validateProjectState();
                if (!problems.isEmpty()) {
                    runInUIThread(new Runnable() {
                        public void run() {
                            offerFixingProblems(features, problems);
                        }
                    });
                }
            }
        }
        catch (CoreException e) {
            // log, but otherwise ignore it
            Activator.logError("Error checking whether project configuration corresponds to project features enablement", e);
        }
    }

    protected void offerFixingProblems(ProjectFeaturesManager features, List<Problem> problems) {
        if (isOkToFixConfigProblems(features.getProject(), problems)) {
            try {
                features.fixupProjectState();
            }
            catch (CoreException e) {
                Activator.logError(e);
                ErrorDialog.openError(getActiveShell(), "Error", "Error fixing project state", e.getStatus());
            }
        }
    }

    protected boolean isOkToFixConfigProblems(IProject project, List<Problem> problems) {
        List<String> problemTexts = new ArrayList<String>();
        for (Problem p : problems)
            problemTexts.add(p.toString());
        return ProblemsMessageDialog.openConfirm(getActiveShell(), "Project Setup Inconsistency",
                "Some configuration settings in project \"" + project.getName() + "\" do not correspond " +
                "to the enabled project features. This may cause build errors as well. " +
                "Do you want to fix the project state?",
                problemTexts, UIUtils.ICON_ERROR);
    }

    /**
     * This builder should precede CDT's builder; check it and warn the user if it's not the case.
     */
    protected void checkOrderOfProjectBuilders() {
        final IProject project = getProject();
        try {
            // find this builder and CDT's builder in the project description
            ICommand[] builders = project.getDescription().getBuildSpec();
            int thisBuilderPos = -1;
            int cdtBuilderPos = -1;
            for (int i=0; i<builders.length; i++) {
                ICommand builder = builders[i];
                String builderId = StringUtils.nullToEmpty(builder.getBuilderName());
                if (thisBuilderPos == -1 && builderId.equals(BUILDER_ID))
                    thisBuilderPos = i;
                if (cdtBuilderPos == -1 && builderId.startsWith("org.eclipse.cdt."))
                    cdtBuilderPos = i;
            }

            // log a warning if something looks fishy
            if (thisBuilderPos == -1)
                Activator.log(IMarker.SEVERITY_WARNING, "Builder " + BUILDER_ID + " is not on project " + project + " and still gets called?");
            if (cdtBuilderPos == -1)
                Activator.log(IMarker.SEVERITY_WARNING, "CDT's builder is missing from project " + project + " when " + BUILDER_ID + " is running! Not configured, or perhaps a custom builder is in use?");

            // warn the user if builders are in reverse order
            if (thisBuilderPos != -1 && cdtBuilderPos != -1 && cdtBuilderPos < thisBuilderPos) {
                runInUIThread(new Runnable() {
                    public void run() {
                        String message =
                            "The C/C++ Project Builder seems to precede the OMNeT++ Makefile Builder " +
                            "in project \"" + project.getName() + "\", which is incorrect. You can fix " +
                            "this problem by removing and adding back the OMNeT++ Nature, using the " +
                            "project's context menu.";
                        MessageDialog.openWarning(getActiveShell(), "Warning", message);
                    }
                });
            }
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    /**
     * This is an attempt to advise the user to switch to the correct CDT configuration,
     * if a wrong one is selected.
     */
    protected void checkActiveCDTConfiguration() {
        IProject project = getProject();
        IManagedBuildInfo buildInfo = ManagedBuildManager.getBuildInfo(project);
        final IConfiguration activeConfig = buildInfo!=null ? buildInfo.getDefaultConfiguration() : null;
        final IToolChain toolChain = activeConfig!=null ? activeConfig.getToolChain() : null;
        if (toolChain==null)
            return;

        boolean supported = isToolChainSupported(toolChain);

        if (!supported) {
            runInUIThread(new Runnable() {
                public void run() {
                    String message =
                        "Toolchain \"" + toolChain.getName() + "\" is not supported on this platform " +
                        "or installation. \n\n" +
                        "Please switch to a different build configuration in the project context menu.";
                    MessageDialog.openWarning(getActiveShell(), "Project "+getProject().getName(), message);
                }
            });
            return;
        }

        // check if the libraries required for the active configuration are present
        // advise the user to switch configs
        boolean libMissing =
                (activeConfig.getBaseId().startsWith("org.omnetpp.cdt.gnu.config.debug") && !OmnetppDirs.isOppsimGccOrClangLibraryPresent(true)) ||
                (activeConfig.getBaseId().startsWith("org.omnetpp.cdt.gnu.config.release") && !OmnetppDirs.isOppsimGccOrClangLibraryPresent(false)) ||
                (activeConfig.getBaseId().startsWith("org.omnetpp.cdt.msvc.config.debug") && !OmnetppDirs.isOppsimVcLibraryPresent(true)) ||
                (activeConfig.getBaseId().startsWith("org.omnetpp.cdt.msvc.config.release") && !OmnetppDirs.isOppsimVcLibraryPresent(false));

        if (libMissing) {
            runInUIThread(new Runnable() {
                public void run() {
                    String message =
                        "OMNeT++ libraries not yet compiled: library files for configuration \"" + activeConfig.getName() + "\" " +
                        "are missing from " + OmnetppDirs.getOmnetppLibDir() + ".\n\n"+
                        "Switch to a different build configuration in the project context menu, " +
                        "or build the OMNeT++ libraries from " +
                        "the command line. (See the Install Guide for help.)";
                    MessageDialog.openWarning(getActiveShell(), "Project "+getProject().getName(), message);
                }
            });
        }
    }

    /**
     * Delegates to Display.syncExec() if it is safe to do so (will not cause lockup),
     * otherwise to Display.asyncExec().
     */
    protected void runInUIThread(Runnable runnable) {
        // When the platform is starting and only the splash screen is displayed, it is not safe to do
        // Display.syncExec() because it can easily cause a deadlock: syncExec() executes when the UI thread
        // processes GUI events, but during startup the UI thread may be busy waiting for some lock (e.g.
        // workspace lock) held by us. To prevent that from happening, we perform an asyncExec() instead of
        // syncExec() if the platform is currently starting up.
        if (!PlatformUI.getWorkbench().isStarting())
            Display.getDefault().syncExec(runnable); // normally
        else
            Display.getDefault().asyncExec(runnable); // during startup, to avoid deadlock
    }

    /**
     * Returns a shell to be used as parent shell for message dialogs.
     */
    protected Shell getActiveShell() {
        // note: Display.getCurrent().getActiveShell() is not good as parent (ProgressDialog would pull down our dialog too when it disappears)
        IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
        return activeWorkbenchWindow==null ? null : activeWorkbenchWindow.getShell();
    }

    /**
     * Returns true if the toolchain is supported currently. Note: toolChain().isSupported() is
     * not good enough, as it doesn't check the toolchain's super classes and platform filters.
     * Here we do both.
     */
    protected static boolean isToolChainSupported(IToolChain toolchain) {
        if (!isToolchainSupportedOnCurrentPlatform(toolchain))
            return false;
        while (toolchain != null) {
            if (!toolchain.isSupported())
                return false;
            toolchain = toolchain.getSuperClass();
        }
        return true;
    }

    protected static String[] getEffectiveOSList(IToolChain toolchain) {
        while(toolchain != null && toolchain.getOSList().length == 0)
            toolchain = toolchain.getSuperClass();
        return toolchain == null ? new String[0] : toolchain.getOSList();
    }

    protected static boolean isToolchainSupportedOnCurrentPlatform(IToolChain toolchain) {
        List<String> osList = Arrays.asList(getEffectiveOSList(toolchain));
        return osList.isEmpty() || osList.contains("all") || osList.contains(Platform.getOS());
    }

    /**
     * Generates all makefiles in this project.
     */
    protected void generateMakefiles(IProgressMonitor monitor) throws CoreException  {
        monitor.subTask("Analyzing dependencies and updating makefiles...");

        // collect folders
        List<IContainer> makemakeFolders = buildSpec.getMakemakeFolders();
        if (makemakeFolders.isEmpty())
            return; // nothing to do

        // get CDT active configuration
        ICProjectDescription projectDescription = CoreModel.getDefault().getProjectDescription(getProject());
        ICConfigurationDescription configuration = projectDescription==null ? null : projectDescription.getActiveConfiguration();
        if (configuration == null) {
            Activator.log(IMarker.SEVERITY_WARNING, "No CDT build info for project " + getProject().getName() + ", skipping makefile generation");
            return;
        }

        // register folders in the marker synchronizer
        for (IContainer makemakeFolder : makemakeFolders)
            markerSynchronizer.register(makemakeFolder);

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        for (IContainer makemakeFolder : makemakeFolders) {
            if (makemakeFolder.isAccessible())
                generateMakefileFor(makemakeFolder, configuration, monitor);
            else
                Debug.println("generateMakefiles(): ignoring nonexisting folder listed in buildspec: " + makemakeFolder.getFullPath());
        }
        Debug.println("Generated " + makemakeFolders.size() + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Generate makefile in the given folder.
     */
    protected void generateMakefileFor(IContainer folder, ICConfigurationDescription configuration, IProgressMonitor monitor) throws CoreException {
        boolean deleteMakeFiles = true;
        try {
            //Debug.println("Generating makefile in: " + folder.getFullPath());
            Assert.isTrue(folder.getProject().equals(getProject()));
            MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
            Assert.isTrue(options != null);
            MetaMakemake.generateMakefile(folder, buildSpec, configuration, monitor);
            deleteMakeFiles = false;
        }
        catch (MakemakeException e) {
            deleteMakeFiles = e.getDeleteMakefile();
            throw Activator.wrapIntoCoreException(e);
        }
        finally {
            if (deleteMakeFiles) {
                // remove stale/incomplete makefile, so that build won't continue with CDT
                try { folder.getFile(new Path("Makefile")).delete(true, null); } catch (CoreException e1) {}
                try { folder.getFile(new Path("Makefile.vc")).delete(true, null); } catch (CoreException e1) {}
            }
        }
    }

    protected void addMarker(IResource resource, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(resource, MARKER_ID, map);
    }

}

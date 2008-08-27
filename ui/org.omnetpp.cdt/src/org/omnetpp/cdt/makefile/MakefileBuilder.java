package org.omnetpp.cdt.makefile;

import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

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
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.util.StringUtils;

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
    @Override @SuppressWarnings("unchecked")
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) {
        try {
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
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    ErrorDialog.openError(Display.getCurrent().getActiveShell(), "Error", "Error refreshing Makefiles", e.getStatus());
                }
            });
        }
        finally {
            markerSynchronizer.runAsWorkspaceJob();
            markerSynchronizer = null;
            buildSpec = null;
        }

        return Activator.getDependencyCache().getProjectGroup(getProject());
    }

    @Override
    protected void clean(IProgressMonitor monitor) throws CoreException {
        super.clean(monitor);
        Activator.getDependencyCache().clean(getProject());
        getProject().deleteMarkers(MARKER_ID, true, IResource.DEPTH_INFINITE);
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
                Display.getDefault().asyncExec(new Runnable() {
                    public void run() {
                        String message = 
                            "The C/C++ Project Builder seems to precede the OMNeT++ Makefile Builder " +
                            "in project \"" + project.getName() + "\", which is incorrect. You can fix " +
                            "this problem by removing and adding back the OMNeT++ Nature, using the " +
                            "project's context menu.";
                        IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                        Shell parent = activeWorkbenchWindow==null ? null : activeWorkbenchWindow.getShell();
                        // note: Display.getCurrent().getActiveShell() is not good as parent (ProgressDialog would pull down our dialog too when it disappears)
                        MessageDialog.openWarning(parent, "Warning", message);
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
        IConfiguration activeConfig = buildInfo!=null ? buildInfo.getDefaultConfiguration() : null;
        final IToolChain toolChain = activeConfig!=null ? activeConfig.getToolChain() : null;
        if (toolChain==null)
            return;
        
        boolean supported = isToolChainSupported(toolChain);
        
        if (!supported) {
            Display.getDefault().asyncExec(new Runnable() {
                public void run() {
                    String message = 
                        "Toolchain \"" + toolChain.getName() + "\" is not supported on this platform " +
                        "or installation. Please go to the Project menu, and activate a different " +
                        "build configuration. (You may need to switch to the C/C++ perspective first, " +
                        "so that the required menu items appear in the Project menu.)";
                    IWorkbenchWindow activeWorkbenchWindow = PlatformUI.getWorkbench().getActiveWorkbenchWindow();
                    Shell parent = activeWorkbenchWindow==null ? null : activeWorkbenchWindow.getShell();
                    // note: Display.getCurrent().getActiveShell() is not good as parent (ProgressDialog would pull down our dialog too when it disappears)
                    MessageDialog.openWarning(parent, "Warning", message);
                }
            });
        }
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
        IContainer[] makemakeFolders = buildSpec.getMakemakeFolders();
        
        // register folders in the marker synchronizer
        for (IContainer makemakeFolder : makemakeFolders)
            markerSynchronizer.register(makemakeFolder);

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        for (IContainer makemakeFolder : makemakeFolders)
            generateMakefileFor(makemakeFolder);
        System.out.println("Generated " + makemakeFolders.length + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Generate makefile in the given folder.
     */
    protected void generateMakefileFor(IContainer folder) throws CoreException {
        boolean ok = false;
        try {
            //System.out.println("Generating makefile in: " + folder.getFullPath());
            Assert.isTrue(folder.getProject().equals(getProject()));
            MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
            Assert.isTrue(options!=null);
            MetaMakemake.generateMakefile(folder, options);
            ok = true;
        }
        catch (MakemakeException e) {
            throw Activator.wrapIntoCoreException(e);
        }
        finally {
            if (!ok) {
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

package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.cdt.Activator;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;

/**
 * Keeps makefiles up to date
 * 
 * @author Andras
 */
//FIXME msg files don't generate proper cross-folder and cross-file dependencies!!!!!!!
//TODO test that cross-project includes work well
//FIXME dependencia generalas szar...
public class MakefileBuilder extends IncrementalProjectBuilder {
    public static final String BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";
    public static final String MARKER_ID = "org.omnetpp.cdt.makefileproblem";
    
    private BuildSpecification buildSpec = null;  // re-read for each build
    private ProblemMarkerSynchronizer markerSynchronizer = null; // new instance for each build

    /**
     * Method declared on IncrementalProjectBuilder. Main entry point.
     */
    @Override @SuppressWarnings("unchecked")
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
        try {
            markerSynchronizer = new ProblemMarkerSynchronizer(MARKER_ID);
            buildSpec = BuildSpecUtils.readBuildSpecFile(getProject()); //XXX possible IllegalArgumentException
            if (buildSpec == null)
                buildSpec = new BuildSpecification();

            // warn for linked resources
            //FIXME why here?
            for (IResource linkedResource : Activator.getDependencyCache().getLinkedResources())
                addMarker(linkedResource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles");

            // refresh makefiles
            generateMakefiles(monitor);
            return Activator.getDependencyCache().getProjectGroup(getProject());
        }
        catch (Exception e) {
            // This is expected to catch mostly IOExceptions. Other (non-fatal) errors 
            // are already converted to markers inside the build methods.
            addMarker(getProject(), IMarker.SEVERITY_ERROR, "Error refreshing Makefiles: " + e.getMessage());
            return null;
        }
        finally {
            markerSynchronizer.runAsWorkspaceJob();
            markerSynchronizer = null;
            buildSpec = null;
        }
    }

    protected void generateMakefiles(IProgressMonitor monitor) throws CoreException, IOException {
        monitor.subTask("Analyzing dependencies..."); //XXX not really -- all such code moved into DependencyCache... 

        // collect folders
        IContainer[] makemakeFolders = buildSpec.getMakemakeFolders();
        
        // register folders in the marker synchronizer
        for (IContainer makemakeFolder : makemakeFolders)
            markerSynchronizer.register(makemakeFolder);

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        monitor.subTask("Updating makefiles...");
        for (IContainer makemakeFolder : makemakeFolders)
            generateMakefileFor(makemakeFolder);
        System.out.println("Generated " + makemakeFolders.length + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Generate makefile in the given folder.
     */
    protected boolean generateMakefileFor(IContainer folder) {
        try {
            //System.out.println("Generating makefile in: " + folder.getFullPath());
            Assert.isTrue(folder.getProject().equals(getProject()) && buildSpec.isMakemakeFolder(folder));
            MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
            boolean changed = MetaMakemake.generateMakefile(folder, options);
            //FIXME remove makefile if any exception occurred here, so that build won't continue with CDT?
            if (changed)
                folder.refreshLocal(IResource.DEPTH_INFINITE, null);
            return changed;
        }
        catch (Exception e) {
            addMarker(folder, IMarker.SEVERITY_ERROR, "Error refreshing Makefile: " + e.getMessage());
            return true;
        }
    }

    protected void addMarker(IResource resource, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(resource, MARKER_ID, map);
    }
    
}

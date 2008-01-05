package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;

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
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
        try {
            if (kind == CLEAN_BUILD)
                Activator.getDependencyCache().clean(getProject());

            markerSynchronizer = new ProblemMarkerSynchronizer(MARKER_ID);
            buildSpec = BuildSpecUtils.readBuildSpecFile(getProject()); //XXX possible IllegalArgumentException
            if (buildSpec == null)
                buildSpec = new BuildSpecification();
            
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
        monitor.subTask("Analyzing dependencies and updating makefiles...");

        // collect folders
        List<IContainer> makemakeFolders = CDTUtils.getSourceFolders(getProject());
        
        // register folders in the marker synchronizer
        for (IContainer makemakeFolder : makemakeFolders)
            markerSynchronizer.register(makemakeFolder);

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        for (IContainer makemakeFolder : makemakeFolders)
            generateMakefileFor(makemakeFolder);
        System.out.println("Generated " + makemakeFolders.size() + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Generate makefile in the given folder.
     */
    protected void generateMakefileFor(IContainer folder) {
        try {
            //System.out.println("Generating makefile in: " + folder.getFullPath());
            Assert.isTrue(folder.getProject().equals(getProject()));
            MakemakeOptions options = buildSpec.getMakemakeOptions(folder);
            if (options == null)
                options = MakemakeOptions.createInitial();
            MetaMakemake.generateMakefile(folder, options);
        }
        catch (CoreException e) {
            addMarker(folder, IMarker.SEVERITY_ERROR, "Error refreshing Makefile: " + e.getMessage());
            // remove stale/incomplete makefile, so that build won't continue with CDT
            //XXX makefile.vc ???
            try { folder.getFile(new Path("Makefile")).delete(true, null); } catch (CoreException e1) {}
        }
    }

    protected void addMarker(IResource resource, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(resource, MARKER_ID, map);
    }
    
}

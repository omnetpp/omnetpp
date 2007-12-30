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

//FIXME why exactly the next lines are needed at all??? chuck them out!            
//            // (re)parse #include statements from C++ files in this folder and referenced folders
//            IProject[] referencedProjects = ProjectUtils.getAllReferencedProjects(getProject());
//            IProject[] projectGroup = (IProject[]) ArrayUtils.add(referencedProjects, 0, getProject());
//            for (IProject project : projectGroup) {
//XXX                
//                if (kind == FULL_BUILD)
//                    Activator.getDependencyCache().collectIncludesFully(project, monitor);
//                else {
//                    IResourceDelta delta = getDelta(project);
//                    if (delta == null)
//                        Activator.getDependencyCache().collectIncludesFully(project, monitor);
//                    else 
//                        Activator.getDependencyCache().collectIncludesIncrementally(delta, monitor);
//                }
//            }


            // warn for linked resources
            //FIXME throw out these too!!! instead, use "messages" from DependencyCache!!!
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
//        long startTime1 = System.currentTimeMillis();
//        System.out.println("Folder collection and dependency analysis: " + (System.currentTimeMillis()-startTime1) + "ms");


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

//    /**
//     * Collect "interesting" folders in this project and all referenced projects;
//     * that is, omits excluded folders, team-private folders ("CVS", ".svn"), etc.
//     */
//    // XXX not needed anymore 
//    protected IContainer[] collectFolders() throws CoreException, IOException {
//        final List<IContainer> result = new ArrayList<IContainer>();
//        
//        class FolderCollector implements IResourceVisitor {
//            BuildSpecification buildSpec;
//            List<IContainer> result;
//            public FolderCollector(BuildSpecification buildSpec, List<IContainer> result) {
//                this.buildSpec = buildSpec;
//                this.result = result;
//            }
//            public boolean visit(IResource resource) throws CoreException {
//                if (MakefileTools.isGoodFolder(resource) /*FIXME and not excluded from build*/) { 
//                    result.add((IContainer)resource);
//                    return true;
//                }
//                return false;
//            }
//        };
//        
//        // collect folders from this project and all referenced projects
//        getProject().accept(new FolderCollector(buildSpec, result));
//        for (IProject referencedProject : getProject().getReferencedProjects()) {
//            BuildSpecification referencedBuildSpec = BuildSpecUtils.readBuildSpecFile(referencedProject);
//            referencedProject.accept(new FolderCollector(referencedBuildSpec, result));
//        }
//
//        // sort by full path
//        Collections.sort(result, new Comparator<IContainer>() {
//            public int compare(IContainer o1, IContainer o2) {
//                return o1.getFullPath().toString().compareTo(o2.getFullPath().toString());
//            }});
//        return result.toArray(new IContainer[]{});
//    }

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

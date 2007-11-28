package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.cdt.makefile.MakefileTools.Include;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;

/**
 * Keeps makefiles up to date
 * 
 * @author Andras
 */
//TODO test that cross-project includes work well
public class MakefileBuilder extends IncrementalProjectBuilder {
    public static final String BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";
    public static final String MARKER_ID = "org.omnetpp.cdt.makefileproblem";
    
    private Map<IFile,List<Include>> fileIncludes = null; // created by first full build

    private BuildSpecification buildSpec = null;  // re-read for each build
    private ProblemMarkerSynchronizer markerSynchronizer = null; // new instance for each build

    private boolean generateMakemakefile = false;

    /**
     * Method declared on IncrementalProjectBuilder. Main entry point.
     */
    @Override @SuppressWarnings("unchecked")
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
        try {
            markerSynchronizer = new ProblemMarkerSynchronizer(MARKER_ID);
            buildSpec = BuildSpecUtils.readBuildSpecFile(getProject());
            if (buildSpec == null)
                buildSpec = new BuildSpecification();

            if (fileIncludes == null || kind == FULL_BUILD)
                fullBuild(monitor);
            else {
                IResourceDelta delta = getDelta(getProject());
                if (delta == null)
                    fullBuild(monitor);
                else 
                    incrementalBuild(delta, monitor);
            }
        }
        catch (Exception e) {
            // This is expected to catch mostly IOExceptions. Other (non-fatal) errors 
            // are already converted to markers inside the build methods.
            addMarker(getProject(), IMarker.SEVERITY_ERROR, "Error refreshing Makefiles: " + e.getMessage());
        }
        finally {
            markerSynchronizer.runAsWorkspaceJob();
            markerSynchronizer = null;
            buildSpec = null;
        }
        return null;
    }

    protected void fullBuild(final IProgressMonitor monitor) throws CoreException, IOException {
        // parse all C++ source files for #include; also warn for linked-in files
        // (note: warning for linked-in folders will be issued in generateMakefiles())
        monitor.subTask("Scanning source files in project " + getProject().getName() + "...");
        fileIncludes = new HashMap<IFile, List<Include>>();
        getProject().accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (MakefileTools.isCppFile(resource) && buildSpec.isMakemakeFolder(resource.getParent())) {
                    warnIfLinkedResource(resource);
                    processFileIncludes((IFile)resource);
                }
                return MakefileTools.isGoodFolder(resource); //FIXME skip folders where no makefile is generated
            }
        });
        generateMakefiles(monitor);
    }

    protected void incrementalBuild(IResourceDelta delta, IProgressMonitor monitor) throws CoreException, IOException {
        monitor.subTask("Scanning changed files in project " + getProject().getName() + "...");
        processDelta(delta);
        generateMakefiles(monitor);  //TODO optimize: maybe only in directories affected by the delta
    }

    protected void processDelta(IResourceDelta delta) throws CoreException {
        // re-parse changed C++ source files for #include; also warn for linked-in files
        // (note: warning for linked-in folders will be issued in generateMakefiles())
        delta.accept(new IResourceDeltaVisitor() {
            public boolean visit(IResourceDelta delta) throws CoreException {
                IResource resource = delta.getResource();
                boolean isSourceFile = MakefileTools.isCppFile(resource) && !buildSpec.isExcludedFromBuild(resource.getParent());
                switch (delta.getKind()) {
                    case IResourceDelta.ADDED:
                        if (isSourceFile) {
                            warnIfLinkedResource(resource);
                            processFileIncludes((IFile)resource);
                        }
                        break;
                    case IResourceDelta.CHANGED:
                        if (isSourceFile) {
                            warnIfLinkedResource(resource);
                            processFileIncludes((IFile)resource);
                        }
                        break;
                    case IResourceDelta.REMOVED: 
                        if (isSourceFile) 
                            fileIncludes.remove(resource);
                        break;
                }
                return MakefileTools.isGoodFolder(resource) && !buildSpec.isExcludedFromBuild((IContainer)resource);
            }
        });
    }


    protected void generateMakefiles(IProgressMonitor monitor) throws CoreException, IOException {
        monitor.subTask("Analyzing dependencies...");
        long startTime1 = System.currentTimeMillis();

        // collect folders
        IContainer[] folders = collectFolders();
        
        // register folders in the marker synchronizer
        for (IContainer folder : folders) {
            markerSynchronizer.register(folder);
            warnIfLinkedResource(folder);
        }
        
        // discover cross-folder dependencies
        Map<IContainer,Set<IContainer>> folderDeps = MakefileTools.calculateDependencies(fileIncludes);
        //MakefileTools.dumpDeps(folderDeps);

        Map<IContainer, Map<IFile, Set<IFile>>> perFileDeps = MakefileTools.calculatePerFileDependencies(fileIncludes);
        System.out.println("Folder collection and dependency analysis: " + (System.currentTimeMillis()-startTime1) + "ms");

        buildSpec.setConfigFileLocation(getProject().getLocation().toOSString()+"/configuser.vc"); //FIXME not here, not hardcoded!
        
        if (generateMakemakefile) {
            //XXX this should probably become body of some Action
            Map<IContainer, String> targetNames = MakefileTools.generateTargetNames(folders);
            String makeMakeFile = MakefileTools.generateMakeMakeFile(folders, folderDeps, targetNames);
            IFile file = getProject().getFile("Makemakefile");
            MakefileTools.ensureFileContent(file, makeMakeFile.getBytes(), monitor);
        }

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        monitor.subTask("Updating makefiles...");
        for (IContainer folder : folders)
            if (folder.getProject().equals(getProject()) && buildSpec.isMakemakeFolder(folder)) 
                generateMakefileFor(folder, folderDeps, perFileDeps);
        System.out.println("Generated " + folders.length + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
    }

    /**
     * Collect "interesting" folders in this project and all referenced projects;
     * that is, omits excluded folders, team-private folders ("CVS", ".svn"), etc.
     */
    protected IContainer[] collectFolders() throws CoreException, IOException {
        final List<IContainer> result = new ArrayList<IContainer>();
        
        class FolderCollector implements IResourceVisitor {
            BuildSpecification buildSpec;
            List<IContainer> result;
            public FolderCollector(BuildSpecification buildSpec, List<IContainer> result) {
                this.buildSpec = buildSpec;
                this.result = result;
            }
            public boolean visit(IResource resource) throws CoreException {
                if (MakefileTools.isGoodFolder(resource) && (buildSpec==null || !buildSpec.isExcludedFromBuild((IContainer)resource))) { 
                    result.add((IContainer)resource);
                    return true;
                }
                return false;
            }
        };
        
        // collect folders from this project and all referenced projects
        getProject().accept(new FolderCollector(buildSpec, result));
        for (IProject referencedProject : getProject().getReferencedProjects()) {
            BuildSpecification referencedBuildSpec = BuildSpecUtils.readBuildSpecFile(referencedProject);
            referencedProject.accept(new FolderCollector(referencedBuildSpec, result));
        }

        // sort by full path
        Collections.sort(result, new Comparator<IContainer>() {
            public int compare(IContainer o1, IContainer o2) {
                return o1.getFullPath().toString().compareTo(o2.getFullPath().toString());
            }});
        return result.toArray(new IContainer[]{});
    }

    /**
     * Generate makefile in the given folder.
     */
    protected boolean generateMakefileFor(IContainer folder, Map<IContainer,Set<IContainer>> folderDeps, Map<IContainer,Map<IFile,Set<IFile>>> perFileDeps) {
        try {
            //System.out.println("Generating makefile in: " + folder.getFullPath());
            Assert.isTrue(folder.getProject().equals(getProject()) && buildSpec.isMakemakeFolder(folder));
            MakemakeOptions options = buildSpec.getFolderOptions(folder);
            if (options == null) 
                options = new MakemakeOptions();
            
            MakemakeOptions tmpOptions = options.clone();
            
            tmpOptions.force = true;
            
            // add dependent folders
            if (folderDeps.containsKey(folder))
                for (IContainer dep : folderDeps.get(folder))
                    tmpOptions.includeDirs.add(dep.getLocation().toString());
            
            // add subfolders
            for (IResource member : folder.members())
                if (MakefileTools.isGoodFolder(member) && !buildSpec.isExcludedFromBuild((IContainer)member))
                    tmpOptions.subdirs.add(member.getName());

            boolean changed = new MakeMake().generateMakefile(folder, tmpOptions, perFileDeps, buildSpec.getConfigFileLocation());
            if (changed)
                folder.refreshLocal(IResource.DEPTH_INFINITE, null);
            return changed;
        }
        catch (Exception e) {
            addMarker(folder, IMarker.SEVERITY_ERROR, "Error refreshing Makefile: " + e.getMessage());
            return true;
        }
    }

    /**
     * Parses the file for the list of #includes, and returns true if it changed 
     * since the previous state.
     */
    protected boolean processFileIncludes(IFile file) throws CoreException {
        List<Include> includes = MakefileTools.parseIncludes(file);

        if (!includes.equals(fileIncludes.get(file))) {
            fileIncludes.put(file, includes);
            return true;
        }
        return false;
    }
    
    protected void warnIfLinkedResource(IResource resource) {
        if (resource.isLinked())
            addMarker(resource, IMarker.SEVERITY_ERROR, "Linked resources are not supported by Makefiles");
    }
    
    protected void addMarker(IResource resource, int severity, String message) {
        Map<String, Object> map = new HashMap<String, Object>();
        map.put(IMarker.SEVERITY, severity);
        map.put(IMarker.MESSAGE, message);
        markerSynchronizer.addMarker(resource, MARKER_ID, map);
    }
    
}

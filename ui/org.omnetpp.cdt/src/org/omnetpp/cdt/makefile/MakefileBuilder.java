package org.omnetpp.cdt.makefile;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.IncrementalProjectBuilder;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.makefile.BuildSpecification.FolderInfo;
import org.omnetpp.cdt.makefile.BuildSpecification.FolderType;
import org.omnetpp.cdt.makefile.MakefileTools.Include;

/**
 * Keeps makefiles up to date
 * 
 * @author Andras
 */
public class MakefileBuilder extends IncrementalProjectBuilder {
    public static final String BUILDER_ID = "org.omnetpp.cdt.MakefileBuilder";

    private BuildSpecification buildSpec = new BuildSpecification();
    private Map<IFile,List<Include>> fileIncludes = null; // created by first full build

    private boolean generateMakemakefile = false;

    /**
     * Method declared on IncrementalProjectBuilder. Main entry point.
     */
    @Override @SuppressWarnings("unchecked")
    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
        if (fileIncludes == null || kind == FULL_BUILD) {
            fullBuild(monitor);
        } else {
            IResourceDelta delta = getDelta(getProject());
            if (delta == null)
                fullBuild(monitor);
            else 
                incrementalBuild(delta, monitor);
        }
        return null;
    }

    protected void fullBuild(final IProgressMonitor monitor) throws CoreException {
        fileIncludes = new HashMap<IFile, List<Include>>();
        getProject().accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==FolderType.GENERATED_MAKEFILE)
                    processFileIncludes((IFile)resource);
                return true;  //FIXME skip cvs/svn folder, "dot" folders, folders where no makefile is generated
            }
        });
        
        generateMakefiles(monitor);

    }

    protected void incrementalBuild(IResourceDelta delta, IProgressMonitor monitor) throws CoreException {
        processDelta(delta);
        generateMakefiles(monitor);  //XXX maybe only in directories affected by the delta
    }

    protected void generateMakefiles(IProgressMonitor monitor) throws CoreException {
        // get folder list
        long startTime1 = System.currentTimeMillis();
        monitor.subTask("Analyzing dependencies...");
        IProject rootContainer = getProject();
        IContainer[] folders = MakefileTools.collectFolders(rootContainer);
        Map<IContainer,Set<IContainer>> folderDeps = MakefileTools.calculateDependencies(fileIncludes);
        //MakefileTools.dumpDeps(folderDeps);

        //FIXME shouldn't we use location IPaths everywhere for included files?? make understands the file system only... 
        Map<IFile, Set<IFile>> perFileDeps = MakefileTools.calculatePerFileDependencies(fileIncludes);
        System.out.println("Folder collection and dependency analysis: " + (System.currentTimeMillis()-startTime1) + "ms");

        buildSpec.setConfigFileLocation(getProject().getLocation().toOSString()+"/configuser.vc"); //FIXME not here, not hardcoded!
        
        if (generateMakemakefile) {
            //XXX this should probably become body of an Action
            Map<IContainer, String> targetNames = MakefileTools.generateTargetNames(folders);
            String makeMakeFile = MakefileTools.generateMakeMakeFile(folders, folderDeps, targetNames);
            IFile file = rootContainer.getProject().getFile("Makemakefile");
            MakefileTools.ensureFileContent(file, makeMakeFile.getBytes(), monitor);
        }

        // generate Makefiles in all folders
        long startTime = System.currentTimeMillis();
        monitor.subTask("Updating makefiles...");
        boolean changed = false;
        for (IContainer folder : folders) {
            try {
                //System.out.println("Generating makefile in: " + folder.getFullPath());
                List<String> args = new ArrayList<String>();
                args.add("-r");  //FIXME rather: explicit subfolder list
                args.add("-n"); //FIXME from folderType
                args.add("-c");
                args.add(buildSpec.getConfigFileLocation());
                if (folderDeps.containsKey(folder))
                    for (IContainer dep : folderDeps.get(folder))
                        args.add("-I" + dep.getLocation().toString());  //FIXME what if contains a space?
                changed |= new MakeMake().run(folder.getLocation().toFile(), args.toArray(new String[]{}), perFileDeps); 
            }
            catch (IOException e) {
                e.printStackTrace(); //FIXME more sophisticated: propagate up?
            }
        }
        System.out.println("Generated " + folders.length + " makefiles in: " + (System.currentTimeMillis()-startTime) + "ms");
        
        // refresh workspace
        if (changed) {
            monitor.subTask("Updating project..."); //XXX needed? CDT's MakeBuilder will do it anyway
            try {
                getProject().refreshLocal(IResource.DEPTH_INFINITE, null);
            } catch (CoreException e) {
                Activator.logError(e);
            }
        }
        
    }

    private void processDelta(IResourceDelta delta) throws CoreException {
        delta.accept(new IResourceDeltaVisitor() {
            public boolean visit(IResourceDelta delta) throws CoreException {
                IResource resource = delta.getResource();
                switch (delta.getKind()) {
                    case IResourceDelta.ADDED:
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==FolderType.GENERATED_MAKEFILE) 
                            processFileIncludes((IFile)resource);
                        break;
                    case IResourceDelta.REMOVED: 
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==FolderType.GENERATED_MAKEFILE) 
                            fileIncludes.remove(resource);
                        break;
                    case IResourceDelta.CHANGED:
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==FolderType.GENERATED_MAKEFILE) 
                            processFileIncludes((IFile)resource);
                        break;
                }
                return true; // continue visiting children
            }
        });
    }

    protected FolderType getFolderType(IContainer folder) {
        // inherit folderType from parent folder
        while (buildSpec.getFolderInfo(folder) == null && !(folder.getParent() instanceof IWorkspaceRoot))
            folder = folder.getParent();
        FolderInfo folderInfo = buildSpec.getFolderInfo(folder);
        return folderInfo==null ? FolderType.GENERATED_MAKEFILE : folderInfo.folderType;
    }
    
    /**
     * Parses the file for the list of #includes, and returns true if it changed 
     * since the previous state.
     */
    protected boolean processFileIncludes(IFile file) throws CoreException {
        List<Include> includes;
        try {
            includes = MakefileTools.parseIncludes(file);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        if (!includes.equals(fileIncludes.get(file))) {
            fileIncludes.put(file, includes);
            return true;
        }
        return false;
    }
}

package org.omnetpp.cdt.makefile;

import java.io.File;
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
import org.omnetpp.cdt.makefile.MakefileTools.Include;

/**
 * Keeps makefiles up to date
 * 
 * @author Andras
 */
public class MakefileBuilder extends IncrementalProjectBuilder {
    public static final String BUILDER_ID = "org.omnetpp.cdt.sampleBuilder";

    private BuildSpecification buildSpec = new BuildSpecification();
    private Map<IFile,List<Include>> fileIncludes = new HashMap<IFile, List<Include>>();

    protected IProject[] build(int kind, Map args, IProgressMonitor monitor) throws CoreException {
        if (kind == FULL_BUILD) {
            fullBuild(monitor);
        } else {
            IResourceDelta delta = getDelta(getProject());
            if (delta == null) {
                fullBuild(monitor);
            } else {
                incrementalBuild(delta, monitor);
            }
        }
        return null;
    }

    protected void fullBuild(final IProgressMonitor monitor) throws CoreException {
        fileIncludes.clear();
        getProject().accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==BuildSpecification.GENERATED_MAKEFILE)
                    processFileIncludes((IFile)resource);
                return true;  //FIXME skip cvs/svn folder, "dot" folders, folders where no makefile is generated
            }
        });
        
        //TODO:
        // get folderlist
        // generate makemakefile
        // invoke makemakefile with target "all"
        // invoke make depend ?
        IProject rootContainer = getProject();
        IContainer[] folders = MakefileTools.collectFolders(rootContainer);
        Map<IFile, List<Include>> fileIncludes = MakefileTools.processFilesIn(folders, monitor);
        Map<IContainer,Set<IContainer>> deps = MakefileTools.calculateDependencies(fileIncludes);
        // dumpDeps(deps);

        Map<IContainer, String> targetNames = MakefileTools.generateTargetNames(folders);
        String makeMakeFile = MakefileTools.generateMakeMakeFile(folders, deps, targetNames);
        System.out.println("\n\n" + makeMakeFile);

        IFile file = rootContainer.getProject().getFile("Makemakefile");
        MakefileTools.ensureFileContent(file, makeMakeFile.getBytes(), monitor);
        //...
        invokeMakemake();

    }

    protected void incrementalBuild(IResourceDelta delta, IProgressMonitor monitor) throws CoreException {
        List<IContainer> changedFolders = new ArrayList<IContainer>();
        delta.accept(new IResourceDeltaVisitor() {
            public boolean visit(IResourceDelta delta) throws CoreException {
                IResource resource = delta.getResource();
                switch (delta.getKind()) {
                    case IResourceDelta.ADDED:
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==BuildSpecification.GENERATED_MAKEFILE)
                            processFileIncludes((IFile)resource);
                        break;
                    case IResourceDelta.REMOVED: 
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==BuildSpecification.GENERATED_MAKEFILE)
                            fileIncludes.remove(resource);
                        break;
                    case IResourceDelta.CHANGED:
                        if (MakefileTools.isCppFile(resource) && getFolderType(resource.getParent())==BuildSpecification.GENERATED_MAKEFILE)
                            processFileIncludes((IFile)resource);
                        break;
                }
                //return true to continue visiting children.
                return true;
            }
        });

        //TODO:
        // get folderlist
        // generate makemakefile
        // invoke makemakefile with list of changed folders!
        // invoke make depend ?
        IProject rootContainer = getProject();
        IContainer[] folders = MakefileTools.collectFolders(rootContainer);
        Map<IFile, List<Include>> fileIncludes = MakefileTools.processFilesIn(folders, monitor);
        Map<IContainer,Set<IContainer>> deps = MakefileTools.calculateDependencies(fileIncludes);
        // dumpDeps(deps);

        Map<IContainer, String> targetNames = MakefileTools.generateTargetNames(folders);
        String makeMakeFile = MakefileTools.generateMakeMakeFile(folders, deps, targetNames);
        System.out.println("\n\n" + makeMakeFile);

        IFile file = rootContainer.getProject().getFile("Makemakefile");
        MakefileTools.ensureFileContent(file, makeMakeFile.getBytes(), monitor);
        
        invokeMakemake();
    }

    private void invokeMakemake() throws CoreException {
        String command = "makemake.cmd";
        String envp[] = null;
        File dir = getProject().getLocation().toFile();
        try {
            Process process = Runtime.getRuntime().exec(command, envp, dir);
            int exitCode = process.waitFor();
            if (exitCode != 0)
                throw new RuntimeException("exit code: " + exitCode);
        }
        catch (InterruptedException e) {
            throw Activator.wrap(e);
        }
        catch (IOException e) {
            throw Activator.wrap(e);
        }
    }

    protected int getFolderType(IContainer folder) {
        // inherit folderType from parent folder
        while (buildSpec.getFolderInfo(folder) == null && !(folder.getParent() instanceof IWorkspaceRoot))
            folder = folder.getParent();
        FolderInfo folderInfo = buildSpec.getFolderInfo(folder);
        return folderInfo==null ? BuildSpecification.GENERATED_MAKEFILE : folderInfo.folderType;
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

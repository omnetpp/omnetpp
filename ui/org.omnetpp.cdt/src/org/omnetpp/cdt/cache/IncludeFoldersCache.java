/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.cdt.cache;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.cdt.core.model.CoreModel;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICSourceEntry;
import org.eclipse.cdt.core.settings.model.util.CDataUtil;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.cdt.Activator;
import org.omnetpp.cdt.CDTUtils;
import org.omnetpp.cdt.build.BuildSpecification;
import org.omnetpp.cdt.build.MakefileTools;
import org.omnetpp.cdt.build.MakemakeOptions;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.StringUtils;


/**
 * Keeps track of per-project include folders (basically all folders under source entries).
 * Its responsibility is to tell CDT when the include paths have changed.
 * 
 * @author Andras
 */
//TODO: should reread everything on project clean
public class IncludeFoldersCache implements IResourceChangeListener {
    private Map<IProject, List<IContainer>> projectIncludeFolders = new HashMap<IProject, List<IContainer>>();

    private List<IProject> todoList = new ArrayList<IProject>();
    private Runnable asyncExecJob = null; 

    public IncludeFoldersCache() {
    }
    
    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }

    public List<IContainer> getProjectIncludeFolders(IProject project) {
        // Note: we MUST NOT compute the folders here, because this method is called
        // from our ScannerInfoCollector classes that must not access the CDT project
        // configuration, see bug #299
        List<IContainer> result = projectIncludeFolders.get(project);
        
        if (result != null)
            return result;
        else {
            // if we don't have anything at the moment, schedule it for later
            schedule(project);
            return new ArrayList<IContainer>(); // nothing available yet
        }
    }

    protected synchronized void schedule(IProject project) {
        todoList.add(project);
        if (asyncExecJob == null) {
            asyncExecJob = new Runnable() {
                public void run() {
                    try {
                        for (IProject project : todoList)
                            projectChanged(project);
                    }
                    catch (CoreException e) {
                        Activator.logError(e);
                    }
                    finally {
                        todoList.clear();
                        asyncExecJob = null;
                    }
                }
            };
            Display.getDefault().asyncExec(asyncExecJob);
        }
    }

    /**
     * Reread include paths when folder gets created or removed
     */
    public synchronized void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null || event.getType() != IResourceChangeEvent.POST_CHANGE)
                return;

            // collect projects in which a folder has been added or deleted
            final Set<IProject> changedProjects = new HashSet<IProject>();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    if (resource instanceof IContainer) {
                        int kind = delta.getKind();
                        if (kind==IResourceDelta.ADDED || kind==IResourceDelta.REMOVED)
                            changedProjects.add(resource.getProject());
                    }
                    return true;
                }
            });

            // and invalidate discovered info for that project
            for (IProject project : changedProjects)
                projectChanged(project);
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    protected synchronized void projectChanged(IProject project) throws CoreException {
        // compute current state
        List<IContainer> folders = computeIncludeFoldersForProject(project);
        
        // and if changed, store it and tell CDT to pick it up
        List<IContainer> oldFolders = projectIncludeFolders.get(project);
        if (!folders.equals(oldFolders)) {
            projectIncludeFolders.put(project, folders);
            CDTUtils.invalidateDiscoveredPathInfo(project); // and ScannerInfoCollector classes will get it from us
        }
    }
    
    /**
     * Returns the include folders inside the project and all referenced projects.
     * This is currently all folders under the source entries, minus those covered
     * by the output paths (out/ dirs)
     */
    protected List<IContainer> computeIncludeFoldersForProject(IProject project) throws CoreException {

        List<IContainer> outFolders = new ArrayList<IContainer>(); 
        BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project);
        if (buildSpec != null) { 
            for(IContainer makemakeFolder : buildSpec.getMakemakeFolders()) {
                MakemakeOptions mo = buildSpec.getMakemakeOptions(makemakeFolder);
                String outRoot = StringUtils.defaultIfEmpty(mo.outRoot, "out");
                IResource outDir = project.findMember(outRoot);
                if (outDir != null && outDir instanceof IContainer)
                    outFolders.add((IContainer) outDir);
            }
        }

        List<IContainer> result = new ArrayList<IContainer>();

        // add project source directories as include dirs for the indexer
        // Note: "*.h" pattern is not good because of includes of the form "subdir/file.h"
        // (i.e. "subdir" might need to be added to the include path too, even if it doesn't contain any header)
        for (IContainer folder : collectDirs(project)) {
            boolean dirOk = true;
            for (IContainer out : outFolders)
                if (out.getFullPath().isPrefixOf(folder.getFullPath()))
                    dirOk = false;
            // do not add as an include dir if it is under one of the out directories
            if (dirOk)
                result.add(folder);
        }
        
        return result;
    }
    
    /**
     * Collects source directories from the project and all dependent projects. Nonexistent,
     * closed and non-CDT dependent projects will be ignored.
     */
    protected List<IContainer> collectDirs(IProject project) throws CoreException {
        List<IContainer> result = new ArrayList<IContainer>();

        // collect dirs from this project
        collectDirs(project, result);

        // collect directories from referenced projects too
        IProject[] referencedProjects = ProjectUtils.getAllReferencedProjects(project);
        for (IProject refProj : referencedProjects)
            collectDirs(refProj, result);

        return result;
    }

    protected void collectDirs(IProject project, final List<IContainer> result) throws CoreException {
        ICProjectDescription projDesc = CoreModel.getDefault().getProjectDescription(project);
        if (projDesc == null)
            return; // likely project closed
        
        final ICSourceEntry[] srcEntries = CDataUtil.makeRelative(project, projDesc.getActiveConfiguration().getSourceEntries());

        for (ICSourceEntry srcEntry : srcEntries) {
            IResource sourceFolder = project.findMember(srcEntry.getFullPath());
            if (sourceFolder != null) {
                sourceFolder.accept(new IResourceVisitor() {
                    public boolean visit(IResource resource) throws CoreException {
                        if (MakefileTools.isGoodFolder(resource)) {
                            if (!CDTUtils.isExcluded(resource, srcEntries))
                                result.add((IContainer)resource);
                            return true;
                        }
                        return false;
                    }
                });
            }
        }
    }
        
}

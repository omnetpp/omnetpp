/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
import org.eclipse.cdt.core.settings.model.CProjectDescriptionEvent;
import org.eclipse.cdt.core.settings.model.ICProjectDescription;
import org.eclipse.cdt.core.settings.model.ICProjectDescriptionListener;
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
 * Keeps track of the include folders we contribute to CDT.
 * per-project include folders (basically all folders under source entries).
 * Its responsibility is to tell CDT when the include paths have changed.
 *
 * @author Andras
 */
public class IncludeFoldersCache {
    // Implementation note: we must be prepared for access from multiple threads, and also
    // take care not to cause deadlock with CDT's configuration access code. Therefore, all
    // access to member variables are synchronized, and we take care not to access CDT while
    // in a synchronized block.

    // deep-include folders for each project
    private Map<IProject, List<IContainer>> projectDeepIncludeFolders = new HashMap<IProject, List<IContainer>>();

    // if getProjectIncludeFolders() is called when we don't have the info yet, we schedule an asyncExec()
    // job to collect it, and in the job we force CDT to ask us again by invalidating CDT scanner info.
    private List<IProject> todoList = new ArrayList<IProject>();
    private Runnable asyncExecJob = null;  // also protected by todoList as lock

    private IResourceChangeListener resourceChangeListener = new IResourceChangeListener() {
        public void resourceChanged(IResourceChangeEvent event) {
            IncludeFoldersCache.this.resourceChanged(event);
        }
    };
    private ICProjectDescriptionListener projectDescriptionListener = new ICProjectDescriptionListener() {
        public void handleEvent(CProjectDescriptionEvent event) {
            IncludeFoldersCache.this.projectDescriptionChanged(event);
        }
    };

    public IncludeFoldersCache() {
    }

    public void hookListeners() {
        ResourcesPlugin.getWorkspace().addResourceChangeListener(resourceChangeListener);
        CoreModel.getDefault().addCProjectDescriptionListener(projectDescriptionListener, CProjectDescriptionEvent.APPLIED);
    }

    public void unhookListeners() {
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(resourceChangeListener);
        CoreModel.getDefault().removeCProjectDescriptionListener(projectDescriptionListener);
    }

    public List<IContainer> getProjectDeepIncludeFolders(IProject project) {
        // Note: we MUST NOT compute the folders here, because this method is called
        // from our ScannerInfoCollector classes that must not access the CDT project
        // configuration, see bug #299
        List<IContainer> result;
        synchronized (projectDeepIncludeFolders) {
            //FIXME TODO: IProject[] referencedProjects = ProjectUtils.getAllReferencedProjects(project);
            result = projectDeepIncludeFolders.get(project);
        }

        if (result != null)
            return result;
        else {
            // if we don't have anything at the moment, schedule it for later
            schedule(project);
            return new ArrayList<IContainer>();
        }
    }

    protected void schedule(IProject project) {
        Runnable asyncRunnable = new Runnable() {
            public void run() {
                IProject[] projects;
                synchronized (todoList) {
                    projects = todoList.toArray(new IProject[]{});
                    todoList.clear();
                    asyncExecJob = null;
                }
                for (IProject project : projects) {
                    try {
                        projectChanged(project);
                    }
                    catch (CoreException e) {
                        Activator.logError(e);
                    }
                }
            }
        };

        synchronized (todoList) {
            todoList.add(project);
            if (asyncExecJob == null) {
                asyncExecJob = asyncRunnable;
                Display.getDefault().asyncExec(asyncExecJob);
            }
        }
    }

    /**
     * Reread include paths when folder gets created or removed
     */
    protected void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null || event.getType() != IResourceChangeEvent.POST_CHANGE)
                return;

            //FIXME rescan also if BuildSpecification has changed!!!!

            // collect projects in which a folder has been added or deleted
            final Set<IProject> changedProjects = new HashSet<IProject>();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    if (resource instanceof IProject && !ProjectUtils.hasOmnetppNature((IProject)resource))
                        return false;  // skip project
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

    protected void projectDescriptionChanged(CProjectDescriptionEvent event) {
        try {
            projectChanged(event.getProject());
        }
        catch (CoreException e) {
            Activator.logError(e);
        }
    }

    protected void projectChanged(IProject project) throws CoreException {
        rescanProject(project);
    }

    public void clean(IProject project) throws CoreException {
        rescanProject(project);
    }

    protected void rescanProject(IProject project) throws CoreException {
        if (!project.isAccessible()) {
            projectDeepIncludeFolders.remove(project);
            return;
        }

        // compute current state
        List<IContainer> folders = collectDeepIncludeFolders(project);

        // and if changed, store it and tell CDT to pick it up
        boolean changed = false;
        synchronized (projectDeepIncludeFolders) {
            List<IContainer> oldFolders = projectDeepIncludeFolders.get(project);
            if (!folders.equals(oldFolders)) {
                projectDeepIncludeFolders.put(project, folders);
                changed = true;
            }
        }

        if (changed)
            CDTUtils.invalidateDiscoveredPathInfo(project); // and ScannerInfoCollector classes will get it from us
    }

    /**
     * Returns all deep-include folders inside the project
     */
    protected List<IContainer> collectDeepIncludeFolders(IProject project) throws CoreException {
        List<IContainer> result = new ArrayList<IContainer>();
        BuildSpecification buildSpec = BuildSpecification.readBuildSpecFile(project);
        if (buildSpec != null) {
            List<IContainer> makeFolders = buildSpec.getMakeFolders();
            List<IContainer> makemakeFolders = buildSpec.getMakemakeFolders();

            // collect output folders, as they'll need to be excluded from the result
            List<IContainer> outFolders = new ArrayList<IContainer>();
            for (IContainer makemakeFolder : makemakeFolders) {
                MakemakeOptions mo = buildSpec.getMakemakeOptions(makemakeFolder);
                String outRoot = StringUtils.defaultIfEmpty(mo.outRoot, "out");
                IResource outDir = project.findMember(outRoot);
                if (outDir != null && outDir instanceof IContainer && !outFolders.contains(outDir))
                    outFolders.add((IContainer) outDir);
            }

            // collect include folders
            for (IContainer makemakeFolder : makemakeFolders) {
                MakemakeOptions options = buildSpec.getMakemakeOptions(makemakeFolder);
                if (options.isDeep && !options.noDeepIncludes)
                    collectDeepIncludeFolders(makemakeFolder, result, makeFolders, outFolders);
            }
        }
        return result;
    }

    protected void collectDeepIncludeFolders(final IContainer makemakeFolder, final List<IContainer> result,
            final List<IContainer> makeFolders, final List<IContainer> outFolders) throws CoreException {
        IProject project = makemakeFolder.getProject();
        ICProjectDescription projDesc = CoreModel.getDefault().getProjectDescription(project);
        if (projDesc == null)
            return; // likely project closed

        final ICSourceEntry[] srcEntries = CDataUtil.makeRelative(project, projDesc.getActiveConfiguration().getSourceEntries());

        if (isUnderOneOfFolders(makemakeFolder, outFolders))
            return;  // whole folder is under an "out" folder (misconfiguration?)

        makemakeFolder.accept(new IResourceVisitor() {
            public boolean visit(IResource resource) throws CoreException {
                if (!MakefileTools.isGoodFolder(resource))
                    return false;  // exclude team-private, etc. folders
                IContainer container = (IContainer)resource;
                if (outFolders.contains(container))
                    return false; // don't descend into an "out" folder
                if (!container.equals(makemakeFolder) && makeFolders.contains(container))
                    return false; // don't descend into another makefile's territory
                if (!CDTUtils.isExcluded(container, srcEntries))
                    result.add(container);
                return true;
            }
        });
    }

    protected boolean isUnderOneOfFolders(IContainer folder, List<IContainer> folders) {
        for (IContainer f : folders)
            if (f.getFullPath().isPrefixOf(folder.getFullPath()))
                return true;
        return false;
    }
}

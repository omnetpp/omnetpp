/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.internal.events.ResourceDelta;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.resources.WorkspaceJob;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.NedSourceFoldersConfiguration;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.NedTreeDifferenceUtils;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.NedElementFactoryEx;
import org.omnetpp.ned.model.ex.NedFileElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeElement;
import org.omnetpp.ned.model.interfaces.INedTypeResolver;
import org.omnetpp.ned.model.notification.INedChangeListener;
import org.omnetpp.ned.model.notification.NedBeginModelChangeEvent;
import org.omnetpp.ned.model.notification.NedChangeListenerList;
import org.omnetpp.ned.model.notification.NedEndModelChangeEvent;
import org.omnetpp.ned.model.notification.NedFileRemovedEvent;
import org.omnetpp.ned.model.notification.NedModelChangeEvent;
import org.omnetpp.ned.model.notification.NedModelEvent;
import org.omnetpp.ned.model.notification.NedStructuralChangeEvent;

/**
 * Parses all NED files in the workspace and makes them available for other
 * plugins for consistency checks among NED files etc.
 *
 * It listens to workspace resource changes and modifies its content based on
 * change notifications.
 *
 * @author andras
 */
@SuppressWarnings("restriction")
public class NedResources extends NedTypeResolver implements INedResources, IResourceChangeListener {
    // singleton instance
    private static NedResources instance = null;

    // list of objects that listen on *all* NED changes
    private NedChangeListenerList nedModelChangeListenerList = null;

    // number of the editors connected to a given NED file
    private final Map<IFile, Integer> connectCount = new HashMap<IFile, Integer>();

    // if tables need to be rebuilt
    // DO NOT SET THIS DIRECTLY! Use invalidate().
    private boolean needsRehash = false;

    // For debugging: We increment this counter whenever a rehash occurs. Checks can be made
    // to assert that the function is not called unnecessarily
    private int debugRehashCounter = 0;

    private boolean nedModelChangeNotificationDisabled = false;
    private boolean refactoringInProgress = false;

    // can be used for computations in a background thread without locking NedResources
    private ImmutableNedTypeResolver immutableCopy = null;

    // caches the result of expression parsing
    private Map<String,INedElement> expressionCache = new HashMap<String, INedElement>();
    private static final INedElement BOGUS_EXPRESSION = NedElementFactoryEx.getInstance().createElement(INedElement.NED_UNKNOWN); // special value to signal syntax error

    // whether a job to call readMissingNedFiles() has been scheduled
    private boolean isLoadingInProgress = false;
    private WorkspaceJob readMissingNedFilesJob = null;

    // job that performs NED validation in the background
    private Job nedValidationJob;

    // a delayed job that initiates NED validation when the user idles a little
    private DelayedJob nedValidationStarterJob = new DelayedJob(400) {
        public void run() {
            if (nedValidationJob == null)
                nedValidationJob =  new NedValidationJob();
            if (nedValidationJob.getState() == Job.NONE)
                nedValidationJob.schedule();
        }
    };

    // listener, so that we don't need to make our nedModelChanged() method public
    private INedChangeListener nedModelChangeListener = new INedChangeListener() {
        public void modelChanged(NedModelEvent event) {
            nedModelChanged(event);
        }
    };

    /**
     * Constructor.
     */
    protected NedResources() {
        NedElement.setDefaultNedTypeResolver(this);
        createBuiltInNedTypes();

        // build the project table on init
        rebuildProjectsTable();

        // register as a workspace listener
        ResourcesPlugin.getWorkspace().addResourceChangeListener(this);
    }

    public void dispose() {
        // remove ourselves from the listener list
        ResourcesPlugin.getWorkspace().removeResourceChangeListener(this);
    }

    public static NedResources getInstance() {
        if (instance == null)
            instance = new NedResources();
        return instance;
    }

    public boolean isRefactoringInProgress() {
        return refactoringInProgress;
    }

    public void setRefactoringInProgress(boolean refactoringInProgress) {
        this.refactoringInProgress = refactoringInProgress;
    }

    public synchronized void setNedFileText(IFile file, String text) {
        NedFileElementEx currentTree = getNedFileElement(file);

        // parse
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer(NEDSYNTAXPROBLEM_MARKERID);
        markerSync.register(file);
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(markerSync);
        errorStore.setFile(file);
        INedElement targetTree = NedTreeUtil.parseNedText(text, errorStore, file.getFullPath().toString(), this);

        if (targetTree.getSyntaxProblemMaxCumulatedSeverity() == INedElement.SEVERITY_NONE) {
            long startTime = System.currentTimeMillis();

            NedTreeDifferenceUtils.Applier treeDifferenceApplier = new NedTreeDifferenceUtils.Applier();
            NedTreeDifferenceUtils.applyTreeDifferences(currentTree, targetTree, treeDifferenceApplier);

            if (treeDifferenceApplier.hasDifferences()) {
                // push tree differences into the official tree
//              Debug.println("pushing text editor changes into INedResources tree:\n  " + treeDifferenceApplier);
                currentTree.fireModelEvent(new NedBeginModelChangeEvent(currentTree));
                currentTree.setSyntaxProblemMaxLocalSeverity(INedElement.SEVERITY_NONE);
                treeDifferenceApplier.apply();
                currentTree.fireModelEvent(new NedEndModelChangeEvent(currentTree));

                // perform marker synchronization in a background job, to avoid deadlocks
                markerSync.runAsWorkspaceJob();

                // force rehash now, so that validation errors appear soon
                rehash();
            }

            long dt = System.currentTimeMillis() - startTime;
            if (dt > 100)
                Debug.println("setNedFileText(): textual changes applied as tree differences in " + dt + "ms");

        }
        else {
            // mark the tree as having a syntax error, so that the graphical doesn't allow editing
            currentTree.setSyntaxProblemMaxLocalSeverity(IMarker.SEVERITY_ERROR);

            // perform marker synchronization in a background job, to avoid deadlocks
            markerSync.runAsWorkspaceJob();
        }
    }

    public synchronized void connect(IFile file) {
        if (connectCount.containsKey(file))
            connectCount.put(file, connectCount.get(file) + 1);
        else {
            if (!nedFiles.containsKey(file))
                readNedFile(file);
            connectCount.put(file, 1);
        }
    }

    public synchronized void disconnect(IFile file) {
        Assert.isTrue(connectCount.containsKey(file));
        int count = connectCount.get(file); // must exist
        if (count <= 1) {
            // there's no open editor -- remove counter and re-read last saved
            // state from disk (provided it has not been deleted)
            connectCount.remove(file);
            if (file.exists())
                readNedFile(file);
        }
        else {
            connectCount.put(file, count - 1);
        }
    }

    public int getConnectCount(IFile file) {
        return connectCount.containsKey(file) ? connectCount.get(file) : 0;
    }

    public boolean hasConnectedEditor(IFile file) {
        return connectCount.containsKey(file);
    }

    /**
     * Reads the given NED file from the disk. May only be called if the file is not already open in an editor.
     */
    public synchronized void readNedFile(IFile file) {
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer();
        doReadNedFile(file, markerSync);
        markerSync.runAsWorkspaceJob();
        rehash();
    }

    /**
     * Internal: reads the given NED file from the disk.
     */
    protected synchronized void doReadNedFile(IFile file, ProblemMarkerSynchronizer markerSync) {
        Assert.isTrue(!hasConnectedEditor(file));
        //Note: the following is a bad idea, because of undefined startup order: the editor calling us might run sooner than readAllNedFiles()
        //Assert.isTrue(isNEDFile(file), "file is outside the NED source folders, or not a NED file at all");

        if (debug)
            Debug.println("reading from disk: " + file.toString());

        // parse the NED file and put it into the hash table
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(markerSync, NEDSYNTAXPROBLEM_MARKERID);
        errorStore.setFile(file);
        NedFileElementEx tree = NedTreeUtil.parseNedFile(file.getLocation().toOSString(), errorStore, file.getFullPath().toString(), this);
        Assert.isNotNull(tree);

        // only store the file if its declared package is not excluded -- that would lead to thrashing, see bug #518
        // (this can only happen to top package.ned files that *define* their own package, for other files
        // we won't get invoked in the first place)
        String filePackage = tree.getPackage();
        String[] excludedPackages = projects.get(file.getProject()).excludedPackageRoots;
        if (filePackage != null) {
            for (String excludedPackage : excludedPackages) {
                if (filePackage.equals(excludedPackage) || filePackage.startsWith(excludedPackage+".")) {
                    if (debug)
                        Debug.println("read " + file.toString() + " but won't store it, because its declared package " + filePackage + " is excluded");
                    return;
                }
            }
        }

        // store
        storeNedFileModel(file, tree);

        // if this is a package.ned, expected package names might have changed
        if (isSourceFolderPackageNedFile(file))
            rebuildProjectsTable();

        invalidate();
    }

    /**
     * Forget a NED file, and throws out all cached info.
     */
    public synchronized void forgetNedFile(IFile file) {
        ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
        doForgetNedFile(file, sync);
        sync.runAsWorkspaceJob();
    }

    protected synchronized void doForgetNedFile(IFile file, ProblemMarkerSynchronizer sync) {
        if (nedFiles.containsKey(file)) {
            // remove our model change listener from the file
            NedFileElementEx nedFileElement = nedFiles.get(file);
            nedFileElement.removeNedChangeListener(nedModelChangeListener);

            // unregister
            nedFiles.remove(file);
            nedElementFiles.remove(nedFileElement);
            invalidate();

            // if this was a package.ned, expected package names might have changed
            if (isSourceFolderPackageNedFile(file))
                rebuildProjectsTable();

            // remove all NED markers (otherwise they'll stay forever)
            sync.register(file);

            // fire notification.
            nedModelChanged(new NedFileRemovedEvent(file));
        }
    }

    // store NED file contents
    protected synchronized void storeNedFileModel(IFile file, NedFileElementEx tree) {
        Assert.isTrue(tree.getResolver() == this, "cannot use another resolver than the one that created the element");
        Assert.isTrue(!connectCount.containsKey(file), "cannot replace the tree while an editor is open");

        NedFileElementEx oldTree = nedFiles.get(file);
        // if the new tree has changed, we have to rehash everything
        if (oldTree == null || !NedTreeUtil.isNedTreeEqual(oldTree, tree)) {
            invalidate();
            nedFiles.put(file, tree);
            nedElementFiles.put(tree, file);
            // add ourselves to the tree root as a listener
            tree.addNedChangeListener(nedModelChangeListener);
            // remove ourselves from the old tree which is no longer used
            if (oldTree != null)
                oldTree.removeNedChangeListener(nedModelChangeListener);
            // fire a ned change notification (new tree added)
            nedModelChanged(new NedStructuralChangeEvent(tree, tree, NedStructuralChangeEvent.Type.INSERTION, tree, tree));
        }
    }

    /**
     * Rebuild hash tables after NED resource change. Note: some errors such as
     * duplicate names only get detected when this gets run!
     */
    public synchronized void rehashIfNeeded() {
        if (!needsRehash)
            return;

        needsRehash = false;
        debugRehashCounter++;

        internalRehash();

        // schedule a validation
        nedValidationStarterJob.restartTimer();
    }

    public synchronized void invalidate() {
        lastChangeSerial++;
        needsRehash = true;
        nedTypeLookupCache.clear();
        nedLikeTypeLookupCache.clear();

        // invalidate all inherited members on all typeInfo objects
        for (NedFileElementEx file : nedElementFiles.keySet())
            invalidateTypeInfo(file);
    }

    protected void rehash() {
        invalidate();
        rehashIfNeeded();
    }

    protected void invalidateTypeInfo(INedElement parent) {
        for (INedElement element : parent) {
            if (element instanceof INedTypeElement) {
                // invalidate
                ((INedTypeElement)element).getNedTypeInfo().invalidateInherited();

                // do inner types too
                if (element instanceof CompoundModuleElementEx) {
                    INedElement typesSection = ((CompoundModuleElementEx)element).getFirstTypesChild();
                    if (typesSection != null)
                        invalidateTypeInfo(typesSection);
                }
            }
        }
    }

    public synchronized void fireBeginChangeEvent() {
        nedModelChanged(new NedBeginModelChangeEvent(null));
    }

    public synchronized void fireEndChangeEvent() {
        nedModelChanged(new NedEndModelChangeEvent(null));
    }

    public synchronized void runWithBeginEndNotification(Runnable runnable) {
        fireBeginChangeEvent();
        try {
            runnable.run();
        } finally {
            fireEndChangeEvent();
        }
    }

    /**
     * To be called on project-level changes: project open/close, project description change
     * (i.e. nature & referred projects), ".nedfolders" file.
     *
     * Also needs to be invoked right on startup, to prevent race conditions with editors.
     * (When an editor starts, the projects table must already be up to date, otherwise
     * the editor's input file might not qualify as "NED file" and that'll cause an error).
     */
    public synchronized void rebuildProjectsTable() {
        // gather projects data
        Map<IProject,ProjectData> tmp = new HashMap<IProject, ProjectData>();
        try {
            IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();
            for (IProject project : omnetppProjects) {
                ProjectData projectData = new ProjectData();
                projectData.referencedProjects = ProjectUtils.getAllReferencedOmnetppProjects(project);
                NedSourceFoldersConfiguration config = ProjectUtils.readNedFoldersFile(project);
                projectData.nedSourceFolders = config.getSourceFolders();
                projectData.excludedPackageRoots = config.getExcludedPackages();
                projectData.nedSourceFolderPackages = new String[projectData.nedSourceFolders.length];
                for (int i=0; i<projectData.nedSourceFolders.length; i++) {
                    IFile packageNedFile = projectData.nedSourceFolders[i].getFile(new Path(PACKAGE_NED_FILENAME));
                    projectData.nedSourceFolderPackages[i] = containsNedFileElement(packageNedFile) ?
                            StringUtils.nullToEmpty(getNedFileElement(packageNedFile).getPackage()) : "";
                }
                tmp.put(project, projectData);
            }
        } catch (Exception e) {
            NedResourcesPlugin.logError("Failed to rebuild projects table", e);
            return;
        }

        // replace table with updated one
        projects.clear();
        projects.putAll(tmp);

        dumpProjectsTable();

        // forget those files which are no longer in our projects or NED folders
        // Note: use "trash" list to avoid ConcurrentModificationException in nedFiles
        List <IFile> trash = new ArrayList<IFile>();
        for (IFile file : nedFiles.keySet())
            if (!isNedFile(file))
                trash.add(file);
        try {
            ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            fireBeginChangeEvent();
            for (IFile file : trash)
                doForgetNedFile(file, sync);
            sync.runAsWorkspaceJob();
        } finally {
            fireEndChangeEvent();
        }

        // invalidate because project dependencies might have changed, even if there was no NED change
        invalidate();
        nedModelChanged(new NedModelChangeEvent(null));  // "anything might have changed"
        if (!isReadMissingNedFilesJobScheduled())
            scheduleReadMissingNedFilesJob();
    }

    public boolean isReadMissingNedFilesJobScheduled() {
        return readMissingNedFilesJob != null && readMissingNedFilesJob.getState() == Job.WAITING;
    }

    /**
     * Schedules a background job to read NED files that are not yet loaded.
     */
    public void scheduleReadMissingNedFilesJob() {
        isLoadingInProgress = true;
        readMissingNedFilesJob = new WorkspaceJob("Parsing NED files...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                try {
                    readMissingNedFiles();
                }
                finally {
                    isLoadingInProgress = false;
                    readMissingNedFilesJob = null;
                }
                return Status.OK_STATUS;
            }
        };
        readMissingNedFilesJob.setRule(ResourcesPlugin.getWorkspace().getRoot());
        readMissingNedFilesJob.setPriority(Job.SHORT);
        readMissingNedFilesJob.setSystem(false);
        readMissingNedFilesJob.schedule();
    }

    public boolean isLoadingInProgress() {
        return isLoadingInProgress;
    }

    /**
     * Reads NED files that are not yet loaded (not in our nedFiles table).
     * This should be run on startup and after rebuildProjectsTable();
     * individual file changes are handled by loadNedFile() calls from the
     * workspace listener.
     */
    public synchronized void readMissingNedFiles() {
        try {
            // disable all ned model notifications until all files have been processed
            nedModelChangeNotificationDisabled = true;
            debugRehashCounter = 0;

            // read NED files that are not yet loaded
            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
            workspaceRoot.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (!nedFiles.containsKey(resource) && isNedFile(resource))
                        doReadNedFile((IFile)resource, sync);
                    return true;
                }
            });
            sync.runAsWorkspaceJob();
            rehashIfNeeded();
        }
        catch (CoreException e) {
            NedResourcesPlugin.logError("Error during workspace refresh: ",e);
        } finally {
            nedModelChangeNotificationDisabled = false;
            Assert.isTrue(debugRehashCounter <= 1, "Too many rehash operations during readMissingNedFiles()");
            nedModelChanged(new NedModelChangeEvent(null));  // "everything changed"
        }
    }

    public synchronized INedTypeResolver getImmutableCopy() {
        if (immutableCopy == null)
            immutableCopy = new ImmutableNedTypeResolver(this);
        return immutableCopy;
    }

    public synchronized boolean isImmutableCopyUpToDate(INedTypeResolver copy) {
        return immutableCopy == copy;
    }

    public synchronized INedElement getParsedNedExpression(String expression) {
        INedElement tree = expressionCache.get(expression);
        if (tree != null)
            return tree==BOGUS_EXPRESSION ? null : tree;

        // parse, and cache the result (BOGUS_EXPRESSION is uses so we can distinguish "not cached"
        // from "syntax error" without an extra expressionCache.contains() call)
        tree = NedTreeUtil.parseNedExpression(expression);
        expressionCache.put(expression, tree==null ? BOGUS_EXPRESSION : tree);
        return tree;
    }

    // ******************* notification helpers ************************************

    public void addNedModelChangeListener(INedChangeListener listener) {
        if (nedModelChangeListenerList == null)
            nedModelChangeListenerList = new NedChangeListenerList();
        nedModelChangeListenerList.add(listener);
    }

    public void removeNedModelChangeListener(INedChangeListener listener) {
        if (nedModelChangeListenerList != null)
            nedModelChangeListenerList.remove(listener);
    }

    /**
     * Respond to model changes
     */
    protected void nedModelChanged(NedModelEvent event) {
        if (event instanceof NedModelChangeEvent)
            immutableCopy = null; // invalidate

        if (nedModelChangeNotificationDisabled)
            return;

        if (event instanceof NedModelChangeEvent) {
            INedElement source = ((NedModelChangeEvent)event).getSource();
            Assert.isTrue(source==null || refactoringInProgress || source instanceof NedFileElementEx || hasConnectedEditor(getNedFile(source.getContainingNedFileElement())), "NED trees not opened in any editor must NOT be changed");
            invalidate();
            nedValidationStarterJob.restartTimer(); //FIXME obey begin/end notifications too!
        }

        // notify generic listeners (like NedFileEditParts who refresh themselves
        // in response to this notification)
        // long startMillis = System.currentTimeMillis();

        if (nedModelChangeListenerList != null)
            nedModelChangeListenerList.fireModelChanged(event);

        // long dt = System.currentTimeMillis() - startMillis;
        // Debug.println("visual notification took " + dt + "ms");
    }

    /**
     * Synchronize the plugin with the resources in the workspace
     */
    public synchronized void resourceChanged(IResourceChangeEvent event) {
        try {
            if (event.getDelta() == null)
                return;
            //printResourceChangeEvent(event);
            final ProblemMarkerSynchronizer sync = new ProblemMarkerSynchronizer();
            event.getDelta().accept(new IResourceDeltaVisitor() {
                public boolean visit(IResourceDelta delta) throws CoreException {
                    IResource resource = delta.getResource();
                    // printDelta(delta);
                    if (isNedFile(resource)) {
                        IFile file = (IFile)resource;
                        switch (delta.getKind()) {
                        case IResourceDelta.REMOVED:
                            doForgetNedFile(file, sync); // includes rebuildProjectsTable() if needed
                            break;
                        case IResourceDelta.ADDED:
                            doReadNedFile(file, sync); // includes rebuildProjectsTable() if needed
                            break;
                        case IResourceDelta.CHANGED:
                            // we are only interested in content changes; ignore marker and property changes
                            if ((delta.getFlags() & IResourceDelta.CONTENT) != 0) {
                                if (!hasConnectedEditor(file))
                                    doReadNedFile(file, sync); // includes rebuildProjectsTable() if needed
                                else if (isSourceFolderPackageNedFile(file))
                                    rebuildProjectsTable(); // handle the case when a package.ned is modified and saved from within a NED Editor
                            }
                            break;
                        }
                    }
                    else if (ProjectUtils.isNedFoldersFile(resource)) {
                        rebuildProjectsTable();
                    }
                    else if (resource instanceof IProject) {
                        switch (delta.getKind()) {
                        case IResourceDelta.REMOVED:
                        case IResourceDelta.ADDED:
                            rebuildProjectsTable();
                            break;
                        case IResourceDelta.CHANGED:
                            // change in natures and referenced projects will be reported as description changes
                            if ((delta.getFlags() & (IResourceDelta.OPEN | IResourceDelta.DESCRIPTION)) != 0)
                                rebuildProjectsTable();
                            break;
                        }
                    }
                    return true;
                }
            });
            sync.runAsWorkspaceJob();
        }
        catch (CoreException e) {
            NedResourcesPlugin.logError("Error during workspace change notification: ", e);
        } finally {
            rehashIfNeeded();
        }

    }

    // Utility functions for debugging
    public static void printResourceChangeEvent(IResourceChangeEvent event) {
        Debug.println("event type: "+event.getType());
        printDelta(event.getDelta(), "  ");
    }

    public static void printDelta(IResourceDelta delta, String indent) {
        // LEGEND: [+] added, [-] removed, [*] changed, [>] and [<] phantom added/removed;
        // then: {CONTENT, MOVED_FROM, MOVED_TO, OPEN, TYPE, SYNC, MARKERS, REPLACED, DESCRIPTION, ENCODING}
        Debug.println(indent + ((ResourceDelta)delta).toDebugString());
        for (IResourceDelta child : delta.getAffectedChildren())
            printDelta(child, indent + "  ");
    }

}

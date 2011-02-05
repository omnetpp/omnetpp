/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

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
import org.eclipse.core.runtime.Status;
import org.eclipse.core.runtime.jobs.Job;
import org.omnetpp.common.Debug;
import org.omnetpp.common.markers.ProblemMarkerSynchronizer;
import org.omnetpp.common.project.ProjectUtils;
import org.omnetpp.common.util.DelayedJob;
import org.omnetpp.ned.model.INedElement;
import org.omnetpp.ned.model.NedElement;
import org.omnetpp.ned.model.NedTreeDifferenceUtils;
import org.omnetpp.ned.model.NedTreeUtil;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
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
//XXX is "element" argument to NedBeginModelChangeEvent useful...? we don't use it in editors/views
//XXX remove "source" from plain NedModelChangeEvent too (and turn "anything might have changed" event into a separate class)
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

    // delayed validation job
    private DelayedJob validationJob = new DelayedJob(400) {
        public void run() {
            new NedValidationJob().schedule();
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
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(file, markerSync);
        INedElement targetTree = NedTreeUtil.parseNedText(text, errorStore, file.getFullPath().toString(), this);

        if (targetTree.getSyntaxProblemMaxCumulatedSeverity() == INedElement.SEVERITY_NONE) {
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
     * May only be called if the file is not already open in an editor.
     */
    public synchronized void readNedFile(IFile file) {
        ProblemMarkerSynchronizer markerSync = new ProblemMarkerSynchronizer();
        readNedFile(file, markerSync);
        markerSync.runAsWorkspaceJob();
        rehash();
    }

    private synchronized void readNedFile(IFile file, ProblemMarkerSynchronizer markerSync) {
        Assert.isTrue(!hasConnectedEditor(file));
        //Note: the following is a bad idea, because of undefined startup order: the editor calling us might run sooner than readAllNedFiles()
        //Assert.isTrue(isNEDFile(file), "file is outside the NED source folders, or not a NED file at all");

        if (debug)
            Debug.println("reading from disk: " + file.toString());

        // parse the NED file and put it into the hash table
        NedMarkerErrorStore errorStore = new NedMarkerErrorStore(file, markerSync, NEDSYNTAXPROBLEM_MARKERID);
        NedFileElementEx tree = NedTreeUtil.parseNedFile(file.getLocation().toOSString(), errorStore, file.getFullPath().toString(), this);
        Assert.isNotNull(tree);

        storeNedFileModel(file, tree);
        invalidate();
    }

    /**
     * Forget a NED file, and throws out all cached info.
     */
    public synchronized void forgetNedFile(IFile file) {
        if (nedFiles.containsKey(file)) {
            // remove our model change listener from the file
            NedFileElementEx nedFileElement = nedFiles.get(file);
            nedFileElement.removeNedChangeListener(nedModelChangeListener);

            // unregister
            nedFiles.remove(file);
            nedElementFiles.remove(nedFileElement);
            invalidate();

            // fire notification.
            nedModelChanged(new NedFileRemovedEvent(file));
        }
    }

    // store NED file contents
    private synchronized void storeNedFileModel(IFile file, NedFileElementEx tree) {
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
        validationJob.restartTimer();
    }

    public synchronized void invalidate() {
        lastChangeSerial++;
        needsRehash = true;
        nedTypeLookupCache.clear();

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

    public void issueErrorsForDuplicates(ProblemMarkerSynchronizer markerSync) {
        // issue error message for duplicates
        for (IProject project : projects.keySet()) {
            ProjectData projectData = projects.get(project);
            for (String name : projectData.duplicates.keySet()) {
                List<INedTypeElement> duplicateList = projectData.duplicates.get(name);
                for (int i = 0; i < duplicateList.size(); i++) {
                    INedTypeElement element = duplicateList.get(i);
                    INedTypeElement otherElement = duplicateList.get(i==0 ? 1 : 0);
                    IFile file = getNedFile(element.getContainingNedFileElement());
                    IFile otherFile = getNedFile(otherElement.getContainingNedFileElement());

                    NedMarkerErrorStore errorStore = new NedMarkerErrorStore(file, markerSync);
                    if (otherFile == null) {
                        errorStore.addError(element, element.getReadableTagName() + " '" + name + "' is a built-in type and cannot be redefined");
                    }
                    else {
                        // add error message to both files
                        String messageHalf = element.getReadableTagName() + " '" + name + "' already defined in ";
                        errorStore.addError(element, messageHalf + otherFile.getFullPath().toString());
                        NedMarkerErrorStore otherErrorStore = new NedMarkerErrorStore(otherFile, markerSync);
                        otherErrorStore.addError(otherElement, messageHalf + file.getFullPath().toString());
                    }
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
        // rebuild table
        projects.clear();
        IProject[] omnetppProjects = ProjectUtils.getOmnetppProjects();
        for (IProject project : omnetppProjects) {
            try {
                ProjectData projectData = new ProjectData();
                projectData.referencedProjects = ProjectUtils.getAllReferencedOmnetppProjects(project);
                projectData.nedSourceFolders = ProjectUtils.readNedFoldersFile(project);
                projects.put(project, projectData);
            }
            catch (Exception e) {
                NedResourcesPlugin.logError(e); //XXX anything else? asyncExec errorDialog?
            }
        }
        dumpProjectsTable();

        // forget those files which are no longer in our projects or NED folders
        // Note: use "trash" list to avoid ConcurrentModificationException in nedFiles
        List <IFile> trash = new ArrayList<IFile>();
        for (IFile file : nedFiles.keySet())
            if (!isNedFile(file))
                trash.add(file);
        try {
            fireBeginChangeEvent();
            for (IFile file : trash)
                forgetNedFile(file);
        } finally {
            fireEndChangeEvent();
        }

        // invalidate because project dependencies might have changed, even if there was no NED change
        invalidate();
        nedModelChanged(new NedModelChangeEvent(null));  // "anything might have changed"
        scheduleReadMissingNedFiles();
    }

    /**
     * Schedules a background job to read NED files that are not yet loaded.
     */
    public void scheduleReadMissingNedFiles() {
        WorkspaceJob job = new WorkspaceJob("Parsing NED files...") {
            @Override
            public IStatus runInWorkspace(IProgressMonitor monitor) throws CoreException {
                readMissingNedFiles();
                return Status.OK_STATUS;
            }
        };
        job.setRule(ResourcesPlugin.getWorkspace().getRoot());
        job.setPriority(Job.SHORT);
        job.setSystem(false);
        job.schedule();
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
                        readNedFile((IFile)resource, sync);
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
            Assert.isTrue(debugRehashCounter <= 1, "Too many rehash operations during readAllNedFilesInWorkspace()");
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
        
        //Debug.println("**** nedModelChanged - notify");
        if (nedModelChangeNotificationDisabled)
            return;

        if (event instanceof NedModelChangeEvent) {
            INedElement source = ((NedModelChangeEvent)event).getSource();
            Assert.isTrue(source==null || refactoringInProgress || source instanceof NedFileElementEx || hasConnectedEditor(getNedFile(source.getContainingNedFileElement())), "NED trees not opened in any editor must NOT be changed");
            invalidate();
            validationJob.restartTimer(); //FIXME obey begin/end notifications too!
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
                            forgetNedFile(file);
                            break;
                        case IResourceDelta.ADDED:
                            readNedFile(file, sync);
                            break;
                        case IResourceDelta.CHANGED:
                            // we are only interested in content changes; ignore marker and property changes
                            if ((delta.getFlags() & IResourceDelta.CONTENT) != 0 && !hasConnectedEditor(file))
                                readNedFile(file, sync);
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
                        case IResourceDelta.OPEN:
                            rebuildProjectsTable();
                            break;
                        case IResourceDelta.CHANGED:
                            // change in natures and referenced projects will be reported as description changes
                            if ((delta.getFlags() & IResourceDelta.DESCRIPTION) != 0)
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
    }

    public static void printDelta(IResourceDelta delta) {
        // LEGEND: [+] added, [-] removed, [*] changed, [>] and [<] phantom added/removed;
        // then: {CONTENT, MOVED_FROM, MOVED_TO, OPEN, TYPE, SYNC, MARKERS, REPLACED, DESCRIPTION, ENCODING}
        Debug.println("  "+((ResourceDelta)delta).toDebugString());
    }

}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Callable;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IResourceDelta;
import org.eclipse.core.resources.IResourceDeltaVisitor;
import org.eclipse.core.resources.IResourceVisitor;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.content.IContentType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.edit.provider.INotifyChangedListener;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.ContentTypes;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IndexFile;
import org.omnetpp.scave.jobs.ResultFileManagerUpdaterJob;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;

/**
 * This class is responsible for loading/unloading result files
 * (scalar and vector files), based on changes in the model
 * (the Inputs object) and in the workspace. Thus it should be
 * hooked up to listen to EMF and workspace changes.
 *
 * @author andras, tomi
 */
public class ResultFilesTracker implements INotifyChangedListener, IResourceChangeListener {

    private static final boolean debug = true;

    private ResultFileManager manager; //backreference to the manager it operates on, the manager is owned by the editor
    private Inputs inputs; // backreference to the Inputs element we watch
    private IPath baseDir; // path of the directory which used to resolve relative paths as a base
    private ResultFileManagerUpdaterJob updaterJob;

    public ResultFilesTracker(ResultFileManager manager, Inputs inputs, IPath baseDir) {
        this.manager = manager;
        this.inputs = inputs;
        this.baseDir = baseDir;
        this.updaterJob = new ResultFileManagerUpdaterJob(manager);
    }

    public boolean deactivate()
    {
        manager = null;
        return updaterJob.cancel();
    }

    /**
     * Listen to EMF model changes.
     */
    public void notifyChanged(Notification notification) {
        if (manager == null)
            return;

        if (notification.isTouch())
            return;

        switch (notification.getEventType()) {
        case Notification.ADD:
        case Notification.ADD_MANY:
        case Notification.REMOVE:
        case Notification.REMOVE_MANY:
        //case Notification.MOVE:
        case Notification.SET:
        //case Notification.UNSET:
            Object notifier = notification.getNotifier();
            if (notifier instanceof Inputs || notifier instanceof InputFile)
                synchronize(false);
        }
    }

    /**
     * Listen to workspace changes. We want to keep our result files in
     * sync with the workspace. In addition to changes in file contents,
     * Inputs can have wildcard filters which may match different files
     * as files get created/deleted in the workspace.
     */
    public void resourceChanged(IResourceChangeEvent event) {
        if (manager == null)
            return;

        try {
            IResourceDelta delta = event.getDelta();
            if (delta != null)
                delta.accept(new ResourceDeltaVisitor());
        } catch (CoreException e) {
            ScavePlugin.logError("Could not refresh the result files", e);
        }
    }

    class ResourceDeltaVisitor implements IResourceDeltaVisitor {
        public boolean visit(IResourceDelta delta) throws CoreException {
            IResource resource = delta.getResource();
            if (!(resource instanceof IFile))
                return true;

            IFile file = (IFile)resource;
            IFile resultFile = IndexFile.isIndexFile(file) ? IndexFile.getVectorFileFor(file) :
                                isResultFile(file) ? file : null;

            if (resultFile == null || isDerived(resultFile) || !inputsMatches(resultFile))
                return false;

            switch (delta.getKind()) {
            case IResourceDelta.ADDED:
                    if (debug) Debug.format("File added: %s%n", file);
                    loadFile(resultFile, false);
                    break;
            case IResourceDelta.REMOVED:
                    if (debug) Debug.format("File removed: %s%n", file);
                    if (isResultFile(file) && !isDerived(file))
                        unloadFile(file, false);
                    break;
            case IResourceDelta.CHANGED:
                    if (debug) Debug.format("File changed: %s%n", file);
                    if ((delta.getFlags() & ~IResourceDelta.MARKERS) != 0)
                        loadFile(resultFile, false);
                    break;
            }
            return false;
        }
    }

    /**
     * Ensure that exactly the result files specified in the Inputs node are loaded.
     * Missing files get loaded, and extra files get unloaded.
     */
    public void synchronize(boolean sync) {
        if (manager == null)
            return;

        if (debug) Debug.println("ResultFileTracker.synchronize()");
        Set<String> loadedFiles =
        ResultFileManager.callWithReadLock(manager, new Callable<Set<String>>() {
            public Set<String> call() {
                Set<String> loadedFiles = new HashSet<String>();
                for (ResultFile file : manager.getFiles().toArray())
                    loadedFiles.add(file.getFilePath());
                return loadedFiles;
            }
        });

        Set<String> filesToBeLoaded = new HashSet<String>();
        List<InputFile> files = new ArrayList<InputFile>();
        List<InputFile> wildcards = new ArrayList<InputFile>();
        partitionInputFiles(files, wildcards);

        // load files specified by path
        for (InputFile inputfile : files) {
            filesToBeLoaded.add(inputfile.getName());
        }
        // load files matching with patterns
        if (wildcards.size() > 0) {
            List<IFile> allFiles = getResultFiles();
            for (IFile file : allFiles)
                for (InputFile wildcard : wildcards)
                    if (matchFile(file, wildcard))
                        filesToBeLoaded.add(file.getFullPath().toString());
        }

        Set<String> filesToBeUnloaded = new HashSet<String>(loadedFiles);
        filesToBeUnloaded.removeAll(filesToBeLoaded);
        filesToBeLoaded.removeAll(loadedFiles);

        for (String file : filesToBeUnloaded)
            unloadFile(file, sync);

        for (String file : filesToBeLoaded)
            loadFile(file, sync);
    }


    private void partitionInputFiles(List<InputFile> files, List<InputFile> wildcards) {
        for (InputFile inputfile : inputs.getInputs()) {
            if (containsWildcard(inputfile.getName()))
                wildcards.add(inputfile);
            else
                files.add(inputfile);
        }
    }

    private boolean containsWildcard(String fileSpec) {
        return fileSpec.indexOf('?') >= 0 || fileSpec.indexOf('*') >= 0;
    }

    /**
     * Returns the list of result files in the workspace.
     */
    private List<IFile> getResultFiles() {
        final List<IFile> files = new ArrayList<IFile>();
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        try {
            root.accept(new IResourceVisitor() {
                public boolean visit(IResource resource) {
                    if (resource instanceof IFile) {
                        IFile file = (IFile)resource;
                        if (isResultFile(file) && !isDerived(file)) {
                            files.add((IFile)resource);
                            return false;
                        }
                    }
                    return true;
                }
            });
        } catch (CoreException e) {
            ScavePlugin.logError(e);
        }
        return files;
    }

    /**
     * Loads the file specified by <code>resourcePath</code> into the ResultFileManager.
     */
    private void loadFile(String resourcePath, boolean sync) {
        IFile file = findResultFileInWorkspace(resourcePath);
        if (file != null)
            loadFile(file, sync);
    }

    /**
     * Loads the specified <code>file</code> into the ResultFileManager.
     * If a vector file is loaded, it checks that the index file is up-to-date.
     * When not, it generates the index first and then loads it from the index.
     */
    private void loadFile(final IFile file, boolean sync) {
        if (sync)
            updaterJob.doLoad(file);
        else
            updaterJob.load(file);
    }

    /**
     * Unloads the file specified  by <code>resourcePath</code> from the ResultFileManager.
     * Has no effect when the file was not loaded.
     */
    private void unloadFile(String resourcePath, boolean sync) {
        IFile file = findResultFileInWorkspace(resourcePath);
        if (file != null)
            unloadFile(file, sync);
    }

    /**
     * Unloads the specified <code>file</code> from the ResultFileManager.
     * Has no effect when the file was not loaded.
     */
    private void unloadFile(final IFile file, boolean sync) {
        if (sync)
            updaterJob.doUnload(file);
        else
            updaterJob.unload(file);
    }

    private IFile findResultFileInWorkspace(String resourcePath) {
        IPath path = new Path(resourcePath);
        if (!path.isAbsolute())
            path = baseDir.append(path);
        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
        IResource resource = workspaceRoot.findMember(path);
        if (resource instanceof IFile) {
            IFile file = (IFile)resource;
            if (isResultFile(file) && !isDerived(file))
                return file;
        }
        return null;
    }

    /**
     * Returns true iff <code>file</code> is a result file (scalar or vector).
     */
    public static boolean isResultFile(IFile file) {
        try {
            if (file.getContentDescription() != null) {
                IContentType contentType = file.getContentDescription().getContentType();
                return ContentTypes.SCALAR.equals(contentType.getId()) || ContentTypes.VECTOR.equals(contentType.getId());
            }
        } catch (CoreException e) {
            // the file might not exists, so content description is not available
            String extension = file.getFileExtension();
            return "sca".equals(extension) ||  "vec".equals(extension)/* || "vci".equals(extension)*/;
        }
        return false;
    }

    /**
     * Returns true if the resource or one of its parents is derived.
     *
     * @return
     */
    public static boolean isDerived(IResource resource) {
        while (!(resource instanceof IProject) && !resource.isDerived())
            resource = resource.getParent();
        return resource.isDerived();
    }

    /**
     * Return true iff the <code>file</code> matches any of the input files.
     */
    private boolean inputsMatches(IFile file) {
        for (InputFile inputfile : inputs.getInputs())
            if (matchFile(file, inputfile))
                return true;
        return false;
    }

    /**
     * Returns true if the <code>workspaceFile</code> matches with the file specification
     * <code>inputFile</code>.
     */
    private boolean matchFile(IFile workspaceFile, InputFile fileSpec) {
        if (fileSpec.getName() == null)
            return false;
        IPath filePatternPath = new Path(fileSpec.getName());
        if (!filePatternPath.isAbsolute()) {
            filePatternPath = baseDir.append(filePatternPath);
        }
        String filePath = workspaceFile.getFullPath().toString();
        String filePattern = filePatternPath.toString();
        return matchPattern(filePath, filePattern);
    }

    /**
     * Matches <code>str</code> against the given <code>pattern</code>.
     * The pattern may contain '?' (any char) and '*' (any char sequence) wildcards.
     */
    private static boolean matchPattern(String str, String pattern) {
        return matchPattern(str, 0, str.length(), pattern, 0, pattern.length());
    }

    /**
     * Matches the given string segment with the given pattern segment.
     */
    private static boolean matchPattern(String str, int strStart, int strEnd, String pattern, int patternStart, int patternEnd) {
        while (strStart < strEnd && patternStart < patternEnd) {
            char strChar = str.charAt(strStart);
            char patternChar = pattern.charAt(patternStart);

            switch (patternChar) {
            case '?':
                if (strChar == '/')
                    return false;
                ++strStart;
                ++patternStart;
                break;
            case '*':
                if (patternStart + 1 == patternEnd) // last char is '*'
                    return true;
                if (pattern.charAt(patternStart + 1) == strChar) { // try to match '*' with empty string
                    boolean matches = matchPattern(str, strStart, strEnd, pattern, patternStart + 1, patternEnd);
                    if (matches)
                        return true;
                }
                // match '*' with strChar
                if (strChar == '/')
                    return false;
                ++strStart;
                break;
            default:
                if (strChar == patternChar) {
                    ++strStart;
                    ++patternStart;
                }
                else
                    return false;
                break;
            }
        }

        return strStart == strEnd && patternStart == patternEnd;
    }
}

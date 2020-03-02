/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.content.IContentType;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.StringVector;
import org.omnetpp.scave.ContentTypes;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.jobs.ResultFileManagerUpdaterJob;
import org.omnetpp.scave.model.IModelChangeListener;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.ModelChangeEvent;
import org.omnetpp.scave.model2.ScaveModelUtil;

/**
 * This class is responsible for loading/unloading result files
 * (scalar and vector files), based on changes in the model
 * (the Inputs object) and in the workspace. Thus it should be
 * hooked up to listen to model and workspace changes.
 *
 * @author andras, tomi
 */
public class ResultFilesTracker implements IResourceChangeListener, IModelChangeListener {

    private static final boolean debug = true;

    private ResultFileManager manager; // backreference to the manager it operates on, the manager is owned by the editor
    private Inputs inputs; // backreference to the Inputs element we watch
    private IContainer anfFolder; // the project or folder to resolve relative paths as a base
    private ResultFileManagerUpdaterJob updaterJob;

    public ResultFilesTracker(ResultFileManager manager, Inputs inputs, IContainer anfFolder) {
        this.manager = manager;
        this.inputs = inputs;
        this.anfFolder = anfFolder;
        this.updaterJob = new ResultFileManagerUpdaterJob(manager);
    }

    public boolean deactivate()
    {
        manager = null;
        return updaterJob.cancel();
    }

    /**
     * Listen to model changes.
     */
    public void modelChanged(ModelChangeEvent event) {
        if (ScaveModelUtil.isInputsChange(event))
            synchronize(false);
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

        synchronize(false); //TODO only if input folders are affected in the change

//        try {
//            IResourceDelta delta = event.getDelta();
//            if (delta != null)
//                delta.accept(new ResourceDeltaVisitor());
//        } catch (CoreException e) {
//            ScavePlugin.logError("Could not refresh the result files", e);
//        }
    }

//    class ResourceDeltaVisitor implements IResourceDeltaVisitor {
//        public boolean visit(IResourceDelta delta) throws CoreException {
//            IResource resource = delta.getResource();
//            if (!(resource instanceof IFile))
//                return true;
//
//            IFile file = (IFile)resource;
//            IFile resultFile = IndexFileUtils.isIndexFile(file) ? IndexFileUtils.getVectorFileFor(file) :
//                                isResultFile(file) ? file : null;
//
//            if (resultFile == null || isDerived(resultFile) || !inputsMatches(resultFile))
//                return false;
//
//            switch (delta.getKind()) {
//            case IResourceDelta.ADDED:
//                    if (debug) Debug.format("File added: %s%n", file);
//                    loadFile(resultFile, false);
//                    break;
//            case IResourceDelta.REMOVED:
//                    if (debug) Debug.format("File removed: %s%n", file);
//                    if (isResultFile(file) && !isDerived(file))
//                        unloadFile(file, false);
//                    break;
//            case IResourceDelta.CHANGED:
//                    if (debug) Debug.format("File changed: %s%n", file);
//                    if ((delta.getFlags() & ~IResourceDelta.MARKERS) != 0)
//                        loadFile(resultFile, false);
//                    break;
//            }
//            return false;
//        }
//    }

    /**
     * Ensure that exactly the result files specified in the Inputs node are loaded.
     * Missing files get loaded, and extra files get unloaded.
     */
    public void synchronize(boolean sync) {
        if (manager == null)
            return;

        long start = System.currentTimeMillis();
        Map<String, String> files = new LinkedHashMap<>();
        for (InputFile input : inputs.getInputs())
            files.putAll(collectResultFiles(input.getName(), anfFolder));
        long end = System.currentTimeMillis();
        Debug.println("collecting files took: " + (end - start) + "ms");

        //TODO run this in a job if sync=false
        ResultFileManager.runWithWriteLock(manager, () -> {
            long start2 = System.currentTimeMillis();
            int loadFlags = ResultFileManagerEx.RELOAD | ResultFileManagerEx.ALLOW_INDEXING | ResultFileManagerEx.SKIP_IF_LOCKED;
            for (Entry<String,String> entry : files.entrySet()) {
                String filePath = entry.getKey();
                String fileLocation = entry.getValue();
                manager.loadFile(filePath, fileLocation, loadFlags, null);
            }
            long end2 = System.currentTimeMillis();
            System.out.println("(re)loading took: " + (end2-start2) + "ms");
        });

//TODO unload
//        Set<String> filesToBeUnloaded = new HashSet<String>(loadedFiles);
//        filesToBeUnloaded.removeAll(filesToBeLoaded);
//        filesToBeLoaded.removeAll(loadedFiles);
//
//        for (String file : filesToBeUnloaded)
//            unloadFile(file, sync);
//
//        for (String file : filesToBeLoaded)
//            loadFile(file, sync);
    }


protected Map<String, String> collectResultFiles(String input, IContainer anfFolder) {
    input = new Path(input).toString(); // canonicalize

    //TODO if no pattern: it can be either folder or file, must check!

    // find longest folder prefix that doesn't contain wildcards
    int lastGoodSlashPos = -1;
    for (int i = 0; i < input.length(); i++) {
        char c = input.charAt(i);
        if (c == '/')
            lastGoodSlashPos = i;
        else if (c == '*' || c == '?')
            break;
    }

    // split to folder+pattern parts
    IContainer folder = null;
    String pattern = null;

    if (lastGoodSlashPos == -1) {
        // relative, and non-wildcard folder part (e.g. "*.vec" or "**/*.vec")
        folder = anfFolder;
        pattern = input;
    }
    else if (lastGoodSlashPos == 0) {
        // absolute path where already the project name contains wildcards
        folder = ResourcesPlugin.getWorkspace().getRoot();  // TODO this assumes that all projects are under the workspace directory, which is not always true
        pattern = input.substring(1);
    }
    else {
        IPath folderPath = new Path(input.substring(0, lastGoodSlashPos));
        boolean isAbsolute = input.charAt(0) == '/';
        if (isAbsolute) {
            IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
            folder = folderPath.segmentCount() == 1 ? root.getProject(folderPath.lastSegment()) : root.getFolder(folderPath);
        }
        else
            folder = anfFolder.getFolder(folderPath);
        pattern = input.substring(lastGoodSlashPos+1);
    }

    String folderLocation = folder.getLocation().toString();

    // collect files
    Map<String,String> filesToLoad = new LinkedHashMap<>();  // workspacePath -> filesystemPath

    if (pattern.isEmpty()) {
        StringVector scaFiles = Common.collectFilesInDirectory(folderLocation, true, ".sca");
        long n = scaFiles.size();
        for (int i = 0; i < n; i++) {
            String fileLocation = scaFiles.get(i);
            String filePath = findInWorkspace(fileLocation, folder, folderLocation);
            filesToLoad.put(filePath, fileLocation);
        }

        StringVector vecFiles = Common.collectFilesInDirectory(folderLocation, true, ".vec");
        n = vecFiles.size();
        for (int i = 0; i < n; i++) {
            String fileLocation = vecFiles.get(i);
            String filePath = findInWorkspace(fileLocation, folder, folderLocation);
            filesToLoad.put(filePath, fileLocation);
        }
    }
    else {
        // we require wildcard patterns to match *files* (TODO we could fix that with some effort)
        StringVector matchingFiles = Common.collectMatchingFiles(folderLocation + "/" + pattern);
        long n = matchingFiles.size();
        for (int i = 0; i < n; i++) {
            String fileLocation = matchingFiles.get(i);
            String filePath = findInWorkspace(fileLocation, folder, folderLocation);
            filesToLoad.put(filePath, fileLocation);
        }

    }

    //TODO remove files that are already loaded but not present in the above list
    return filesToLoad;
}

protected String findInWorkspace(String fileLocation, IContainer containingFolder, String containingFolderLocation) {
    Assert.isTrue(fileLocation.startsWith(containingFolderLocation));
    return containingFolder.getFullPath().append(new Path(StringUtils.removeStart(fileLocation, containingFolderLocation))).toString();
}

//    private void partitionInputFiles(List<InputFile> files, List<InputFile> wildcards) {
//        for (InputFile inputfile : inputs.getInputs()) {
//            if (containsWildcard(inputfile.getName()))
//                wildcards.add(inputfile);
//            else
//                files.add(inputfile);
//        }
//    }

    private boolean containsWildcard(String fileSpec) {
        return fileSpec.indexOf('?') >= 0 || fileSpec.indexOf('*') >= 0;
    }

//    /**
//     * Returns the list of result files in the workspace.
//     */
//    private List<IFile> getResultFiles() {
//        final List<IFile> files = new ArrayList<IFile>();
//        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
//        try {
//            root.accept(new IResourceVisitor() {
//                public boolean visit(IResource resource) {
//                    if (resource instanceof IFile) {
//                        IFile file = (IFile)resource;
//                        if (isResultFile(file) && !isDerived(file)) {
//                            files.add((IFile)resource);
//                            return false;
//                        }
//                    }
//                    return true;
//                }
//            });
//        } catch (CoreException e) {
//            ScavePlugin.logError(e);
//        }
//        return files;
//    }

//    /**
//     * Loads the file specified by <code>resourcePath</code> into the ResultFileManager.
//     */
//    private void loadFile(String resourcePath, boolean sync) {
//        IFile file = findResultFileInWorkspace(resourcePath);
//        if (file != null)
//            loadFile(file, sync);
//        IFolder folder = findFolderInWorkspace(resourcePath);
//        if (folder!= null)
//            loadFolder(folder, sync);
//    }

//TODO
//    /**
//     * Loads the specified <code>file</code> into the ResultFileManager.
//     * If a vector file is loaded, it checks that the index file is up-to-date.
//     * When not, it generates the index first and then loads it from the index.
//     */
//    private void loadFile(final IFile file, boolean sync) {
//        if (sync)
//            updaterJob.doLoad(file);
//        else
//            updaterJob.load(file);
//    }
//
//    /**
//     * Unloads the file specified  by <code>resourcePath</code> from the ResultFileManager.
//     * Has no effect when the file was not loaded.
//     */
//    private void unloadFile(String resourcePath, boolean sync) {
//        IFile file = findResultFileInWorkspace(resourcePath);
//        if (file != null)
//            unloadFile(file, sync);
//    }

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

//    /**
//     * Loads the specified <code>folder</code> into the ResultFileManager.
//     * If a vector file is loaded, it checks that the index file is up-to-date.
//     * When not, it generates the index first and then loads it from the index.
//     */
//    private void loadFolder(final IFolder folder, boolean sync) {
//        if (sync)
//            updaterJob.doLoad(folder);
//        else
//            throw new RuntimeException("TODO");
//            // updaterJob.load(folder);
//    }

//    private IFile findResultFileInWorkspace(String resourcePath) {
//        IPath path = new Path(resourcePath);
//        if (!path.isAbsolute())
//            path = anfFolder.append(path);
//        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
//        IResource resource = workspaceRoot.findMember(path);
//        if (resource instanceof IFile) {
//            IFile file = (IFile)resource;
//            if (isResultFile(file) && !isDerived(file))
//                return file;
//        }
//        return null;
//    }
//
//    private IFolder findFolderInWorkspace(String resourcePath) {
//        IPath path = new Path(resourcePath);
//        if (!path.isAbsolute())
//            path = anfFolder.append(path);
//        IWorkspaceRoot workspaceRoot = ResourcesPlugin.getWorkspace().getRoot();
//        IResource resource = workspaceRoot.findMember(path);
//        if (resource instanceof IFolder)
//            return (IFolder)resource;
//        return null;
//    }

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

//    /**
//     * Return true iff the <code>file</code> matches any of the input files.
//     */
//    private boolean inputsMatches(IFile file) {
//        for (InputFile inputfile : inputs.getInputs())
//            if (matchFile(file, inputfile))
//                return true;
//        return false;
//    }
//
//    /**
//     * Returns true if the <code>workspaceFile</code> matches with the file specification
//     * <code>inputFile</code>.
//     */
//    private boolean matchFile(IFile workspaceFile, InputFile fileSpec) {
//        if (fileSpec.getName() == null)
//            return false;
//        IPath filePatternPath = new Path(fileSpec.getName());
//        if (!filePatternPath.isAbsolute()) {
//            filePatternPath = anfFolder.append(filePatternPath);
//        }
//        String filePath = workspaceFile.getFullPath().toString();
//        String filePattern = filePatternPath.toString();
//        return matchPattern(filePath, filePattern);
//    }
//

//TODO still used from InputsTree:

    /**
     * Matches <code>str</code> against the given <code>pattern</code>.
     * The pattern may contain '?' (any char) and '*' (any char sequence) wildcards.
     */
    public static boolean matchPattern(String str, String pattern) {
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

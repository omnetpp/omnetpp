/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IResourceChangeEvent;
import org.eclipse.core.resources.IResourceChangeListener;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.StringVector;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
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

        //TODO run this in a job if sync=false

        long start = System.currentTimeMillis();
        Map<String, Map<String, String>> files = new LinkedHashMap<>(); //TODO we could use a flat list of structs -- would be easier to understand
        for (InputFile input : inputs.getInputs())
            files.put(input.getName(), collectResultFiles(input.getName(), anfFolder));
        long end = System.currentTimeMillis();
        Debug.println("collecting files took: " + (end - start) + "ms");

        //TOO unloadFile() (invoked from RELOAD) is still very slow!

        ResultFileManager.runWithWriteLock(manager, () -> {
            long start2 = System.currentTimeMillis();
            int loadFlags = ResultFileManagerEx.RELOAD | ResultFileManagerEx.ALLOW_INDEXING | ResultFileManagerEx.SKIP_IF_LOCKED;
            for (String inputName : files.keySet()) {
                for (Entry<String,String> entry : files.get(inputName).entrySet()) {
                    String filePath = entry.getKey();
                    String fileLocation = entry.getValue();
                    manager.loadFile(filePath, fileLocation, inputName, loadFlags, null);
                }
            }
            long end2 = System.currentTimeMillis();
            Debug.println("loading took: " + (end2-start2) + "ms");
        });

        long startUnload = System.currentTimeMillis();

        // collect set of file names from 'files'
        Set<String> fileSet = new HashSet<>();
        for (Map<String,String> pathToLocation : files.values())
            fileSet.addAll(pathToLocation.keySet());

        // determine list of files to be unloaded
        ResultFileList loadFiles = ResultFileManager.callWithReadLock(manager, () -> { return manager.getFiles(); });
        List<ResultFile> filesToBeUnloaded = new ArrayList<>();
        for (int i = 0; i < loadFiles.size(); i++)
            if (!fileSet.contains(loadFiles.get(i).getFilePath()))
                filesToBeUnloaded.add(loadFiles.get(i));

        // unload
        if (!filesToBeUnloaded.isEmpty()) {
            ResultFileManager.runWithWriteLock(manager, () -> {
                for (ResultFile file : filesToBeUnloaded)
                    manager.unloadFile(file);
            });
        }
        long endUnload = System.currentTimeMillis();
        Debug.println("unloading " + filesToBeUnloaded.size() + " files took: " + (endUnload-startUnload) + "ms");
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
}

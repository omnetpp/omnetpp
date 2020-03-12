/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;
import java.util.stream.Collectors;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.ListenerList;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.SubMonitor;
import org.eclipse.jface.dialogs.ProgressMonitorDialog;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.Debug;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.StringVector;
import org.omnetpp.common.util.DisplayUtils;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engineext.IResultFilesChangeListener;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
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
public class ResultFilesTracker implements IModelChangeListener {

    private static final boolean debug = true;

    private ResultFileManager manager; // backreference to the manager it operates on, the manager is owned by the editor
    private Inputs inputs; // backreference to the Inputs element we watch
    private IContainer anfFolder; // the project or folder to resolve relative paths as a base
    private ListenerList<IResultFilesChangeListener> listeners = new ListenerList<>();

    public ResultFilesTracker(ResultFileManager manager, Inputs inputs, IContainer anfFolder) {
        this.manager = manager;
        this.inputs = inputs;
        this.anfFolder = anfFolder;
    }

    public boolean deactivate() {
        manager = null;
        return false;
    }

    public void addChangeListener(IResultFilesChangeListener listener) {
        listeners.add(listener);
    }

    public void removeChangeListener(IResultFilesChangeListener listener) {
        listeners.remove(listener);
    }

    protected void notifyListeners(ResultFileManagerChangeEvent event) {
        for (IResultFilesChangeListener listener : listeners)
            listener.resultFileManagerChanged(event);
    }

    /**
     * Listen to model changes.
     */
    public void modelChanged(ModelChangeEvent event) {
        if (ScaveModelUtil.isInputsChange(event))
            synchronize();
    }

    public void reloadResultFiles() {
        ResultFileManager.runWithWriteLock(manager, () -> manager.clear()); //TODO async
        synchronize();
    }

    /**
     * Ensure that exactly the result files specified in the Inputs node are loaded.
     * Missing files get loaded, and extra files get unloaded.
     */
    public void synchronize() {
        if (manager == null)
            return;

        DisplayUtils.runNowOrAsyncInUIThread(() -> {
            try {
                InterruptedFlag interruptedFlag = new InterruptedFlag();
                ProgressMonitorDialog dialog = new ProgressMonitorDialog(Display.getCurrent().getActiveShell()) {
                    @Override
                    protected void cancelPressed() {
                        super.cancelPressed();
                        interruptedFlag.setFlag(true);
                    }
                };
                dialog.run(true, true, (monitor)-> ResultFileManager.runWithWriteLock(manager, () -> doSynchronize(monitor, interruptedFlag)));
            }
            catch (InvocationTargetException e) {
                ScavePlugin.logError(e);
            }
            catch (InterruptedException e) {}

        });
    }

    public void doSynchronize(IProgressMonitor monitor, InterruptedFlag interruptedFlag) {
        manager.checkWriteLock(); // must run with write lock

        SubMonitor subMonitor = SubMonitor.convert(monitor, 100);

        Map<String, Map<String, String>> files = new LinkedHashMap<>(); //TODO we could use a flat list of structs -- would be easier to understand

        Debug.time("Collecting files", 1, () -> {
            subMonitor.setTaskName("Collecting files");
            subMonitor.split(10); // 10%
            for (InputFile input : inputs.getInputs())
                files.put(input.getName(), collectResultFiles(input.getName(), anfFolder));
        });

        //TODO unloadFile() (invoked from RELOAD) is still very slow!
        //TODO handle java.lang.RuntimeException: Result file loading interrupted

        Debug.time("Loading files", 1, () -> {
            subMonitor.setTaskName("Loading files");

            int numFiles = files.values().stream().collect(Collectors.summingInt((map) -> map.size()));
            subMonitor.setWorkRemaining(numFiles);

            int loadFlags = ResultFileManagerEx.RELOAD_IF_CHANGED | ResultFileManagerEx.ALLOW_INDEXING | ResultFileManagerEx.SKIP_IF_LOCKED;
            outer: for (String inputName : files.keySet()) {
                for (Entry<String,String> entry : files.get(inputName).entrySet()) {
                    String filePath = entry.getKey();
                    String fileLocation = entry.getValue();
                    manager.loadFile(filePath, fileLocation, inputName, loadFlags, interruptedFlag);
                    if (interruptedFlag.getFlag())
                        break outer;
                    subMonitor.worked(1); //TODO if there are many files, report less frequently but more work (e.g. worked(10) every 10 files)
                    //TODO:
                    //ScavePlugin.logError("Could not load file: " + file.getLocation().toOSString(), e);
                    //ScaveMarkers.setMarker(file, MARKERTYPE_SCAVEPROBLEM, IMarker.SEVERITY_ERROR, "Could not load file. Reason: "+e.getMessage(), -1);

                }
            }
        });

        Debug.time("Unloading extra files", 1, () -> {
            subMonitor.setTaskName("Unloading extra files");
            subMonitor.setWorkRemaining(10);
            // collect set of file names from 'files'
            Set<String> fileSet = new HashSet<>();
            for (Map<String,String> pathToLocation : files.values())
                fileSet.addAll(pathToLocation.keySet());

            // determine list of files to be unloaded
            ResultFileList loadFiles = ResultFileManagerEx.callWithReadLock(manager, () -> manager.getFiles());
            List<ResultFile> filesToBeUnloaded = new ArrayList<>();
            for (int i = 0; i < loadFiles.size(); i++)
                if (!fileSet.contains(loadFiles.get(i).getFilePath()))
                    filesToBeUnloaded.add(loadFiles.get(i));

            // unload
            if (!filesToBeUnloaded.isEmpty())
                for (ResultFile file : filesToBeUnloaded)
                    manager.unloadFile(file);
        });

        // notify listeners (TODO only if there was actually any change)
        notifyListeners(new ResultFileManagerChangeEvent(manager));

        monitor.done();
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

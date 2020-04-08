/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.engineext;

import org.eclipse.core.runtime.ListenerList;
import org.omnetpp.common.engine.ILock;
import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StringSet;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;

/**
 * ResultFileManager with added checks and minimal extra functionality.
 *
 * @author andras
 */
public class ResultFileManagerEx extends ResultFileManager {
    // What to do if file is already loaded:
    public static int RELOAD = ResultFileManager.LoadFlags.RELOAD.swigValue(); // reloaded if already loaded
    public static int RELOAD_IF_CHANGED = ResultFileManager.LoadFlags.RELOAD_IF_CHANGED.swigValue(); // reload only if file changed since being loaded (this is the default)
    public static int NEVER_RELOAD = ResultFileManager.LoadFlags.NEVER_RELOAD.swigValue(); // don't reload if already loaded
    // What to do if index file is missing or out of date:
    public static int ALLOW_INDEXING = ResultFileManager.LoadFlags.ALLOW_INDEXING.swigValue(); // create index (this is the default)
    public static int SKIP_IF_NO_INDEX = ResultFileManager.LoadFlags.SKIP_IF_NO_INDEX.swigValue(); // don't load
    public static int ALLOW_LOADING_WITHOUT_INDEX = ResultFileManager.LoadFlags.ALLOW_LOADING_WITHOUT_INDEX.swigValue(); // load without creating an index (i.e. scan .vec file instead of .vci file)
    // What to do if there is a lock file (indicating that the file is being written to)
    public static int SKIP_IF_LOCKED = ResultFileManager.LoadFlags.SKIP_IF_LOCKED.swigValue(); // don't load (this is the default)
    public static int IGNORE_LOCK_FILE = ResultFileManager.LoadFlags.IGNORE_LOCK_FILE.swigValue(); // pretend lock file doesn't exist
    public static int VERBOSE = ResultFileManager.LoadFlags.VERBOSE.swigValue(); // print on stdout what it's doing

    private ListenerList<IResultFileManagerDisposeListener> disposeListeners = new ListenerList<>();

    public void addDisposeListener(IResultFileManagerDisposeListener listener) {
        disposeListeners.add(listener);
    }

    public void removeDisposeListener(IResultFileManagerDisposeListener listener) {
        disposeListeners.remove(listener);
    }

    protected void notifyDisposeListeners() {
        for (IResultFileManagerDisposeListener listener : disposeListeners)
            listener.resultFileManagerDisposed(this);
    }

    /*-------------------------------------------
     *               Writer methods
     *-------------------------------------------*/
    public void dispose() {
        delete();
        notifyDisposeListeners();
    }

    @Override
    public void clear() {
        checkNotDeleted();
        super.clear();
    }

    @Override
    public ResultFile loadFile(String displayName, String osFileName, int flags, InterruptedFlag interrupted) {
        checkNotDeleted();
        checkWriteLock();
        return super.loadFile(displayName, osFileName, flags, interrupted);
    }

    @Override
    public void unloadFile(ResultFile file) {
        checkNotDeleted();
        checkWriteLock();
        super.unloadFile(file);
    }

    @Override
    public void unloadFile(String fileName) {
        checkNotDeleted();
        checkWriteLock();
        super.unloadFile(fileName);
    }


    /*-------------------------------------------
     *               Reader methods
     *-------------------------------------------*/


    public boolean isDisposed() {
        return getCPtr(this) == 0;
    }

    @Override
    public ILock getReadLock() {
        checkNotDeleted();
        return super.getReadLock();
    }

    @Override
    public ILock getWriteLock() {
        checkNotDeleted();
        return super.getWriteLock();
    }

    @Override
    public IDList getAllScalars() {
        checkNotDeleted();
        return super.getAllScalars();
    }

    @Override
    public IDList getAllVectors() {
        checkNotDeleted();
        return super.getAllVectors();
    }

    @Override
    public IDList getAllHistograms() {
        checkNotDeleted();
        return super.getAllHistograms();
    }

    @Override
    public IDList getAllItems() {
        checkNotDeleted();
        return super.getAllItems();
    }

    @Override
    public IDList getVectorsInFileRun(FileRun fileRun) {
        checkNotDeleted();
        return super.getVectorsInFileRun(fileRun);
    }

    @Override
    public IDList filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
        checkNotDeleted();
        return super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter);
    }

    /*
     * The rest adds check() calls only.
     */

    @Override
    public IDList filterIDList(IDList idlist, String pattern, int limit, InterruptedFlag interrupted) {
        checkNotDeleted();
        return super.filterIDList(idlist, pattern, limit, interrupted);
    }

    @Override
    public IDList filterIDList(IDList idlist, String pattern, int limit) {
        checkNotDeleted();
        return super.filterIDList(idlist, pattern, limit);
    }

    @Override
    public IDList filterIDList(IDList idlist, String pattern) {
        checkNotDeleted();
        return super.filterIDList(idlist, pattern);
    }

    @Override
    public ResultFile getFile(String fileName) {
        checkNotDeleted();
        return super.getFile(fileName);
    }

    @Override
    public FileRun getFileRun(ResultFile file, Run run) {
        checkNotDeleted();
        return super.getFileRun(file, run);
    }

    @Override
    public ResultFileList getFiles() {
        checkNotDeleted();
        return super.getFiles();
    }

    @Override
    public ResultFileList getFilesForRun(Run run) {
        checkNotDeleted();
        return super.getFilesForRun(run);
    }

    @Override
    public long getItemByName(FileRun fileRun, String module, String name) {
        checkNotDeleted();
        return super.getItemByName(fileRun, module, name);
    }

    @Override
    public StringVector getModuleFilterHints(IDList idlist) {
        checkNotDeleted();
        return super.getModuleFilterHints(idlist);
    }

    @Override
    public StringVector getFilePathFilterHints(ResultFileList fileList) {
        checkNotDeleted();
        return super.getFilePathFilterHints(fileList);
    }

    @Override
    public StringVector getRunNameFilterHints(RunList runList) {
        checkNotDeleted();
        return super.getRunNameFilterHints(runList);
    }

    @Override
    public StringVector getResultNameFilterHints(IDList idlist) {
        checkNotDeleted();
        return super.getResultNameFilterHints(idlist);
    }

    @Override
    public StringVector getParamAssignmentFilterHints(RunList runList, String key) {
        checkNotDeleted();
        return super.getParamAssignmentFilterHints(runList, key);
    }

    @Override
    public StringVector getResultItemAttributeFilterHints(IDList idlist, String attrName) {
        checkNotDeleted();
        return super.getResultItemAttributeFilterHints(idlist, attrName);
    }

    @Override
    public StringVector getRunAttributeFilterHints(RunList runList, String attrName) {
        checkNotDeleted();
        return super.getRunAttributeFilterHints(runList, attrName);
    }

    @Override
    public Run getRunByName(String runName) {
        checkNotDeleted();
        return super.getRunByName(runName);
    }

    @Override
    public RunList getRuns() {
        checkNotDeleted();
        return super.getRuns();
    }

    @Override
    public RunList getRunsInFile(ResultFile file) {
        checkNotDeleted();
        return super.getRunsInFile(file);
    }

    @Override
    public ResultItem getItem(long id) {
        checkNotDeleted();
        return super.getItem(id);
    }

    @Override
    public ScalarResult getScalar(long id) {
        checkNotDeleted();
        return super.getScalar(id);
    }

    @Override
    public VectorResult getVector(long id) {
        checkNotDeleted();
        return super.getVector(id);
    }

    @Override
    public HistogramResult getHistogram(long id) {
        checkNotDeleted();
        return super.getHistogram(id);
    }

    @Override
    public ResultFileList getUniqueFiles(IDList ids) {
        checkNotDeleted();
        return super.getUniqueFiles(ids);
    }

    @Override
    public StringSet getUniqueModuleNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueModuleNames(ids);
    }

    @Override
    public StringSet getUniqueResultNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueResultNames(ids);
    }

    @Override
    public RunList getUniqueRuns(IDList ids) {
        checkNotDeleted();
        return super.getUniqueRuns(ids);
    }

    @Override
    public StringSet getUniqueResultAttributeNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueResultAttributeNames(ids);
    }

    @Override
    public StringSet getUniqueResultAttributeValues(IDList ids, String attrName) {
        checkNotDeleted();
        return super.getUniqueResultAttributeValues(ids, attrName);
    }

    @Override
    public StringSet getUniqueParamAssignmentKeys(RunList runList) {
        checkNotDeleted();
        return super.getUniqueParamAssignmentKeys(runList);
    }

    @Override
    public StringSet getUniqueRunAttributeNames(RunList runList) {
        checkNotDeleted();
        return super.getUniqueRunAttributeNames(runList);
    }

    @Override
    public boolean isFileLoaded(String fileName) {
        checkNotDeleted();
        return super.isFileLoaded(fileName);
    }

    @Override
    public boolean hasStaleID(IDList ids) {
        checkNotDeleted();
        return super.hasStaleID(ids);
    }

    @Override
    public boolean isStaleID(long id) {
        checkNotDeleted();
        return super.isStaleID(id);
    }

    @Override
    protected Object clone() throws CloneNotSupportedException {
        checkNotDeleted();
        return super.clone();
    }

    @Override
    public boolean equals(Object obj) {
        checkNotDeleted();
        return super.equals(obj);
    }

    @Override
    public int hashCode() {
        checkNotDeleted();
        return super.hashCode();
    }

    @Override
    public String toString() {
        checkNotDeleted();
        return super.toString();
    }

    /*-------------------------------------
     *            Helpers
     *-------------------------------------*/

    // Note: It's not necessary to call this method from other methods, because
    // the underlying C++ functions all contain READER_MUTEX.
    public void checkReadLock() {
        checkNotDeleted();
        super.checkReadLock();
    }

    // Note: It's usually not necessary to call this method from other methods, because
    // the underlying C++ functions all contain WRITER_MUTEX.
    public void checkWriteLock() {
        checkNotDeleted();
        super.checkWriteLock();
    }

    private void checkNotDeleted() {
        if (getCPtr(this) == 0)
            throw new IllegalStateException("Tried to access a deleted ResultFileManagerEx.");
    }

    public static int getRunNumber(Run run) {
        try {
            return Integer.parseInt(run.getAttribute("runnumber"));
        } catch (Exception e) {
            return 0;
        }
    }
}

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
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringSet;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engineext.ResultFileManagerChangeEvent.ChangeType;

/**
 * ResultFileManager with notification capability. Also re-wraps
 * all returned IDLists into IDListEx (IDList with notification
 * capability.)
 *
 * @author andras
 */
public class ResultFileManagerEx extends ResultFileManager {

    private ListenerList changeListeners = new ListenerList();
    private ListenerList disposeListeners = new ListenerList();

    public void addChangeListener(IResultFilesChangeListener listener) {
        changeListeners.add(listener);
    }

    public void removeChangeListener(IResultFilesChangeListener listener) {
        changeListeners.remove(listener);
    }

    protected void notifyChangeListeners(ResultFileManagerChangeEvent event) {
        for (Object listener : changeListeners.getListeners())
            ((IResultFilesChangeListener)listener).resultFileManagerChanged(event);
    }

    public void addDisposeListener(IResultFileManagerDisposeListener listener) {
        disposeListeners.add(listener);
    }

    public void removeDisposeListener(IResultFileManagerDisposeListener listener) {
        disposeListeners.remove(listener);
    }

    protected void notifyDisposeListeners() {
        for (Object listener : disposeListeners.getListeners())
            ((IResultFileManagerDisposeListener)listener).resultFileManagerDisposed(this);
    }

    /*-------------------------------------------
     *               Writer methods
     *-------------------------------------------*/
    public void dispose() {
        delete();
        notifyDisposeListeners();
    }

    @Override
    public ResultFile loadFile(String filename) {
        getWriteLock().lock();
        try {
            checkNotDeleted();
            ResultFile file = super.loadFile(filename);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.LOAD, filename));
            return file;
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public ResultFile loadFile(String filename, String osFileName) {
        getWriteLock().lock();
        try {
            checkNotDeleted();
            ResultFile file = super.loadFile(filename, osFileName);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.LOAD, filename));
            return file;
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public ResultFile loadFile(String filename, String osFileName, boolean reload) {
        getWriteLock().lock();
        try {
            checkNotDeleted();
            ResultFile file = super.loadFile(filename, osFileName, reload);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.LOAD, filename));
            return file;
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public void unloadFile(ResultFile file) {
        getWriteLock().lock();
        try {
            checkNotDeleted();
            String filename = file.getFilePath();
            super.unloadFile(file);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.UNLOAD, filename));
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public long addComputedVector(int vectorId, String name, String file, StringMap attributes, long computationID, long input, Object processingOp) {
        checkNotDeleted();
        getWriteLock().lock();
        try {
            checkNotDeleted();
            long id = super.addComputedVector(vectorId, name, file, attributes, computationID, input, processingOp);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.COMPUTED_ITEM_ADDED, id));
            return id;
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public long addComputedScalar(String name, String module, String runName, double value, StringMap attributes, Object computeScalarNode) {
        checkNotDeleted();
        getWriteLock().lock();
        try {
            checkNotDeleted();
            long id = super.addComputedScalar(name, module, runName, value, attributes, computeScalarNode);
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.COMPUTED_ITEM_ADDED, id));
            return id;
        }
        finally {
            getWriteLock().unlock();
        }
    }

    @Override
    public void clearComputedScalars() {
        checkNotDeleted();
        getWriteLock().lock();
        try {
            checkNotDeleted();
            super.clearComputedScalars();
            notifyChangeListeners(new ResultFileManagerChangeEvent(this, ChangeType.COMPUTED_SCALARS_CLEARED));
        }
        finally {
            getWriteLock().unlock();
        }
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
        IDList idlist = super.getAllScalars();
        return wrap(idlist);
    }

    @Override
    public IDList getAllVectors() {
        checkNotDeleted();
        IDList idlist = super.getAllVectors();
        return wrap(idlist);
    }

    @Override
    public IDList getAllHistograms() {
        checkNotDeleted();
        IDList idlist = super.getAllHistograms();
        return wrap(idlist);
    }

    @Override
    public IDList getAllItems() {
        checkNotDeleted();
        IDList idlist = super.getAllItems();
        return wrap(idlist);
    }

    @Override
    public IDList getVectorsInFileRun(FileRun fileRun) {
        checkReadLock();
        IDList idlist = super.getVectorsInFileRun(fileRun);
        return wrap(idlist);
    }

    @Override
    public IDList filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
        checkReadLock();
        IDList result = super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter);
        return wrap(result);
    }

    /*
     * The rest adds check() calls only.
     */

    @Override
    public IDList filterIDList(IDList idlist, String pattern) {
        checkNotDeleted();
        IDList result = super.filterIDList(idlist, pattern);
        return wrap(result);
    }

    @Override
    public ResultFile getFile(String fileName) {
        // checkReadLock();
        return super.getFile(fileName);
    }

    @Override
    public StringVector getFileAndRunNumberFilterHints(IDList idlist) {
        checkNotDeleted();
        return super.getFileAndRunNumberFilterHints(idlist);
    }

    @Override
    public FileRun getFileRun(ResultFile file, Run run) {
        checkReadLock();
        return super.getFileRun(file, run);
    }

    @Override
    public ResultFileList getFiles() {
        checkReadLock();
        return super.getFiles();
    }

    @Override
    public ResultFileList getFilesForRun(Run run) {
        checkReadLock();
        return super.getFilesForRun(run);
    }

    @Override
    public long getItemByName(FileRun fileRun, String module, String name) {
        checkReadLock();
        return super.getItemByName(fileRun, module, name);
    }

    @Override
    public StringVector getModuleFilterHints(IDList idlist) {
        checkNotDeleted();
        return super.getModuleFilterHints(idlist);
    }



    @Override
    public StringVector getFilePathFilterHints(ResultFileList fileList) {
        checkReadLock();
        return super.getFilePathFilterHints(fileList);
    }

    @Override
    public StringVector getRunNameFilterHints(RunList runList) {
        checkReadLock();
        return super.getRunNameFilterHints(runList);
    }

    @Override
    public StringVector getNameFilterHints(IDList idlist) {
        checkNotDeleted();
        return super.getNameFilterHints(idlist);
    }

    @Override
    public StringVector getParamAssignmentFilterHints(RunList runList, String key) {
        checkReadLock();
        return super.getParamAssignmentFilterHints(runList, key);
    }

    @Override
    public StringVector getResultItemAttributeFilterHints(IDList idlist,
            String attrName) {
        checkNotDeleted();
        return super.getResultItemAttributeFilterHints(idlist, attrName);
    }

    @Override
    public StringVector getRunAttributeFilterHints(RunList runList,
            String attrName) {
        checkReadLock();
        return super.getRunAttributeFilterHints(runList, attrName);
    }

    @Override
    public Run getRunByName(String runName) {
        checkReadLock();
        return super.getRunByName(runName);
    }

    @Override
    public RunList getRuns() {
        checkReadLock();
        return super.getRuns();
    }

    @Override
    public RunList getRunsInFile(ResultFile file) {
        checkReadLock();
        return super.getRunsInFile(file);
    }

    @Override
    public ResultItem getItem(long id) {
        checkReadLock();
        return super.getItem(id);
    }

    @Override
    public ScalarResult getScalar(long id) {
        checkReadLock();
        return super.getScalar(id);
    }

    @Override
    public VectorResult getVector(long id) {
        checkReadLock();
        return super.getVector(id);
    }

    @Override
    public HistogramResult getHistogram(long id) {
        checkReadLock();
        return super.getHistogram(id);
    }

    @Override
    public ResultFileList getUniqueFiles(IDList ids) {
        checkReadLock();
        return super.getUniqueFiles(ids);
    }

    @Override
    public StringSet getUniqueModuleNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueModuleNames(ids);
    }

    @Override
    public StringSet getUniqueNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueNames(ids);
    }

    @Override
    public RunList getUniqueRuns(IDList ids) {
        checkReadLock();
        return super.getUniqueRuns(ids);
    }

    @Override
    public StringSet getUniqueAttributeNames(IDList ids) {
        checkNotDeleted();
        return super.getUniqueAttributeNames(ids);
    }

    @Override
    public StringSet getUniqueAttributeValues(IDList ids, String attrName) {
        checkNotDeleted();
        return super.getUniqueAttributeValues(ids, attrName);
    }

    @Override
    public StringSet getUniqueParamAssignmentKeys(RunList runList) {
        checkReadLock();
        return super.getUniqueParamAssignmentKeys(runList);
    }

    @Override
    public StringSet getUniqueRunAttributeNames(RunList runList) {
        checkReadLock();
        return super.getUniqueRunAttributeNames(runList);
    }

    @Override
    public boolean isFileLoaded(String fileName) {
        checkNotDeleted();
        return super.isFileLoaded(fileName);
    }

    @Override
    public long getComputedID(long computationID, long inputID) {
        checkNotDeleted();
        return super.getComputedID(computationID, inputID);
    }

    @Override
    public ResultFile getComputedScalarFile() {
        checkNotDeleted();
        return super.getComputedScalarFile();
    }

    @Override
    public IDList getComputedScalarIDs(Object node) {
        checkNotDeleted();
        return wrap(super.getComputedScalarIDs(node));
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
    private void checkNotDeleted() {
        if (getCPtr(this) == 0)
            throw new IllegalStateException("Tried to access a deleted ResultFileManagerEx.");
    }

    private IDList wrap(IDList obj) {
        return obj; // TODO eliminate wrap() calls
    }
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.engineext;

import org.eclipse.core.runtime.ListenerList;
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

/**
 * ResultFileManager with notification capability. Also re-wraps
 * all returned IDLists into IDListEx (IDList with notification 
 * capability.)
 * 
 * Because the underlying C++ implementation is not thread-safe,
 * this class ensures that the C++ ResultFileManager is not accessed
 * concurrently while its state in changing (as a result of load(), unload()).
 * Concurrent reads are allowed however.
 * 
 * @author andras
 */
public class ResultFileManagerEx extends ResultFileManager {

	private ListenerList changeListeners = new ListenerList();
	private ListenerList disposeListeners = new ListenerList();
	
	/*
	 * Listener interface.
	 * No locking needed, because the listener list implementation is
	 * concurrent and the C++ ResultFileManager is not accessed. 
	 */
	
	public void addChangeListener(IResultFilesChangeListener listener) {
		changeListeners.add(listener);
	}

	public void removeChangeListener(IResultFilesChangeListener listener) {
		changeListeners.remove(listener);
	}
	
	protected void notifyChangeListeners() {
		for (Object listener : changeListeners.getListeners())
			((IResultFilesChangeListener)listener).resultFileManagerChanged(this);
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
		try {
			writeLock.lock();
			delete();
		}
		finally {
			writeLock.unlock();
		}
		notifyDisposeListeners();
	}
	
	@Override
	public ResultFile loadFile(String filename) {
		ResultFile file = null;
		try {
			writeLock.lock();
			checkNotDeleted();
			file = super.loadFile(filename);
		}
		finally {
			writeLock.unlock();
		}
		notifyChangeListeners();
		return file;
	}

	@Override
	public ResultFile loadFile(String filename, String osFileName) {
		ResultFile file = null;
		try {
			writeLock.lock();
			checkNotDeleted();
			file = super.loadFile(filename, osFileName);
		}
		finally {
			writeLock.unlock();
		}
		notifyChangeListeners();
		return file;
	}
	
	@Override
	public void unloadFile(ResultFile file) {
		try {
			writeLock.lock();
			checkNotDeleted();
			super.unloadFile(file);
		}
		finally {
			writeLock.unlock();
		}
		notifyChangeListeners();
	}
	
	@Override
	public long addComputedVector(int vectorId, String name, String file, StringMap attributes, long computationID, long input, Object processingOp) {
		long id;
		try {
			writeLock.lock();
			checkNotDeleted();
			id = super.addComputedVector(vectorId, name, file, attributes, computationID, input, processingOp);
		}
		finally {
			writeLock.unlock();
		}
		notifyChangeListeners();
		return id;
	}

	/*-------------------------------------------
	 *               Reader methods
	 *-------------------------------------------*/
	public boolean isDisposed() {
		return getCPtr(this) == 0;
	}
	
	@Override
	public IDList getAllScalars() {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getAllScalars();
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}

	@Override
	public IDList getAllVectors() {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getAllVectors();
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}
	
	@Override
	public IDList getAllHistograms() {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getAllHistograms();
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}
	
	@Override
	public IDList getAllItems() {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getAllItems();
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}

	@Override
	public IDList getScalarsInFileRun(FileRun fileRun) {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getScalarsInFileRun(fileRun);
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}

	@Override
	public IDList getVectorsInFileRun(FileRun fileRun) {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getVectorsInFileRun(fileRun);
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}

	@Override
	public IDList getHistogramsInFileRun(FileRun fileRun) {
		IDList idlist;
		try {
			readLock.lock();
			checkNotDeleted();
			idlist = super.getHistogramsInFileRun(fileRun);
		}
		finally {
			readLock.unlock();
		}
		return wrap(idlist);
	}

	@Override
	public IDList filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
		IDList result;
		try {
			readLock.lock();
			checkNotDeleted();
			result = super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter);
		}
		finally {
			readLock.unlock();
		}
		return wrap(result);
	}
	
	/*
	 * The rest adds check() calls only.
	 */

	@Override
	public ResultItem _getItem(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super._getItem(id);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultFileList filterFileList(ResultFileList fileList, String filePathPattern) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.filterFileList(fileList, filePathPattern);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public IDList filterIDList(IDList idlist, String pattern) {
		IDList result;
		try {
			readLock.lock();
			checkNotDeleted();
			result = super.filterIDList(idlist, pattern);
		}
		finally {
			readLock.unlock();
		}
		return wrap(result);
	}

	@Override
	public RunList filterRunList(RunList runList, String runNameFilter, StringMap attrFilter) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.filterRunList(runList, runNameFilter, attrFilter);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultFile getFile(String fileName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFile(fileName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getFileAndRunNumberFilterHints(IDList idlist) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFileAndRunNumberFilterHints(idlist);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public FileRun getFileRun(ResultFile file, Run run) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFileRun(file, run);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public FileRunList getFileRuns(ResultFileList fileList, RunList runList) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFileRuns(fileList, runList);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultFileList getFiles() {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFiles();
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultFileList getFilesForRun(Run run) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFilesForRun(run);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public long getItemByName(FileRun fileRun, String module, String name) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getItemByName(fileRun, module, name);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getModuleFilterHints(IDList idlist) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getModuleFilterHints(idlist);
		}
		finally {
			readLock.unlock();
		}
	}
	
	

	@Override
	public StringVector getFilePathFilterHints(ResultFileList fileList) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getFilePathFilterHints(fileList);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getRunNameFilterHints(RunList runList) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getRunNameFilterHints(runList);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getNameFilterHints(IDList idlist) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getNameFilterHints(idlist);
		}
		finally {
			readLock.unlock();
		}
	}
	
	@Override
	public StringVector getModuleParamFilterHints(RunList runList,
			String paramName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getModuleParamFilterHints(runList, paramName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getResultItemAttributeFilterHints(IDList idlist,
			String attrName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getResultItemAttributeFilterHints(idlist, attrName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringVector getRunAttributeFilterHints(RunList runList,
			String attrName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getRunAttributeFilterHints(runList, attrName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public Run getRunByName(String runName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getRunByName(runName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public RunList getRuns() {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getRuns();
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public RunList getRunsInFile(ResultFile file) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getRunsInFile(file);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultItem getItem(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getItem(id);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ScalarResult getScalar(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getScalar(id);
		}
		finally {
			readLock.unlock();
		}
	}
	
	@Override
	public VectorResult getVector(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getVector(id);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public HistogramResult getHistogram(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getHistogram(id);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueRunAttributeValues(RunList runList,
			String attrName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueRunAttributeValues(runList, attrName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public FileRunList getUniqueFileRuns(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueFileRuns(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public ResultFileList getUniqueFiles(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueFiles(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueModuleNames(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueModuleNames(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueNames(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueNames(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public RunList getUniqueRuns(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueRuns(ids);
		}
		finally {
			readLock.unlock();
		}
	}
	
	@Override
	public StringSet getUniqueAttributeNames(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueAttributeNames(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueAttributeValues(IDList ids, String attrName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueAttributeValues(ids, attrName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueModuleParamNames(RunList runList) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueModuleParamNames(runList);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueModuleParamValues(RunList runList,
			String paramName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueModuleParamValues(runList, paramName);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public StringSet getUniqueRunAttributeNames(RunList runList) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getUniqueRunAttributeNames(runList);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public boolean isFileLoaded(String fileName) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.isFileLoaded(fileName);
		}
		finally {
			readLock.unlock();
		}
	}
	
	@Override
	public long getComputedID(long computationID, long inputID) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.getComputedID(computationID, inputID);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public boolean hasStaleID(IDList ids) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.hasStaleID(ids);
		}
		finally {
			readLock.unlock();
		}
	}

	@Override
	public boolean isStaleID(long id) {
		try {
			readLock.lock();
			checkNotDeleted();
			return super.isStaleID(id);
		}
		finally {
			readLock.unlock();
		}
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

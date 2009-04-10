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
		delete();
		notifyDisposeListeners();
	}
	
	@Override
	public ResultFile loadFile(String filename) {
		checkNotDeleted();
		ResultFile file = super.loadFile(filename);
		notifyChangeListeners();
		return file;
	}

	@Override
	public ResultFile loadFile(String filename, String osFileName) {
		checkNotDeleted();
		ResultFile file = super.loadFile(filename, osFileName);
		notifyChangeListeners();
		return file;
	}
	
	@Override
	public void unloadFile(ResultFile file) {
		checkNotDeleted();
		super.unloadFile(file);
		notifyChangeListeners();
	}
	
	@Override
	public long addComputedVector(int vectorId, String name, String file, StringMap attributes, long computationID, long input, Object processingOp) {
		checkNotDeleted();
		long id = super.addComputedVector(vectorId, name, file, attributes, computationID, input, processingOp);
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
	public IDList getScalarsInFileRun(FileRun fileRun) {
		checkNotDeleted();
		IDList idlist = super.getScalarsInFileRun(fileRun);
		return wrap(idlist);
	}

	@Override
	public IDList getVectorsInFileRun(FileRun fileRun) {
		checkNotDeleted();
		IDList idlist = super.getVectorsInFileRun(fileRun);
		return wrap(idlist);
	}

	@Override
	public IDList getHistogramsInFileRun(FileRun fileRun) {
		checkNotDeleted();
		IDList idlist = super.getHistogramsInFileRun(fileRun);
		return wrap(idlist);
	}

	@Override
	public IDList filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
		checkNotDeleted();
		IDList result = super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter);
		return wrap(result);
	}
	
	/*
	 * The rest adds check() calls only.
	 */

	@Override
	public ResultItem _getItem(long id) {
		checkNotDeleted();
		return super._getItem(id);
	}

	@Override
	public ResultFileList filterFileList(ResultFileList fileList, String filePathPattern) {
		checkNotDeleted();
		return super.filterFileList(fileList, filePathPattern);
	}

	@Override
	public IDList filterIDList(IDList idlist, String pattern) {
		checkNotDeleted();
		IDList result = super.filterIDList(idlist, pattern);
		return wrap(result);
	}

	@Override
	public RunList filterRunList(RunList runList, String runNameFilter, StringMap attrFilter) {
		checkNotDeleted();
		return super.filterRunList(runList, runNameFilter, attrFilter);
	}

	@Override
	public ResultFile getFile(String fileName) {
		checkNotDeleted();
		return super.getFile(fileName);
	}

	@Override
	public StringVector getFileAndRunNumberFilterHints(IDList idlist) {
		checkNotDeleted();
		return super.getFileAndRunNumberFilterHints(idlist);
	}

	@Override
	public FileRun getFileRun(ResultFile file, Run run) {
		checkNotDeleted();
		return super.getFileRun(file, run);
	}

	@Override
	public FileRunList getFileRuns(ResultFileList fileList, RunList runList) {
		checkNotDeleted();
		return super.getFileRuns(fileList, runList);
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
	public StringVector getNameFilterHints(IDList idlist) {
		checkNotDeleted();
		return super.getNameFilterHints(idlist);
	}
	
	@Override
	public StringVector getModuleParamFilterHints(RunList runList,
			String paramName) {
		checkNotDeleted();
		return super.getModuleParamFilterHints(runList, paramName);
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
	public StringSet getUniqueRunAttributeValues(RunList runList,
			String attrName) {
		checkNotDeleted();
		return super.getUniqueRunAttributeValues(runList, attrName);
	}

	@Override
	public FileRunList getUniqueFileRuns(IDList ids) {
		checkNotDeleted();
		return super.getUniqueFileRuns(ids);
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
	public StringSet getUniqueNames(IDList ids) {
		checkNotDeleted();
		return super.getUniqueNames(ids);
	}

	@Override
	public RunList getUniqueRuns(IDList ids) {
		checkNotDeleted();
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
	public StringSet getUniqueModuleParamNames(RunList runList) {
		checkNotDeleted();
		return super.getUniqueModuleParamNames(runList);
	}

	@Override
	public StringSet getUniqueModuleParamValues(RunList runList,
			String paramName) {
		checkNotDeleted();
		return super.getUniqueModuleParamValues(runList, paramName);
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
	public long getComputedID(long computationID, long inputID) {
		checkNotDeleted();
		return super.getComputedID(computationID, inputID);
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

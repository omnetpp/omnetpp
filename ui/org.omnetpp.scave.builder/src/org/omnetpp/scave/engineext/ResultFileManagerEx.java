package org.omnetpp.scave.engineext;

import java.util.ArrayList;

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

	private ArrayList<IResultFilesChangeListener> listeners = new ArrayList<IResultFilesChangeListener>();
	
	private void checkDeleted() {
		if (getCPtr(this) == 0)
			throw new IllegalStateException("Tried to access a deleted ResultFileManagerEx.");
	}

	public void addListener(IResultFilesChangeListener listener) {
		listeners.add(listener);
	}

	public void removeListener(IResultFilesChangeListener listener) {
		listeners.remove(listener);
	}
	
	protected void notifyListeners() {
		for (IResultFilesChangeListener listener : listeners.toArray(new IResultFilesChangeListener[listeners.size()]))
			listener.resultFileManagerChanged(this);
	}

	@Override
	public ResultFile loadFile(String filename) {
		checkDeleted();
		ResultFile file = super.loadFile(filename);
		notifyListeners();
		return file;
	}

	@Override
	public ResultFile loadFile(String filename, String osFileName) {
		checkDeleted();
		ResultFile file = super.loadFile(filename, osFileName);
		notifyListeners();
		return file;
	}
	
	@Override
	public void unloadFile(ResultFile file) {
		checkDeleted();
		super.unloadFile(file);
		notifyListeners();
	}

	private IDListEx wrap(IDList obj) {
		return new IDListEx(obj); // re-wrap C++ object to "Ex" class 
	}
	
	@Override
	public IDListEx getAllScalars() {
		checkDeleted();
		return wrap(super.getAllScalars());
	}

	@Override
	public IDListEx getAllVectors() {
		checkDeleted();
		return wrap(super.getAllVectors());
	}

	@Override
	public IDList getScalarsInFileRun(FileRun fileRun) {
		checkDeleted();
		return wrap(super.getScalarsInFileRun(fileRun));
	}

	@Override
	public IDList getVectorsInFileRun(FileRun fileRun) {
		checkDeleted();
		return wrap(super.getVectorsInFileRun(fileRun));
	}

	@Override
	public IDListEx filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
		checkDeleted();
		return wrap(super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter));
	}
	
	/*
	 * The rest adds check() calls only.
	 */

	@Override
	public ResultItem _getItem(long id) {
		checkDeleted();
		return super._getItem(id);
	}

	@Override
	public ResultFileList filterFileList(ResultFileList fileList,
			String filePathPattern) {
		checkDeleted();
		return super.filterFileList(fileList, filePathPattern);
	}

	@Override
	public IDList filterIDList(IDList idlist, String pattern) {
		checkDeleted();
		return super.filterIDList(idlist, pattern);
	}

	@Override
	public RunList filterRunList(RunList runList, String runNameFilter,
			StringMap attrFilter) {
		checkDeleted();
		return super.filterRunList(runList, runNameFilter, attrFilter);
	}

	@Override
	public IDList getAllHistograms() {
		checkDeleted();
		return super.getAllHistograms();
	}

	@Override
	public ResultFile getFile(String fileName) {
		checkDeleted();
		return super.getFile(fileName);
	}

	@Override
	public StringVector getFileAndRunNumberFilterHints(IDList idlist) {
		checkDeleted();
		return super.getFileAndRunNumberFilterHints(idlist);
	}

	@Override
	public FileRun getFileRun(ResultFile file, Run run) {
		checkDeleted();
		return super.getFileRun(file, run);
	}

	@Override
	public FileRunList getFileRuns(ResultFileList fileList, RunList runList) {
		checkDeleted();
		return super.getFileRuns(fileList, runList);
	}

	@Override
	public ResultFileList getFiles() {
		checkDeleted();
		return super.getFiles();
	}

	@Override
	public ResultFileList getFilesForRun(Run run) {
		checkDeleted();
		return super.getFilesForRun(run);
	}

	@Override
	public HistogramResult getHistogram(long id) {
		checkDeleted();
		return super.getHistogram(id);
	}

	@Override
	public IDList getHistogramsInFileRun(FileRun fileRun) {
		checkDeleted();
		return super.getHistogramsInFileRun(fileRun);
	}

	@Override
	public ResultItem getItem(long id) {
		checkDeleted();
		return super.getItem(id);
	}

	@Override
	public long getItemByName(FileRun fileRun, String module, String name) {
		checkDeleted();
		return super.getItemByName(fileRun, module, name);
	}

	@Override
	public StringVector getModuleFilterHints(IDList idlist) {
		checkDeleted();
		return super.getModuleFilterHints(idlist);
	}

	@Override
	public StringVector getNameFilterHints(IDList idlist) {
		checkDeleted();
		return super.getNameFilterHints(idlist);
	}

	@Override
	public Run getRunByName(String runName) {
		checkDeleted();
		return super.getRunByName(runName);
	}

	@Override
	public RunList getRuns() {
		checkDeleted();
		return super.getRuns();
	}

	@Override
	public RunList getRunsInFile(ResultFile file) {
		checkDeleted();
		return super.getRunsInFile(file);
	}

	@Override
	public ScalarResult getScalar(long id) {
		checkDeleted();
		return super.getScalar(id);
	}

	@Override
	public int getTypeOf(long id) {
		checkDeleted();
		return super.getTypeOf(id);
	}

	@Override
	public StringVector getUniqueAttributeValues(RunList runList,
			String attrName) {
		checkDeleted();
		return super.getUniqueAttributeValues(runList, attrName);
	}

	@Override
	public FileRunList getUniqueFileRuns(IDList ids) {
		checkDeleted();
		return super.getUniqueFileRuns(ids);
	}

	@Override
	public ResultFileList getUniqueFiles(IDList ids) {
		checkDeleted();
		return super.getUniqueFiles(ids);
	}

	@Override
	public StringSet getUniqueModuleNames(IDList ids) {
		checkDeleted();
		return super.getUniqueModuleNames(ids);
	}

	@Override
	public StringSet getUniqueNames(IDList ids) {
		checkDeleted();
		return super.getUniqueNames(ids);
	}

	@Override
	public RunList getUniqueRuns(IDList ids) {
		checkDeleted();
		return super.getUniqueRuns(ids);
	}

	@Override
	public VectorResult getVector(long id) {
		checkDeleted();
		return super.getVector(id);
	}

	@Override
	public boolean isFileLoaded(String fileName) {
		checkDeleted();
		return super.isFileLoaded(fileName);
	}

	@Override
	public HistogramResult uncheckedGetHistogram(long id) {
		checkDeleted();
		return super.uncheckedGetHistogram(id);
	}

	@Override
	public ResultItem uncheckedGetItem(long id) {
		checkDeleted();
		return super.uncheckedGetItem(id);
	}

	@Override
	public ScalarResult uncheckedGetScalar(long id) {
		checkDeleted();
		return super.uncheckedGetScalar(id);
	}

	@Override
	public VectorResult uncheckedGetVector(long id) {
		checkDeleted();
		return super.uncheckedGetVector(id);
	}

	@Override
	protected Object clone() throws CloneNotSupportedException {
		checkDeleted();
		return super.clone();
	}

	@Override
	public boolean equals(Object obj) {
		checkDeleted();
		return super.equals(obj);
	}

	@Override
	public int hashCode() {
		checkDeleted();
		return super.hashCode();
	}

	@Override
	public String toString() {
		checkDeleted();
		return super.toString();
	}

	@Override
	public long addComputedVector(String name, String file, long computationID, long input, Object processingOp) {
		checkDeleted();
		long id = super.addComputedVector(name, file, computationID, input, processingOp);
		notifyListeners();
		return id;
	}
}

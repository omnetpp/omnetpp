package org.omnetpp.scave.engineext;

import java.util.ArrayList;

import org.omnetpp.scave.engine.FileRun;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * ResultFileManager with notification capability. Also re-wraps
 * all returned IDLists into IDListEx (IDList with notification 
 * capability.)
 * 
 * @author andras
 */
public class ResultFileManagerEx extends ResultFileManager {

	private ArrayList<IResultFilesChangeListener> listeners = new ArrayList<IResultFilesChangeListener>();

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
		ResultFile file = super.loadFile(filename);
		notifyListeners();
		return file;
	}

	@Override
	public ResultFile loadFile(String filename, String osFileName) {
		ResultFile file = super.loadFile(filename, osFileName);
		notifyListeners();
		return file;
	}
	
	@Override
	public void unloadFile(ResultFile file) {
		super.unloadFile(file);
		notifyListeners();
	}

	private IDListEx wrap(IDList obj) {
		return new IDListEx(obj); // re-wrap C++ object to "Ex" class 
	}
	
	@Override
	public IDListEx getAllScalars() {
		return wrap(super.getAllScalars());
	}

	@Override
	public IDListEx getAllVectors() {
		return wrap(super.getAllVectors());
	}

	@Override
	public IDList getScalarsInFileRun(FileRun fileRun) {
		return wrap(super.getScalarsInFileRun(fileRun));
	}

	@Override
	public IDList getVectorsInFileRun(FileRun fileRun) {
		return wrap(super.getVectorsInFileRun(fileRun));
	}

	@Override
	public IDListEx filterIDList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
		return wrap(super.filterIDList(idlist, fileAndRunFilter, moduleFilter, nameFilter));
	}
}

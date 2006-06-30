package org.omnetpp.scave.engineext;

import java.util.ArrayList;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
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
	public File loadFile(String filename) {
		File file = super.loadFile(filename);
		notifyListeners();
		return file;
	}

	@Override
	public void unloadFile(File file) {
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
	public IDListEx getFilteredList(IDList idlist, FileRunList fileAndRunFilter, String moduleFilter, String nameFilter) {
		return wrap(super.getFilteredList(idlist, fileAndRunFilter, moduleFilter, nameFilter));
	}
}

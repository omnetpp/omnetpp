package org.omnetpp.scave.engineext;

import java.util.ArrayList;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringSet;

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
			listener.changed(this);
	}

	@Override
	public Run addRun(File file) {
		Run run = super.addRun(file);
		notifyListeners();
		return run;
	}

	@Override
	public File addScalarFile() {
		File file = super.addScalarFile();
		notifyListeners();
		return file;
	}

	@Override
	public File addVectorFile() {
		File file = super.addVectorFile();
		notifyListeners();
		return file;
	}

	@Override
	public File loadScalarFile(String filename) {
		File file = super.loadScalarFile(filename);
		notifyListeners();
		return file;
	}

	@Override
	public File loadVectorFile(String filename) {
		File file = super.loadVectorFile(filename);
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
	public IDList getAllScalars() {
		return wrap(super.getAllScalars());
	}

	@Override
	public IDList getAllVectors() {
		return wrap(super.getAllVectors());
	}

	@Override
	public IDList getDataInFile(File file) {
		return wrap(super.getDataInFile(file));
	}

	@Override
	public IDList getDataInRun(Run run) {
		return wrap(super.getDataInRun(run));
	}

	@Override
	public IDList getFilteredList(IDList idlist, String fileAndRunFilter, String moduleFilter, String nameFilter) {
		return wrap(super.getFilteredList(idlist, fileAndRunFilter, moduleFilter, nameFilter));
	}
}

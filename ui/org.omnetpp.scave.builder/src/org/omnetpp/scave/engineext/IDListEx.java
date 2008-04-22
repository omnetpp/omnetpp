package org.omnetpp.scave.engineext;

import java.util.ArrayList;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;

/**
 * IDList with notification capability.
 * @author andras
 */
public class IDListEx extends IDList {
	private ArrayList<IIDListChangeListener> listeners = new ArrayList<IIDListChangeListener>();

	public IDListEx(IDList obj) {
		super(IDList.getCPtr(obj), true);
		obj.swigDisown();
	}
	
	public void addListener(IIDListChangeListener listener) {
		listeners.add(listener);
	}

	public void removeListener(IIDListChangeListener listener) {
		listeners.remove(listener);
	}
	
	protected void notifyListeners() {
		for (IIDListChangeListener listener : listeners.toArray(new IIDListChangeListener[listeners.size()]))
			listener.idlistChanged(this);
	}

	@Override
	public void add(long x) {
		super.add(x);
		notifyListeners();
	}

	@Override
	public void erase(int i) {
		super.erase(i);
		notifyListeners();
	}

	@Override
	public void intersect(IDList ids) {
		super.intersect(ids);
		notifyListeners();
	}

	@Override
	public void merge(IDList ids) {
		super.merge(ids);
		notifyListeners();
	}

	@Override
	public void reverse() {
		super.reverse();
		notifyListeners();
	}

	@Override
	public void set(IDList ids) {
		super.set(ids);
		notifyListeners();
	}

//	@Override
//	public void set(int i, long x) {
//		super.set(i, x);
//		notifyListeners();
//	}

	@Override
	public void sortByDirectory(ResultFileManager mgr, boolean ascending) {
		super.sortByDirectory(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortByFileAndRun(ResultFileManager mgr, boolean ascending) {
		super.sortByFileAndRun(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortByRunAndFile(ResultFileManager mgr, boolean ascending) {
		super.sortByRunAndFile(mgr, ascending);
		notifyListeners();
	}
	
	@Override
	public void sortByFileName(ResultFileManager mgr, boolean ascending) {
		super.sortByFileName(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortByModule(ResultFileManager mgr, boolean ascending) {
		super.sortByModule(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortByName(ResultFileManager mgr, boolean ascending) {
		super.sortByName(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortByRun(ResultFileManager mgr, boolean ascending) {
		super.sortByRun(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortScalarsByValue(ResultFileManager mgr, boolean ascending) {
		super.sortScalarsByValue(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortVectorsByLength(ResultFileManager mgr, boolean ascending) {
		super.sortVectorsByLength(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortVectorsByMean(ResultFileManager mgr, boolean ascending) {
		super.sortVectorsByMean(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void sortVectorsByStdDev(ResultFileManager mgr, boolean ascending) {
		super.sortVectorsByStdDev(mgr, ascending);
		notifyListeners();
	}

	@Override
	public void substract(IDList ids) {
		super.substract(ids);
		notifyListeners();
	}

	@Override
	public void substract(long x) {
		super.substract(x);
		notifyListeners();
	}

	@Override
	public IDListEx dup() {
		return new IDListEx(super.dup()); // re-wrap into "Ex" class
	}

}

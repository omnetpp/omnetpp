/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;


import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.Run;

/**
 * Wraps ResultFileManager with change notification capability.
 */
public class ResultFileManagerModel extends ResultFileManager {

    ModelChangeListenerList listeners = new ModelChangeListenerList();

    public void addListener(IModelChangeListener l) {
        listeners.add(l);
    }

    public void removeListener(IModelChangeListener l) {
        listeners.remove(l);
    }

    public Run addRun(File file) {
        Run ret = super.addRun(file);
        listeners.notifyListeners();
        return ret;
    }

    public File addScalarFile() {
        File ret = super.addScalarFile();
        listeners.notifyListeners();
        return ret;
    }

    public File addVectorFile() {
        File ret = super.addVectorFile();
        listeners.notifyListeners();
        return ret;
    }

    public File loadScalarFile(String filename) {
        long t0 = System.currentTimeMillis();
        File ret = super.loadScalarFile(filename);
        System.out.println("loading "+filename+" (native): " + (System.currentTimeMillis()-t0));
        listeners.notifyListeners();
        return ret;
    }

    public File loadVectorFile(String filename) {
        long t0 = System.currentTimeMillis();
        File ret = super.loadVectorFile(filename);
        System.out.println("scanning "+filename+" (native): " + (System.currentTimeMillis()-t0));
        listeners.notifyListeners();
        return ret;
    }

    public void unloadFile(File file) {
        super.unloadFile(file);
        listeners.notifyListeners();
    }
}

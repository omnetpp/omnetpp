/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;

public class Analysis extends ModelObject {
    protected Inputs inputs = new Inputs();
    protected Folder rootFolder = new Folder();

    public Analysis() {
        inputs.parent = this;
        rootFolder.parent = this;
    }

    public Inputs getInputs() {
        return inputs;
    }

    public void setInputs(Inputs inputs) {
        this.inputs.parent = null;
        this.inputs = inputs;
        this.inputs.parent = this;
        notifyListeners();
    }

    public Folder getRootFolder() {
        return rootFolder;
    }

    public void setRootFolder(Folder folder) {
        this.rootFolder.parent = null;
        this.rootFolder = folder;
        this.rootFolder.parent = this;
        notifyListeners();
    }

    public boolean contains(AnalysisItem item) {
        return rootFolder.contains(item);
    }

    public void checkIdUniqueness() {
        checkIdUniqueness(rootFolder, new HashMap<Integer,AnalysisItem>());
    }

    private static void checkIdUniqueness(AnalysisItem item, Map<Integer,AnalysisItem> idsSeen) {
        int id = item.getId();
        if (idsSeen.containsKey(id)) {
            AnalysisItem other = idsSeen.get(id);
            Assert.isTrue(other.getId() == id);
            throw new RuntimeException("id=" + id + " of " + item.toString() + " conflicts with id of " + other.toString());
        }
        idsSeen.put(id, item);
        if (item instanceof Folder)
            for (AnalysisItem child : ((Folder)item).getItems())
                checkIdUniqueness(child, idsSeen);
    }

    @Override
    public Analysis clone() throws CloneNotSupportedException {
        Analysis clone = (Analysis)super.clone();
        clone.inputs = inputs.clone();
        clone.rootFolder = rootFolder.clone();
        return clone;
    }
}


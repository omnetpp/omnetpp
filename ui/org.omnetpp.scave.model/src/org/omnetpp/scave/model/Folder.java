/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.runtime.Assert;

public class Folder extends ModelObject {
    protected List<AnalysisItem> items = new ArrayList<AnalysisItem>();

    public List<AnalysisItem> getItems() {
        return Collections.unmodifiableList(items);
    }

    public void setItems(List<AnalysisItem> items) {
        for (AnalysisItem i : this.items)
            i.parent = null;
        this.items = items;
        for (AnalysisItem i : this.items)
            i.parent = this;

        for (int i = 0; i < items.size(); ++i)
            Assert.isTrue(i == items.indexOf(items.get(i)));

        notifyListeners();
    }

    public void add(AnalysisItem item) {
        add(item, items.size());
    }

    public void add(AnalysisItem item, int index) {
        Assert.isTrue(findById(item.getId()) == null, "ID not unique");
        item.parent = this;
        items.add(index, item);
        notifyListeners();
    }

    public void remove(AnalysisItem item) {
        item.parent = null;
        items.remove(item);
        notifyListeners();
    }

    public AnalysisItem findById(int id) {
        for (AnalysisItem item : items)
            if (item.getId() == id)
                return item;
        return null;
    }

    @Override
    protected Folder clone() throws CloneNotSupportedException {
        Folder clone = (Folder)super.clone();

        clone.items = new ArrayList<AnalysisItem>(items.size());

        for (int i = 0; i < items.size(); ++i)
            clone.items.add(items.get(i).clone());

        return clone;
    }
}

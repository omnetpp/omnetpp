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

public class Folder extends AnalysisItem {
    protected List<AnalysisItem> items = new ArrayList<AnalysisItem>();

    public Folder() {
    }

    public Folder(String name) {
        setName(name);
    }

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
        item.parent = this;
        items.add(index, item);
        notifyListeners();
    }

    public void remove(AnalysisItem item) {
        item.parent = null;
        items.remove(item);
        notifyListeners();
    }

    public AnalysisItem findChildById(int id) {
        for (AnalysisItem item : items)
            if (item.getId() == id)
                return item;
        return null;
    }

    public AnalysisItem findRecursivelyById(int id) {
        for (AnalysisItem item : items) {
            if (item.getId() == id)
                return item;
            if (item instanceof Folder) {
                AnalysisItem found = ((Folder)item).findRecursivelyById(id);
                if (found != null)
                    return found;
            }
        }
        return null;
    }

    public boolean contains(AnalysisItem item) {
        for (; item != null; item = item.getParentFolder())
            if (item == this)
                return true;
        return false;
    }

    public List<Folder> getPathSegments() {
        List<Folder> path = new ArrayList<Folder>();
        Folder folder = this;
        path.add(folder);
        while (folder.getParentFolder() != null) {
            folder = folder.getParentFolder();
            path.add(folder);
        }
        Collections.reverse(path);
        return path;
    }

    public String getPathAsString(String separator) {
        if (getParentFolder() == null)
            return "<root>";
        else {
            String result = null;
            Folder f = this;
            while (f.getParentFolder() != null) {
                if (result == null)
                    result = f.getName();
                else
                    result = f.getName() + separator + result;
                f = f.getParentFolder();
            }
            return result;
        }
    }

    @Override
    protected Folder clone() throws CloneNotSupportedException {
        Folder clone = (Folder)super.clone();

        clone.items = new ArrayList<AnalysisItem>();
        for (int i = 0; i < items.size(); ++i)
            clone.add(items.get(i).clone());

        return clone;
    }
}

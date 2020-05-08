/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.Collections;
import java.util.List;

public class Folder extends AnalysisItem {
    protected List<AnalysisItem> items;

    public List<AnalysisItem> getItems() {
        return Collections.unmodifiableList(items);
    }

    public void setItems(List<AnalysisItem> items) {
        for (AnalysisItem i : this.items)
            i.parent = null;
        this.items = items;
        for (AnalysisItem i : this.items)
            i.parent = this;
        notifyListeners();
    }

    public void addItem(AnalysisItem item) {
        item.parent = this;
        items.add(item);
        notifyListeners();
    }

    public void removeItem(AnalysisItem item) {
        item.parent = null;
        items.remove(item);
        notifyListeners();
    }
}

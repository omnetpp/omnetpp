/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import java.util.HashSet;
import java.util.Random;
import java.util.Set;

import org.omnetpp.common.util.StringUtils;

public abstract class AnalysisItem extends ModelObject {
    protected static Random idSource = new Random(System.nanoTime());
    protected static Set<Integer> usedIds = new HashSet<>();

    protected int id;
    protected String name;

    public AnalysisItem() {
        assignNewId();
    }

    public int getId() {
        return id;
    }

    public boolean setIdIfUnique(int id) {
        if (usedIds.contains(id))
            return false; // retain original, guaranteed unique ID
        this.id = id;
        usedIds.add(id);
        notifyListeners();
        return true;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    public Folder getParentFolder() {
        return parent instanceof Folder ? (Folder)parent : null;
    }

    public void assignNewId() {
        do { id = idSource.nextInt(0, Integer.MAX_VALUE); } while (usedIds.contains(id));
        usedIds.add(id);
    }

    @Override
    protected AnalysisItem clone() throws CloneNotSupportedException {
        return (AnalysisItem)super.clone();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        AnalysisItem other = (AnalysisItem)obj;
        if (id != other.id)
            return false;
        if (name == null) {
            if (other.name != null)
                return false;
        } else if (!name.equals(other.name))
            return false;
        return true;
    }

    @Override
    public String toString() {
        return getClass().getSimpleName() + " '" + StringUtils.nullToEmpty(getName()) + "' (id=" + getId() + ")";
    }

}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

public abstract class AnalysisItem extends ModelObject {
    protected static int nextId = 1;

    protected int id = (nextId++);
    protected String name;

    public int getId() {
        return id;
    }

    public void setId(int id) {
        this.id = id;
        if (id >= nextId)
            nextId = id + 1;
        notifyListeners();
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
        notifyListeners();
    }

    public void assignNewId() {
        setId(nextId++);
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

}

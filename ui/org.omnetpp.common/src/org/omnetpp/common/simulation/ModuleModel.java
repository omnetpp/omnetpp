package org.omnetpp.common.simulation;

import java.util.ArrayList;
import java.util.List;

import org.omnetpp.common.displaymodel.IDisplayString;

public class ModuleModel {
    private int id;
    private String name;
    private int index;
    private int size = -1;
    private ModuleModel parent;
    private String typeName;
    private String className;
    private IDisplayString displayString;
    private List<ModuleModel> submodules;
    private List<GateModel> gates;

    public ModuleModel() {
    }

    public ModuleModel(ModuleModel parent) {
        if (parent != null)
            parent.addSubmodule(this);
    }

    public ModuleModel(ModuleModel parent, int id) {
        this(parent);
        this.id = id;
    }

    public void addSubmodule(ModuleModel module) {
        if (module.getParentModule() != null)
            throw new RuntimeException("submodule already has a parent");
        if (submodules == null)
            submodules = new ArrayList<ModuleModel>(2);
        submodules.add(module);
        module.parent = this;
    }

    public void removeSubmodule(ModuleModel module) {
        if (module.getParentModule() != this)
            throw new RuntimeException("not parent of this submodule");
        submodules.remove(module);
        module.parent = null;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getTypeName() {
        return typeName;
    }

    public void setTypeName(String type) {
        this.typeName = type;
    }

    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

    public int getIndex() {
        return index;
    }

    public void setIndex(int index) {
        this.index = index;
    }

    public boolean isVector() {
        return size!=-1;
    }

    public int getVectorSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public String getFullName() {
        return !isVector() ? name : name+"["+index+"]";
    }

    public String getFullPath() {
        return parent == null ? getFullName() : parent.getFullPath() + "." + getFullName();
    }

    public ModuleModel getParentModule() {
        return parent;
    }

    public List<ModuleModel> getSubmodules() {
        return submodules;
    }

    public IDisplayString getDisplayString() {
        return displayString;
    }
    public void setDisplayString(IDisplayString displayString) {
        this.displayString = displayString;
    }

    public int getId() {
        return id;
    }

    public void setId(int moduleId) {
        this.id = moduleId;
    }

    public ModuleModel getSubmodule(String name) {
        // TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
        if (submodules != null)
            for (ModuleModel m : submodules)
                if (name.equals(m.getName()) && m.getVectorSize()<0)
                    return m;

        return null;
    }

    public ModuleModel getSubmodule(String name, int index) {
        // TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
        if (submodules != null)
            for (ModuleModel m : submodules)
                if (name.equals(m.getName()) && m.getIndex()==index)
                    return m;
        return null;
    }

    // TODO: this method to be removed -- ID is globally unique so it makes no sense
    public ModuleModel getSubmoduleByID(int id) {
        if (submodules != null)
            for (ModuleModel submodule : submodules)
                if (submodule.getId() == id)
                    return submodule;
        return null;
    }

    // TODO: removeGate, more efficient storage, etc
    public void addGate(GateModel gate) {
        if (gates == null)
            gates = new ArrayList<GateModel>(2);
        gates.add(gate);
    }

    public GateModel getGate(int i) {
        return gates.get(index);
    }

    public int getNumGates() {
        if (gates == null)
            return 0;
        else
            return gates.size();
    }

    public GateModel getGateById(int id) {
        if (gates != null)
            for (GateModel gate : gates)
                if (gate.getId() == id)
                    return gate;
        return null;
    }

    public ModuleModel getCommonAncestorModule(ModuleModel otherModule) {
        if (isAncestorModuleOf(otherModule))
            return this;
        else if (otherModule.isAncestorModuleOf(this))
            return otherModule;
        else
            return getCommonAncestorModule(otherModule.getParentModule());
    }

    public boolean isAncestorModuleOf(ModuleModel otherModule) {
        while (otherModule != null) {
            if (otherModule.equals(this))
                return true;
            else
                otherModule = otherModule.getParentModule();
        }
        return false;
    }

    @Override
    public String toString() {
        return typeName + "(" + id + ")";
    }

    @Override
    public int hashCode() {
        final int PRIME = 31;
        int result = 1;
        result = PRIME * result + id;
        return result;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        final ModuleModel other = (ModuleModel) obj;
        if (id != other.id)
            return false;
        return true;
    }
}

package org.omnetpp.simulation.model.c;

public class cQueue extends cObject {
    private ModuleModel ownerModule;
    private int size;
    private int count;

    public cQueue(ModuleModel ownerModule, String name, int size) {
        super(name);
        this.ownerModule = ownerModule;
        this.size = size;
    }

    public ModuleModel getOwnerModule() {
        return ownerModule;
    }

    public void setOwnerModule(ModuleModel ownerModule) {
        this.ownerModule = ownerModule;
    }

    public int getSize() {
        return size;
    }

    public void setSize(int size) {
        this.size = size;
    }

    public int getCount() {
        return count;
    }

    public void setCount(int count) {
        this.count = count;
    }
}

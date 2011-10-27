package org.omnetpp.common.simulation;

public class QueueModel {
    private ModuleModel ownerModule;
    private String name;
    private int size;
    private int count;

    public QueueModel(ModuleModel ownerModule, String name, int size) {
        this.ownerModule = ownerModule;
        this.name = name;
        this.size = size;
    }

    public ModuleModel getOwnerModule() {
        return ownerModule;
    }

    public void setOwnerModule(ModuleModel ownerModule) {
        this.ownerModule = ownerModule;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
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

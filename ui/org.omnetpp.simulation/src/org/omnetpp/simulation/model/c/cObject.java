package org.omnetpp.simulation.model.c;

public class cObject {
    private String name;
    
    public cObject(String name) {
        this.name = name;
    }
    
    public String getName() {
        return name;
    }

    public String getFullName() {
        return name + "[0]";  //TODO
    }

    public String getFullPath() {
        return "todo.host." + getFullName();  //TODO
    }

    public String getClassName() {
        return getClass().getSimpleName(); //TODO
    }

    public String info() {
        return "<info string todo>"; //TODO
    }
    
    public cObject[] getChildObjects() {
        return new cObject[0];
    }
    
    public boolean isZombie() {
        return false; //TODO kiirtani
    }
}

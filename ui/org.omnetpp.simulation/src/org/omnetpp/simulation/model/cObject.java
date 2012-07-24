package org.omnetpp.simulation.model;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.omnetpp.simulation.controller.SimulationController;

/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
//TODO ha a getterek beallitananak egy "accessed" flaget, ezt fel lehetne hasznalni arra, hogy az erdektelen objektumokat unloadoljuk a controllerben refreshStatus()-kor!!!
//TODO illetve ha az unload() kinullazza a benne levo referenciakat (es az object cache is weakref-eket tart), akkor a nem hivatkozott objektumok tudnak garbage collectalodni is!!! 
public class cObject {
    private SimulationController controller;
    private long objectId;
    private boolean isFilledIn = false;
    private boolean isFieldsFilledIn = false;
    private boolean isDisposed = false;

    public long lastAccessSerial; // SimulationController's refreshSerial when this object was last accessed; XXX obsolete, should be removed!

    private String className;
    private String name;
    private String fullName;
    private String fullPath;
    private String icon;
    private String info;
    private cObject owner;
    private cObject[] childObjects;

    public static class Field {
        public String name;
        public String type;
        public String declaredOn;
        public boolean isArray;
        public boolean isCompound;
        public boolean isPointer;
        public boolean isCObject;
        public boolean isCOwnedObject;
        public boolean isEditable;
        public String structName;
        public Object value;
        public Object[] values;
    }
    
    private Field[] fields;  //XXX temporary
    
    public cObject(SimulationController controller, long id) {
        this.controller = controller;
        this.objectId = id;
    }

    public SimulationController getController() {
        return controller;
    }
    
    public long getObjectId() {
        return objectId;
    }

    public boolean isFilledIn() {
        return isFilledIn;
    }

    public boolean isDisposed() {
        return isDisposed;
    }

    // for SimulationController's use only!
    public void fillFromJSON(Map jsonObject) {
        doFillFromJSON(jsonObject);
        isFilledIn = true;
    }
    
    protected void doFillFromJSON(Map jsonObject) {
        className = (String) jsonObject.get("className");
        name = (String) jsonObject.get("name");
        fullName = (String) jsonObject.get("fullName");
        fullPath = (String) jsonObject.get("fullPath");
        icon = (String) jsonObject.get("icon");
        info = (String) jsonObject.get("info");
        owner = controller.getObjectByJSONRef((String) jsonObject.get("owner"));

        List jsonChildren = (List) jsonObject.get("children");
        childObjects = new cObject[jsonChildren.size()];
        for (int i = 0; i < childObjects.length; i++)
            childObjects[i] = controller.getObjectByJSONRef((String) jsonChildren.get(i));
    }
        
    public void load() throws IOException {
        if (isDisposed) 
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        controller.loadObject(this);
    }
    
    public void safeLoad() {
        // convenience method
        try {
            load();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    public void loadFields() throws IOException {
        if (isDisposed) 
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        controller.loadObjectFields(this);
    }
    
    public void safeLoadFields() {
        // convenience method
        try {
            loadFields();
        }
        catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    //FIXME revise
    public void fillFieldsFromJSON(Map jsonObject) {
        List<Field> fields = new ArrayList<Field>();
        for (Object o: (List)jsonObject.get("fields")) {
            Map jfield = (Map)o;
            Field f = new Field();
            f.name = (String) jfield.get("name");
            f.type = (String) jfield.get("type");
            f.declaredOn = (String) jfield.get("declaredOn");
            f.isArray = Boolean.TRUE.equals((Boolean)jfield.get("isArray"));
            f.isCompound = Boolean.TRUE.equals((Boolean)jfield.get("isCompound"));
            f.isPointer = Boolean.TRUE.equals((Boolean)jfield.get("isPointer"));
            f.isCObject = Boolean.TRUE.equals((Boolean)jfield.get("isCObject"));
            f.isCOwnedObject = Boolean.TRUE.equals((Boolean)jfield.get("isCOwnedObject"));
            f.isEditable = Boolean.TRUE.equals((Boolean)jfield.get("isEditable"));
            f.structName = (String) jfield.get("declaredOn");
            f.value = f.isArray ? null : jfield.get("value");
            f.values = f.isArray ? ((List) jfield.get("value")).toArray() : null;
            fields.add(f);
        }
        this.fields = fields.toArray(new Field[]{});
        isFieldsFilledIn = true;
    }

    //FIXME revise, rename, move,...
    public boolean isFieldsFilledIn() {
        return isFieldsFilledIn;
    }

    //FIXME revise
    public Field[] getFields() {
        checkState();
        if (!isFieldsFilledIn) 
            throw new InvalidSimulationObjectException("fields of object " + getObjectId() + "-" + getClass().getSimpleName() + " not yet filled in");
        return fields;
    }
    
    public void markAsDisposed() {
        isDisposed = true;
    }

    protected void checkState() {
        if (!isFilledIn) 
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is not yet filled in");
        if (isDisposed) 
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
    }
    
    public String getClassName() {
        checkState();
        return className;
    }
    
    public String getIcon() {
        checkState();
        return icon;
    }
    
    public String getName() {
        checkState();
        return name;
    }
    
    public String getFullName() {
        checkState();
        return fullName;
    }
    
    public String getFullPath() {
        checkState();
        return fullPath;
    }
    
    public String getInfo() {
        checkState();
        return info;
    }
    
    public cObject getOwner() {
        checkState();
        return owner;
    }

    public cObject[] getChildObjects() {
        checkState();
        return childObjects;
    }
    
    @Override
    public int hashCode() {
        return (int) (objectId ^ (objectId >>> 32)) + 31*controller.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        cObject other = (cObject) obj;
        return controller == other.controller && objectId == other.objectId;
    }

    @Override
    public String toString() {
        if (!isFilledIn)
            return "[id=" + objectId + ",unfilled]";
        else if (isDisposed)
            return "[id=" + objectId + ",disposed]";
        else
            return "(" + className + ") " + fullName + "[id=" + objectId + "]";
    }
}

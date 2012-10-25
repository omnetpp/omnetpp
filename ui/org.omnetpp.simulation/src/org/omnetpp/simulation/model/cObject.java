package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;


/**
 * Represents a cObject in the simulation.
 * @author Andras
 */
//TODO implement frozen objects!!!! freeze(), isFrozen();  references inside frozen objects: freeze them too, even if they are unfilled?
public class cObject {
    private Simulation simulation;
    private long objectId;
    private boolean isFilledIn = false;
    private boolean isFieldsFilledIn = false;
    private boolean isDisposed = false; //TODO deleted (~C++)

    private cClassDescriptor descriptor;
    private String className;
    private String name;
    private String fullName;
    private String fullPath;
    private String info;
    private cObject owner;
    private String icon; // the @icon property - cached here for efficiency
    private cObject[] childObjects;

    private Field[] fields;
    private long lastAccessEventNumber;

    public cObject(Simulation simulation, long id) {
        this.simulation = simulation;
        this.objectId = id;
    }

    public Simulation getSimulation() {
        return simulation;
    }

    public long getObjectId() {
        return objectId;
    }

    /**
     * Note: deleted objects are also 'not filled in'.
     */
    public boolean isFilledIn() {
        return isFilledIn;
    }

    public boolean isDisposed() {
        return isDisposed;
    }

    public long getLastAccessEventNumber() {
        return lastAccessEventNumber;
    }

    // technically public, but should only be called from the Simulation class!
    @SuppressWarnings("rawtypes")
    public void fillFromJSON(Map jsonObject) {
        doFillFromJSON(jsonObject);
        isFilledIn = true;
    }

    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        className = (String) jsonObject.get("className");
        name = (String) jsonObject.get("name");
        fullName = (String) jsonObject.get("fullName");
        fullPath = (String) jsonObject.get("fullPath");
        info = (String) jsonObject.get("info");
        owner = simulation.getObjectByJSONRef((String) jsonObject.get("owner"));
        icon = (String) jsonObject.get("icon");
        descriptor = (cClassDescriptor) simulation.getObjectByJSONRef((String) jsonObject.get("descriptor"));

        List jsonChildren = (List) jsonObject.get("children");
        childObjects = new cObject[jsonChildren.size()];
        for (int i = 0; i < childObjects.length; i++)
            childObjects[i] = simulation.getObjectByJSONRef((String) jsonChildren.get(i));
    }

    public void loadIfUnfilled() throws CommunicationException {
        if (!isFilledIn())
            load();
    }

    public void load() throws CommunicationException {
        if (isDisposed) {
            Assert.isTrue(simulation.isInFailureMode()); // otherwise it should not occur!
            // Note: we use CommunicationException (not InvalidSimulationObjectException) because we want it to be a *checked* exception, and handled together with other CommunicationExceptions
            throw new CommunicationException("refusing to load object " + getObjectId() + "-" + getClass().getSimpleName() + " because it is already deleted");
        }
        simulation.loadObject(this);
    }

    public void unload() {
        System.out.println("@@@ unloading: " + toString());
        Assert.isTrue(isFilledIn && !isDisposed);
        isFilledIn = false;
        isFieldsFilledIn = false;
        clearReferences(); // clear references to other objects, to allow them to be garbage collected
    }

    protected void clearReferences() {
        owner = null;
        childObjects = null;
        fields = null;
    }

    public void loadFieldsIfUnfilled() throws CommunicationException {
        if (!isFieldsFilledIn())
            loadFields();
        descriptor.loadIfUnfilled(); // for convenience -- will be needed for accessing the fields anyway
    }

    public void loadFields() throws CommunicationException {
        if (isDisposed)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        descriptor.loadIfUnfilled(); // for convenience -- will be needed for accessing the fields anyway
        simulation.loadObjectFields(this);
    }

    @SuppressWarnings("rawtypes")
    public void fillFieldsFromJSON(Map jsonObject) {
        // IMPORTANT: we MUST NOT access cClassDescriptor here, because it might not be loaded at this point!
        // We must be able to load fields without cClassDescriptor present, and have it loaded only later.

        List jsonFields = (List) jsonObject.get("fields");
        int numFields = jsonFields.size();
        boolean haveDesc = isFilledIn() && getDescriptor().isFilledIn();
        if (haveDesc)
            Assert.isTrue(numFields == getDescriptor().getFieldDescriptors().length);  // basic sanity check
        fields = new Field[numFields];

        for (int fieldId = 0; fieldId < numFields; fieldId++) {
            Map jsonField = (Map) jsonFields.get(fieldId);
            if (haveDesc)
                Assert.isTrue(getDescriptor().getFieldDescriptor(fieldId).getName().equals(jsonField.get("name")));  // basic sanity check
            boolean isObjRef = Boolean.TRUE.equals((Boolean)jsonField.get("isObjRef"));
            Field field;
            if (jsonField.containsKey("value")) {
                Object value = jsonField.get("value");
                if (isObjRef)
                    value = simulation.getObjectByJSONRef((String)value);
                field = new Field(this, fieldId, value);
            }
            else {
                Object[] values = ((List)jsonField.get("values")).toArray();
                if (isObjRef)
                    for (int i = 0; i < values.length; i++)
                        values[i] = simulation.getObjectByJSONRef((String)values[i]);
                field = new Field(this, fieldId, values);
            }
            fields[fieldId] = field;
        }

        // success
        isFieldsFilledIn = true;
    }

    /**
     * Note: deleted objects are also 'not filled in'.
     */
    public boolean isFieldsFilledIn() {
        return isFieldsFilledIn;
    }

    public Field[] getFields() {
        checkState();
        if (!isFieldsFilledIn)
            throw new InvalidSimulationObjectException("fields of object " + getObjectId() + "-" + getClass().getSimpleName() + " not yet filled in");
        if (!descriptor.isFilledIn())
            throw new RuntimeException("must load the class descriptor (cClassDescriptor) of the object to access its fields");
        return fields;
    }

    public Field getField(String name) {
        Field[] fields = getFields();
        int fieldId = getFieldIndex(name);
        return fieldId == -1 ? null : fields[fieldId];
    }

    public int getFieldIndex(String name) {
        checkState();
        return descriptor.getFieldDescriptorIndex(name);
    }

    public void markAsDisposed() {
        isDisposed = true;
        isFilledIn = isFieldsFilledIn = false;
        clearReferences(); // allow other objects to be garbage collected
    }

    protected void checkState() {
        if (!isFilledIn)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is not yet filled in");
        if (isDisposed)
            throw new InvalidSimulationObjectException("object " + getObjectId() + "-" + getClass().getSimpleName() + " is already deleted");
        lastAccessEventNumber = simulation.getLastEventNumber();
    }

    public String getClassName() {
        checkState();
        return className;
    }

    /**
     * Utility function: it returns a short type name for the object suitable
     * for displaying on the GUI, doing best effort. For most objects it returns
     * the C++ class name; for modules and channels, it tries to obtain and
     * return the NED type name without package. If the NED type cannot be
     * obtained, the method falls back to the C++ class name.
     */
    public String getShortTypeName() {
        String className = getClassName();
        return !className.contains("::") ? className : StringUtils.substringAfterLast(className, "::"); // remove C++ namespace qualifiers
    }

    public String getIcon() {
        checkState();
        return icon; // in theory the same as getDescriptor().getProperty("icon"), but we access it frequently, and even before class descriptor is loaded
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

    public cClassDescriptor getDescriptor() {
        checkState();
        return descriptor;
    }

    public cObject[] getChildObjects() {
        checkState();
        return childObjects;
    }

    @Override
    public int hashCode() {
        return (int) (objectId ^ (objectId >>> 32)) + 31*simulation.hashCode();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null || getClass() != obj.getClass())
            return false;
        cObject other = (cObject) obj;
        return simulation == other.simulation && objectId == other.objectId;
    }

    @Override
    public String toString() {
        if (!isFilledIn)
            return "[oid=" + objectId + ",unfilled]";
        else if (isDisposed)
            return "[oid=" + objectId + ",disposed]";
        else
            return "(" + className + ") " + fullPath + "[oid=" + objectId + "]";
    }
}

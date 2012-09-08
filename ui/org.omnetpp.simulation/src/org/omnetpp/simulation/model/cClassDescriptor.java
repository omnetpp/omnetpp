package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;
import org.omnetpp.simulation.controller.Simulation;

/**
 * TODO
 * 
 * @author Andras
 */
public class cClassDescriptor extends cObject {
    private cClassDescriptor baseClassDescriptor;
    private boolean extendsCObject;
    private Map<String, String> properties;
    private FieldDescriptor[] fieldDescriptors;

    public cClassDescriptor(Simulation simulation, long id) {
        super(simulation, id);
    }

    public cClassDescriptor getBaseClassDescriptor() {
        return baseClassDescriptor;
    }

    public boolean isExtendsCObject() {
        checkState();
        return extendsCObject;
    }

    public Map<String, String> getProperties() {
        checkState();
        return properties;
    }

    public String getProperty(String name) {
        checkState();
        return properties.get(name);
    }

    public FieldDescriptor[] getFieldDescriptors() {
        checkState();
        return fieldDescriptors;
    }

    public FieldDescriptor getFieldDescriptor(int fieldId) {
        checkState();
        return fieldDescriptors[fieldId];
    }

    @Override
    @SuppressWarnings({ "rawtypes", "unchecked" })
    public void fillFromJSON(Map jsonObject) {
        super.fillFromJSON(jsonObject);
        baseClassDescriptor = (cClassDescriptor) getSimulation().getObjectByJSONRef((String) jsonObject.get("baseClassDescriptor"));
        extendsCObject = Boolean.TRUE.equals((Boolean)jsonObject.get("extendsCObject"));
        properties = (Map<String, String>) jsonObject.get("properties");  //TODO check it's really String->String

        List jsonFields = (List) jsonObject.get("fields");
        fieldDescriptors = new FieldDescriptor[jsonFields.size()];
        for (int i = 0; i < fieldDescriptors.length; i++) {
            Map jsonField = (Map)jsonFields.get(i);
            fieldDescriptors[i] = new FieldDescriptor(jsonField);
        }
    }

}

package org.omnetpp.scave.writers.example;

import java.util.HashMap;
import java.util.Map;

import org.omnetpp.scave.writers.IOutputScalarManager;
import org.omnetpp.scave.writers.IOutputVector;

public class Component {
    private SimulationManager sim;
    private Component parent;
    private String name;

    public Component(String name, SimulationManager sim, Component parent) {
        this.name = name;
        this.sim = sim;
        this.parent = parent;
        sim.components.add(this);
    }

    public String getFullPath() {
        return parent==null ? name : parent.getFullPath() + "." + name;
    }

    public void schedule(double time, Event e) {
        e.time = time;
        sim.fes.add(e);
    }

    public double now() {
        return sim.now;
    }

    protected void recordSummaryResults() {
        // OVERRIDE AND ADD recordScalar() CALLS
    }

    public void recordScalar(String name, double value) {
        recordScalar(name, value, (Map<String,String>)null);
    }

    public void recordScalar(String name, double value, String unit) {
        Map<String,String> attributes = new HashMap<String, String>();
        attributes.put(IOutputScalarManager.ATTR_UNIT, unit);
        recordScalar(name, value, attributes);
    }

    public void recordScalar(String name, double value, Map<String,String> attributes) {
        sim.scalarManager.recordScalar(getFullPath(), name, value, attributes);
    }

    public IOutputVector createOutputVector(String name) {
        return createOutputVector(name, null);
    }

    public IOutputVector createOutputVector(String name, String unit, String type, String interpolationMode) {
        Map<String,String> attributes = new HashMap<String, String>();
        if (unit != null)
            attributes.put(IOutputScalarManager.ATTR_UNIT, unit);
        if (type != null)
            attributes.put(IOutputScalarManager.ATTR_TYPE, type);
        if (interpolationMode != null)
            attributes.put(IOutputScalarManager.ATTR_INTERPOLATIONMODE, interpolationMode);
        return sim.vectorManager.createVector(getFullPath(), name, attributes);
    }

    public IOutputVector createOutputVector(String name, String enumName, String[] names, int[] values) {
        StringBuilder enumDesc = new StringBuilder();
        for (int i=0; i<names.length; i++)
            enumDesc.append((i==0?"":",") + names[i] + "=" + values[i]);
        Map<String,String> attributes = new HashMap<String, String>();
        attributes.put(IOutputScalarManager.ATTR_ENUMNAME, enumName);
        attributes.put(IOutputScalarManager.ATTR_ENUM, enumDesc.toString());
        return sim.vectorManager.createVector(getFullPath(), name, attributes);
    }

    public IOutputVector createOutputVector(String name, Map<String,String> attributes) {
        return sim.vectorManager.createVector(getFullPath(), name, attributes);
    }
}

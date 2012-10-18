package org.omnetpp.simulation.model;

import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

public class cPar extends cObject {
    private String value; // value or expression, in string form (string literals WITH quotes)
    private String type;
    private boolean isVolatile;
    private String unit;
    // private cProperties properties; -- not serialized yet

    public cPar(Simulation simulation, long id) {
        super(simulation, id);
    }

    public String getValue() {
        checkState();
        return value;
    }

    public String getType() {
        checkState();
        return type;
    }

    public boolean isVolatile() {
        checkState();
        return isVolatile;
    }

    public String getUnit() {
        checkState();
        return unit;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        value = (String) jsonObject.get("value");
        type = (String) jsonObject.get("type");
        isVolatile = (Boolean) jsonObject.get("isVolatile");
        unit = (String) jsonObject.get("unit");
    }

    @Override
    protected void clearReferences() {
        super.clearReferences();
        //properties = null;
    }
}

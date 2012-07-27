package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;

import org.omnetpp.simulation.controller.SimulationController;

/**
 * TODO
 * @author Andras
 */
public class cModule extends cComponent {
    private int moduleId;  // module ID
    private int index;
    private int vectorSize; // -1 if not vector
    private cGate[] gates;
    private cModule[] submodules;

    public cModule(SimulationController controller, long id) {
        super(controller, id);
    }

    public int getId() {
        checkState();
        return moduleId;
    }

    public int getIndex() {
        checkState();
        return index;
    }

    public boolean isVector() {
        checkState();
        return vectorSize != -1;
    }

    public int getVectorSize() {
        checkState();
        return vectorSize;
    }

    public cModule[] getSubmodules() {
        checkState();
        return submodules;
    }

    public cModule getSubmodule(String name) {
        // TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
        checkState();
        if (submodules != null)
            for (cModule m : submodules)
                if (name.equals(m.getName()) && m.getVectorSize()<0)
                    return m;

        return null;
    }

    public cModule getSubmodule(String name, int index) {
        // TODO: use more efficient data structure to avoid linear search! e.g. map of submodule vectors etc
        checkState();
        if (submodules != null)
            for (cModule m : submodules)
                if (name.equals(m.getName()) && m.getIndex()==index)
                    return m;
        return null;
    }

    public cGate[] getGates() {
        return gates;
    }

    public cGate getGate(int i) {
        checkState();
        return gates[i];
    }

    public int getNumGates() {
        checkState();
        return gates.length;
    }

    public cGate getGateById(int id) {
        checkState();
        if (gates != null)
            for (cGate gate : gates)
                if (gate.getId() == id)
                    return gate;
        return null;
    }

    public cModule getCommonAncestorModule(cModule otherModule) {
        checkState();
        if (isAncestorModuleOf(otherModule))
            return this;
        else if (otherModule.isAncestorModuleOf(this))
            return otherModule;
        else
            return getCommonAncestorModule(otherModule.getParentModule());
    }

    public boolean isAncestorModuleOf(cModule otherModule) {
        checkState();
        while (otherModule != null) {
            if (otherModule.equals(this))
                return true;
            else
                otherModule = otherModule.getParentModule();
        }
        return false;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        moduleId = ((Number)jsonObject.get("moduleId")).intValue();
        index = ((Number)jsonObject.get("index")).intValue();
        vectorSize = ((Number)jsonObject.get("vectorSize")).intValue();

        List jsonGates = (List) jsonObject.get("gates");
        gates = new cGate[jsonGates.size()];
        for (int i = 0; i < gates.length; i++)
            gates[i] = (cGate) getController().getObjectByJSONRef((String) jsonGates.get(i));

        List jsonSubmodules = (List) jsonObject.get("submodules");
        submodules = new cModule[jsonSubmodules.size()];
        for (int i = 0; i < submodules.length; i++)
            submodules[i] = (cModule) getController().getObjectByJSONRef((String) jsonSubmodules.get(i));

    }
}

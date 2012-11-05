package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;

import org.apache.commons.lang.StringUtils;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.simulation.controller.CommunicationException;
import org.omnetpp.simulation.controller.Simulation;

/**
 * TODO
 * @author Andras
 */
public class cComponent extends cObject {
    private cComponentType componentType;
    private String shortTypeName;
    private cModule parentModule;
    private cPar[] parameters;
    private IDisplayString displayString;

    public cComponent(Simulation simulation, long id) {
        super(simulation, id);
    }

    public cComponentType getComponentType() {
        checkState();
        return componentType;
    }

    /**
     * Returns the component's fully qualified NED type name, or null if the
     * information for the NED type could not be obtained.
     */
    public String getNedTypeName() {
        checkState();
        if (componentType == null)
            return null;
        try {
            componentType.loadIfUnfilled();
            return componentType.getFullName();
        }
        catch (CommunicationException e) {
            return null;
        }
    }

    @Override
    public String getShortTypeName() {
        if (shortTypeName == null) {
            // computed here and cached; discarded on every refresh
            String nedTypeName = getNedTypeName();
            if (nedTypeName == null)
                return super.getShortTypeName() + "?"; // but do not cache, as module are rarely refreshed
            shortTypeName = !nedTypeName.contains(".") ? nedTypeName : StringUtils.substringAfterLast(nedTypeName, ".");
        }
        return shortTypeName;
    }

    public cModule getParentModule() {
        checkState();
        return parentModule;
    }

    public IDisplayString getDisplayString() {
        checkState();
        return displayString;
    }

    public cPar[] getParameters() {
        checkState();
        return parameters;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
System.out.println("******* REFRESHING " + toString());

        super.doFillFromJSON(jsonObject);

        componentType = (cComponentType) getSimulation().getObjectByJSONRef((String) jsonObject.get("componentType"));
        shortTypeName = null; // computed on demand
        parentModule = (cModule) getSimulation().getObjectByJSONRef((String) jsonObject.get("parentModule"));
        displayString = new DisplayString((String) jsonObject.get("displayString"));

        List jsonParameters = (List) jsonObject.get("parameters");
        parameters = new cPar[jsonParameters.size()];
        for (int i = 0; i < parameters.length; i++)
            parameters[i] = (cPar) getSimulation().getObjectByJSONRef((String) jsonParameters.get(i));
    }

    protected void clearReferences() {
        super.clearReferences();
        componentType = null;
        parentModule = null;
        parameters = null;
    }

}

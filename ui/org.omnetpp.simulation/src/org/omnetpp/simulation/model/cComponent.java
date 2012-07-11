package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;

import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.simulation.controller.SimulationController;

/**
 * TODO
 * @author Andras
 */
public class cComponent extends cObject {
	private cModule parentModule;
	private cPar[] parameters;
	private IDisplayString displayString;

    public cComponent(SimulationController controller, long id) {
        super(controller, id);
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
	protected void doFillFromJSON(Map jsonObject) {
	    super.doFillFromJSON(jsonObject);

	    parentModule = (cModule) getController().getObjectByJSONRef((String) jsonObject.get("parentModule"));
	    displayString = new DisplayString((String) jsonObject.get("displayString"));

        List jsonParameters = (List) jsonObject.get("parameters");
        parameters = new cPar[jsonParameters.size()];
        for (int i = 0; i < parameters.length; i++)
            parameters[i] = (cPar) getController().getObjectByJSONRef((String) jsonParameters.get(i));
	}
}

package org.omnetpp.runtimeenv.handlers;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import org.eclipse.core.commands.AbstractHandler;
import org.eclipse.core.commands.ExecutionEvent;
import org.eclipse.core.commands.ExecutionException;
import org.eclipse.jface.viewers.ArrayContentProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.ui.dialogs.ListDialog;
import org.omnetpp.experimental.simkernel.swig.Simkernel;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cModuleType;
import org.omnetpp.experimental.simkernel.swig.cRegistrationList;
import org.omnetpp.experimental.simkernel.swig.cSimulation;
import org.omnetpp.runtimeenv.Activator;
import org.omnetpp.runtimeenv.SimulationManager;


/**
 * Handler for the "Setup Inifile Configuration" action
 * @author Andras
 */
public class SetupNetworkHandler extends AbstractHandler {

    public SetupNetworkHandler() {
	}

	public Object execute(ExecutionEvent event) throws ExecutionException {
		SimulationManager simulationManager = Activator.getSimulationManager();
		if (simulationManager.checkRunning())
			return null;

    	String localPackage = simulationManager.getLocalPackage();
    	List<cModuleType> networkTypes = getNetworkTypes();

    	List<String> networkNames = new ArrayList<String>();
    	List<String> localNetworkNames = new ArrayList<String>();

    	for (cModuleType net : networkTypes) {
    		String networkName = net.getName();
    		String networkQName = net.getFullName();
            if (networkQName.equals(localPackage + "." + networkName))
                localNetworkNames.add(networkName);
            else
                networkNames.add(networkQName);
    	}
    	Collections.sort(networkNames);  //XXX should really be dictionary short...
    	Collections.sort(localNetworkNames);

    	networkNames.addAll(0, localNetworkNames);

    	ListDialog dialog = new ListDialog(null); //XXX parent!
    	dialog.setContentProvider(new ArrayContentProvider());
    	dialog.setLabelProvider(new LabelProvider());
    	dialog.setInput(networkNames);
    	dialog.setTitle("Select Network");
    	dialog.setMessage("Select network to set up:");
		if (dialog.open() == ListDialog.OK && dialog.getResult().length > 0) {
			String networkName = (String)dialog.getResult()[0];
			//XXX next stuff should be using a progress monitor...
			simulationManager.newNetwork(networkName);
			cModule systemModule = cSimulation.getActiveSimulation().getSystemModule();
			if (systemModule != null)
				Activator.openInspector2(systemModule, true);
		}
	    return null;
	}

    //XXX to some util class	
	public static List<cModuleType> getModuleTypes() {
		List<cModuleType> result = new ArrayList<cModuleType>();
    	cRegistrationList componentTypes = Simkernel.getRegisteredComponentTypes();
		for (int i=0; i < componentTypes.size(); i++) {
			cModuleType moduleType = cModuleType.cast(componentTypes.get(i));
			if (moduleType != null)
				result.add(moduleType);
		}
		return result;
	}

	//XXX to some util class	
	public static List<cModuleType> getNetworkTypes() {
		List<cModuleType> result = new ArrayList<cModuleType>();
    	cRegistrationList componentTypes = Simkernel.getRegisteredComponentTypes();
		for (int i=0; i < componentTypes.size(); i++) {
			cModuleType moduleType = cModuleType.cast(componentTypes.get(i));
			if (moduleType != null && moduleType.isNetwork())
				result.add(moduleType);
		}
		return result;
	}
}

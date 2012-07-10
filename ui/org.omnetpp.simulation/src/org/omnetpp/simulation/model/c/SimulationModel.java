package org.omnetpp.simulation.model.c;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class SimulationModel {
	private ModuleModel rootModule;
	private HashMap<Integer, ModuleModel> idToModuleMap = new HashMap<Integer, ModuleModel>();
	private HashMap<String, ModuleModel> pathToModuleMap = new HashMap<String, ModuleModel>();

	// KLUDGE: TODO: remove this and create a generic, extensible root model
	private Object routingTable;
	public void setRoutingTable(Object routingTable) {
        this.routingTable = routingTable;
    }
	public Object getRoutingTable() {
        return routingTable;
    }

    // KLUDGE: TODO: remove this and create a generic, extensible root model
    private Object interfaceTable;
    public void setInterfaceTable(Object interfaceTable) {
        this.interfaceTable = interfaceTable;
    }
    public Object getInterfaceTable() {
        return interfaceTable;
    }

	public SimulationModel(ModuleModel rootModule) {
		this.rootModule = rootModule;
		addModule(rootModule);
	}

    public int getModuleCount() {
        return idToModuleMap.size();
    }

	/* to be called after module creation */
	public void addModule(ModuleModel module) {
		idToModuleMap.put(module.getId(), module);
		pathToModuleMap.put(module.getFullPath(), module);
	}

    public void removeModule(ModuleModel module) {
        removeModule(module.getId());
    }

	public void removeModule(int id) {
		ModuleModel module = getModuleById(id);
		idToModuleMap.remove(id);
		pathToModuleMap.remove(module.getFullPath());
	}

	public ModuleModel getRootModule() {
		return rootModule;
	}

	public ModuleModel getModuleById(int id) {
		return idToModuleMap.get(id);
	}

	public ModuleModel getModuleByPath(String fullPath) {
		return pathToModuleMap.get(fullPath);
	}

    public Map<ModuleModel, ModuleModel> findShortestPaths(ModuleModel sourceModule, ModuleModel targetModule) {
        Set<ModuleModel> unsettledModules = new HashSet<ModuleModel>();
        Map<ModuleModel, ModuleModel> predecessorModules = new HashMap<ModuleModel, ModuleModel>();
        Map<ModuleModel, Integer> distances = new HashMap<ModuleModel, Integer>();
        unsettledModules.addAll(idToModuleMap.values());
        for (ModuleModel moduleModel : unsettledModules)
            distances.put(moduleModel, Integer.MAX_VALUE);
        distances.put(sourceModule, 0);
        while (unsettledModules.size() > 0) {
            int selectedDistance = Integer.MAX_VALUE;
            ModuleModel selectedModule = null;
            for (ModuleModel module : unsettledModules) {
                Integer distance = distances.get(module);
                if (distance <= selectedDistance) {
                    selectedDistance = distance;
                    selectedModule = module;
                }
            }
            if (selectedDistance == Integer.MAX_VALUE || (targetModule != null && selectedDistance > distances.get(targetModule)))
                break;
            unsettledModules.remove(selectedModule);
            for (ModuleModel neighborModule : getNeighbors(selectedModule)) {
                int neighborDistance = distances.get(neighborModule);
                if (selectedDistance + 1 < neighborDistance) {
                    distances.put(neighborModule, selectedDistance + 1);
                    predecessorModules.put(neighborModule, selectedModule);
                }
            }
        }
        return predecessorModules;
    }

    public ArrayList<ModuleModel> findShortestPathBetween(ModuleModel sourceModule, ModuleModel targetModule) {
        return getPath(findShortestPaths(sourceModule, targetModule), targetModule);
    }

    protected List<ModuleModel> getNeighbors(ModuleModel moduleModel) {
        ArrayList<ModuleModel> neighbors = new ArrayList<ModuleModel>();
        for (int i = 0; i < moduleModel.getNumGates(); i++) {
            GateModel gateModel = moduleModel.getGate(i);
            ConnectionModel incomingConnectionModel = gateModel.getIncomingConnection();
            if (incomingConnectionModel != null && !neighbors.contains(incomingConnectionModel.getSourceModule()))
                neighbors.add(incomingConnectionModel.getSourceModule());
            ConnectionModel outgoingConnectionModel = gateModel.getOutgoingConnection();
            if (outgoingConnectionModel != null && !neighbors.contains(outgoingConnectionModel.getDestinationModule()))
                neighbors.add(outgoingConnectionModel.getDestinationModule());
        }
        return neighbors;
    }

    protected ArrayList<ModuleModel> getPath(Map<ModuleModel, ModuleModel> predecessorModules, ModuleModel targetModule) {
        ArrayList<ModuleModel> path = new ArrayList<ModuleModel>();
        ModuleModel currentModule = targetModule;
        while (predecessorModules.get(currentModule) != null) {
            path.add(0, currentModule);
            currentModule = predecessorModules.get(currentModule);
        }
        path.add(0, currentModule);
        return path;
    }
}

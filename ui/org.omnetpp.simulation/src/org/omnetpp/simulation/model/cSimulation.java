package org.omnetpp.simulation.model;

import java.util.List;
import java.util.Map;

import org.omnetpp.simulation.controller.Simulation;

/**
 * Represents objects derived from the cSimulation C++ class in the simulation.
 *
 * @author Andras
 */
//TODO finish
public class cSimulation extends cObject {
    private cModule rootModule;
    private cModule[] modules;
    private cObject scheduler;
    private cMessageHeap messageQueue;

    public cSimulation(Simulation simulation, long id) {
        super(simulation, id);
    }

    public cObject getScheduler() {
        checkState();
        return scheduler;
    }

    public cMessageHeap getMessageQueue() {
        checkState();
        return messageQueue;
    }

    public cModule getRootModule() {
        checkState();
        return rootModule;
    }

    public cModule getModuleById(int id) {
        checkState();
        return modules[id];
    }

    public int getLastModuleId() {
        checkState();
        return modules.length - 1;
    }

    @Override
    @SuppressWarnings("rawtypes")
    protected void doFillFromJSON(Map jsonObject) {
        super.doFillFromJSON(jsonObject);

        scheduler = getSimulation().getObjectByJSONRef((String) jsonObject.get("scheduler"));
        messageQueue = (cMessageHeap) getSimulation().getObjectByJSONRef((String) jsonObject.get("messageQueue"));
        rootModule = (cModule) getSimulation().getObjectByJSONRef((String) jsonObject.get("systemModule"));
        List jsonModules = (List) jsonObject.get("modules");
        if (modules == null || modules.length != jsonModules.size())
            modules = new cModule[jsonModules.size()];
        for (int id = 0; id < modules.length; id++)
            modules[id] = (cModule) getSimulation().getObjectByJSONRef((String) jsonModules.get(id));
    }

    protected void clearReferences() {
        super.clearReferences();
        rootModule = null;
        modules = null;
        scheduler = null;
        messageQueue = null;
    }

//  // KLUDGE: TODO: remove this and create a generic, extensible root model
//  private Object routingTable;
//  public void setRoutingTable(Object routingTable) {
//        this.routingTable = routingTable;
//    }
//  public Object getRoutingTable() {
//        return routingTable;
//    }
//
//    // KLUDGE: TODO: remove this and create a generic, extensible root model
//    private Object interfaceTable;
//    public void setInterfaceTable(Object interfaceTable) {
//        this.interfaceTable = interfaceTable;
//    }
//    public Object getInterfaceTable() {
//        return interfaceTable;
//    }
//
//    public int getModuleCount() {
//        return idToModuleMap.size();
//    }
//
//  /* to be called after module creation */
//  public void addModule(cModule module) {
//      idToModuleMap.put(module.getId(), module);
//      pathToModuleMap.put(module.getFullPath(), module);
//  }
//
//    public void removeModule(cModule module) {
//        removeModule(module.getId());
//    }
//
//  public void removeModule(int id) {
//      cModule module = getModuleById(id);
//      idToModuleMap.remove(id);
//      pathToModuleMap.remove(module.getFullPath());
//  }
//
//  public cModule getModuleByPath(String fullPath) {
//      return pathToModuleMap.get(fullPath);
//  }
//
//  XXX what for?
//    public Map<cModule, cModule> findShortestPaths(cModule sourceModule, cModule targetModule) {
//        Set<cModule> unsettledModules = new HashSet<cModule>();
//        Map<cModule, cModule> predecessorModules = new HashMap<cModule, cModule>();
//        Map<cModule, Integer> distances = new HashMap<cModule, Integer>();
//        unsettledModules.addAll(idToModuleMap.values());
//        for (cModule moduleModel : unsettledModules)
//            distances.put(moduleModel, Integer.MAX_VALUE);
//        distances.put(sourceModule, 0);
//        while (unsettledModules.size() > 0) {
//            int selectedDistance = Integer.MAX_VALUE;
//            cModule selectedModule = null;
//            for (cModule module : unsettledModules) {
//                Integer distance = distances.get(module);
//                if (distance <= selectedDistance) {
//                    selectedDistance = distance;
//                    selectedModule = module;
//                }
//            }
//            if (selectedDistance == Integer.MAX_VALUE || (targetModule != null && selectedDistance > distances.get(targetModule)))
//                break;
//            unsettledModules.remove(selectedModule);
//            for (cModule neighborModule : getNeighbors(selectedModule)) {
//                int neighborDistance = distances.get(neighborModule);
//                if (selectedDistance + 1 < neighborDistance) {
//                    distances.put(neighborModule, selectedDistance + 1);
//                    predecessorModules.put(neighborModule, selectedModule);
//                }
//            }
//        }
//        return predecessorModules;
//    }
//
//    public ArrayList<cModule> findShortestPathBetween(cModule sourceModule, cModule targetModule) {
//        return getPath(findShortestPaths(sourceModule, targetModule), targetModule);
//    }
//
//
//    protected List<cModule> getNeighbors(cModule moduleModel) {
//        ArrayList<cModule> neighbors = new ArrayList<cModule>();
//        for (int i = 0; i < moduleModel.getNumGates(); i++) {
//            cGate gateModel = moduleModel.getGate(i);
////FIXME TODO
////            ConnectionModel incomingConnectionModel = gateModel.getIncomingConnection();
////            if (incomingConnectionModel != null && !neighbors.contains(incomingConnectionModel.getSourceModule()))
////                neighbors.add(incomingConnectionModel.getSourceModule());
////            ConnectionModel outgoingConnectionModel = gateModel.getOutgoingConnection();
////            if (outgoingConnectionModel != null && !neighbors.contains(outgoingConnectionModel.getDestinationModule()))
////                neighbors.add(outgoingConnectionModel.getDestinationModule());
//        }
//        return neighbors;
//    }
//
//    protected ArrayList<cModule> getPath(Map<cModule, cModule> predecessorModules, cModule targetModule) {
//        ArrayList<cModule> path = new ArrayList<cModule>();
//        cModule currentModule = targetModule;
//        while (predecessorModules.get(currentModule) != null) {
//            path.add(0, currentModule);
//            currentModule = predecessorModules.get(currentModule);
//        }
//        path.add(0, currentModule);
//        return path;
//    }

}

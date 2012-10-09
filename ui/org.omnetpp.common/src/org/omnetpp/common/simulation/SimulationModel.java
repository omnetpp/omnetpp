package org.omnetpp.common.simulation;

import java.util.HashMap;

public class SimulationModel {
    private ModuleModel rootModule;
    private HashMap<Integer, ModuleModel> idToModuleMap = new HashMap<Integer, ModuleModel>();
    private HashMap<String, ModuleModel> pathToModuleMap = new HashMap<String, ModuleModel>();

    public SimulationModel(ModuleModel rootModule) {
        this.rootModule = rootModule;
        addModule(rootModule);
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
}

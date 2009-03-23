package org.omnetpp.runtimeenv.editors;

import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IPersistableElement;
import org.omnetpp.experimental.simkernel.swig.cModule;
import org.omnetpp.experimental.simkernel.swig.cSimulation;

public class ModuleIDEditorInput implements IEditorInput {
    private int moduleID;

    public ModuleIDEditorInput(int moduleID) {
        this.moduleID = moduleID;
    }
    
    @Override
    public boolean exists() {
        return cSimulation.getActiveSimulation().getModule(moduleID) != null;
    }

    @Override
    public ImageDescriptor getImageDescriptor() {
        return null;
    }

    @Override
    public String getName() {
        String name = getModuleFullPath();
        return name!=null ? name : "module #"+moduleID;
    }

    public int getModuleID() {
        return moduleID;
    }
    
    public String getModuleFullPath() {
        cModule module = cSimulation.getActiveSimulation().getModule(moduleID);
        return module == null ? null : module.getFullPath();
    }

    @Override
    public IPersistableElement getPersistable() {
        return null;
    }

    @Override
    public String getToolTipText() {
        return "module #"+moduleID;
    }

    @Override
    @SuppressWarnings("unchecked")
    public Object getAdapter(Class adapter) {
        return null;
    }

}

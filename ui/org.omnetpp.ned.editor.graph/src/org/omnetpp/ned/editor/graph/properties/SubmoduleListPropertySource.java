package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;
import org.omnetpp.ned2.model.SubmoduleNodeEx;
import org.omnetpp.resources.NEDResourcesPlugin;

/**
 * @author rhornig
 * Property source to display all submodules for a given compound module
 */
public class SubmoduleListPropertySource extends NotifiedPropertySource {
    protected CompoundModuleNodeEx model;
    protected PropertyDescriptor[] pdesc;
    protected int totalSubmoduleCount;
    protected int inheritedSubmoduleCount;
    
    public SubmoduleListPropertySource(CompoundModuleNodeEx model) {
        super(model);
        this.model = model;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<SubmoduleNodeEx> submodules = NEDResourcesPlugin.getNEDResources().getAllSubmodules(model.getName());
        
        pdesc = new PropertyDescriptor[submodules.size()];
        totalSubmoduleCount = inheritedSubmoduleCount = 0;
        for(SubmoduleNodeEx smodule : submodules) {
            String definedIn = "";
            if (smodule.getCompoundModule() != model) {
                inheritedSubmoduleCount++;
                definedIn= " (inherited from "+smodule.getCompoundModule().getName()+")";
            }
            pdesc[totalSubmoduleCount] = new PropertyDescriptor(smodule.getNameWithIndex()+definedIn, smodule.getType());
            pdesc[totalSubmoduleCount].setCategory("submodules");
            pdesc[totalSubmoduleCount].setDescription("Submodule "+smodule.getNameWithIndex()+" with type "+smodule.getType()
                                                        +definedIn+" - (read only)");
            totalSubmoduleCount++;
        }
        
        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        // yust a little summary - show the number of submodules
        String summary = "";
        // if the property descriptor is not yet build, build it now
        if (pdesc == null) 
            getPropertyDescriptors();
        
        if (pdesc != null )
            summary ="total: "+totalSubmoduleCount+" (inherited: "+inheritedSubmoduleCount+")";
        return summary;
    }

    @Override
    public Object getPropertyValue(Object id) {
        return id;
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        return false;
    }

    @Override
    public boolean isPropertySet(Object id) {
        return false;
    }

    @Override
    public void resetPropertyValue(Object id) {
    }

    @Override
    public void setPropertyValue(Object id, Object value) {
    }

}

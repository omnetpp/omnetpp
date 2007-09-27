package org.omnetpp.ned.editor.graph.properties.util;

import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Property source to display all submodules for a given compound module
 *
 * @author rhornig
 */
public class SubmoduleListPropertySource extends NedBasePropertySource {
    public final static String CATEGORY = "submodules";
    public final static String DESCRIPTION = "List of submodules (direct and inherited)";
    protected PropertyDescriptor[] pdesc;
    protected int totalSubmoduleCount;
    protected int inheritedSubmoduleCount;

    public SubmoduleListPropertySource(CompoundModuleElementEx model) {
        super(model);
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        List<SubmoduleElementEx> submodules = ((CompoundModuleElementEx)getModel()).getSubmodules();

        pdesc = new PropertyDescriptor[submodules.size()];
        totalSubmoduleCount = inheritedSubmoduleCount = 0;
        for (SubmoduleElementEx smodule : submodules) {
            String definedIn = "";
            if (smodule.getCompoundModule() != getModel()) {
                inheritedSubmoduleCount++;
                definedIn= " (inherited from "+smodule.getCompoundModule().getName()+")";
            }
            String typeString = "".equals(smodule.getLikeType()) || smodule.getLikeType() == null ?
                            smodule.getType() : "<"+smodule.getLikeParam()+"> like "+smodule.getLikeType();
            pdesc[totalSubmoduleCount] = new PropertyDescriptor(smodule, typeString);
            pdesc[totalSubmoduleCount].setCategory(CATEGORY);
            pdesc[totalSubmoduleCount].setDescription("Submodule "+smodule.getNameWithIndex()+" with type "+typeString
                                                        +definedIn+" - (read only)");
            totalSubmoduleCount++;
        }

        return pdesc;
    }

    @Override
    public Object getEditableValue() {
        // just a little summary - show the number of submodules
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
        if (!(id instanceof SubmoduleElementEx))
            return getEditableValue();

        return ((SubmoduleElementEx)id).getNameWithIndex();
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

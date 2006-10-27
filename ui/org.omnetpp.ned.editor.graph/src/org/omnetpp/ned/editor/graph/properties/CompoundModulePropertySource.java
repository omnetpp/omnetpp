package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;

public class CompoundModulePropertySource extends DelegatingPropertySource {
    // compound module display properties
    protected static class CompoundModuleDisplayPropertySource extends DisplayPropertySource {
        protected CompoundModuleNodeEx model;

        public CompoundModuleDisplayPropertySource(CompoundModuleNodeEx model) {
            super(model);
            this.model = model;
            setDisplayString(model.getDisplayString());
            // submodule inherited properties
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.WIDTH, 
                                                	 DisplayString.Prop.OVIMAGECOLORPCT));
            // direct compound module properties
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.MODULE_X, 
               	 									 DisplayString.Prop.MODULE_UNIT));
        }

        @Override
        public void modelChanged() {
            if(model != null)
                setDisplayString(model.getDisplayString());
        }
    }
    
    // compound module specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public static final String BASE_CATEGORY = "Base";
        public enum Prop { Network, Submodules, Parameters, Gates, Properties }
        protected IPropertyDescriptor[] descriptors;
        protected CompoundModuleNodeEx model;
        CheckboxPropertyDescriptor networkProp;
        SubmoduleListPropertySource submodlist;
        ParameterListPropertySource paramlist;

        public BasePropertySource(CompoundModuleNodeEx connectionNodeModel) {
            model = connectionNodeModel;
            
            // create a property source for the submodule list
            submodlist = new SubmoduleListPropertySource(model);
            // one ofr the parameters list
            paramlist = new ParameterListPropertySource(model);
            // set up property descriptors
            networkProp = new CheckboxPropertyDescriptor(Prop.Network, "network");
            networkProp.setCategory(BASE_CATEGORY);
            networkProp.setDescription("Is this compound module used as a network instance?");

            PropertyDescriptor submodProp = new PropertyDescriptor(Prop.Submodules,"submodules");
            submodProp.setCategory(BASE_CATEGORY);
            submodProp.setDescription("List of submodules and inherited submodules");
            
            PropertyDescriptor paramsProp = new PropertyDescriptor(Prop.Parameters,"parameters");
            paramsProp.setCategory(BASE_CATEGORY);
            paramsProp.setDescription("List of parameters and inherited parameters");

            descriptors = new IPropertyDescriptor[] { networkProp , submodProp, paramsProp};
        }

        public Object getEditableValue() {
            return null;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Network.equals(propName))  
                return model.getIsNetwork(); 

            if (Prop.Submodules.equals(propName))  
                return submodlist; 
            
            if (Prop.Parameters.equals(propName))  
                return paramlist; 

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Network.equals(propName)) 
                model.setIsNetwork((Boolean)value);
        }

        public boolean isPropertySet(Object propName) {
            return true;
        }

        public void resetPropertyValue(Object propName) {
        }

        public boolean isPropertyResettable(Object propName) {
            return false;
        }
    }

    // constructor
    public CompoundModulePropertySource(CompoundModuleNodeEx compModuleNodeModel) {
        super(compModuleNodeModel);
        // create a nested displayPropertySource
        addPropertySource(new NamePropertySource(compModuleNodeModel));
        addPropertySource(new BasePropertySource(compModuleNodeModel));
        addPropertySource(new CompoundModuleDisplayPropertySource(compModuleNodeModel));
    }

}

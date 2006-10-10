package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.displaymodel.DisplayString;
import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.ned2.model.CompoundModuleNodeEx;

public class CompoundModulePropertySource extends DelegatingPropertySource {
    // compound module display properties
    private static class CompoundModuleDisplayPropertySource extends DisplayPropertySource {
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
        public enum Prop { Network  }
        protected IPropertyDescriptor[] descriptors;
        protected CompoundModuleNodeEx model;
        CheckboxPropertyDescriptor networkProp;

        public BasePropertySource(CompoundModuleNodeEx connectionNodeModel) {
            model = connectionNodeModel;
            
            // set up property descriptors
            networkProp = new CheckboxPropertyDescriptor(Prop.Network, "network");
            networkProp.setCategory("Base");
            networkProp.setDescription("Is this compound module used as a network instance?");
            
            descriptors = new IPropertyDescriptor[] { networkProp };
        }

        public Object getEditableValue() {
            return model.getIsNetwork();
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Network.equals(propName))  
                return model.getIsNetwork(); 
            
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

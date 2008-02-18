package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.core.resources.IProject;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;

import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.*;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class CompoundModulePropertySource extends MergedPropertySource {
    // compound module display properties
    protected static class CompoundModuleDisplayPropertySource extends DisplayPropertySource {
        protected CompoundModuleElementEx model;

        public CompoundModuleDisplayPropertySource(CompoundModuleElementEx model) {
            super(model);
            this.model = model;
            // submodule inherited properties
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.WIDTH,
                                                	 DisplayString.Prop.OVIMAGECOLORPCT));
            // direct compound module properties
            supportedProperties.addAll(EnumSet.range(DisplayString.Prop.MODULE_X,
               	 									 DisplayString.Prop.MODULE_SCALE));
        }

    }

    // compound module specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public static final String BASE_CATEGORY = "Base";
        public enum Prop { Network }
        protected IPropertyDescriptor[] descriptors;
        protected CompoundModuleElementEx model;
        CheckboxPropertyDescriptor networkProp;

        public BasePropertySource(CompoundModuleElementEx connectionNodeModel) {
            model = connectionNodeModel;

            // set up property descriptors
            networkProp = new CheckboxPropertyDescriptor(Prop.Network, "network");
            networkProp.setCategory(BASE_CATEGORY);
            networkProp.setDescription("Is this compound module used as a network instance?");

            descriptors = new IPropertyDescriptor[] { networkProp  };
        }

        public Object getEditableValue() {
            return null;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Network.equals(propName))
                return model.isNetwork();

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
    public CompoundModulePropertySource(final CompoundModuleElementEx nodeModel) {
        super(nodeModel);
        // create a nested displayPropertySource
        mergePropertySource(new NamePropertySource(nodeModel, new TypeNameValidator(nodeModel)));
        mergePropertySource(new BasePropertySource(nodeModel));
        // extends
        mergePropertySource(new ExtendsPropertySource(nodeModel) {
            @Override
            protected List<String> getPossibleValues() {
                IProject project = NEDResourcesPlugin.getNEDResources().getNedFile(nodeModel.getContainingNedFileElement()).getProject();
                List<String> moduleNames = new ArrayList<String>(NEDResourcesPlugin.getNEDResources().getModuleQNames(project));
                Collections.sort(moduleNames);
                return moduleNames;
            }
        });
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(nodeModel),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(nodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // gates
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(nodeModel),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // submodules
        mergePropertySource(new DelegatingPropertySource(
                new SubmoduleListPropertySource(nodeModel),
                SubmoduleListPropertySource.CATEGORY,
                SubmoduleListPropertySource.DESCRIPTION));
        // display
        mergePropertySource(new CompoundModuleDisplayPropertySource(nodeModel));
    }

}

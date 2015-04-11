/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties;

import java.util.EnumSet;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ExtendsPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.InterfacesListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;

/**
 * TODO add documentation
 *
 * @author rhornig
 */
public class SimpleModulePropertySource extends MergedPropertySource {

    protected static class SimpleModuleDisplayPropertySource extends DisplayPropertySource {
        protected SimpleModuleElementEx model;

        public SimpleModuleDisplayPropertySource(SimpleModuleElementEx model) {
            super(model);
            this.model = model;
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.SHAPE_WIDTH,
                                                      DisplayString.Prop.IMAGE_SIZE));
        }

    }

    // simple module specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public static final String BASE_CATEGORY = "Base";
        public enum Prop { Network }
        protected IPropertyDescriptor[] descriptors;
        protected SimpleModuleElementEx model;
        CheckboxPropertyDescriptor networkProp;

        public BasePropertySource(SimpleModuleElementEx connectionNodeModel) {
            model = connectionNodeModel;

            // set up property descriptors
            networkProp = new CheckboxPropertyDescriptor(Prop.Network, "network");
            networkProp.setCategory(BASE_CATEGORY);
            networkProp.setDescription("Is this simple module used as a network instance?");

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

    public SimpleModulePropertySource(final SimpleModuleElementEx modelElement) {
        super(modelElement);
        //create name
        mergePropertySource(new NamePropertySource(modelElement, new TypeNameValidator(modelElement)));
        mergePropertySource(new BasePropertySource(modelElement));
        // extends
        mergePropertySource(
                new ExtendsPropertySource(modelElement, getPossibleTypeDisplayNames(modelElement, INedResources.SIMPLE_MODULE_FILTER)));
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(modelElement),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameter list property
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(modelElement),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(modelElement),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // create a nested displayPropertySource
        mergePropertySource(new SimpleModuleDisplayPropertySource(modelElement));

    }

}

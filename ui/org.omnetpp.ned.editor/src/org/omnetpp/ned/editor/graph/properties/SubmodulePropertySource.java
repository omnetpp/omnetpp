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
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.SubmoduleNameValidator;
import org.omnetpp.ned.editor.graph.properties.util.TypePropertySource;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;

/**
 * Properties of the submodule element
 *
 * @author rhornig
 */
public class SubmodulePropertySource extends MergedPropertySource {

    // submodule specific display property description
    protected static class SubmoduleDisplayPropertySource extends DisplayPropertySource {

        public SubmoduleDisplayPropertySource(SubmoduleElementEx model) {
            super(model);
            supportedProperties.addAll( EnumSet.range(DisplayString.Prop.X,
                                                      DisplayString.Prop.TOOLTIP));
        }

    }

    public SubmodulePropertySource(final SubmoduleElementEx modelElement) {
        super(modelElement);
        // name
        mergePropertySource(new NamePropertySource(modelElement, new SubmoduleNameValidator(modelElement)));
        // create a nested displayPropertySource name
        mergePropertySource(new BasePropertySource(modelElement));
        // type
        mergePropertySource(new TypePropertySource(modelElement,
                                                   getPossibleTypeDisplayNames(modelElement, INedResources.MODULE_FILTER),
                                                   getPossibleTypeDisplayNames(modelElement, INedResources.MODULEINTERFACE_FILTER)
                                                   ));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(modelElement),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // gates
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(modelElement),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // display
        mergePropertySource(new SubmoduleDisplayPropertySource(modelElement));
    }

    // submodule module specific properties
    protected static class BasePropertySource implements IPropertySource2 {
        public static final String BASE_CATEGORY = "Base";
        public enum Prop { Dynamic }
        protected IPropertyDescriptor[] descriptors;
        protected SubmoduleElementEx model;
        CheckboxPropertyDescriptor dynamicProp;

        public BasePropertySource(SubmoduleElementEx connectionNodeModel) {
            model = connectionNodeModel;

            // set up property descriptors
            dynamicProp = new CheckboxPropertyDescriptor(Prop.Dynamic, "dynamic");
            dynamicProp.setCategory(BASE_CATEGORY);
            dynamicProp.setDescription("Whether this submodule module is created by user code");

            descriptors = new IPropertyDescriptor[] { dynamicProp  };
        }

        public Object getEditableValue() {
            return null;
        }

        public IPropertyDescriptor[] getPropertyDescriptors() {
            return descriptors;
        }

        public Object getPropertyValue(Object propName) {
            if (Prop.Dynamic.equals(propName))
                return model.isDynamic();

            return null;
        }

        public void setPropertyValue(Object propName, Object value) {
            if (Prop.Dynamic.equals(propName))
                model.setIsDynamic((Boolean)value);
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
}

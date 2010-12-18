/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.Collections;
import java.util.EnumSet;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.common.properties.CheckboxPropertyDescriptor;
import org.omnetpp.ned.core.INedResources;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.editor.graph.properties.util.DelegatingPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.DisplayPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.SubmoduleNameValidator;
import org.omnetpp.ned.editor.graph.properties.util.TypePropertySource;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

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

    public SubmodulePropertySource(final SubmoduleElementEx submoduleNodeModel) {
        super(submoduleNodeModel);
        // create a nested displayPropertySource name
        mergePropertySource(new NamePropertySource(submoduleNodeModel, new SubmoduleNameValidator(submoduleNodeModel)));
        mergePropertySource(new BasePropertySource(submoduleNodeModel));
        // type
        mergePropertySource(new TypePropertySource(submoduleNodeModel) {
            protected List<String> getPossibleTypeValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                INedTypeLookupContext context = submoduleNodeModel.getEnclosingLookupContext();
                List<String> moduleNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.MODULE_FILTER));
                // remove also the containing type
                if (context instanceof CompoundModuleElementEx) 
                    moduleNames.remove(((CompoundModuleElementEx)context).getName());
                // add type names that need fully qualified names
                moduleNames.addAll(res.getInvisibleTypeNames(context, INedResources.MODULE_FILTER));
                Collections.sort(moduleNames);
                return moduleNames;
            }
            protected List<String> getPossibleLikeTypeValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                CompoundModuleElementEx context = submoduleNodeModel.getCompoundModule();
                List<String> moduleNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.MODULEINTERFACE_FILTER));
                moduleNames.addAll(res.getInvisibleTypeNames(context, INedResources.MODULEINTERFACE_FILTER));
                Collections.sort(moduleNames);
                return moduleNames;
            }
        });
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(submoduleNodeModel),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // gates
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(submoduleNodeModel),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // display
        mergePropertySource(new SubmoduleDisplayPropertySource(submoduleNodeModel));
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

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
import org.omnetpp.ned.editor.graph.properties.util.ExtendsPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.GateListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.InterfacesListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.NamePropertySource;
import org.omnetpp.ned.editor.graph.properties.util.ParameterListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.SubmoduleListPropertySource;
import org.omnetpp.ned.editor.graph.properties.util.TypeNameValidator;
import org.omnetpp.ned.model.DisplayString;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.interfaces.INedTypeLookupContext;

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
            networkProp.setDescription("Whether this compound module used as a network instance");

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
    public CompoundModulePropertySource(final CompoundModuleElementEx compoundModule) {
        super(compoundModule);
        // create a nested displayPropertySource
        mergePropertySource(new NamePropertySource(compoundModule, new TypeNameValidator(compoundModule)));
        mergePropertySource(new BasePropertySource(compoundModule));
        // extends
        mergePropertySource(new ExtendsPropertySource(compoundModule) {
            @Override
            protected List<String> getPossibleValues() {
                INedResources res = NedResourcesPlugin.getNedResources();
                INedTypeLookupContext context = compoundModule.getEnclosingLookupContext();
                List<String> moduleNames = new ArrayList<String>(res.getVisibleTypeNames(context, INedResources.COMPOUND_MODULE_FILTER));
                // remove ourselves to avoid direct cycle
                moduleNames.remove(compoundModule.getName());
                // remove also the containing type
                if (context instanceof CompoundModuleElementEx) 
                    moduleNames.remove(((CompoundModuleElementEx)context).getName());
                // add type names that need fully qualified names
                moduleNames.addAll(res.getInvisibleTypeNames(context, INedResources.COMPOUND_MODULE_FILTER));
                Collections.sort(moduleNames);
                return moduleNames;
            }
        });
        // interfaces
        mergePropertySource(new DelegatingPropertySource(
                new InterfacesListPropertySource(compoundModule),
                InterfacesListPropertySource.CATEGORY,
                InterfacesListPropertySource.DESCRIPTION));
        // parameters
        mergePropertySource(new DelegatingPropertySource(
                new ParameterListPropertySource(compoundModule),
                ParameterListPropertySource.CATEGORY,
                ParameterListPropertySource.DESCRIPTION));
        // gates
        mergePropertySource(new DelegatingPropertySource(
                new GateListPropertySource(compoundModule),
                GateListPropertySource.CATEGORY,
                GateListPropertySource.DESCRIPTION));
        // submodules
        mergePropertySource(new DelegatingPropertySource(
                new SubmoduleListPropertySource(compoundModule),
                SubmoduleListPropertySource.CATEGORY,
                SubmoduleListPropertySource.DESCRIPTION));
        // display
        mergePropertySource(new CompoundModuleDisplayPropertySource(compoundModule));
    }

}

package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.gef.EditPart;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.model.ex.ChannelInterfaceNodeEx;
import org.omnetpp.ned.model.ex.ChannelNodeEx;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceNodeEx;
import org.omnetpp.ned.model.ex.SimpleModuleNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;

/**
 * Creates a propertysource for a controller part. Each propertysource class must be 
 * registered here 
 * @author rhornig
 *
 */
public  class NedPropertySourceAdapterFactory implements IAdapterFactory {
        public Object getAdapter(Object part, Class adapterType) {
            if (!(part instanceof EditPart))
                return null;
            // try to get the adapter from the model (if it was previously created)
            NEDElement nedModel = (NEDElement)((EditPart)part).getModel();
            // try to get the property source from the object directly 
            IPropertySource propSource = ((IPropertySourceSupport)part).getPropertySource(); 
            if (propSource != null) 
                return propSource;
            
            // if no adapter exists for this model object, create one
            if (nedModel instanceof SubmoduleNodeEx) 
                propSource = new SubmodulePropertySource((SubmoduleNodeEx)nedModel);
            
            if (nedModel instanceof CompoundModuleNodeEx) 
                propSource = new CompoundModulePropertySource((CompoundModuleNodeEx)nedModel);

            if (nedModel instanceof SimpleModuleNodeEx) 
                propSource = new SimpleModulePropertySource((SimpleModuleNodeEx)nedModel);

            if (nedModel instanceof ConnectionNodeEx) 
                propSource = new ConnectionPropertySource((ConnectionNodeEx)nedModel);

            if (nedModel instanceof ChannelNodeEx) 
                propSource = new ChannelPropertySource((ChannelNodeEx)nedModel);

            if (nedModel instanceof ModuleInterfaceNodeEx) 
                propSource = new ModuleInterfacePropertySource((ModuleInterfaceNodeEx)nedModel);

            if (nedModel instanceof ChannelInterfaceNodeEx) 
                propSource = new ChannelInterfacePropertySource((ChannelInterfaceNodeEx)nedModel);

            // set the read only flag on the property source
            if ((part instanceof IReadOnlySupport) && (propSource instanceof MergedPropertySource))
                ((MergedPropertySource)propSource).setReadOnly(
                        !((IReadOnlySupport)part).isEditable());
            
            // store the created adapter into the controller so we can reuse it later
            if(propSource != null) 
                ((IPropertySourceSupport)part).setPropertySource(propSource);

            return propSource;
        }

        public Class[] getAdapterList() {
            return new Class[]{ IPropertySource.class };
        }
        
}

package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelInterfaceNodeEx;
import org.omnetpp.ned.model.ex.ChannelNodeEx;
import org.omnetpp.ned.model.ex.CompoundModuleNodeEx;
import org.omnetpp.ned.model.ex.ConnectionNodeEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceNodeEx;
import org.omnetpp.ned.model.ex.SimpleModuleNodeEx;
import org.omnetpp.ned.model.ex.SubmoduleNodeEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;

/**
 * Provides property sources for NED elements
 * @author rhornig
 */
public class NedEditPartPropertySourceProvider implements IPropertySourceProvider {

    public IPropertySource getPropertySource(Object object) {
        IPropertySource propSource = null;
        if (object instanceof IPropertySource)
            propSource = (IPropertySource)object;
        
        if ((object instanceof IModelProvider))
        {
            // try to get the adapter from the model (if it was previously created)
            INEDElement nedModel = ((IModelProvider)object).getNEDModel();
            // try to get the property source from the object directly
            if (object instanceof IPropertySourceSupport) {
                propSource = ((IPropertySourceSupport)object).getPropertySource(); 
                if (propSource != null) 
                    return propSource;
            }
            
            // if no property source exists for this model object, create one
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
            if ((object instanceof IReadOnlySupport) && (propSource instanceof MergedPropertySource))
                ((MergedPropertySource)propSource).setReadOnly(
                        !((IReadOnlySupport)object).isEditable());
            
            // store the created property source into the controller so we can reuse it later
            if (propSource != null && object instanceof IPropertySourceSupport) 
                ((IPropertySourceSupport)object).setPropertySource(propSource);
        }
        return propSource;
    }
}

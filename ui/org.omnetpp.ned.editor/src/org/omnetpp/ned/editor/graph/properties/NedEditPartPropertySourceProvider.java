package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.ned.editor.graph.edit.IReadOnlySupport;
import org.omnetpp.ned.editor.graph.properties.util.MergedPropertySource;
import org.omnetpp.ned.model.INEDElement;
import org.omnetpp.ned.model.ex.ChannelInterfaceElementEx;
import org.omnetpp.ned.model.ex.ChannelElementEx;
import org.omnetpp.ned.model.ex.CompoundModuleElementEx;
import org.omnetpp.ned.model.ex.ConnectionElementEx;
import org.omnetpp.ned.model.ex.ModuleInterfaceElementEx;
import org.omnetpp.ned.model.ex.SimpleModuleElementEx;
import org.omnetpp.ned.model.ex.SubmoduleElementEx;
import org.omnetpp.ned.model.interfaces.IModelProvider;

/**
 * Provides property sources for NED elements
 *
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
            INEDElement nedModel = ((IModelProvider)object).getNedModel();
            
            // if no property source exists for this model object, create one
            if (nedModel instanceof SubmoduleElementEx) 
                propSource = new SubmodulePropertySource((SubmoduleElementEx)nedModel);
            
            if (nedModel instanceof CompoundModuleElementEx) 
                propSource = new CompoundModulePropertySource((CompoundModuleElementEx)nedModel);
    
            if (nedModel instanceof SimpleModuleElementEx) 
                propSource = new SimpleModulePropertySource((SimpleModuleElementEx)nedModel);
    
            if (nedModel instanceof ConnectionElementEx) 
                propSource = new ConnectionPropertySource((ConnectionElementEx)nedModel);
    
            if (nedModel instanceof ChannelElementEx) 
                propSource = new ChannelPropertySource((ChannelElementEx)nedModel);
    
            if (nedModel instanceof ModuleInterfaceElementEx) 
                propSource = new ModuleInterfacePropertySource((ModuleInterfaceElementEx)nedModel);
    
            if (nedModel instanceof ChannelInterfaceElementEx) 
                propSource = new ChannelInterfacePropertySource((ChannelInterfaceElementEx)nedModel);
    
            // set the read only flag on the property source
            if ((object instanceof IReadOnlySupport) && (propSource instanceof MergedPropertySource))
                ((MergedPropertySource)propSource).setReadOnly(
                        !((IReadOnlySupport)object).isEditable());
            
        }
        return propSource;
    }
}

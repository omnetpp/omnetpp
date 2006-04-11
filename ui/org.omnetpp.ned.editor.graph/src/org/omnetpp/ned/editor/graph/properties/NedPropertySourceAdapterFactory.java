package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.ned.editor.graph.model.CompoundModuleNodeEx;
import org.omnetpp.ned.editor.graph.model.ConnectionNodeEx;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Creates a propertysource for a NEDElement. Each propertysource class must be registered here 
 * @author rhornig
 *
 */
public  class NedPropertySourceAdapterFactory implements IAdapterFactory {
        private static String IPROPERTY_SOURCE_ADAPTER = "IPROPERTY_SOURCE_ADAPTER";
        public Object getAdapter(Object model, Class adapterType) {
            // try to get the adapter from the model (if it was previously created)
            NEDElement nedModel = (NEDElement)model;
            IPropertySource adapter = (IPropertySource)nedModel.getUserData(IPROPERTY_SOURCE_ADAPTER);
            if (adapter != null) 
                return adapter;
            
            // if no adapter exists for this model object, create one
            if (nedModel instanceof SubmoduleNodeEx) 
                adapter = new SubmodulePropertySource((SubmoduleNodeEx)nedModel);
            
            if (nedModel instanceof CompoundModuleNodeEx) 
                adapter = new CompoundModulePropertySource((CompoundModuleNodeEx)nedModel);

            if (nedModel instanceof ConnectionNodeEx) 
                adapter = new ConnectionPropertySource((ConnectionNodeEx)nedModel);

            // store the created adapter into the model so we can reuse it later
            if(adapter != null) 
                nedModel.setUserData(IPROPERTY_SOURCE_ADAPTER, adapter);
            return adapter;
        }

        public Class[] getAdapterList() {
            return new Class[]{ IPropertySource.class };
        }
        
}

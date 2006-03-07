package org.omnetpp.ned.editor.graph.properties;

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.ned.editor.graph.model.SubmoduleNodeEx;
import org.omnetpp.ned2.model.NEDElement;

/**
 * Creates a propertysource for a NEDElement. Each propertysource class must be registered here 
 * @author rhornig
 *
 */
public  class NedPropertySourceAdapterFactory implements IAdapterFactory {
        private static String IPROPERTY_SOURCE_ADAPTER = "IPROPERTY_SOURCE_ADAPTER";
        // TODO this method culd be optimized if we would store/cache the created adapter
        // back into the NEDElement and create new one ONLY if no acapter is currently cached
        // ie. NEDElement must have a setPropertySource, getPropertySource method.
        public Object getAdapter(Object model, Class adapterType) {
            // try to get the adapter from the model (if it was previously created)
            NEDElement nedModel = (NEDElement)model;
            IPropertySource adapter = (IPropertySource)nedModel.getUserData(IPROPERTY_SOURCE_ADAPTER);
            if (adapter != null) 
                return adapter;
            
            // if no adapter exists for this model object, create one
            if (nedModel instanceof SubmoduleNodeEx) 
                adapter = new SubmodulePropertySource((SubmoduleNodeEx)nedModel);
            
            // store the created adapter into the model so we can reuse it later
            nedModel.setUserData(IPROPERTY_SOURCE_ADAPTER, adapter);
            return adapter;
        }

        public Class[] getAdapterList() {
            return new Class[]{ IPropertySource.class };
        }
        
}

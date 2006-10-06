package org.omnetpp.ned.editor.graph.properties;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.ned2.model.INEDChangeListener;
import org.omnetpp.ned2.model.NEDElement;

/**
 * @author rhornig
 * Merges several IPropertySource into a single PropertySource
 */
public class DelegatingPropertySource implements IPropertySource2, INEDChangeListener {

	List<IPropertySource> psList = new ArrayList<IPropertySource>(); 
	
	DelegatingPropertySource(NEDElement model) {
        // register the propertysource as a listener for the model so it will be notified
        // once someone changes the underlying model
        model.addListener(this);
    }

    /**
	 * Adds a propertysource to be merged into this 
	 * @param ps
	 */
	public void addPropertySource(IPropertySource ps) {
		psList.add(ps);
	}
	
	/**
	 * Remove a property source from the merged list
	 * @param ps
	 */
	public void removePropertySource(IPropertySource ps) {
		psList.remove(ps);
	}
	
	public boolean isPropertyResettable(Object id) {
		// ask all regitered PS if it knows about this
		for(IPropertySource psrc : psList)
			if (psrc instanceof IPropertySource2) 
				if (((IPropertySource2)psrc).isPropertyResettable(id))
					return true;
			
		return false;
	}

	public boolean isPropertySet(Object id) {
		// ask all regitered PS if it knows about this
		for(IPropertySource psrc : psList)
			if (psrc instanceof IPropertySource2) 
				if (((IPropertySource2)psrc).isPropertySet(id))
					return true;
			
		return false;
	}

	public Object getEditableValue() {
		// by default return ourselves
		// TODO or maybe a copy of ourselves ??? Test with editing multiple objects
		return this;
	}

	public IPropertyDescriptor[] getPropertyDescriptors() {
		List<IPropertyDescriptor> mergedPDList = new ArrayList<IPropertyDescriptor>();
		// walk trhough all property source and merge its descriptors into a single list
		for(IPropertySource psrc : psList) 
			for(IPropertyDescriptor pdesc : psrc.getPropertyDescriptors())
				mergedPDList.add(pdesc);
		
		return mergedPDList.toArray(new IPropertyDescriptor[mergedPDList.size()]);
	}

	public Object getPropertyValue(Object id) {
		// ask all registered property sources for the given value
		for(IPropertySource psrc : psList) {
			Object result = psrc.getPropertyValue(id);
			if (result != null) return result;
		}
			
		return null;
	}

	public void resetPropertyValue(Object id) {
		// try to reset the value on all registered property source
		for(IPropertySource psrc : psList)
			psrc.resetPropertyValue(id);
	}

	public void setPropertyValue(Object id, Object value) {
		// try to set the value on all registered property source
		for(IPropertySource psrc : psList)
			psrc.setPropertyValue(id, value);
	}

    // the following methods notify the property source 
    public void attributeChanged(NEDElement node, String attr) {
        modelChanged();
    }

    public void childInserted(NEDElement node, NEDElement where, NEDElement child) {
        modelChanged();
    }

    public void childRemoved(NEDElement node, NEDElement child) {
        modelChanged();
    }
    
    public void modelChanged() {
    }
}

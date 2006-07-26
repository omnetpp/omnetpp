package org.omnetpp.scave2.editors;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave2.charting.ChartProperties;
import org.omnetpp.scave2.charting.ChartSWTWrapper;

/**
 * Provides properties of scave model objects and charts.
 *
 * @author tomi
 */
public class ScavePropertySourceProvider implements IPropertySourceProvider {
	
	IPropertySourceProvider delegate;
	
	public ScavePropertySourceProvider(AdapterFactory adapterFactory) {
		this.delegate = new AdapterFactoryContentProvider(adapterFactory);
	}

	public IPropertySource getPropertySource(Object object) {
		IPropertySource propertySource;
		if (object instanceof ChartSWTWrapper)
			propertySource = new ChartProperties((ChartSWTWrapper)object);
		else if (object instanceof PropertySource)
			propertySource = (PropertySource)object;
		else
			propertySource = delegate.getPropertySource(object);
		return propertySource;
	}
}

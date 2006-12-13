package org.omnetpp.scave2.editors;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave2.charting.ChartSWTWrapper;
import org.omnetpp.scave2.model2.ChartProperties;

/**
 * Provides properties of scave model objects and charts.
 *
 * @author tomi
 */
public class ScavePropertySourceProvider implements IPropertySourceProvider {
	
	IPropertySourceProvider delegate;
	ResultFileManager manager;
	
	public ScavePropertySourceProvider(AdapterFactory adapterFactory, ResultFileManager manager) {
		this.delegate = new AdapterFactoryContentProvider(adapterFactory);
		this.manager = manager;
	}

	public IPropertySource getPropertySource(Object object) {
		IPropertySource propertySource;
		if (object instanceof Chart)
			propertySource = ChartProperties.createPropertySource((Chart)object, manager);
		else if (object instanceof PropertySource)
			propertySource = (PropertySource)object;
		else
			propertySource = delegate.getPropertySource(object);
		return propertySource;
	}
}

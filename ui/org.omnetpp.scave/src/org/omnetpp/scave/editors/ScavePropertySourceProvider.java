package org.omnetpp.scave.editors;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryContentProvider;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.scave.charting.ChartProperties;
import org.omnetpp.scave.charting.ChartProperties.VectorChartProperties;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model2.ChartLine;
import org.omnetpp.scave.model2.SetOperationPropertySource;

/**
 * Provides properties of scave model objects and charts.
 *
 * @author tomi
 */
public class ScavePropertySourceProvider implements IPropertySourceProvider {
	
	AdapterFactory adapterFactory;
	IPropertySourceProvider delegate;
	ResultFileManager manager;
	
	public ScavePropertySourceProvider(AdapterFactory adapterFactory, ResultFileManager manager) {
		this.adapterFactory = adapterFactory;
		this.delegate = new AdapterFactoryContentProvider(adapterFactory);
		this.manager = manager;
	}

	public IPropertySource getPropertySource(Object object) {
		if (object instanceof Chart)
			return ChartProperties.createPropertySource((Chart)object, manager);
		else if (object instanceof SetOperation) {
			IItemPropertySource itemPropertySource =
				(IItemPropertySource) adapterFactory.adapt(object, IItemPropertySource.class);
			return new SetOperationPropertySource((SetOperation)object, itemPropertySource, manager);
		}
		else if (object instanceof PropertySource)
			return (PropertySource)object;
		else if (object instanceof ChartLine) {
			ChartLine lineID = (ChartLine)object;
			ChartProperties properties = ChartProperties.createPropertySource(lineID.getChart(), manager); 
			if (properties instanceof VectorChartProperties)
				return ((VectorChartProperties)properties).getLineProperties(lineID.getKey());
		}
	
		return delegate.getPropertySource(object);
	}
}

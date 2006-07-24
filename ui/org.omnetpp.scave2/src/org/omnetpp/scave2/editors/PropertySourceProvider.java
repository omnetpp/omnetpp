package org.omnetpp.scave2.editors;

import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;
import org.omnetpp.scave2.charting.ChartProperties;
import org.omnetpp.scave2.charting.ChartSWTWrapper;

public class PropertySourceProvider implements IPropertySourceProvider {
	
	IPropertySourceProvider delegate;
	
	public PropertySourceProvider(IPropertySourceProvider delegate) {
		this.delegate = delegate;
	}

	public IPropertySource getPropertySource(Object object) {
		if (object instanceof ChartSWTWrapper)
			return new ChartProperties((ChartSWTWrapper)object);
		return delegate.getPropertySource(object);
	}
}

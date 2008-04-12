package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScatterChart;

public class ScatterChartProperties extends VectorChartProperties
{
	public ScatterChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
		super(chart, properties, manager);
		Assert.isLegal(chart == null || chart instanceof ScatterChart);
	}
}
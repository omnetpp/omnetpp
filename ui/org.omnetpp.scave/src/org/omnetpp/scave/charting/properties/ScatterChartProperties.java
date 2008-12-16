/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
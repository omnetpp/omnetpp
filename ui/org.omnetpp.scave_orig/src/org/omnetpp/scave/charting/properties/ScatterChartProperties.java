/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

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
    public static final String PROP_X_AXIS_LOGARITHMIC = "X.Axis.Log";

    public ScatterChartProperties(Chart chart, List<Property> properties, ResultFileManager manager) {
        super(chart, properties, manager);
        Assert.isLegal(chart == null || chart instanceof ScatterChart);
    }

    @org.omnetpp.common.properties.Property(category="Axes",id=PROP_X_AXIS_LOGARITHMIC,
            description="Applies a logarithmic transformation to the x values.")
    public boolean getXAxisLogarithmic() { return getBooleanProperty(PROP_X_AXIS_LOGARITHMIC); }
    public void setXAxisLogarithmic(boolean flag) { setProperty(PROP_X_AXIS_LOGARITHMIC, flag); }
    public boolean defaultXAxisLogarithmic() { return ChartDefaults.DEFAULT_X_AXIS_LOGARITHMIC; }
}
/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.commands.CommandStack;

public class BarChartVisualProperties extends ChartVisualProperties
{
    // private static final String DEFAULT_BAR_PROPERTIES_ID = "default";

    public static final String
        // Titles
        PROP_WRAP_LABELS        = "X.Label.Wrap",
        // Bars
        PROP_BAR_BASELINE       = "Bars.Baseline",
        PROP_BAR_PLACEMENT      = "Bar.Placement";

    public enum BarPlacement {
        Aligned,
        Overlap,
        InFront,
        Stacked,
    }

    public BarChartVisualProperties(Chart chart) {
        this(chart, null);
    }

    public BarChartVisualProperties(Chart chart, CommandStack commandStack) {
        this(chart, chart.getProperties(), commandStack);
    }

    public BarChartVisualProperties(Chart chart, List<Property> properties, CommandStack commandStack) {
        super(chart, properties, commandStack);
    }

    /*======================================================================
     *                             Titles
     *======================================================================*/
    @org.omnetpp.common.properties.Property(category="Titles",id=PROP_WRAP_LABELS,
            description="If true labels are wrapped, otherwise aligned vertically.")
    public boolean getWrapLabels() { return getBooleanProperty(PROP_WRAP_LABELS); }
    public void setWrapLabels(boolean wrap) { setProperty(PROP_WRAP_LABELS, wrap); }
    public boolean defaultWrapLabels() { return ChartDefaults.DEFAULT_WRAP_LABELS; }

    /*======================================================================
     *                             Bars
     *======================================================================*/
    @org.omnetpp.common.properties.Property(category="Plot",id=PROP_BAR_BASELINE,
            description="Baseline of the bars.")
    public Double getBarBaseline() { return getDoubleProperty(PROP_BAR_BASELINE); }
    public void setBarBaseline(Double baseline) { setProperty(PROP_BAR_BASELINE, baseline); }
    public Double defaultBarBaseline() { return ChartDefaults.DEFAULT_BAR_BASELINE; }

    @org.omnetpp.common.properties.Property(category="Plot",id=PROP_BAR_PLACEMENT,
            description="Arrangement of the bars within a group.")
    public BarPlacement getBarPlacement() { return getEnumProperty(PROP_BAR_PLACEMENT, BarPlacement.class); }
    public void setBarPlacement(BarPlacement placement) { setProperty(PROP_BAR_PLACEMENT, placement); }
    public BarPlacement defaultBarPlacement() { return ChartDefaults.DEFAULT_BAR_PLACEMENT; }

//    @org.omnetpp.common.properties.Property(category="Plot",id="Bars",displayName="Bars",
//            description="Properties of individual bars. Default is applied to all properties not set individually.")
//    public IPropertySource getBarProperties() {
//        IPropertyDescriptor[] descriptors = new IPropertyDescriptor[0];
//        return new BasePropertySource(descriptors) {
//            @Override
//            public Object getPropertyValue(Object id) {
//                return new BarVisualProperties(BarChartVisualProperties.this,
//                        id == DEFAULT_BAR_PROPERTIES_ID ? null : (String)id);
//            }
//        };
//    }
}
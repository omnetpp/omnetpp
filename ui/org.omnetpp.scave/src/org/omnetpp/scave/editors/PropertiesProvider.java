package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.util.Converter;
import org.omnetpp.scave.charting.properties.ChartDefaults;
import org.omnetpp.scave.charting.properties.ChartVisualProperties;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.pychart.IChartPropertiesProvider;

public class PropertiesProvider implements IChartPropertiesProvider {
    Chart chart;

    public PropertiesProvider(Chart chart) {
        this.chart = chart;
    }

    @Override
    public Map<String, String> getChartProperties() {
        Map<String, String> props = new HashMap<String, String>();

        for (Property prop : chart.getProperties())
            props.put(prop.getName(), prop.getValue());

        return props;
    }

    @Override
    public Map<String, String> getDefaultChartProperties() {

        IPropertySource props = ChartVisualProperties.createPropertySource(chart);

        IPropertyDescriptor[] descriptors = props.getPropertyDescriptors();

        Map<String, String> result = new HashMap<String, String>();

        for (IPropertyDescriptor desc : descriptors) {
            String name = desc.getId().toString();
            Object def = ChartDefaults.getDefaultPropertyValue(name);

            if (def == null)
                result.put(name, null);
            else {
                String stringDefault = null;

                if (def instanceof FontData)
                    stringDefault = Converter.fontdataToString((FontData) def);
                else if (def instanceof Boolean)
                    stringDefault = Converter.booleanToString((Boolean) def);
                else if (def instanceof Double)
                    stringDefault = Converter.doubleToString((Double) def);
                else if (def instanceof Integer)
                    stringDefault = Converter.integerToString((Integer) def);
                else if (def instanceof RGB)
                    stringDefault = Converter.rgbToString((RGB) def);
                else
                    stringDefault = def.toString();

                result.put(name, stringDefault);
            }
        }

        // TODO: Also add the declared form properties of the template of the chart.
        // ... except there is not much point since changing them will not have any effect?

        return result;
    }

    @Override
    public String getChartName() {
        return chart.getName();
    }

    @Override
    public String getChartType() {
        return chart.getType().toString().toLowerCase();
    }

    @Override
    public void setSuggestedChartName(String name) {
        System.out.println("SUGGESTED NAME: " + name);
    }
}
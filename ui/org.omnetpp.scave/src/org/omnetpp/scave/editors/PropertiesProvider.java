package org.omnetpp.scave.editors;

import java.util.HashMap;
import java.util.Map;

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
        // TODO
//        ChartProperties props = ChartProperties.createPropertySource(chart, null);
//
//        IPropertyDescriptor[] descriptors = props.getPropertyDescriptors();
//
//        Map<String, String> result = new HashMap<String, String>();
//
//        for (IPropertyDescriptor desc : descriptors) {
//            String name = desc.getId().toString();
//            Object def = ChartDefaults.getDefaultPropertyValue(name);
//
//            if (def == null)
//                result.put(name, null);
//            else {
//                String stringDefault = null;
//
//                if (def instanceof FontData)
//                    stringDefault = Converter.fontdataToString((FontData) def);
//                else if (def instanceof Boolean)
//                    stringDefault = Converter.booleanToString((Boolean) def);
//                else if (def instanceof Double)
//                    stringDefault = Converter.doubleToString((Double) def);
//                else if (def instanceof Integer)
//                    stringDefault = Converter.integerToString((Integer) def);
//                else if (def instanceof RGB)
//                    stringDefault = Converter.rgbToString((RGB) def);
//                else
//                    stringDefault = def.toString();
//
//                result.put(name, stringDefault);
//            }
//        }
        //return result;

        return null;
    }

    @Override
    public String getChartName() {
        return chart.getName();
    }
}
/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Property;

public class MatplotlibChartProperties implements IPropertySource2 {
    Chart chart;
    List<Property> properties;

    public MatplotlibChartProperties(Chart chart) {
        this.chart = chart;
        this.properties = chart.getProperties();
    }

    public Property getProperty(String propertyName) {
        for (Property property : properties)
            if (property.getName().equals(propertyName))
                return property;
        return null;
    }

    /**
     * Sets the property value in the property list. If the value is null then
     * the property node is deleted.
     */
    protected void doSetProperty(String propertyName, String propertyValue) {
        Property property = getProperty(propertyName);

        if (property == null && propertyValue != null) { // add new property
            property = new Property(propertyName, propertyValue);
            properties.add(property);
        } else if (property != null && propertyValue != null) // change existing property
            property.setValue(propertyValue);
        else if (property != null && propertyValue == null) // delete existing property
            properties.remove(property);
    }

    public void setProperty(String propertyName, String propertyValue) {
        String defaultValue = null;
        if (defaultValue != null && defaultValue.equals(propertyValue))
            propertyValue = null;
        doSetProperty(propertyName, propertyValue);
    }

    public List<Property> getProperties() {
        return properties;
    }

    public static MatplotlibChartProperties createPropertySource(Chart chart) {
        return new MatplotlibChartProperties(chart);
    }

    @Override
    public Object getEditableValue() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        // TODO Auto-generated method stub
        ArrayList<IPropertyDescriptor> descriptors = new ArrayList<IPropertyDescriptor>();

        for (Property p : properties)
            descriptors.add(new TextPropertyDescriptor(p.getName(), p.getName()));

        return descriptors.toArray(new IPropertyDescriptor[0]);
    }

    @Override
    public Object getPropertyValue(Object id) {
        for (Property p : properties)
            if (p.getName().equals(id))
                return p.getValue();
        return "<no value>";
    }

    @Override
    public void resetPropertyValue(Object id) {
        // TODO Auto-generated method stub

    }

    @Override
    public void setPropertyValue(Object id, Object value) {
        // TODO Auto-generated method stub

    }

    @Override
    public boolean isPropertyResettable(Object id) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean isPropertySet(Object id) {
        // TODO Auto-generated method stub
        return false;
    }

}

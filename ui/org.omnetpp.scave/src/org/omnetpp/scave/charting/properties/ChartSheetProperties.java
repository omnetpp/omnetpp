/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.RemoveCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.common.properties.PropertySource;
import org.omnetpp.common.util.Converter;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * Property source for chart sheets.
 * @author levy
 */
public class ChartSheetProperties extends PropertySource {

    /**
     * Property names used in the model.
     */
    public static final String
        PROP_COLUMN_COUNT          = "ColumnCount",
        PROP_MIN_CHART_WIDTH       = "MinChartWidth",
        PROP_MIN_CHART_HEIGHT      = "MinChartHeight",
        PROP_DISPLAY_CHART_DETAILS = "DisplayChartDetails";

    public static final Integer DEFAULT_COLUMN_COUNT          = 2;
    public static final Integer DEFAULT_MIN_CHART_WIDTH       = 320;
    public static final Integer DEFAULT_MIN_CHART_HEIGHT      = 200;
    public static final boolean DEFAULT_DISPLAY_CHART_DETAILS = true;

    public static ChartSheetProperties createPropertySource(ChartSheet chartSheet, IPropertySource delegate) {
        return createPropertySource(chartSheet, chartSheet.getProperties(), delegate);
    }

    public static ChartSheetProperties createPropertySource(ChartSheet chartSheet, List<Property> properties, IPropertySource delegate) {
        return new ChartSheetProperties(chartSheet, properties, delegate);
    }

    protected ChartSheet chartSheet;                 // the chart sheet what the properties belongs to
    protected List<Property> properties; // the chart sheet properties, might not be contained by the chart sheet yet
    protected EditingDomain domain;      // editing domain to execute changes, if null the property list changed directly
    protected IPropertySource delegate;

    public ChartSheetProperties(ChartSheet chartSheet, List<Property> properties, IPropertySource delegate) {
        this.chartSheet = chartSheet;
        this.properties = properties;
        this.delegate = delegate;
        this.domain = properties == chartSheet.getProperties() ? AdapterFactoryEditingDomain.getEditingDomainFor(chartSheet) : null;
    }

    public List<Property> getProperties() {
        return properties;
    }

    @Override
    public IPropertyDescriptor[] getPropertyDescriptors() {
        return (IPropertyDescriptor[])ArrayUtils.addAll(delegate.getPropertyDescriptors(), super.getPropertyDescriptors());
    }

    @Override
    public Object getPropertyValue(Object id) {
        if (!isKnownProperty(id))
            return delegate.getPropertyValue(id);
        else
            return super.getPropertyValue(id);
    }

    @Override
    public void setPropertyValue(Object id, Object value) {
        if (!isKnownProperty(id))
            delegate.setPropertyValue(id, value);
        else
            super.setPropertyValue(id, value);
    }

    @Override
    public boolean isPropertySet(Object id) {
        if (!isKnownProperty(id))
            return delegate.isPropertySet(id);
        else
            return super.isPropertySet(id);
    }

    @Override
    public boolean isPropertyResettable(Object id) {
        if (!isKnownProperty(id)) {
            if (delegate instanceof IPropertySource2)
                return ((IPropertySource2)delegate).isPropertyResettable(id);
            else
                return false;
        }
        else
            return super.isPropertyResettable(id);
    }

    @Override
    public void resetPropertyValue(Object id) {
        if (!isKnownProperty(id)) {
            if (delegate instanceof IPropertySource2)
                ((IPropertySource2)delegate).resetPropertyValue(id);
        }
        else
            super.resetPropertyValue(id);
    }

    /*======================================================================
     *                             Main
     *======================================================================*/

    @org.omnetpp.common.properties.Property(category="Main",id=PROP_COLUMN_COUNT,displayName="column count",
            description="Number of chart columns on the sheet")
    public Integer getColumnCount() { return getIntegerProperty(PROP_COLUMN_COUNT, DEFAULT_COLUMN_COUNT); }
    public void setColumnCount(Integer value) { setProperty(PROP_COLUMN_COUNT, value, DEFAULT_COLUMN_COUNT); }
    public Integer defaultColumnCount() { return DEFAULT_COLUMN_COUNT; }

    @org.omnetpp.common.properties.Property(category="Main",id=PROP_MIN_CHART_WIDTH,displayName="min chart width",
            description="Minimum width of individual charts")
    public Integer getMinChartWidth() { return getIntegerProperty(PROP_MIN_CHART_WIDTH, DEFAULT_MIN_CHART_WIDTH); }
    public void setMinChartWidth(Integer value) { setProperty(PROP_MIN_CHART_WIDTH, value, DEFAULT_MIN_CHART_WIDTH); }
    public Integer defaultMinChartWidth() { return DEFAULT_MIN_CHART_WIDTH; }

    @org.omnetpp.common.properties.Property(category="Main",id=PROP_MIN_CHART_HEIGHT,displayName="min chart height",
            description="Minimum height of individual charts")
    public Integer getMinChartHeight() { return getIntegerProperty(PROP_MIN_CHART_HEIGHT, DEFAULT_MIN_CHART_HEIGHT); }
    public void setMinChartHeight(Integer value) { setProperty(PROP_MIN_CHART_HEIGHT, value, DEFAULT_MIN_CHART_HEIGHT); }
    public Integer defaultMinChartHeight() { return DEFAULT_MIN_CHART_HEIGHT; }

    @org.omnetpp.common.properties.Property(category="Main",id=PROP_DISPLAY_CHART_DETAILS,displayName="chart details",
            description="Hide or show axes, legend, title and other extra information")
    public Boolean getDisplayChartDetails() { return getBooleanProperty(PROP_DISPLAY_CHART_DETAILS, DEFAULT_DISPLAY_CHART_DETAILS); }
    public void setDisplayChartDetails(Boolean value) { setProperty(PROP_DISPLAY_CHART_DETAILS, value, DEFAULT_DISPLAY_CHART_DETAILS); }
    public Boolean defaultDisplayChartDetails() { return DEFAULT_DISPLAY_CHART_DETAILS; }

    /*---------------------------------------------------------------
     *                   Helpers
     *---------------------------------------------------------------*/

    public Property getProperty(String propertyName) {
        for (Property property : properties)
            if (property.getName().equals(propertyName))
                return property;
        return null;
    }

    public String getStringProperty(String propertyName, String defaultValue) {
        Property property = getProperty(propertyName);
        return property != null ? StringUtils.defaultString(property.getValue()) : defaultValue;
    }

    public Boolean getBooleanProperty(String propertyName, Boolean defaultValue) {
        Property property = getProperty(propertyName);
        return property != null ? Boolean.valueOf(property.getValue()) : defaultValue;
    }

    public Integer getIntegerProperty(String propertyName, Integer defaultValue) {
        Property property = getProperty(propertyName);
        return property != null ? Converter.stringToInteger(property.getValue()) : defaultValue;
    }

    /**
     * Sets the property value in the property list.
     * If the value is null then the property node is deleted.
     * If the editing domain was set it will change the list by executing a command,
     * otherwise it modifies the list directly.
     */
    protected void doSetProperty(String propertyName, String propertyValue) {
        ScaveModelPackage model = ScaveModelPackage.eINSTANCE;
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
        Property property = getProperty(propertyName);

        if (property == null && propertyValue != null ) { // add new property
            property = factory.createProperty();
            property.setName(propertyName);
            property.setValue(propertyValue);
            if (domain == null)
                properties.add(property);
            else
                domain.getCommandStack().execute(
                    AddCommand.create(domain, chartSheet, model.getChartSheet_Properties(), property));
        }
        else if (property != null && propertyValue != null) { // change existing property
            if (domain == null)
                property.setValue(propertyValue);
            else
                domain.getCommandStack().execute(
                    SetCommand.create(domain, property, model.getProperty_Value(), propertyValue));
        }
        else if (property != null && propertyValue == null){ // delete existing property
            if (domain == null)
                properties.remove(property);
            else
                domain.getCommandStack().execute(
                    RemoveCommand.create(domain, chartSheet, model.getChartSheet_Properties(), property));
        }
    }

    public void setProperty(String propertyName, String propertyValue, String defaultValue) {
        if (defaultValue != null && defaultValue.equals(propertyValue))
            propertyValue = null;
        doSetProperty(propertyName, propertyValue);
    }

    public void setProperty(String propertyName, Boolean propertyValue, Boolean defaultValue) {
        if (defaultValue != null && defaultValue.equals(propertyValue))
            propertyValue = null;
        doSetProperty(propertyName, propertyValue == null ? null : String.valueOf(propertyValue));
    }

    public void setProperty(String propertyName, Integer propertyValue, Integer defaultValue) {
        if (defaultValue != null && defaultValue.equals(propertyValue))
            propertyValue = null;
        doSetProperty(propertyName, Converter.integerToString(propertyValue));
    }

    protected IPropertyDescriptor[] createDescriptors(Object defaultId, String[] ids, String[] names) {
        if (ids == null || names == null)
            return new IPropertyDescriptor[0];
        Assert.isTrue(ids.length == names.length);

        IPropertyDescriptor[] descriptors = new IPropertyDescriptor[ids.length+1];
        descriptors[0] = new PropertyDescriptor(defaultId, "default");
        for (int i= 0; i < ids.length; ++i)
            descriptors[i+1] = new PropertyDescriptor(ids[i], names[i]);

        return descriptors;
    }

    protected String propertyName(String baseName, String elementId) {
        return elementId == null ? baseName : baseName + "/" + elementId;
    }

    protected boolean isKnownProperty(Object id) {
        return id instanceof String && getInfo((String)id) != null;
    }
}

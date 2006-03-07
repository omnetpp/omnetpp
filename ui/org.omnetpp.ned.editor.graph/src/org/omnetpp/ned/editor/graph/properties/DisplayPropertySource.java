package org.omnetpp.ned.editor.graph.properties;

import java.util.LinkedHashMap;

import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.omnetpp.ned.editor.graph.misc.ColorFactory;
import org.omnetpp.ned2.model.DisplayString;

// TODO implement the property list as Enum ?
abstract public class DisplayPropertySource implements IPropertySource2 {

    // string parser for handling the parsing of the display string  
    protected DisplayString dispString;

    
    static class IntDesc {
        String tagName;
        int tagPos = 0;
        String id;
        String name;
        String description;
        String category;
        String[] filters;
        Class descriptorClass;
        Class sourceClass;

        public IntDesc(String tagName, int tagPos, String id, String name, String description, String category, String[] filters, Class descriptorClass, Class sourceClass) {
            super();
            this.tagName = tagName;
            this.tagPos = tagPos;
            this.id = id;
            this.name = name;
            this.description = description;
            this.category = category;
            this.filters = filters;
            this.descriptorClass = descriptorClass;
            this.sourceClass = sourceClass;
        }
    }
    
    public DisplayPropertySource() {
        
    }
    
    public DisplayPropertySource(String dispString) {
        setValue(dispString);
    }
    
    /**
     * Set the content and parse it 
     * @param dispString
     */
    public void setValue(String dispStr) {
        dispString = new DisplayString(dispStr);
    }
    
    
    /**
     * @return The readable display string
     */
    public String getValue() {
        return dispString.toString();
    }
    
    public Object getEditableValue() {
        return dispString.toString();
    }

    /**
     * @param propName
     * @return Null if tag does not exist, Tag.DEFAULT_VALUE if the value is empty or the property value itself
     */
    public String getStringProperty(String propName) {
        IntDesc pd = getPropDescMap().get(propName);
        // unknown tag
        if(pd == null) return null;

        return dispString.getTagArg(pd.tagName, pd.tagPos );
    }


    /**
     * Returns the property value as an Integer
     * @param propName
     * @return The value as Integer or 0 if value was empty or non number, 
     *  and <code>null</code> if the tag was not present at all
     */
    public Integer getIntegerProperty(String propName) {
        String strVal = getStringProperty(propName);
        // if tag not present at all
        if(strVal == null) return null;
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        return new Integer(0);
    }

    // returns the tag value as string or the default value 
    public String getStringPropertyDef(String propName) {
        String value = getStringProperty(propName);
        // return null to indicate that the tag was totally missing
        if(value == null) return null;

        // check if this is a variable (starting with $) and use the variable deault set
        if(value.startsWith("$"))
            value = getVariableDefaults().getStringProperty(propName);
        // if tag was present, but the argument was empty, look for default values
        if(DisplayString.Tag.DEFAULT_VALUE.equals(value))
            value = getEmptyDefaults().getStringProperty(propName);
        // if no default was defined for this tag/argument return default_value
        if(value == null) return DisplayString.Tag.DEFAULT_VALUE;

        return value;
    }
    
    /**
     * @param propName
     * @return The value of property as Integer / 
     * the variableDefault if it is a variable ($var) / the global default value / 0 if no 
     * defaults were specified / <code>null</code> if the tag itself does not exist 
     */
    public Integer getIntegerPropertyDef(String propName) {
        String strVal = getStringProperty(propName);
        // if tag not present at all
        if(strVal == null) return null;
        // check if this is a variable (starting with $) and use the variable deault set
        if(strVal.startsWith("$"))
            strVal = getVariableDefaults().getStringProperty(propName);
        
        // otherwise use the original one
        if (!DisplayString.Tag.DEFAULT_VALUE.equals(strVal))
            try {
                return Integer.valueOf(strVal);
            } catch (NumberFormatException e) { 
                return new Integer(0); 
            }
        // look for 'empty' default value if no values were specified either in dispstring or as variable default
        strVal = getEmptyDefaults().getStringProperty(propName);
        // return 0 if no default defined
        if(strVal == null) return new Integer(0);
        try {
            return Integer.valueOf(strVal);
        } catch (NumberFormatException e) { }
        // return 0 if conversion was again unsuccessful
        return new Integer(0);
    }
    

    public int getIntPropertyDef(String propName, int defValue) {
        int val = defValue;
        try {
            val = getIntegerPropertyDef(propName).intValue();
        } catch (Exception e) {
        }
        return val;
    }
    
    // property request for property sheet
    public Object getPropertyValue(Object propName) {
        // TODO create property sources according to the description too
        String tagVal;
        IntDesc pd = getPropDescMap().get(propName);
        // no such property
        if(pd == null)
            tagVal = DisplayString.Tag.DEFAULT_VALUE;  
        else
            tagVal = dispString.getTagArg(pd.tagName, pd.tagPos );
        // if the tag does not exists yet, return default empty value
        if (tagVal == null) tagVal = DisplayString.Tag.DEFAULT_VALUE; 
        
        if(pd.descriptorClass == ColorPropertyDescriptor.class)
            return ColorFactory.asRGB(tagVal);
        
        return tagVal;
    }

    public void setPropertyValue(Object propName, Object value) {
        IntDesc pd = getPropDescMap().get(propName);
        // no such property
        if(pd == null) return;
        // if it is a color, convert it to string
        if(value instanceof RGB)
            value = ColorFactory.asString((RGB)value);
        
        if (value != null)        
            dispString.setTagArg(pd.tagName, pd.tagPos, value.toString());
        else 
            dispString.setTagArg(pd.tagName, pd.tagPos, null);
        
        fireDisplayStringChanged();
    }


    public void resetPropertyValue(Object propName) {
        IntDesc pd = getPropDescMap().get(propName);
        // no such property
        if(pd == null) return;  
        dispString.setTagArg(pd.tagName, pd.tagPos, null);

        fireDisplayStringChanged();
    }

    public boolean isPropertySet(Object propName) {
        IntDesc pd = getPropDescMap().get(propName);
        // no such property so it's not set
        if(pd == null) return false;  
        
        String val = dispString.getTagArg(pd.tagName, pd.tagPos);
        return val != null && !DisplayString.Tag.DEFAULT_VALUE.equals(val);
    }

    public String toString() {
        return getValue();
    }

    public boolean isPropertyResettable(Object id) {
        return true;
    }
    
    abstract protected LinkedHashMap<String,IntDesc> getPropDescMap();
    
    abstract protected DisplayPropertySource getEmptyDefaults();

    abstract protected DisplayPropertySource getVariableDefaults();

    abstract protected void fireDisplayStringChanged(); 

}
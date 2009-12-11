package org.omnetpp.scave.editors;

import org.eclipse.jface.viewers.CellEditor;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model2.ResultItemRef;

/**
 * Property source for result items
 * 
 * @author Andras
 */
//TODO: attributes, histogram bins
public class ResultItemPropertySource implements IPropertySource {
    static class ReadonlyPropertyDescriptor extends TextPropertyDescriptor {
        ReadonlyPropertyDescriptor(Object id, String displayName) {
            super(id, displayName);
        }

        @Override
        public CellEditor createPropertyEditor(Composite parent) {
            return null;
        }
    }

    public static final String PROP_NAME = "Name";
    public static final String PROP_MODULE = "Module";
    public static final String PROP_TYPE = "Type";
    public static final String PROP_VALUE = "Value";
    public static final String PROP_COUNT = "Count";
    public static final String PROP_MIN = "Min";
    public static final String PROP_MAX = "Max";
    public static final String PROP_MEAN = "Mean";
    public static final String PROP_STDDEV = "StdDev";
    public static final String PROP_VARIANCE = "Variance";
    public static final String PROP_START_EVENT_NUM = "Start event number";
    public static final String PROP_END_EVENT_NUM = "End event number";
    public static final String PROP_START_TIME = "Start time";
    public static final String PROP_END_TIME = "End time";

    public static final String[] BASE_PROPERTY_IDS = { PROP_MODULE, PROP_NAME, PROP_TYPE };
    
    public static final String[] SCALAR_PROPERTY_IDS = { 
        PROP_MODULE, PROP_NAME, PROP_TYPE, 
        PROP_VALUE 
    };
    public static final String[] VECTOR_PROPERTY_IDS = { 
        PROP_MODULE, PROP_NAME, PROP_TYPE, 
        PROP_COUNT, PROP_MIN, PROP_MAX, PROP_MEAN, PROP_STDDEV, PROP_VARIANCE,  
        PROP_START_EVENT_NUM, PROP_END_EVENT_NUM, PROP_START_TIME, PROP_END_TIME
    };
    public static final String[] HISTOGRAM_PROPERTY_IDS = { 
        PROP_MODULE, PROP_NAME, PROP_TYPE, 
        PROP_COUNT, PROP_MIN, PROP_MAX, PROP_MEAN, PROP_STDDEV, PROP_VARIANCE  
    };

    protected static final IPropertyDescriptor[] BASE_PROPERTY_DESCS = makeDescriptors(BASE_PROPERTY_IDS);
    protected static final IPropertyDescriptor[] SCALAR_PROPERTY_DESCS = makeDescriptors(SCALAR_PROPERTY_IDS);
    protected static final IPropertyDescriptor[] VECTOR_PROPERTY_DESCS = makeDescriptors(VECTOR_PROPERTY_IDS);
    protected static final IPropertyDescriptor[] HISTOGRAM_PROPERTY_DESCS = makeDescriptors(HISTOGRAM_PROPERTY_IDS);

    private static IPropertyDescriptor[] makeDescriptors(String ids[]) {
        IPropertyDescriptor[] descs = new IPropertyDescriptor[ids.length];
        for (int i = 0; i < descs.length; i++)
            descs[i] = new ReadonlyPropertyDescriptor(ids[i], StringUtils.capitalize(ids[i]));
        return descs;
    }

    private ResultFileManager manager;
    private long id;

    public ResultItemPropertySource(ResultItemRef resultItemRef) {
        manager = resultItemRef.getResultFileManager();
        id = resultItemRef.getID();
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        ResultItem item = manager.getItem(id);
        if (item instanceof VectorResult)
            return VECTOR_PROPERTY_DESCS;
        if (item instanceof ScalarResult)
            return SCALAR_PROPERTY_DESCS;
        if (item instanceof HistogramResult)
            return HISTOGRAM_PROPERTY_DESCS;
        return BASE_PROPERTY_DESCS;
    }

    public Object getPropertyValue(Object propertyId) {
        ResultItem resultItem = manager.getItem(id);
        
        if (propertyId.equals(PROP_MODULE)) return resultItem.getModuleName();
        if (propertyId.equals(PROP_NAME)) return resultItem.getName();
        if (propertyId.equals(PROP_TYPE)) return resultItem.getType().toString().replaceAll("TYPE_", "").toLowerCase();

        if (resultItem instanceof ScalarResult) {
            ScalarResult scalar = (ScalarResult)resultItem;
            if (propertyId.equals(PROP_VALUE)) return scalar.getValue();
        }
        else if (resultItem instanceof VectorResult) {
            VectorResult vector = (VectorResult)resultItem;
            if (propertyId.equals(PROP_COUNT)) return vector.getCount();
            if (propertyId.equals(PROP_MIN)) return vector.getMin();
            if (propertyId.equals(PROP_MAX)) return vector.getMax();
            if (propertyId.equals(PROP_MEAN)) return vector.getMean();
            if (propertyId.equals(PROP_STDDEV)) return vector.getStddev();
            if (propertyId.equals(PROP_VARIANCE)) return vector.getVariance();
            if (propertyId.equals(PROP_START_EVENT_NUM)) return vector.getStartEventNum();
            if (propertyId.equals(PROP_END_EVENT_NUM)) return vector.getEndEventNum();
            if (propertyId.equals(PROP_START_TIME)) return vector.getStartTime();
            if (propertyId.equals(PROP_END_TIME)) return vector.getEndTime();
        }
        else if (resultItem instanceof HistogramResult) {
            HistogramResult histogram = (HistogramResult)resultItem;
            if (propertyId.equals(PROP_COUNT)) return histogram.getCount();
            if (propertyId.equals(PROP_MIN)) return histogram.getMin();
            if (propertyId.equals(PROP_MAX)) return histogram.getMax();
            if (propertyId.equals(PROP_MEAN)) return histogram.getMean();
            if (propertyId.equals(PROP_STDDEV)) return histogram.getStddev();
            if (propertyId.equals(PROP_VARIANCE)) return histogram.getVariance();
        }
        throw new IllegalArgumentException("no such property: "+ propertyId);
    }

    public boolean isPropertySet(Object id) {
        return false;
    }

    public void resetPropertyValue(Object id) {
    }

    public void setPropertyValue(Object id, Object value) {
    }

    public Object getEditableValue() {
        return null; // not editable
    }
}

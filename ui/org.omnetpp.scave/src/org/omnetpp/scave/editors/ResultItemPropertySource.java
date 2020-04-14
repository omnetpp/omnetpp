package org.omnetpp.scave.editors;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.DoubleVector;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Scave;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model2.ResultItemRef;

/**
 * Property source for result items
 *
 * @author Andras
 */
//FIXME: what if "id" becomes invalid? do we need a lock to access ResultFileManager?
//TODO copy stuff from DataTable.getCellValue()
public class ResultItemPropertySource implements IPropertySource {
    // display labels as well as property IDs
    public static final String PROP_DIRECTORY = "Folder";
    public static final String PROP_FILE = "File";
    public static final String PROP_CONFIG = "Config";
    public static final String PROP_RUNNUMBER = "Run number";
    public static final String PROP_RUN_ID = "Run Id";
    public static final String PROP_EXPERIMENT = "Experiment";
    public static final String PROP_MEASUREMENT = "Measurement";
    public static final String PROP_REPLICATION = "Replication";
    public static final String PROP_KIND = "Kind";
    public static final String PROP_NAME = "Name";
    public static final String PROP_MODULE = "Module";
    public static final String PROP_TYPE = "Type";
    public static final String PROP_VALUE = "Value";
    public static final String PROP_COUNT = "Count";
    public static final String PROP_SUMWEIGHTS = "Sum of weights";
    public static final String PROP_MIN = "Min";
    public static final String PROP_MAX = "Max";
    public static final String PROP_MEAN = "Mean";
    public static final String PROP_STDDEV = "StdDev";
    public static final String PROP_NUMBINS = "Number of bins";
    public static final String PROP_VECTOR_ID = "Vector Id";
    public static final String PROP_START_EVENT_NUM = "Start event number";
    public static final String PROP_END_EVENT_NUM = "End event number";
    public static final String PROP_START_TIME = "Start time";
    public static final String PROP_END_TIME = "End time";

    public static final String[] BASE_PROPERTY_IDS = {
        PROP_KIND, PROP_MODULE, PROP_NAME, PROP_TYPE
    };

    public static final String[] CONTAINER_PROPERTY_IDS = {
        PROP_DIRECTORY, PROP_FILE, PROP_RUN_ID,
        PROP_CONFIG, PROP_RUNNUMBER,
        PROP_EXPERIMENT, PROP_MEASUREMENT, PROP_REPLICATION
    };

    public static final String[] SCALAR_PROPERTY_IDS = {
        PROP_VALUE
    };
    public static final String[] VECTOR_PROPERTY_IDS = {
        PROP_COUNT, PROP_MIN, PROP_MAX, PROP_MEAN, PROP_STDDEV,
        PROP_START_EVENT_NUM, PROP_END_EVENT_NUM, PROP_START_TIME, PROP_END_TIME,
        PROP_VECTOR_ID
    };
    public static final String[] STATISTICS_PROPERTY_IDS = {
        PROP_COUNT, PROP_SUMWEIGHTS, PROP_MEAN, PROP_STDDEV, PROP_MIN, PROP_MAX
    };
    public static final String[] HISTOGRAM_PROPERTY_IDS = {
        PROP_COUNT, PROP_SUMWEIGHTS, PROP_MEAN, PROP_STDDEV, PROP_MIN, PROP_MAX, PROP_NUMBINS
    };

    protected static final IPropertyDescriptor[] BASE_PROPERTY_DESCS = makeDescriptors(BASE_PROPERTY_IDS, "", "General"); // not "Base" because we this to appear after "Fields" in the property sheet
    protected static final IPropertyDescriptor[] CONTAINER_PROPERTY_DESCS = makeDescriptors(CONTAINER_PROPERTY_IDS, "", "Location"); // not "Container" because we want this group to appear at the bottom of the property sheet
    protected static final IPropertyDescriptor[] SCALAR_PROPERTY_DESCS = makeDescriptors(SCALAR_PROPERTY_IDS, "", "Fields");
    protected static final IPropertyDescriptor[] VECTOR_PROPERTY_DESCS = makeDescriptors(VECTOR_PROPERTY_IDS, "", "Fields");
    protected static final IPropertyDescriptor[] STATISTICS_PROPERTY_DESCS = makeDescriptors(STATISTICS_PROPERTY_IDS, "", "Fields");
    protected static final IPropertyDescriptor[] HISTOGRAM_PROPERTY_DESCS = makeDescriptors(HISTOGRAM_PROPERTY_IDS, "", "Fields");

    private static IPropertyDescriptor[] makeDescriptors(String ids[], String prefix, String category) {
        IPropertyDescriptor[] descs = new IPropertyDescriptor[ids.length];
        for (int i = 0; i < descs.length; i++)
            descs[i] = new ReadonlyPropertyDescriptor(prefix+ids[i], StringUtils.capitalize(ids[i]), category);
        return descs;
    }

    private ResultFileManager manager;
    private long id;

    public ResultItemPropertySource(ResultItemRef resultItemRef) {
        manager = resultItemRef.getResultFileManager();
        id = resultItemRef.getID();
    }

    public IPropertyDescriptor[] getPropertyDescriptors() {
        return ResultFileManager.callWithReadLock(manager, () -> {
            ResultItem item = manager.getItem(id);

            IPropertyDescriptor[] fields =
                    (item instanceof VectorResult) ? VECTOR_PROPERTY_DESCS :
                        (item instanceof ScalarResult) ? SCALAR_PROPERTY_DESCS :
                            (item instanceof HistogramResult) ? HISTOGRAM_PROPERTY_DESCS :
                                (item instanceof StatisticsResult) ? STATISTICS_PROPERTY_DESCS :
                                    new IPropertyDescriptor[0];

            IPropertyDescriptor[] attrs = makeDescriptors(item.getAttributes().keys().toArray(), "@", "Attributes");

            IPropertyDescriptor[] bins = null;
            if (item instanceof HistogramResult) {
                HistogramResult histogram = (HistogramResult)item;
                DoubleVector binEdges = histogram.getHistogram().getBinEdges();
                int numBins = histogram.getHistogram().getNumBins();
                bins = new IPropertyDescriptor[numBins];
                for (int i = 0; i < numBins; i++) {
                    double bin1 = binEdges.get(i);
                    double bin2 = binEdges.get(i+1);
                    bins[i] = new ReadonlyPropertyDescriptor("%"+i, "[" + String.valueOf(bin1) + ", " + String.valueOf(bin2) + ")", "Bins");
                }
            }
            return concat(BASE_PROPERTY_DESCS, CONTAINER_PROPERTY_DESCS, fields, attrs, bins);
        });
    }

    private IPropertyDescriptor[] concat(IPropertyDescriptor[] a1, IPropertyDescriptor[] a2, IPropertyDescriptor[] a3, IPropertyDescriptor[] a4, IPropertyDescriptor[] a5) {
        return (IPropertyDescriptor[])ArrayUtils.addAll(a1, ArrayUtils.addAll(a2, ArrayUtils.addAll(a3, ArrayUtils.addAll(a4, a5))));
    }

    public Object getPropertyValue(final Object propertyId) {
        return ResultFileManager.callWithReadLock(manager, () -> {
            ResultItem resultItem = manager.getItem(id);

            // result attribute
            if (propertyId instanceof String && propertyId.toString().charAt(0)=='@')
                return resultItem.getAttribute(propertyId.toString().substring(1));

            // histogram bin
            if (resultItem instanceof HistogramResult && propertyId instanceof String && propertyId.toString().charAt(0)=='%') {
                int i = Integer.parseInt(propertyId.toString().substring(1));
                double value = ((HistogramResult)resultItem).getHistogram().getBinValues().get(i);
                double valueFloor = Math.floor(value);
                return value == valueFloor ? String.valueOf((long)valueFloor) : String.valueOf(value);
            }

            if (propertyId.equals(PROP_DIRECTORY)) return resultItem.getFileRun().getFile().getDirectory();
            if (propertyId.equals(PROP_FILE)) return resultItem.getFileRun().getFile().getFileName();
            if (propertyId.equals(PROP_CONFIG)) return StringUtils.nullToEmpty(resultItem.getFileRun().getRun().getAttribute(Scave.CONFIGNAME));
            if (propertyId.equals(PROP_RUNNUMBER)) return StringUtils.nullToEmpty(resultItem.getFileRun().getRun().getAttribute(Scave.RUNNUMBER));
            if (propertyId.equals(PROP_RUN_ID)) return resultItem.getFileRun().getRun().getRunName();
            if (propertyId.equals(PROP_EXPERIMENT)) return StringUtils.nullToEmpty(resultItem.getFileRun().getRun().getAttribute(Scave.EXPERIMENT));
            if (propertyId.equals(PROP_MEASUREMENT)) return StringUtils.nullToEmpty(resultItem.getFileRun().getRun().getAttribute(Scave.MEASUREMENT));
            if (propertyId.equals(PROP_REPLICATION)) return StringUtils.nullToEmpty(resultItem.getFileRun().getRun().getAttribute(Scave.REPLICATION));

            if (propertyId.equals(PROP_MODULE)) return resultItem.getModuleName();
            if (propertyId.equals(PROP_NAME)) return resultItem.getName();
            if (propertyId.equals(PROP_TYPE)) return resultItem.getDataType().toString().replaceAll("TYPE_", "").toLowerCase();

            if (resultItem instanceof ScalarResult) {
                ScalarResult scalar = (ScalarResult)resultItem;
                if (propertyId.equals(PROP_KIND)) return "scalar";
                if (propertyId.equals(PROP_VALUE)) return scalar.getValue();
            }
            else if (resultItem instanceof VectorResult) {
                VectorResult vector = (VectorResult)resultItem;
                if (propertyId.equals(PROP_KIND)) return "vector";
                if (propertyId.equals(PROP_COUNT)) return vector.getStatistics().getCount();
                if (propertyId.equals(PROP_MIN)) return vector.getStatistics().getMin();
                if (propertyId.equals(PROP_MAX)) return vector.getStatistics().getMax();
                if (propertyId.equals(PROP_MEAN)) return vector.getStatistics().getMean();
                if (propertyId.equals(PROP_STDDEV)) return vector.getStatistics().getStddev();
                if (propertyId.equals(PROP_START_EVENT_NUM)) return vector.getStartEventNum();
                if (propertyId.equals(PROP_END_EVENT_NUM)) return vector.getEndEventNum();
                if (propertyId.equals(PROP_START_TIME)) return vector.getStartTime();
                if (propertyId.equals(PROP_END_TIME)) return vector.getEndTime();
                if (propertyId.equals(PROP_VECTOR_ID)) return vector.getVectorId();
            }
            else if (resultItem instanceof StatisticsResult) {
                StatisticsResult statistics = (StatisticsResult)resultItem;
                if (propertyId.equals(PROP_KIND)) return (statistics instanceof HistogramResult ? "histogram" : "statistics") + ", "
                + (statistics.getStatistics().isWeighted() ? "weighted" : "unweighted");
                if (propertyId.equals(PROP_COUNT)) return statistics.getStatistics().getCount();
                if (propertyId.equals(PROP_SUMWEIGHTS)) return statistics.getStatistics().isWeighted() ? statistics.getStatistics().getSumWeights() : "n/a";
                if (propertyId.equals(PROP_MIN)) return statistics.getStatistics().getMin();
                if (propertyId.equals(PROP_MAX)) return statistics.getStatistics().getMax();
                if (propertyId.equals(PROP_MEAN)) return statistics.getStatistics().getMean();
                if (propertyId.equals(PROP_STDDEV)) return statistics.getStatistics().getStddev();
                if (propertyId.equals(PROP_NUMBINS)) return statistics instanceof HistogramResult ? ((HistogramResult)resultItem).getHistogram().getNumBins() : "n/a";
            }
            else {
                if (propertyId.equals(PROP_KIND)) return "other";
            }
            throw new IllegalArgumentException("no such property: "+ propertyId);
        });
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

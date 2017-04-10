/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.FILE;
import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;
import static org.omnetpp.scave.engine.ResultItemField.RUN;
import static org.omnetpp.scave.engine.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave.engine.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave.engine.RunAttribute.REPLICATION;
import static org.omnetpp.scave.engine.RunAttribute.RUNNUMBER;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.apache.commons.lang3.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.dataset.CompoundXYDataset;
import org.omnetpp.scave.charting.dataset.HistogramDataset;
import org.omnetpp.scave.charting.dataset.IHistogramDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.dataset.ScalarScatterPlotDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.charting.dataset.VectorScatterPlotDataset;
import org.omnetpp.scave.engine.DataSorter;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYDataset;
import org.omnetpp.scave.engine.XYDatasetVector;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;

/**
 * This class calculates the content of a chart
 * applying the operations described by the chart.
 *
 * @author tomi
 */
public class DatasetManager {

    private static final boolean debug = false;

    public static IDList getIDListFromDataset(ResultFileManager manager, Chart chart, ResultType type) {
        checkReadLock(manager);
        
        //TODO rewrite!
        
        IDList idList;
        switch (type) {
	        case SCALAR_LITERAL: idList = manager.getAllScalars(); break;
	        case VECTOR_LITERAL: idList = manager.getAllVectors(); break;
	        case HISTOGRAM_LITERAL: idList = manager.getAllHistograms(); break;
	        default: idList = new IDList(); //TODO 
        }
        
        String filterExpression = chart.getInput();
        if (StringUtils.isBlank(filterExpression))
        	idList = new IDList();
        else {
        	System.out.println("FILTER: '" + filterExpression + "'");
        	idList = manager.filterIDList(idList, filterExpression);
        	System.out.println("MATCHED " + idList.size() + " ITEMS");
        }
        return idList;
    }

    public static XYArray[] getDataFromDataset(ResultFileManager manager, Chart chart) {
    	return new XYArray[0];
//TODO    	
//        checkReadLock(manager);
//        DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
//        DataflowManager dataflowManager = builder.build(chart, lastItemToProcess, true);
//        List<Node> arrayBuilders = builder.getArrayBuilders();
//        if (debug) dataflowManager.dump();
//        XYArray[] result;
//        try {
//            result = executeDataflowNetwork(dataflowManager, arrayBuilders, null);
//        }
//        finally {
//            dataflowManager.delete();
//        }
//        return result;
    }

    public static XYArray[] getDataOfVectors(ResultFileManager manager, IDList idlist, IProgressMonitor monitor) {
    	return new XYArray[0];
//TODO    	
//        checkReadLock(manager);
//        DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
//        DataflowManager dataflowManager = builder.build(idlist);
//        List<Node> arrayBuilders = builder.getArrayBuilders();
//        XYArray[] result;
//        try {
//            result = executeDataflowNetwork(dataflowManager, arrayBuilders, monitor);
//        }
//        finally {
//            dataflowManager.delete();
//        }
//        return result;
    }

//    private static XYArray[] executeDataflowNetwork(DataflowManager manager, List<Node> arrayBuilders, IProgressMonitor monitor) {
//        long startTime = System.currentTimeMillis();
//        if (arrayBuilders.size() > 0) // XXX DataflowManager crashes when there are no sinks
//            manager.execute(monitor);
//        if (debug) Debug.println("execute dataflow network: "+(System.currentTimeMillis()-startTime)+" ms");
//
//        XYArray[] result = new XYArray[arrayBuilders.size()];
//        for (int i = 0; i < result.length; ++i)
//            result[i] = arrayBuilders.get(i).getArray();
//        return result;
//    }

    public static ScalarDataset createScalarDataset(BarChart chart, ResultFileManager manager, IProgressMonitor progressMonitor) {
        //TODO update progressMonitor
        checkReadLock(manager);
        IDList idlist = DatasetManager.getIDListFromDataset(manager, chart, ResultType.SCALAR_LITERAL);
        return new ScalarDataset(idlist, chart.getGroupByFields(), chart.getBarFields(),
                                        chart.getAveragedFields(), manager);
    }

    public static VectorDataset createVectorDataset(LineChart chart, ResultFileManager manager, IProgressMonitor progressMonitor) {
        checkReadLock(manager);
        return createVectorDataset(chart, chart.getLineNameFormat(), true, manager, progressMonitor);
    }

    public static VectorDataset createVectorDataset(LineChart chart, String lineNameFormat, boolean computeData, ResultFileManager manager, IProgressMonitor progressMonitor) {
    	return new VectorDataset();
//TODO    	
//        checkReadLock(manager);
//
//        DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
//        DataflowManager dataflowManager = builder.build(chart, chart, computeData);
//        IDList idlist = builder.getDisplayedIDs();
//
//        XYArray[] dataValues = null;
//        if (dataflowManager != null) {
//            List<Node> arrayBuilders = builder.getArrayBuilders();
//            if (debug) dataflowManager.dump();
//            try {
//                dataValues = executeDataflowNetwork(dataflowManager, arrayBuilders, progressMonitor);
//            }
//            finally {
//                dataflowManager.delete();
//                dataflowManager = null;
//            }
//        }
//
//        if (progressMonitor != null && progressMonitor.isCanceled())
//            return null;
//
//        ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
//        String title = items.length <= 1 ? null : defaultTitle(items);
//        String lineTitleFormat = defaultResultItemTitleFormat(items);
//
//        return dataValues != null ?
//                new VectorDataset(title, idlist, dataValues, lineNameFormat, lineTitleFormat, manager) :
//                new VectorDataset(title, idlist, lineNameFormat, lineTitleFormat, manager);
    }

//    public static Pair<IDList,XYArray[]> readAndComputeVectorData(Chart chart, DatasetItem target, ResultFileManager manager, IProgressMonitor monitor) {
//        Assert.isNotNull(chart);
//        checkReadLock(manager);
//
//        DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
//        DataflowManager dataflowManager = builder.build(chart, target, true);
//        IDList idlist = builder.getDisplayedIDs();
//
//        List<Node> arrayBuilders = builder.getArrayBuilders();
//        if (debug) dataflowManager.dump();
//        XYArray[] dataValues;
//        try {
//            dataValues = executeDataflowNetwork(dataflowManager, arrayBuilders, monitor);
//        }
//        finally {
//            dataflowManager.delete();
//        }
//
//        return new Pair<IDList, XYArray[]>(idlist, dataValues);
//    }

    public static IXYDataset createScatterPlotDataset(ScatterChart chart, ResultFileManager manager, IProgressMonitor monitor) {
        checkReadLock(manager);

        //TODO update progressMonitor
        if (chart != null) {
            IDList scalars = DatasetManager.getIDListFromDataset(manager, chart, ResultType.SCALAR_LITERAL);
            IDList vectors = DatasetManager.getIDListFromDataset(manager, chart, ResultType.VECTOR_LITERAL);

            // process scalars
            XYDatasetVector xyScalars = null;
            List<IsoLineData[]> isoLineIds = null;
            if (!scalars.isEmpty()) {
                IsoLineData xData = IsoLineData.fromFilterPattern(chart.getXDataPattern());
                Assert.isLegal(xData != null && xData.getModuleName() != null && xData.getDataName() != null, "X data is not selected.");

                String xModuleName = xData.getModuleName();
                String xScalarName = xData.getDataName();
                List<String> isoPatterns = chart.getIsoDataPattern();
                boolean averageReplications = chart.isAverageReplications();

                StringVector isoModuleNames = new StringVector();
                StringVector isoScalarNames = new StringVector();
                StringVector isoAttrNames = new StringVector();
                collectIsoParameters(isoPatterns, isoModuleNames, isoScalarNames, isoAttrNames);

                DataSorter sorter = new DataSorter(manager);
                ResultItemFields rowFields = new ResultItemFields(MODULE, NAME);
                ResultItemFields columnFields = averageReplications ? new ResultItemFields(EXPERIMENT, MEASUREMENT) :
                                                                  new ResultItemFields(RUN, EXPERIMENT, MEASUREMENT, REPLICATION);
                xyScalars = sorter.prepareScatterPlot3(scalars,
                                xModuleName, xScalarName, rowFields, columnFields,
                                isoModuleNames, isoScalarNames, new ResultItemFields(isoAttrNames));
                // XXX isoLineIds should be returned by prepareScatterPlot3
                isoLineIds = collectIsoLineIds(scalars, isoModuleNames, isoScalarNames, isoAttrNames, xyScalars, manager);
                // assertOrdered(xyScalars);
            }

            // process vectors
            XYArray[] xyVectors = null;
            IDList yVectors = null;
//TODO            
//            if (!vectors.isEmpty()) {
//                DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
//                DataflowManager dataflowManager = builder.build(chart, chart, true /*computeData*/); // XXX
//                yVectors = builder.getDisplayedIDs();
//
//                if (dataflowManager != null) {
//                    List<Node> arrayBuilders = builder.getArrayBuilders();
//                    if (debug) dataflowManager.dump();
//                    try {
//                        xyVectors = executeDataflowNetwork(dataflowManager, arrayBuilders, monitor);
//                    }
//                    finally {
//                        dataflowManager.delete();
//                        dataflowManager = null;
//                    }
//                    if (xyVectors != null) {
//                        for (int i = 0; i < xyVectors.length; ++i)
//                            xyVectors[i].sortByX();
//                    }
//                }
//            }

            // compose results
            if (xyScalars != null && xyVectors == null)
                return createScatterPlotDataset(xyScalars, isoLineIds, manager);
            else if (xyScalars == null && xyVectors != null)
                return new VectorScatterPlotDataset(yVectors, xyVectors, manager);
            else if (xyScalars != null && xyVectors != null)
                return new CompoundXYDataset(
                        createScatterPlotDataset(xyScalars, isoLineIds, manager),
                        new VectorScatterPlotDataset(yVectors, xyVectors, manager));

        }
        return null;
    }

    /**
     * Collect the module/scalar names or attribute names from the filter patterns.
     */
    private static void collectIsoParameters(List<String> isoPatterns,
                            StringVector isoModuleNames, StringVector isoScalarNames, StringVector isoAttrNames) {

        for (String pattern : isoPatterns) {
            IsoLineData filter = IsoLineData.fromFilterPattern(pattern);
            if (filter != null && filter.isValid()) {
                if (filter.getModuleName() != null && filter.getDataName() != null) {
                    isoModuleNames.add(filter.getModuleName());
                    isoScalarNames.add(filter.getDataName());
                }
                else if (filter.getAttributeName() != null) {
                    isoAttrNames.add(filter.getAttributeName());
                }
            }
        }
    }

    private static List<IsoLineData[]> collectIsoLineIds(IDList scalars,
            StringVector isoModuleNames, StringVector isoScalarNames, StringVector isoAttrNames,
            XYDatasetVector xyScalars, ResultFileManager manager) {
        checkReadLock(manager);
        List<IsoLineData[]> isoLineIds = new ArrayList<IsoLineData[]>((int)xyScalars.size());
        for (int i = 0; i < xyScalars.size(); ++i) {
            XYDataset xyData = xyScalars.get(i);
            Assert.isTrue(xyData.getRowCount() > 0);
            String runName = xyData.getRowFieldNoCheck(0, FLD_RUN);
            FilterUtil filter = new FilterUtil();
            filter.setField(RUN, runName);
            List<IsoLineData> isoLineId = new ArrayList<IsoLineData>((int)(isoModuleNames.size()+isoAttrNames.size()));
            for (int j = 0; j < isoModuleNames.size(); ++j) {
                filter.setField(MODULE, isoModuleNames.get(j));
                filter.setField(NAME, isoScalarNames.get(j));
                IDList idlist = manager.filterIDList(scalars, filter.getFilterPattern());
                if (idlist.size() > 0) {
                    ScalarResult scalar = manager.getScalar(idlist.get(0));
                    IsoLineData data = new IsoLineData(isoModuleNames.get(j), isoScalarNames.get(j));
                    data.setValue(String.valueOf(scalar.getValue()));
                    isoLineId.add(data);
                }
            }
            for (int j = 0; j < isoAttrNames.size(); ++j) {
                String attrName = isoAttrNames.get(j);
                String value = xyData.getRowFieldNoCheck(0, new ResultItemField(attrName));
                if (value != null) {
                    IsoLineData data = new IsoLineData(attrName);
                    data.setValue(value);
                    isoLineId.add(data);
                }
            }
            isoLineIds.add(isoLineId.toArray(new IsoLineData[isoLineId.size()]));
        }

        return isoLineIds;
    }

    public static void assertOrdered(XYDatasetVector xyDatasets) {
        for(int i = 0; i < xyDatasets.size(); ++i) {
            XYDataset ds = xyDatasets.get(i);
            double prevX = Double.NEGATIVE_INFINITY;
            for (int j = 0; j < ds.getColumnCount(); ++j) {
                double x = ds.getValue(0, j).getMean();
                if (!Double.isNaN(x)) {
                    if (x < prevX)
                        Assert.isTrue(false, "Not ordered");
                    prevX = x;
                }
            }
        }
    }

    public static IXYDataset createScatterPlotDataset(XYDatasetVector xydatasets, List<IsoLineData[]> isoLineIds, ResultFileManager manager) {
        Assert.isTrue(isoLineIds == null || xydatasets.size() == isoLineIds.size());
        checkReadLock(manager);

        IXYDataset[] datasets = new IXYDataset[(int)xydatasets.size()];
        for (int i = 0; i < datasets.length; ++i)
            datasets[i] = new ScalarScatterPlotDataset(xydatasets.get(i), isoLineIds == null ? null : isoLineIds.get(i));
        return new CompoundXYDataset(datasets);
    }

    public static IXYDataset createXYDataset(Chart chart, boolean computeData, ResultFileManager manager, IProgressMonitor monitor) {
        checkReadLock(manager);

        if (chart instanceof LineChart) {
            LineChart lineChart = (LineChart)chart;
            return createVectorDataset(lineChart, lineChart.getLineNameFormat(), computeData, manager, monitor);
        }
        else if (chart instanceof ScatterChart)
            return createScatterPlotDataset((ScatterChart)chart, manager, monitor);
        else
            return null;
    }

    public static IHistogramDataset createHistogramDataset(HistogramChart chart, ResultFileManager manager, IProgressMonitor monitor) {
        checkReadLock(manager);
        IDList idlist = chart != null ? getIDListFromDataset(manager, chart, ResultType.HISTOGRAM_LITERAL): new IDList();
        return new HistogramDataset(idlist, manager);
    }

    public static String[] getResultItemNames(IDList idlist, String nameFormat, ResultFileManager manager) {
        checkReadLock(manager);
        ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
        String format = StringUtils.defaultIfEmpty(nameFormat, defaultNameFormat(items));
        return ResultItemFormatter.formatResultItems(format, items);
    }

    private static final ResultItemField
        FLD_FILE = new ResultItemField(FILE),
        FLD_RUN = new ResultItemField(RUN),
        FLD_RUNNUMBER = new ResultItemField(RUNNUMBER),
        FLD_MODULE = new ResultItemField(MODULE),
        FLD_NAME = new ResultItemField(NAME),
        FLD_EXPERIMENT = new ResultItemField(EXPERIMENT),
        FLD_MEASUREMENT = new ResultItemField(MEASUREMENT),
        FLD_REPLICATION = new ResultItemField(REPLICATION);

    private static final ResultItemField[] FIELDS_OF_LINENAMES = new ResultItemField[] {
        FLD_FILE, FLD_RUN,
        FLD_RUNNUMBER, FLD_MODULE,
        FLD_NAME, FLD_EXPERIMENT,
        FLD_MEASUREMENT, FLD_REPLICATION
    };

    private static final List<ResultItemField> complement(List<ResultItemField> fields) {
        List<ResultItemField> result = new ArrayList<ResultItemField>(Arrays.asList(FIELDS_OF_LINENAMES));
        result.removeAll(fields);
        return result;
    }

    /**
     * Returns the default format string for names of the {@code items}.
     * It is "${file} ${run} ${run-number} ${module} ${name} ${experiment} ${measurement} ${replication}",
     * but fields that are the same for each item are omitted.
     * If all the fields has the same value in {@code items}, then the "${index}" is used as
     * the format string.
     */
    private static String defaultNameFormat(ResultItem[] items) {

        if (items.length <= 1)
            return "${module} ${name}";

        List<ResultItemField> differentFields = complement(getCommonFields(items, FIELDS_OF_LINENAMES));
        if (differentFields.isEmpty())
            return "${module} ${name} - ${index}";
        else
            return nameFormatUsingFields(differentFields);
    }

    public static String defaultResultItemTitleFormat(ResultItem[] items) {
        String format = defaultNameFormat(items);
        return format.replace("${name}", "${title_or_name}");
    }

    private static final ResultItemField[] titleFields = new ResultItemField[] {
        FLD_NAME, FLD_MODULE, FLD_EXPERIMENT, FLD_MEASUREMENT, FLD_REPLICATION
    };

    public static String defaultTitle(ResultItem[] items) {
        List<ResultItemField> fields = getCommonFields(items, titleFields);
        // remove computed file name
        if (fields.contains(FLD_NAME) || fields.contains(FLD_MODULE)) {
            fields.remove(FLD_EXPERIMENT);
            fields.remove(FLD_MEASUREMENT);
            fields.remove(FLD_REPLICATION);
        }
        return fields.isEmpty() ? null : ResultItemFormatter.formatResultItem(nameFormatUsingFields(fields), items[0]);
    }

    /**
     * Returns the fields from {@code fields} that has the same value
     * in {@code items}.
     */
    private static List<ResultItemField> getCommonFields(ResultItem[] items, ResultItemField[] fields) {
        List<ResultItemField> commonFields = new ArrayList<ResultItemField>();
        int itemCount = items.length;

        if (itemCount > 0) {
            for (ResultItemField field : fields) {
                String firstValue = field.getFieldValue(items[0]);
                int i;
                for (i = 1; i < itemCount; ++i) {
                    String value = field.getFieldValue(items[i]);
                    if (!ObjectUtils.equals(firstValue, value))
                        break;
                }
                if (i == itemCount)
                    commonFields.add(field);
            }
        }

        return commonFields;
    }

    /**
     * Generates a format string from the specified fields
     * for formatting result items.
     */
    private static String nameFormatUsingFields(List<ResultItemField> fields) {
        StringBuilder sbFormat = new StringBuilder();
        char separator = ' ';
        int lastIndex = fields.size() - 1;
        for (int i = 0; i < fields.size(); i++) {
            sbFormat.append("${").append(fields.get(i).getName()).append('}');
            if (i != lastIndex)
                sbFormat.append(separator);
        }
        return sbFormat.toString();
    }

//    /**
//     * Selects ids from a {@code source} IDList.
//     * If no filters given or the first filter is a Deselect, then a "Select all" is implicitly
//     * executed first.
//     */
//    public static IDList select(IDList source, List<SelectDeselectOp> filters, ResultFileManager manager, ResultType type) {
//        Assert.isNotNull(source);
//        Assert.isNotNull(filters);
//        checkReadLock(manager);
//
//        // Select all if no select at the beginning
//        IDList result = new IDList();
//        if (filters.isEmpty() || filters.get(0) instanceof Deselect) {
//            if (type == null)
//                result.merge(source);
//            else {
//                int resultType = ScaveModelUtil.asInternalResultType(type);
//                for (int i = 0; i < source.size(); ++i) {
//                    long id = source.get(i);
//                    if (ResultFileManager.getTypeOf(id) == resultType)
//                        result.add(id);
//                }
//            }
//        }
//        // Execute filters
//        for (SelectDeselectOp filter : filters) {
//            if (type == null || filter.getType()==type) {
//                if (filter instanceof Select) {
//                    result.merge(select(source, filter, manager));
//                }
//                else if (filter instanceof Deselect) {
//                    result.subtract(select(source, filter, manager));
//                }
//            }
//        }
//        return result;
//    }
//
//    public static IDList select(IDList source, SetOperation op, ResultFileManager manager) {
//        checkReadLock(manager);
//
//        // check cached IDs
//        if (op.getCachedIDs() instanceof IDList) {
//            IDList cachedIDs = (IDList)op.getCachedIDs();
//            if (cachedIDs.size() > 0 && !manager.hasStaleID(cachedIDs))
//                return cachedIDs;
//        }
//
//        // select IDs by the filter
//        Chart sourceDataset = op.getSourceDataset();
//        ResultType type = op.getType();
//        IDList idlist = selectInternal(source, sourceDataset, type, op.getFilterPattern(), manager);
//
//        // remove IDs matching Excepts
//        List<Except> excepts = null;
//        if (op instanceof Select)
//            excepts = ((Select)op).getExcepts();
//        else if (op instanceof Deselect)
//            excepts = ((Deselect)op).getExcepts();
//        else if (op instanceof Add)
//            excepts = ((Add)op).getExcepts();
//        else if (op instanceof Discard)
//            excepts = ((Discard)op).getExcepts();
//
//        if (excepts != null)
//            for (Except except : excepts)
//                idlist.subtract(selectInternal(idlist, sourceDataset, type, except.getFilterPattern(), manager));
//
//        // update cached IDs
//        if (op.getCachedIDs() != null)
//            op.setCachedIDs(idlist);
//
//        return idlist;
//    }
//
//    private static IDList selectInternal(IDList source, Chart sourceDataset, ResultType type, String filterPattern, ResultFileManager manager) {
//        IDList sourceIDList = source != null ? source :
//                              sourceDataset == null ? ScaveModelUtil.getAllIDs(manager, type) :
//                              DatasetManager.getIDListFromDataset(manager, sourceDataset, null, type);
//
//        return ScaveModelUtil.filterIDList(sourceIDList, new Filter(filterPattern), manager);
//    }
//
//    /*
//     * Computed vectors
//     */
//    static long ensureComputedResultItem(ProcessingOp operation, IDList inputs, int vectorId, ResultFileManager manager, /*out*/ List<IStatus> warnings) {
//        Assert.isTrue(inputs.size() > 0);
//        checkReadLock(manager);
//        long computationID = getFilterNodeID(operation);
//        long id = manager.getComputedID(computationID, inputs.get(0)); // XXX
//        if (id == -1) {
//            //Debug.format("Add computed vector: (%x,%x)%n", computationID, inputID);
//            String name = calculateComputedVectorName(operation, inputs, manager);
//            String fileName = ComputedResultFileLocator.instance().getComputedFile(operation);
//            if (fileName == null)
//                throw new RuntimeException("Cannot locate folder for computed file.");
//            VectorResult vector = manager.getVector(inputs.get(0)); // XXX
//            StringMap attributes = mapVectorAttributes(vector, operation, warnings);
//            id = manager.addComputedVector(vectorId, name, fileName, attributes, computationID, inputs.get(0), operation);
//        }
//        return id;
//    }
//
//    private static String calculateComputedVectorName(ProcessingOp operation, IDList input, ResultFileManager manager) {
//        StringBuilder sb = new StringBuilder();
//        sb.append(operation.getOperation()).append("(");
//        for (int i = 0; i < input.size(); ++i) {
//            long id = input.get(i);
//            VectorResult vector = manager.getVector(id);
//            if (i > 0) {
//                sb.append(";");
//                if (i > 10) {
//                    sb.append("...");
//                    break;
//                }
//            }
//            sb.append(vector.getName());
//        }
//        sb.append(")");
//        return sb.toString();
//    }
//
//    private static long getFilterNodeID(ProcessingOp operation) {
//        return operation.hashCode();
//    }
//
//    static StringMap mapVectorAttributes(VectorResult input, ProcessingOp processingOp, /*out*/List<IStatus> warnings) {
//        String operation = processingOp.getOperation();
//        StringMap inputAttrs = input.getAttributes();
//        StringMap outAttrs = new StringMap();
//        if (operation == null)
//            return outAttrs;
//        for (String key : inputAttrs.keys().toArray()) // TODO use clone()
//            outAttrs.set(key, inputAttrs.get(key));
//
//        NodeType type = NodeTypeRegistry.getInstance().getNodeType(operation);
//        StringVector warningList = new StringVector();
//        type.mapVectorAttributes(outAttrs, warningList);
//        for (int i = 0; i < warningList.size(); ++i)
//            warnings.add(ScavePlugin.getWarningStatus(warningList.get(i)));
//        return outAttrs;
//    }
//
//    public static long getComputationHash(ProcessingOp processingOp, ResultFileManager manager) {
//        checkReadLock(manager);
//        long hash = 1;
//        IDList inputIDs = getComputationInput(processingOp, manager);
//        if (inputIDs != null)
//            for (int i = 0; i < inputIDs.size(); ++i)
//                hash = 31 * hash + inputIDs.get(i);
//
//        String operation = processingOp.getOperation();
//        hash = 31 * hash + (operation == null ? 0 : operation.hashCode());
//
//        List<String> groupByFields = processingOp.getGroupBy();
//        hash = 31 * hash + (groupByFields == null ? 0 : groupByFields.hashCode());
//
//        List<Param> params = processingOp.getParams();
//        if (params != null)
//            for (Param param : params) {
//                String name = param.getName();
//                String value = param.getValue();
//                hash = 31 * hash + (name == null ? 0 : name.hashCode());
//                hash = 31 * hash + (value == null ? 0 : value.hashCode());
//            }
//        return hash;
//    }
//
//    public static IDList getComputationInput(ProcessingOp processingOp, ResultFileManager manager) {
//        checkReadLock(manager);
//        Chart chart = ScaveModelUtil.findEnclosingDataset(processingOp);
//        if (chart == null)
//            return null;
//        return getIDListFromDataset(manager, chart, processingOp, true, ResultType.VECTOR_LITERAL);
//    }
//
//    public static List<IDList> groupIDs(ProcessingOp operation, IDList idlist, ResultFileManager manager) {
//        List<IDList> result = new ArrayList<IDList>();
//        if (ScaveModelUtil.isFilterOperation(operation)) {
//            for (int i = 0; i < idlist.size(); ++i) {
//                IDList ids = new IDList();
//                ids.add(idlist.get(i));
//                result.add(ids);
//            }
//        }
//        else if (ScaveModelUtil.isMergerOperation(operation)) {
//            DataSorter sorter = new DataSorter(manager);
//            ResultItemFields fields = ScaveModelUtil.getGroupByFields(operation);
//            IDVectorVector groups = sorter.groupByFields(idlist, fields);
//            for (int i = 0; i < groups.size(); ++i) {
//                IDVector group = groups.get(i);
//                IDList groupAsIDList = new IDList();
//                for (int j = 0; j < group.size(); ++j) {
//                    groupAsIDList.add(group.get(j));
//                }
//                result.add(groupAsIDList);
//            }
//        }
//        return result;
//    }
//
//    public static ResultItemField[] getSelectableGroupByFields(ProcessingOp operation, ResultFileManager manager) {
//        List<ResultItemField> fields = new ArrayList<ResultItemField>();
//        Chart chart = ScaveModelUtil.findEnclosingDataset(operation);
//        if (chart != null) {
//            IDList idlist = getIDListFromDataset(manager, chart, operation, true, ResultType.VECTOR_LITERAL);
//            List<String> fieldNames = ScaveModelUtil.getResultItemFields(idlist, manager);
//            for (String name : fieldNames)
//                fields.add(new ResultItemField(name));
//        }
//        return fields.toArray(new ResultItemField[fields.size()]);
//    }

    private static void checkReadLock(ResultFileManager manager) {
        if (manager != null)
            manager.checkReadLock();
    }
}

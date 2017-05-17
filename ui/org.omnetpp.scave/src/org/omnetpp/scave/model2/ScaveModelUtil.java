/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model2;

import static org.omnetpp.scave.engine.ResultItemField.MODULE;
import static org.omnetpp.scave.engine.ResultItemField.NAME;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.Callable;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.Enumerator;
import org.eclipse.emf.common.util.TreeIterator;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.EStructuralFeature.Setting;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.omnetpp.common.Debug;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.properties.ChartProperties;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StatisticsResult;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.HistogramChart;
import org.omnetpp.scave.model.InputFile;
import org.omnetpp.scave.model.Inputs;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.Property;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.ScaveModelPackage;

/**
 * A collection of static methods to manipulate model objects
 * @author andras
 */
public class ScaveModelUtil {
    private static final ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
    private static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;

    public static InputFile createInput(String name) {
        InputFile input = factory.createInputFile();
        input.setName(name);
        return input;
    }

    public static Chart createChart(String name, String title, ResultType type) {
        Chart chart;
        if (type==ResultType.SCALAR_LITERAL)
            chart = createBarChart(name);
        else if (type==ResultType.VECTOR_LITERAL)
            chart = createLineChart(name);
        else if (type==ResultType.HISTOGRAM_LITERAL)
            chart = createHistogramChart(name);
        else
            throw new IllegalArgumentException();

        Property property = factory.createProperty();
        property.setName(ChartProperties.PROP_GRAPH_TITLE);
        property.setValue(title);
        chart.getProperties().add(property);

        return chart;
    }

    public static BarChart createBarChart(String name) {
        BarChart chart = factory.createBarChart();
        chart.setName(name);
        return chart;
    }

    public static LineChart createLineChart(String name) {
        LineChart chart = factory.createLineChart();
        chart.setName(name);
        return chart;
    }

    public static HistogramChart createHistogramChart(String name) {
        HistogramChart chart = factory.createHistogramChart();
        chart.setName(name);
        return chart;
    }

    public static ChartSheet createChartSheet(String name) {
        ChartSheet chartsheet = factory.createChartSheet();
        chartsheet.setName(name);
        return chartsheet;
    }

    public static List<String> getIDListAsFilters(IDList ids, String[] runidFields, ResultFileManager manager) {
        Assert.isNotNull(runidFields);
        String[] filterFields = getFilterFieldsFor(runidFields);
        List<String> filters = new ArrayList<String>(ids.size());
        for (int i = 0; i < ids.size(); ++i) {
            long id = ids.get(i);
            String filter = new FilterUtil(manager.getItem(id), filterFields).getFilterPattern(); //TODO include: getTypeOf(item)
            filters.add(filter);
        }
        return filters;
    }

    public static void addInputFiles(EditingDomain domain, Analysis analysis, List<String> list) {
        ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;
        ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
        Inputs inputs = analysis.getInputs();
        List<InputFile> inputFiles = new ArrayList<>();
        for (String item : list) {
            InputFile inputFile = factory.createInputFile();
            inputFile.setName(item);
            inputFiles.add(inputFile);
        }
        Command command = AddCommand.create(domain, inputs, pkg.getInputs_Inputs(), inputFiles);
        domain.getCommandStack().execute(command);
    }

    public static void setInputFile(EditingDomain domain, InputFile inputFile, String value) {
        Command command = SetCommand.create(domain, inputFile, pkg.getInputFile_Name(), value);
        domain.getCommandStack().execute(command);
    }

    private static String[] getFilterFieldsFor(String[] runidFields) {
        Assert.isNotNull(runidFields);
        int runidFieldCount = runidFields.length;
        String[] filterFields = new String[runidFieldCount+2];
        System.arraycopy(runidFields, 0, filterFields, 0, runidFieldCount);
        filterFields[runidFieldCount] = MODULE;
        filterFields[runidFieldCount + 1] = NAME;
        return filterFields;
    }

    /**
     * Returns the data types displayed on the chart.
     */
    public static ResultType[] getDataTypesOfChart(Chart chart) {
        if (chart instanceof BarChart)
            return new ResultType[] {ResultType.SCALAR_LITERAL};
        else if (chart instanceof LineChart)
            return new ResultType[] {ResultType.VECTOR_LITERAL};
        else if (chart instanceof HistogramChart)
            return new ResultType[] {ResultType.HISTOGRAM_LITERAL};
        else if (chart instanceof ScatterChart)
            return new ResultType[] {ResultType.SCALAR_LITERAL,ResultType.VECTOR_LITERAL,ResultType.HISTOGRAM_LITERAL};
        else
            throw new IllegalArgumentException("Unknown chart type: " + chart.getClass().getName());
    }

    /**
     * Returns the names of result types.
     */
    public static String[] getResultTypeNames() {
        return getEnumNames(ResultType.VALUES);
    }

    /**
     * Returns the names of an EMF enum members.
     */
    public static String[] getEnumNames(List<? extends Enumerator> enumValues) {
        String[] names = new String[enumValues.size()];
        int i = 0;
        for (Enumerator value : enumValues) {
            names[i++] = value.getName();
        }
        return names;
    }

    /**
     * Returns the analysis node of the specified resource.
     * It is assumed that the resource contains exactly one analysis node as
     * content.
     */
    public static Analysis getAnalysis(Resource resource) {
        Assert.isTrue(resource.getContents().size() == 1 && resource.getContents().get(0) instanceof Analysis,
                "Analysis node not found in: " + resource.getURI().toString());
        return (Analysis)resource.getContents().get(0);
    }

    /**
     * Returns the analysis node containing <code>eobject</code>.
     */
    public static Analysis getAnalysis(EObject eobject) {
        Assert.isTrue(eobject.eClass().getEPackage() == pkg,
                "Scave model object expected, received: " + eobject.toString());
        return findEnclosingOrSelf(eobject, Analysis.class);
    }

    public static EObject getPreviousSibling(EObject eobject) {
        EObject parent = eobject.eContainer();
        EStructuralFeature feature = eobject.eContainingFeature();
        if (parent != null && feature != null) {
            @SuppressWarnings("unchecked")
            EList<EObject> siblings = ((EList<EObject>)parent.eGet(feature));
            int index = siblings.indexOf(eobject);
            if (index > 0)
                return siblings.get(index - 1);
        }
        return null;
    }


    /**
     * Finds an enclosing object having type {@code type}.
     * If the {@code object} itself has the type, it is returned.
     */
    @SuppressWarnings("unchecked")
    public static <T extends EObject> T findEnclosingOrSelf(EObject object, Class<T> type) {
        while (object != null && !type.isInstance(object))
            object = object.eContainer();
        return (T)object;
    }

    /**
     * Returns all object in the container having the specified type.
     */
    @SuppressWarnings("unchecked")
    public static <T extends EObject> List<T> findObjects(EObject container, Class<T> type) {
        ArrayList<T> objects = new ArrayList<>();
        for (TreeIterator<EObject> iterator = container.eAllContents(); iterator.hasNext(); ) {
            EObject object = iterator.next();
            if (type.isInstance(object))
                objects.add((T)object);
        }
        return objects;
    }

    /**
     * Returns all objects in the resource having the specified type.
     */
    @SuppressWarnings("unchecked")
    public static <T extends EObject> List<T> findObjects(Resource resource, Class<T> type) {
        ArrayList<T> objects = new ArrayList<>();
        for (TreeIterator<EObject> iterator = resource.getAllContents(); iterator.hasNext(); ) {
            EObject object = iterator.next();
            if (type.isInstance(object))
                objects.add((T)object);
        }
        return objects;
    }

    /**
     * Collect charts from the given collection.
     */
    public static List<Chart> collectCharts(Collection<?> items) {
        List<Chart> charts = new ArrayList<>();
        for (Object item : items)
            if (item instanceof Chart)
                charts.add((Chart)item);
        return charts;
    }

    /**
     * Collect unreferenced charts from the given collection.
     */
    public static Collection<Chart> collectUnreferencedCharts(Collection<?> items) {
        List<Chart> charts = collectCharts(items);
        if (charts.size() > 0) {
            Map<EObject,Collection<Setting>> references = ScaveCrossReferencer.find(charts.get(0).eResource());
            charts.removeAll(references.keySet());
        }
        return charts;
    }

    /**
     * Collect references to scave objects.
     * Currently the only references are from chart sheets to charts,
     * so the scope of the search is limited to chart sheets.
     */
    static class ScaveCrossReferencer extends EcoreUtil.CrossReferencer {

        private static final long serialVersionUID = 2380168634189516829L;

        protected ScaveCrossReferencer(Collection<?> eobjects) {
            super(eobjects);
        }

        public static Map<EObject, Collection<EStructuralFeature.Setting>> find(Resource resource) {
            return EcoreUtil.CrossReferencer.find(Collections.singleton(resource));
        }

        @Override
        protected boolean containment(EObject eObject) {
            return eObject instanceof Resource ||
                   eObject instanceof Analysis;
        }
    }

    public static Property getChartProperty(Chart chart, String propertyName) {
        for (Object object : chart.getProperties()) {
            Property property = (Property)object;
            if (property.getName().equals(propertyName))
                return property;
        }
        return null;
    }

    public static void setChartProperty(EditingDomain ed, Chart chart, String propertyName, String propertyValue) {
        Property property = getChartProperty(chart, propertyName);
        Command command;
        if (property == null) {
            property = factory.createProperty();
            property.setName(propertyName);
            property.setValue(propertyValue);
            command = AddCommand.create(
                        ed,
                        chart,
                        pkg.getChart_Properties(),
                        property);
        }
        else {
            command = SetCommand.create(
                        ed,
                        property,
                        pkg.getProperty_Value(),
                        propertyValue);
        }
        ed.getCommandStack().execute(command);
    }

    public static IDList getAllIDs(ResultFileManager manager, ResultType type) {
        if (type == null)
            return manager.getAllItems(false, true, true);

        switch (type.getValue()) {
        case ResultType.SCALAR: return manager.getAllScalars(false, true, true);
        case ResultType.VECTOR: return manager.getAllVectors();
        case ResultType.STATISTICS: return manager.getAllStatistics();
        case ResultType.HISTOGRAM: return manager.getAllHistograms();
        }
        Assert.isTrue(false, "Unknown dataset type: " + type);
        return null;
    }

    public static ResultItem[] getResultItems(IDList idlist, ResultFileManager manager) {
        int size = idlist.size();
        ResultItem[] items = new ResultItem[size];
        for (int i = 0; i < size; ++i)
            items[i] = manager.getItem(idlist.get(i));
        return items;
    }

    public static IDList filterIDList(IDList idlist, Filter filter, ResultFileManager manager) {
        Assert.isTrue(filter.getFilterPattern()!=null);
        return manager.filterIDList(idlist, filter.getFilterPattern());
    }

    /**
     * Returns an ordered array of distinct values of the {@code field} attribute
     * of the result items found in {@code idlist}.
     */
    public static String[] getFieldValues(IDList idlist, ResultItemField field, ResultFileManager manager) {
        Set<String> values = new HashSet<>();
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            ResultItem item = manager.getItem(id);
            String value = field.getFieldValue(item);
            if (!StringUtils.isEmpty(value))
                values.add(value);
        }
        String[] result = values.toArray(new String[values.size()]);
        Arrays.sort(result);
        return result;
    }


    public static IsoLineData[] getModuleAndDataPairs(IDList idlist, ResultFileManager manager, boolean addRunAttributes) {
        manager.checkReadLock();
        Set<IsoLineData> values = new HashSet<>();
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            ResultItem item = manager.getItem(id);
            IsoLineData pair = new IsoLineData(item.getModuleName(), item.getName());
            if (pair.isValid())
                values.add(pair);
        }

        if (addRunAttributes)
        {
            //StringVector runAttributes = RunAttribute.getAttributeNames();
            StringVector runAttributes = manager.getUniqueRunAttributeNames(manager.getUniqueRuns(idlist)).keys();
            for (int i = 0; i < runAttributes.size(); ++i)
                values.add(new IsoLineData(runAttributes.get(i)));
        }

        IsoLineData[] result = values.toArray(new IsoLineData[values.size()]);
        Arrays.sort(result);
        return result;
    }

    public static List<String> getResultItemFields(IDList idlist, ResultFileManager manager) {
        List<String> fields = new ArrayList<>();
        fields.add(ResultItemField.FILE);
        fields.add(ResultItemField.RUN);
        fields.add(ResultItemField.MODULE);
        fields.add(ResultItemField.NAME);
        fields.addAll(getRunAttributeNames(idlist, manager));
        return fields;
    }

    public static List<String> getRunAttributeNames(IDList idlist, ResultFileManager manager) {
        StringVector runAttributes = manager.getUniqueRunAttributeNames(manager.getUniqueRuns(idlist)).keys();
        return Arrays.asList(runAttributes.toArray());
    }


    public static void dumpIDList(String header, final IDList idlist, final ResultFileManager manager) {
        Debug.print(header);
        if (idlist.size() == 0)
            Debug.println("Empty");
        else {
            Debug.println();
            ResultFileManager.callWithReadLock(manager, new Callable<Object>() {
                @Override
                public Object call() {
                    for (int i = 0; i < idlist.size(); ++i) {
                        ResultItem r = manager.getItem(idlist.get(i));
                        Debug.println(
                            String.format("File: %s Run: %s Module: %s Name: %s",
                                r.getFileRun().getFile().getFilePath(),
                                r.getFileRun().getRun().getRunName(),
                                r.getModuleName(),
                                r.getName()));
                    }
                    return null;
                }
            });
        }
    }

    /**
     *
     */
    public static int asInternalResultType(ResultType type) {
        switch (type.getValue()) {
        case ResultType.SCALAR: return ResultFileManager.SCALAR;
        case ResultType.VECTOR: return ResultFileManager.VECTOR;
        case ResultType.STATISTICS: return ResultFileManager.STATISTICS;
        case ResultType.HISTOGRAM: return ResultFileManager.HISTOGRAM;
        default: Assert.isTrue(false, "Unknown ResultType:"+type); return 0;
        }
    }

    public static ResultType asResultType(int internalResultType) {
        if (internalResultType == ResultFileManager.SCALAR)
            return ResultType.SCALAR_LITERAL;
        else if (internalResultType == ResultFileManager.VECTOR)
            return ResultType.VECTOR_LITERAL;
        else if (internalResultType == ResultFileManager.STATISTICS)
            return ResultType.STATISTICS_LITERAL;
        else if (internalResultType == ResultFileManager.HISTOGRAM)
            return ResultType.HISTOGRAM_LITERAL;
        else {
            Assert.isTrue(false, "Unknown internal ResultType:"+internalResultType);
            return null;
        }
    }

    public static ResultType getTypeOf(ResultItem item) {
        if (item instanceof ScalarResult)
            return ResultType.SCALAR_LITERAL;
        else if (item instanceof VectorResult)
            return ResultType.VECTOR_LITERAL;
        else if (item instanceof HistogramResult) // must precede StatisticsResult test
            return ResultType.HISTOGRAM_LITERAL;
        else if (item instanceof StatisticsResult)
            return ResultType.STATISTICS_LITERAL;
        else {
            Assert.isTrue(false, "Unknown result item: "+item);
            return null;
        }
    }

    public static boolean isFilterOperation(String operation) {
        NodeType nodeType = getNodeType(operation);
        return nodeType != null && isFilterOperation(nodeType);
    }

    public static boolean isFilterOperation(NodeType nodeType) {
        return hasCategory(nodeType, "filter");
    }

    public static boolean isMergerOperation(String operation) {
        NodeType nodeType = getNodeType(operation);
        return nodeType != null && isMergerOperation(nodeType);
    }

    public static boolean isMergerOperation(NodeType nodeType) {
        return hasCategory(nodeType, "merger");
    }

    public static NodeType getNodeType(String name) {
        Assert.isNotNull(name);
        NodeTypeRegistry registry = NodeTypeRegistry.getInstance();
        return registry.exists(name) ? registry.getNodeType(name) : null;
    }

    public static boolean hasCategory(String nodeTypeName, String category) {
        Assert.isNotNull(nodeTypeName);
        Assert.isNotNull(category);
        NodeType nodeType = getNodeType(nodeTypeName);
        return nodeType != null ? hasCategory(nodeType, category) : false;
    }

    public static boolean hasCategory(NodeType nodeType, String category) {
        Assert.isNotNull(nodeType);
        Assert.isNotNull(category);
        return nodeType.getCategory().equals(category);
    }

    public static IFile getFileOfEObject(EObject object) {
        URI uri = object.eResource().getURI();
        if (uri.isPlatformResource())
            return (IFile) ResourcesPlugin.getWorkspace().getRoot().findMember(uri.toPlatformString(true));
        return null;
    }
}

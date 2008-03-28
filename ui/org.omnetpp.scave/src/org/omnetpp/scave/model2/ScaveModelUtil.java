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

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.common.command.Command;
import org.eclipse.emf.common.command.CompoundCommand;
import org.eclipse.emf.common.util.Enumerator;
import org.eclipse.emf.common.util.TreeIterator;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.EStructuralFeature.Setting;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.emf.edit.command.AddCommand;
import org.eclipse.emf.edit.command.SetCommand;
import org.eclipse.emf.edit.domain.EditingDomain;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.editors.ScaveEditor;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.Analysis;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.ChartSheet;
import org.omnetpp.scave.model.ChartSheets;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.HistogramChart;
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

	private static final String DEFAULT_CHARTSHEET_NAME = "default";

	private static final ScaveModelFactory factory = ScaveModelFactory.eINSTANCE;
	private static final ScaveModelPackage pkg = ScaveModelPackage.eINSTANCE;

	public static ChartSheet createDefaultChartSheet() {
		ChartSheet chartsheet = factory.createChartSheet();
		chartsheet.setName(DEFAULT_CHARTSHEET_NAME);
		return chartsheet;
	}

	public static Dataset createDataset(String name) {
		Dataset dataset = factory.createDataset();
		dataset.setName(name);
		return dataset;
	}

	public static Dataset createDataset(String name, Filter filter, ResultType type) {
		Dataset dataset = createDataset(name);
		dataset.getItems().add(createAdd(filter, type));
		return dataset;
	}

	public static Dataset createTemporaryDataset(String name, IDList ids, String[] runidFields, ResultFileManager manager) {
		Dataset dataset = createDataset(name);
		dataset.getItems().addAll(createAdds(ids, runidFields, manager, true));
		return dataset;
	}

	public static Chart createChart(String name, ResultType type) {
		if (type==ResultType.SCALAR_LITERAL)
			return createBarChart(name);
		else if (type==ResultType.VECTOR_LITERAL)
			return createLineChart(name);
		else if (type==ResultType.HISTOGRAM_LITERAL)
			return createHistogramChart(name);
		else
			throw new IllegalArgumentException();
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
	
	public static Add createAdd(String filterString, ResultType type) {
		Add add = factory.createAdd();
		add.setFilterPattern(filterString);
		add.setType(type);
		return add;
	}

	public static Add createAdd(Filter filter, ResultType type) {
		return createAdd(filter.getFilterPattern(), type);
	}
	
	/**
	 * Generates an Add command with filter pattern to identify item.
	 * @param filterFields may be null (meaning run/module/name)
	 */
	public static Add createAdd(ResultItem item, String[] filterFields) {
		return createAdd(new FilterUtil(item,filterFields).getFilterPattern(), getTypeOf(item));
	}

	/**
	 * Generates Add commands with filter patterns that identify elements in items[].
	 * @param runidFields  may be null (meaning autoselect)
	 */
	public static Collection<Add> createAdds(ResultItem[] items, String[] runidFields) {
		return createAddsWithFields(items, getFilterFieldsFor(runidFields));
	}
	
	public static Collection<Add> createAdds(IDList ids, String[] runidFields, ResultFileManager manager, boolean cacheIDs) {
		String[] filterFields = getFilterFieldsFor(runidFields);
		List<Add> adds = new ArrayList<Add>(ids.size());
		for (int i = 0; i < ids.size(); ++i) {
			long id = ids.get(i);
			Add add = createAdd(manager.getItem(id), filterFields);
			if (cacheIDs) {
				IDList cachedIDs = new IDList();
				cachedIDs.add(id);
				add.setCachedIDs(cachedIDs);
			}
			adds.add(add);
		}
		return adds;
	}
	
	public static Collection<Add> createAddsWithFields(ResultItem[] items, String[] filterFields) {
		List<Add> adds = new ArrayList<Add>(items.length);
		for (ResultItem item : items)
			adds.add(createAdd(item, filterFields));
		return adds;
	}
	
	private static String[] getFilterFieldsFor(String[] runidFields) {
		String[] filterFields = null;
		if (runidFields != null) {
			int runidFieldCount = runidFields.length;
			filterFields = new String[runidFieldCount];
			System.arraycopy(runidFields, 0, filterFields, 0, runidFieldCount);
			filterFields[runidFieldCount] = MODULE;
			filterFields[runidFieldCount + 1] = NAME;
		}
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
	@SuppressWarnings("unchecked")
	public static String[] getResultTypeNames() {
		return getEnumNames((List<ResultType>)ResultType.VALUES);
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

	public static ChartSheet getDefaultChartSheet(Resource resource) {
		Analysis analysis = getAnalysis(resource);
		for (ChartSheet chartsheet : (List<ChartSheet>)analysis.getChartSheets().getChartSheets())
			if (DEFAULT_CHARTSHEET_NAME.equals(chartsheet.getName()))
				return chartsheet;
		return null;
	}

	public static Dataset findEnclosingDataset(EObject eobject) {
		EObject parent = eobject.eContainer();
		while (parent != null && !(parent instanceof Dataset))
			parent = parent.eContainer();
		return (Dataset)parent;
	}

	/**
	 * Returns the datasets in the resource.
	 */
	public static List<Dataset> findDatasets(EObject eobject) {
		List<Dataset> result = new ArrayList<Dataset>();
		Analysis analysis = getAnalysis(eobject);
		if (analysis != null && analysis.getDatasets() != null) {
			for (Object object : analysis.getDatasets().getDatasets()) {
				Dataset dataset = (Dataset)object;
				result.add(dataset);
			}
		}
		return result;
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
		ArrayList<T> objects = new ArrayList<T>();
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
		ArrayList<T> objects = new ArrayList<T>();
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
		List<Chart> charts = new ArrayList<Chart>();
		for (Object item : items)
			if (item instanceof Chart) {
				charts.add((Chart)item);
			}
			else if (item instanceof Dataset || item instanceof Group) {
				for (TreeIterator<EObject> iter = ((EObject)item).eAllContents(); iter.hasNext(); ) {
					Object object = iter.next();
					if (object instanceof Chart)
						charts.add((Chart)object);
					else if (!(object instanceof Dataset || object instanceof Group))
						iter.prune();
				}
			}
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
				   eObject instanceof Analysis ||
				   eObject instanceof ChartSheets ||
				   eObject instanceof ChartSheet;
		}
	}

	public static boolean isTemporaryChart(Chart chart, ScaveEditor editor) {
		return ScaveModelUtil.findEnclosingOrSelf(chart, Analysis.class) == editor.getTempAnalysis();
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
		if (type == null) {
			IDList idlist = new IDList();
			idlist.merge(manager.getAllScalars());
			idlist.merge(manager.getAllVectors());
			idlist.merge(manager.getAllHistograms());
			return idlist;
		}
		
		switch (type.getValue()) {
		case ResultType.SCALAR: return manager.getAllScalars();
		case ResultType.VECTOR:	return manager.getAllVectors();
		case ResultType.HISTOGRAM: return manager.getAllHistograms();
		}
		Assert.isTrue(false, "Unknown dataset type: " + type);
		return null;
	}

	public static ResultItem[] getResultItems(IDList idlist, ResultFileManager manager) {
		int size = (int)idlist.size();
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
		Set<String> values = new HashSet<String>();
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
	

	public static ModuleAndData[] getModuleAndDataPairs(IDList idlist, ResultFileManager manager) {
		Set<ModuleAndData> values = new HashSet<ModuleAndData>();
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			ResultItem item = manager.getItem(id);
			ModuleAndData pair = new ModuleAndData(item.getModuleName(), item.getName());
			if (pair.isValid())
				values.add(pair);
		}
		ModuleAndData[] result = values.toArray(new ModuleAndData[values.size()]);
		Arrays.sort(result);
		return result;
	}
	

	/**
	 * Returns the default chart sheet.
	 * When the resource did not contain default chart sheet a new one is created,
	 * and a AddCommand is appended to the <code>command</code>, that adds
	 * the new chart sheet to the resource.
	 */
	public static ChartSheet getOrCreateDefaultChartSheet(EditingDomain ed, CompoundCommand command, Resource resource) {
		ChartSheet chartsheet = getDefaultChartSheet(resource);
		if (chartsheet == null) {
			chartsheet = createDefaultChartSheet();
			command.append(
				AddCommand.create(
					ed,
					getAnalysis(resource).getChartSheets(),
					pkg.getChartSheets_ChartSheets(),
					chartsheet,
					0));
		}
		return chartsheet;
	}

	public static void dumpIDList(String header, IDList idlist, ResultFileManager manager) {
		System.out.print(header);
		if (idlist.size() == 0)
			System.out.println("Empty");
		else {
			System.out.println();
			for (int i = 0; i < idlist.size(); ++i) {
				ResultItem r = manager.getItem(idlist.get(i));
				System.out.println(
					String.format("File: %s Run: %s Module: %s Name: %s",
						r.getFileRun().getFile().getFilePath(),
						r.getFileRun().getRun().getRunName(),
						r.getModuleName(),
						r.getName()));
			}
		}
	}
	
	/**
	 * 
	 */
	public static int asInternalResultType(ResultType type) {
		switch (type.getValue()) {
		case ResultType.SCALAR: return ResultFileManager.SCALAR;
		case ResultType.VECTOR: return ResultFileManager.VECTOR;
		case ResultType.HISTOGRAM: return ResultFileManager.HISTOGRAM;
		default: Assert.isTrue(false, "Unknown ResultType:"+type); return 0;
		}
	}
	
	public static ResultType asResultType(int internalResultType) {
		if (internalResultType == ResultFileManager.SCALAR)
			return ResultType.SCALAR_LITERAL;
		else if (internalResultType == ResultFileManager.VECTOR)
			return ResultType.VECTOR_LITERAL;
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
		else if (item instanceof HistogramResult)
			return ResultType.HISTOGRAM_LITERAL;
		else {
			Assert.isTrue(false, "Unknown result item: "+item);
			return null;
		}
	}
}

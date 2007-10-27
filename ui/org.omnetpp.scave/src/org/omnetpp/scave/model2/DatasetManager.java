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
import java.util.List;

import org.apache.commons.lang.ObjectUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.dataset.CompoundXYDataset;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.dataset.ScalarScatterPlotDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.charting.dataset.VectorScatterPlotDataset;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ResultItemField;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYDataset;
import org.omnetpp.scave.engine.XYDatasetVector;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.BarChart;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.LineChart;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.SetOperation;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

/**
 * This class calculates the content of a dataset
 * applying the operations described by the dataset.
 * 
 * @author tomi
 */
public class DatasetManager {

	public static IDList getIDListFromDataset(ResultFileManager manager, Dataset dataset, DatasetItem lastItemToProcess, ResultType type) {
		ProcessDatasetSwitch processor = new ProcessDatasetSwitch(manager, lastItemToProcess, type);
		processor.doSwitch(dataset);
		return processor.getIDList();
	}

	/**
	 * Visitor-like thingy, where doSwitch() traverses the dataset (or whatever model item was 
	 * passed into it), and calculates an IDList of the given type. It stops at the "target" item. 
	 */
	static class ProcessDatasetSwitch extends ScaveModelSwitch<Object> {

		private ResultFileManager manager;
		private ResultType type;
		private IDList idlist;
		private EObject target;
		private boolean finished;

		public ProcessDatasetSwitch(ResultFileManager manager, EObject target, ResultType type) {
			this.manager = manager;
			this.type = type;
			this.target = target;
			this.idlist = new IDList();
		}

		public IDList getIDList() {
			return idlist != null ? idlist : new IDList();
		}

		@Override
		protected Object doSwitch(int classifierID, EObject object) {
			Object result = this;
			if (!finished) {
				result = super.doSwitch(classifierID, object);
				if (object == target)
					finished = true;
			}
			return result;
		}

		public Object caseDataset(Dataset dataset) {
			if (dataset.getBasedOn() != null)
				idlist = getIDListFromDataset(manager, dataset.getBasedOn(), null, type);

			for (Object item : dataset.getItems())
				doSwitch((EObject)item);

			return this;
		}

		public Object caseAdd(Add add) {
			if (type == null || add.getType() == type)
				idlist.merge(select(null, add));
			return this;
		}

		public Object caseDiscard(Discard discard) {
			if (type == null || discard.getType() == type)
				idlist.substract(select(idlist, discard));
			return this;
		}

		public Object caseGroup(Group group) {
			if (EcoreUtil.isAncestor(group, target))
				for (Object item : group.getItems())
					doSwitch((EObject)item);
			return this;
		}

		public Object caseApply(Apply apply) {
			if (apply.getOperation() != null) {
				for (int i = 0; i < idlist.size(); ++i) {
					long id = idlist.get(i);
					idlist.set(i, ensureComputedResultItem(apply, id, manager));
				}
			}
			return this;
		}

		public Object caseCompute(Compute compute) {
			if (compute.getOperation() != null) {
				int size = (int)idlist.size();
				for (int i = 0; i < size; ++i) {
					long id = idlist.get(i);
					idlist.add(ensureComputedResultItem(compute, id, manager));
				}
			}
			return this;
		}

		public Object caseChart(Chart chart) {
			if (chart == target)
				idlist = select(idlist, chart.getFilters());
			return this;
		}

		private IDList select(IDList source, List<SelectDeselectOp> filters) {
			// if no select, then interpret it as "select all" -- so we add an 
			// artificial "Select All" node into filters[] before proceeding
			//XXX there is an almost identical function in DataflowNetworkBuilder
			if (filters.size() == 0 || filters.get(0) instanceof Deselect) {
				Select selectAll = ScaveModelFactory.eINSTANCE.createSelect();
				selectAll.setType(type);
				filters = new ArrayList<SelectDeselectOp>(filters);
				filters.add(0, selectAll);
			}

			return DatasetManager.select(source, filters, manager, type);
		}

		private IDList select(IDList source, AddDiscardOp op) {
			return DatasetManager.select(source, op, manager);
		}
	}

	public static XYArray[] getDataFromDataset(ResultFileManager manager, Dataset dataset, DatasetItem lastItemToProcess) {
		DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
		DataflowManager dataflowManager = builder.build(dataset, lastItemToProcess, true);
		List<Node> arrayBuilders = builder.getArrayBuilders();
		dataflowManager.dump();
		return executeDataflowNetwork(dataflowManager, arrayBuilders);
	}
	
	public static XYArray getDataOfVector(ResultFileManager manager, long id) {
		IDList idlist = new IDList();
		idlist.add(id);
		XYArray[] data = getDataOfVectors(manager, idlist);
		Assert.isTrue(data != null && data.length == 1);
		return data[0];
	}
	
	public static XYArray[] getDataOfVectors(ResultFileManager manager, IDList idlist) {
		DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
		DataflowManager dataflowManager = builder.build(idlist);
		List<Node> arrayBuilders = builder.getArrayBuilders();
		return executeDataflowNetwork(dataflowManager, arrayBuilders);
	}
	
	private static XYArray[] executeDataflowNetwork(DataflowManager manager, List<Node> arrayBuilders) {
		long startTime = System.currentTimeMillis();
		if (arrayBuilders.size() > 0) // XXX DataflowManager crashes when there are no sinks
			manager.execute();
		System.out.println("dataflow network: "+(System.currentTimeMillis()-startTime)+" ms");

		XYArray[] result = new XYArray[arrayBuilders.size()];
		for (int i = 0; i < result.length; ++i)
			result[i] = arrayBuilders.get(i).getArray();
		return result;
	}

	@SuppressWarnings("unchecked")
	public static ScalarDataset createScalarDataset(BarChart chart, ResultFileManager manager, IProgressMonitor progressMonitor) {
		//TODO update progressMonitor
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
		List<String> rowFieldNames = (List<String>)chart.getGroupBy();
		List<String> columnFieldNames = (List<String>)chart.getBarFields();
		ResultItemFields rowFields = rowFieldNames == null || rowFieldNames.isEmpty() ? null :
									new ResultItemFields(StringVector.fromArray(
											rowFieldNames.toArray(new String[rowFieldNames.size()])));
		ResultItemFields columnFields = columnFieldNames == null || columnFieldNames.isEmpty() ? null :
										new ResultItemFields(StringVector.fromArray(
											columnFieldNames.toArray(new String[columnFieldNames.size()])));
 		return new ScalarDataset(idlist, rowFields, columnFields, manager);
	}
	
	public static VectorDataset createVectorDataset(LineChart chart, ResultFileManager manager, IProgressMonitor progressMonitor) {
		return createVectorDataset(chart, null, chart.getLineNameFormat(), true, manager, progressMonitor);
	}
	
	public static VectorDataset createVectorDataset(LineChart chart, Dataset dataset,
			String lineNameFormat, boolean computeData, ResultFileManager manager, IProgressMonitor progressMonitor) {
		//TODO update progressMonitor
		if (dataset == null)
			dataset = ScaveModelUtil.findEnclosingDataset(chart);
		
		DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
		DataflowManager dataflowManager = builder.build(dataset, chart, computeData);
		IDList idlist = builder.getDisplayedIDs();

		XYArray[] dataValues = null;
		if (dataflowManager != null) {
			List<Node> arrayBuilders = builder.getArrayBuilders();
			dataflowManager.dump();
			dataValues = executeDataflowNetwork(dataflowManager, arrayBuilders);
		}
		
		return dataValues != null ?
				new VectorDataset(idlist, dataValues, lineNameFormat, manager) :
				new VectorDataset(idlist, lineNameFormat, manager);
	}
	
	public static IXYDataset createScatterPlotDataset(ScatterChart chart, ResultFileManager manager, IProgressMonitor monitor) {
		return createScatterPlotDataset(chart, null, manager, monitor);
	}
	
	public static IXYDataset createScatterPlotDataset(ScatterChart chart, Dataset dataset, ResultFileManager manager, IProgressMonitor monitor) {
		
		//TODO update progressMonitor
		if (dataset == null)
			dataset = ScaveModelUtil.findEnclosingDataset(chart);
		if (dataset != null) {
			IDList scalars = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			IDList vectors = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL);
			
			XYDatasetVector xyScalars = null;
			if (!scalars.isEmpty()) {
				ModuleAndData xData = ModuleAndData.fromFilterPattern(chart.getXDataPattern());
				Assert.isLegal(xData.isValid(), "X data is not selected.");
				
				String xModuleName = xData.getModuleName();
				String xScalarName = xData.getDataName();
				List<String> isoPatterns = chart.getIsoDataPattern();
				boolean averageReplications = chart.isAverageReplications();
				
				StringVector isoModuleNames = new StringVector();
				StringVector isoScalarNames = new StringVector();
				for (String pattern : isoPatterns) {
					ModuleAndData moduleAndData = ModuleAndData.fromFilterPattern(pattern);
					if (moduleAndData.isValid()) {
						isoModuleNames.add(moduleAndData.getModuleName());
						isoScalarNames.add(moduleAndData.getDataName());
					}
				}
				
				ScalarDataSorter sorter = new ScalarDataSorter(manager);
				ResultItemFields rowFields = new ResultItemFields(MODULE, NAME);
				ResultItemFields columnFields = averageReplications ? new ResultItemFields(EXPERIMENT, MEASUREMENT) :
					                                              new ResultItemFields(RUN, EXPERIMENT, MEASUREMENT, REPLICATION);
				xyScalars = sorter.prepareScatterPlot3(scalars,
								xModuleName, xScalarName, rowFields, columnFields,
								isoModuleNames, isoScalarNames);
				
				// check ordered
//				for(int i = 0; i < xyScalars.size(); ++i) {
//					XYDataset ds = xyScalars.get(i);
//					double prevX = Double.NEGATIVE_INFINITY;
//					for (int j = 0; j < ds.getColumnCount(); ++j) {
//						double x = ds.getValue(0, j).mean();
//						if (!Double.isNaN(x)) {
//							if (x < prevX)
//								Assert.isTrue(false, "Not ordered");
//							prevX = x;
//						}
//					}
//				}
				
			}
			
			XYArray[] xyVectors = null;
			IDList yVectors = null;
			if (!vectors.isEmpty()) {
				DataflowNetworkBuilder builder = new DataflowNetworkBuilder(manager);
				DataflowManager dataflowManager = builder.build(dataset, chart, true /*computeData*/); // XXX
				yVectors = builder.getDisplayedIDs();

				if (dataflowManager != null) {
					List<Node> arrayBuilders = builder.getArrayBuilders();
					dataflowManager.dump();
					xyVectors = executeDataflowNetwork(dataflowManager, arrayBuilders);
					if (xyVectors != null) {
						for (int i = 0; i < xyVectors.length; ++i)
							xyVectors[i].sortByX();
					}
				}
			}
			
			if (xyScalars != null && xyVectors == null)
				return createScatterPlotDataset(xyScalars);
			else if (xyScalars == null && xyVectors != null)
				return new VectorScatterPlotDataset(yVectors, xyVectors, manager);
			else if (xyScalars != null && xyVectors != null)
				return new CompoundXYDataset(
						createScatterPlotDataset(xyScalars),
						new VectorScatterPlotDataset(yVectors, xyVectors, manager));
			
		}
		return null;
	}
	
	public static IXYDataset createScatterPlotDataset(XYDatasetVector xydatasets) {
		IXYDataset[] datasets = new IXYDataset[(int)xydatasets.size()];
		for (int i = 0; i < datasets.length; ++i)
			datasets[i] = new ScalarScatterPlotDataset(xydatasets.get(i));
		return new CompoundXYDataset(datasets);
	}
	
	public static IXYDataset createXYDataset(Chart chart, Dataset dataset, boolean computeData, ResultFileManager manager, IProgressMonitor monitor) {
		if (chart instanceof LineChart) {
			LineChart lineChart = (LineChart)chart;
			return createVectorDataset(lineChart, dataset, lineChart.getLineNameFormat(), computeData, manager, null);
		}
		else if (chart instanceof ScatterChart)
			return createScatterPlotDataset((ScatterChart)chart, dataset, manager, monitor);
		else
			return null;
	}
	
	public static String[] getResultItemNames(IDList idlist, String nameFormat, ResultFileManager manager) {
		ResultItem[] items = ScaveModelUtil.getResultItems(idlist, manager);
		String format = StringUtils.isEmpty(nameFormat) ? getLineNameFormat(items) : nameFormat;
		return ResultItemFormatter.formatResultItems(format, items);
	}
	
	private static final ResultItemField[] FIELDS_OF_LINENAMES = new ResultItemField[] {
		new ResultItemField(FILE), new ResultItemField(RUN),
		new ResultItemField(RUNNUMBER), new ResultItemField(MODULE),
		new ResultItemField(NAME), new ResultItemField(EXPERIMENT),
		new ResultItemField(MEASUREMENT), new ResultItemField(REPLICATION)
	};

	
	/**
	 * Returns the default format string for names of the lines in {@code items}.
	 * It is "{file} {run} {run-number} {module} {name} {experiment} {measurement} {replication}",
	 * but fields that are the same for each item are omitted.
	 * If all the fields has the same value in {@code items}, then the "{index}" is used as 
	 * the format string. 
	 */
	private static String getLineNameFormat(ResultItem[] items) {
		if (items.length == 0)
			return "";
		
		StringBuffer sbFormat = new StringBuffer();
		char separator = ' ';
		for (ResultItemField field : FIELDS_OF_LINENAMES) {
			String firstValue = field.getFieldValue(items[0]);
			
			for (int i = 1; i < items.length; ++i) {
				String value = field.getFieldValue(items[i]);
				if (!ObjectUtils.equals(firstValue, value)) {
					sbFormat.append('{').append(field.getName()).append('}').append(separator);
					break;
				}
			}
		}
		
		if (sbFormat.length() > 0)
			sbFormat.deleteCharAt(sbFormat.length() - 1);
		else
			sbFormat.append("{index}");
		
		return sbFormat.toString();
	}
	

	public static IDList select(IDList source, List<SelectDeselectOp> filters, ResultFileManager manager, ResultType type) {
		IDList result = new IDList();
		for (SelectDeselectOp filter : filters) {
			if (type == null || filter.getType()==type) {
				if (filter instanceof Select) {
					result.merge(select(source, (Select)filter, manager));
				}
				else if (filter instanceof Deselect) {
					result.substract(select(source, (Deselect)filter, manager));
				}
			}
		}
		return result;
	}

	public static IDList select(IDList source, SetOperation op, ResultFileManager manager) {
		Dataset sourceDataset = op.getSourceDataset();
		ResultType type = op.getType();
		IDList idlist = selectInternal(source, sourceDataset, type, op.getFilterPattern(), manager);

		List<Except> excepts = null;
		if (op instanceof Select)
			excepts = ((Select)op).getExcepts();
		else if (op instanceof Deselect)
			excepts = ((Deselect)op).getExcepts();
		else if (op instanceof Add)
			excepts = ((Add)op).getExcepts();
		else if (op instanceof Discard)
			excepts = ((Discard)op).getExcepts();

		if (excepts != null)
			for (Except except : excepts)
				idlist.substract(selectInternal(idlist, sourceDataset, type, except.getFilterPattern(), manager));

		return idlist;
	}

	private static IDList selectInternal(IDList source, Dataset sourceDataset, ResultType type, String filterPattern, ResultFileManager manager) {
		IDList sourceIDList = source != null ? source :
							  sourceDataset == null ? ScaveModelUtil.getAllIDs(manager, type) :
							  DatasetManager.getIDListFromDataset(manager, sourceDataset, null, type);

		return ScaveModelUtil.filterIDList(sourceIDList, new Filter(filterPattern), manager);
	}
	
	
	static long ensureComputedResultItem(ProcessingOp operation, long inputID, ResultFileManager manager) {
		long nodeID = getFilterNodeID(operation);
		long id = manager.getComputedVector(nodeID, inputID);
		if (id == -1) {
			//System.out.format("Add computed vector: (%x,%x)%n", nodeID, inputID);
			VectorResult vector = manager.getVector(inputID);
			String name = String.format("%s(%s)", operation.getOperation(), vector.getName());
			id = manager.addComputedVector(name, nodeID, inputID);
		}
		return id;
	}
	
	static long getFilterNodeID(ProcessingOp operation) {
		return operation.hashCode();
//		Dataset dataset = ScaveModelUtil.findEnclosingDataset(operation);
//		Assert.isNotNull(dataset);
//		long datasetID = (long)dataset.hashCode();
//		long operationID = (long)operation.hashCode();
//		return (datasetID << 32) | (operationID & 0xFFFFFFFF);
	}
}

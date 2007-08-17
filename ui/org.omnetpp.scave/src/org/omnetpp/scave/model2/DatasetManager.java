package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.scave.charting.dataset.IXYDataset;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.dataset.ScatterPlotDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarFields;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.XYArray;
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
			// IDs are not changed
			return this;
		}

		public Object caseCompute(Compute compute) {
			// TODO
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
		ScalarFields rowFields = rowFieldNames == null || rowFieldNames.isEmpty() ? null :
									new ScalarFields(StringVector.fromArray(
											rowFieldNames.toArray(new String[rowFieldNames.size()])));
		ScalarFields columnFields = columnFieldNames == null || columnFieldNames.isEmpty() ? null :
										new ScalarFields(StringVector.fromArray(
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
	
	public static ScatterPlotDataset createScatterPlotDataset(ScatterChart chart, ResultFileManager manager, IProgressMonitor monitor) {
		return createScatterPlotDataset(chart, null, manager, monitor);
	}
	
	public static ScatterPlotDataset createScatterPlotDataset(ScatterChart chart, Dataset dataset, ResultFileManager manager, IProgressMonitor monitor) {
		Assert.isLegal(chart.getXDataModule() != null, "Module name is not set");
		Assert.isLegal(chart.getXDataName() != null, "Data name is not set");
		
		//TODO update progressMonitor
		if (dataset == null)
			dataset = ScaveModelUtil.findEnclosingDataset(chart);
		if (dataset != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			return new ScatterPlotDataset(idlist, chart.getXDataModule(), chart.getXDataName(),
												chart.isAverageReplications(), manager);
		}
		return null;
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
}

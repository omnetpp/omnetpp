package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.dataset.ScatterPlotDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IDVectorVector;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.ScalarDataSorter;
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
	static class ProcessDatasetSwitch extends ScaveModelSwitch {

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
			if (add.getType() == type)
				idlist.merge(select(null, add));
			return this;
		}

		public Object caseDiscard(Discard discard) {
			if (discard.getType() == type)
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
		builder.build(dataset, lastItemToProcess);
		builder.close();

		List<Node> arrayBuilders = builder.getOutputs();
		DataflowManager dataflowManager = builder.getDataflowManager();
		
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
		builder.build(idlist);
		builder.close();
		return executeDataflowNetwork(builder.getDataflowManager(), builder.getOutputs());
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
		List<String> fieldNames = (List<String>)chart.getGroupBy();
		ScalarFields fields = fieldNames != null ?  new ScalarFields(StringVector.fromArray(fieldNames.toArray(new String[fieldNames.size()]))) : null;
 		return new ScalarDataset(idlist, fields, manager);
	}
	
	public static VectorDataset createVectorDataset(Chart chart, ResultFileManager manager, IProgressMonitor progressMonitor) {
		//TODO update progressMonitor
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		XYArray[] dataValues = getDataFromDataset(manager, dataset, chart);
		IDList idlist = getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL);
		String[] dataNames = getResultItemIDs(idlist, manager);
		return new VectorDataset(dataNames, dataValues);
	}
	
	public static ScatterPlotDataset createScatterPlotDataset(ScatterChart chart, ResultFileManager manager, IProgressMonitor monitor) {
		//TODO update progressMonitor
		Dataset dataset = ScaveModelUtil.findEnclosingDataset(chart);
		if (dataset != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			ScalarDataSorter sorter = new ScalarDataSorter(manager);
			IDVectorVector data = sorter.prepareScatterPlot(idlist, chart.getModuleName(), chart.getDataName());
			return new ScatterPlotDataset(data, manager);
		}
		return null;
	}

	public static String[] getYDataNames(Chart chart, ResultFileManager manager) {
		if (chart == null)
			return null;
		return getYDataNames(chart, ScaveModelUtil.findEnclosingDataset(chart), manager);
	}
	
	public static String[] getYDataNames(Chart chart, Dataset dataset, ResultFileManager manager) {
		if (chart == null || dataset == null || manager == null)
			return null;
		
		if (chart instanceof LineChart) {
			IDList idlist = getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL);
			return getResultItemIDs(idlist, manager);
		}
		else if (chart instanceof ScatterChart) {
			ScatterChart sc = (ScatterChart)chart;
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			ScalarDataSorter sorter = new ScalarDataSorter(manager);
			IDVectorVector data = sorter.prepareScatterPlot(idlist, sc.getModuleName(), sc.getDataName());
			return ScatterPlotDataset.computeNames(data, manager);
		}
		else
			Assert.isLegal(false, "Unknown chart type: " + chart.getClass().getName());
		
		return null;
	}
	
	
	/**
	 * Returns the ids of data items in the <code>idlist</code>.
	 * The id is formed from the file name, run number, run id, module name,
	 * data name, experiment, measurement and replication.
	 * Constant fields will be omitted from the id.
	 */
	public static String[] getResultItemIDs(IDList idlist, ResultFileManager manager) {
		String[][] nameFragments = new String[(int)idlist.size()][];
		for (int i = 0; i < idlist.size(); ++i) {
			ResultItem item = manager.getItem(idlist.get(i));
			Run run = item.getFileRun().getRun();
			nameFragments[i] = new String[8];
			nameFragments[i][0] = item.getFileRun().getFile().getFilePath();
			nameFragments[i][1] = String.valueOf(run.getRunNumber());
			nameFragments[i][2] = String.valueOf(run.getRunName());
			nameFragments[i][3] = item.getModuleName();
			nameFragments[i][4] = item.getName();
			nameFragments[i][5] = run.getAttribute(RunAttribute.EXPERIMENT);
			nameFragments[i][6] = run.getAttribute(RunAttribute.REPLICATION);
			nameFragments[i][7] = run.getAttribute(RunAttribute.MEASUREMENT);
		}

		boolean[] same = new boolean[8];
		Arrays.fill(same, true);
		for (int i = 1; i < nameFragments.length; ++i) {
			for (int j = 0; j < 8; ++j)
				if (same[j] && !equals(nameFragments[0][j], nameFragments[i][j]))
					same[j] = false;
		}

		String[] result = new String[nameFragments.length];
		for (int i = 0; i < result.length; ++i) {
			StringBuffer id = new StringBuffer(30);
			for (int j = 0; j < 8; ++j)
				if (!same[j])
					id.append(nameFragments[i][j]).append(" ");
			if (id.length() == 0)
				id.append(i);
			else
				id.deleteCharAt(id.length() - 1);
			result[i] = id.toString();
		}
		return result;
	}

	private static boolean equals(Object first, Object second) {
		return first == null && second == null ||
				first != null && first.equals(second);
	}

	public static IDList select(IDList source, List<SelectDeselectOp> filters, ResultFileManager manager, ResultType type) {
		IDList result = new IDList();
		for (SelectDeselectOp filter : filters) {
			if (filter.getType()==type) {
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
		IDList idlist = selectInternal(source, op, manager);

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
				idlist.substract(selectInternal(idlist, except, manager));

		return idlist;
	}

	private static IDList selectInternal(IDList source, SetOperation op, ResultFileManager manager) {
		Dataset sourceDataset = op.getSourceDataset();
		ResultType type = op.getType();
		IDList sourceIDList = source != null ? source :
							  sourceDataset == null ? ScaveModelUtil.getAllIDs(manager, type) :
							  DatasetManager.getIDListFromDataset(manager, sourceDataset, null, type);

		return ScaveModelUtil.filterIDList(sourceIDList, new Filter(op.getFilterPattern()), manager);
	}
}

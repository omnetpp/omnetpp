package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.omnetpp.scave.charting.dataset.ScalarDataset;
import org.omnetpp.scave.charting.dataset.ScatterPlotDataset;
import org.omnetpp.scave.charting.dataset.VectorDataset;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IDVector;
import org.omnetpp.scave.engine.IDVectorVector;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.ScalarDataSorter;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Except;
import org.omnetpp.scave.model.Group;
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

		List<Node> outputs = builder.getOutputs();

		builder.getDataflowManager().dump();

		long startTime = System.currentTimeMillis();
		if (outputs.size() > 0) // XXX DataflowManager crashes when there are no sinks
			builder.getDataflowManager().execute();
		System.out.println("dataflow network: "+(System.currentTimeMillis()-startTime)+" ms");

		XYArray[] result = new XYArray[outputs.size()];
		for (int i = 0; i < result.length; ++i)
			result[i] = outputs.get(i).getArray();
		return result;
	}

	public static ScalarDataset createScalarDataset(Chart chart, Dataset dataset, ResultFileManager manager, IProgressMonitor progressMonitor) {
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
		ScalarDataset ds = new ScalarDataset();
		//TODO update progressMonitor
		for (int i = 0; i < idlist.size(); ++i) {
			ScalarResult scalar = manager.getScalar(idlist.get(i));
			ds.addValue(scalar.getFileRun().getRun().getRunName(),
					    scalar.getModuleName()+"\n"+scalar.getName(),
					    scalar.getValue());
		}
		return ds;
	}

	public static VectorDataset createVectorDataset(Chart chart, Dataset dataset, ResultFileManager manager, IProgressMonitor progressMonitor) {
		//TODO update progressMonitor
		XYArray[] dataValues = getDataFromDataset(manager, dataset, chart);
		IDList idlist = getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL);
		String[] dataNames = getResultItemIDs(idlist, manager);
		return new VectorDataset(dataNames, dataValues);
	}
	
	public static String[] getVectorDataNames(Chart chart, ResultFileManager manager) {
		Dataset dataset;
		String[] names = null;
		if (chart != null && manager != null &&
				(dataset = ScaveModelUtil.findEnclosingDataset(chart)) != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.VECTOR_LITERAL);
			names = DatasetManager.getResultItemNames(idlist, manager);
		}
		return names;
	}
	
	public static ScatterPlotDataset createScatterPlotDataset(ScatterChart chart, Dataset dataset, ResultFileManager manager, IProgressMonitor progressMonitor) {
		IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
		ScalarDataSorter sorter = new ScalarDataSorter(manager);
		IDVectorVector data = sorter.prepareScatterPlot(idlist, chart.getModuleName(), chart.getDataName());
		String[] names = getScatterPlotDataNames(chart, manager);
		return new ScatterPlotDataset(data, names, manager);
	}
	
	public static String[] getScatterPlotDataNames(ScatterChart chart, ResultFileManager manager) {
		Dataset dataset;
		String[] names = null;
		if (chart != null && manager != null &&
				(dataset = ScaveModelUtil.findEnclosingDataset(chart)) != null) {
			IDList idlist = DatasetManager.getIDListFromDataset(manager, dataset, chart, ResultType.SCALAR_LITERAL);
			ScalarDataSorter sorter = new ScalarDataSorter(manager);
			IDVectorVector data = sorter.prepareScatterPlot(idlist, chart.getModuleName(), chart.getDataName());
			names = new String[(int)data.size()-1];
			for (int i=0; i<names.length; ++i) {
				IDVector v = data.get(i+1);
				names[i] = String.valueOf(i);
				for (int j=0; j<v.size(); ++j) {
					long id = v.get(j);
					if (id != -1) {
						ResultItem item = manager.getItem(id);
						names[i] = item.getModuleName() + " " + item.getName();
						break;
					}
				}
			}
		}
		return names;
	}

	public static String[] getResultItemNames(IDList idlist, ResultFileManager manager) {
		return getResultItemIDs(idlist, manager);
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

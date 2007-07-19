package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.Port;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorFileReaderNode;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

/**
 * Builds a dataflow network for a dataset.
 *
 * @author tomi
 */
// TODO: add tee node to the C code
// TODO: implement DataflowManager.remove(Node node) (removes node and connected nodes recursively)
// TODO: find identifier for a computed result (computed flag in id?)
public class DataflowNetworkBuilder {

	private DataflowManager dataflowManager;
	private ResultFileManager resultfileManager;
	private NodeTypeRegistry factory;

	private Map<ResultFile,Node> fileToSourceNodeMap = new HashMap<ResultFile,Node>();
	private Map<Long,Port> idToPortMap = new HashMap<Long,Port>();
	private List<Node> outputNodes = new ArrayList<Node>();

	private IDList chartDisplayedIds;

	public DataflowNetworkBuilder(ResultFileManager resultfileManager) {
		this.resultfileManager = resultfileManager;
		this.dataflowManager = new DataflowManager();
		this.factory = NodeTypeRegistry.instance();
	}

	public DataflowManager getDataflowManager() {
		return dataflowManager;
	}

	public void build(Dataset dataset, final DatasetItem target) {

		ScaveModelSwitch<Object> modelSwitch = new ScaveModelSwitch<Object>() {
			private boolean finished;

			/**
			 * Adds "vectorfilereader" nodes for ids in the base dataset.
			 */
			public Object caseDataset(Dataset dataset) {
				// build network for the base dataset first
				Dataset baseDataset = dataset.getBasedOn();
				if (baseDataset != null) {
					build(baseDataset, null);
				}
				// process items
				for (Object item : dataset.getItems())
					doSwitch((EObject)item);
				return this;
			}

			/**
			 * Adds "vectorfilereader" nodes for ids selected by the add operation.
			 */
			public Object caseAdd(Add add) {
				if (add.getType()==ResultType.VECTOR_LITERAL)
					addSourceNodes(select(null, add));
				return this;
			}

			/**
			 * Removes "vectorfilereader" ports (and filter nodes connected to it)
			 * of the ids selected by the discard operation.
			 */
			public Object caseDiscard(Discard discard) {
				if (discard.getType()==ResultType.VECTOR_LITERAL)
					removeSources(select(getIDs(), discard));
				return this;
			}

			/**
			 * Adds a filter node for each port selected by the apply operation.
			 */
			public Object caseApply(Apply apply) {
				IDList idlist = select(getIDs(), apply.getFilters());
				for (int i = 0; i < idlist.size(); ++i) {
					long id = idlist.get(i);
					addFilterNode(id, apply.getOperation(), apply.getParams());
				}
				return this;
			}

			public Object caseCompute(Compute compute) {
				// TODO
				return this;
			}

			/**
			 * Filters the output according to the chart's filters when the chart is
			 * the target of the dataflow network.
			 *
			 * Currently no method to remove nodes from the dataflow network,
			 * so we remember the ids and filter the output nodes accordingly.
			 */
			public Object caseChart(Chart chart) {
				if (chart == target)
					chartDisplayedIds = select(getIDs(), chart.getFilters());
				return this;
			}

			/**
			 * If the target is in the group, then process its items,
			 * otherwise ignore it. (Operations in a group have no
			 * effects outside the group.)
			 */
			public Object caseGroup(Group group) {
				if (EcoreUtil.isAncestor(group, target)) {
					for (Object item : group.getItems())
						doSwitch((EObject)item);
				}
				return this;
			}

			public Object defaultCase(EObject object) {
				return this; // do nothing
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
		};

		modelSwitch.doSwitch(dataset);
	}
	
	/**
	 * Builds a dataflow network for reading the data of the given vectors.
	 */
	public void build(IDList idlist) {
		for (int i = 0; i < (int)idlist.size(); ++i) {
			addSourceNode(idlist.get(i));
		}
		close();
	}

	
	public IDList getIDs() {
		Set<Long> keys = idToPortMap.keySet();
		return IDList.fromArray(keys.toArray(new Long[keys.size()]));
	}
	
	public IDList getDisplayedIDs() {
		return chartDisplayedIds != null ? chartDisplayedIds : IDList.EMPTY;
	}
	
	/**
	 * Adds an arraybuilder node for each open ports.
	 */
	public void close() {
		IDList idlist = getIDs();
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addSinkNode(id);
		}
	}

	/**
	 * Returns the output nodes.
	 */
	public List<Node> getOutputs() {
		return outputNodes;
	}

	/*=========================
	 *          Helpers
	 *=========================*/
	private static final StringMap EMPTY_ATTRS = new StringMap();

	private Node createNode(String typeName, StringMap attrs) {
		Assert.isLegal(factory.exists(typeName), "Unknown node type: " + typeName);
		NodeType nodeType = factory.getNodeType(typeName);
		return nodeType.create(dataflowManager, attrs);
	}

	private Node getOrCreateSourceNode(long id) {
		ResultFile file = resultfileManager.getVector(id).getFileRun().getFile();
		Node sourceNode = fileToSourceNodeMap.get(file);
		if (sourceNode == null) {
			StringMap attrs = new StringMap();
			attrs.set("filename", file.getFileSystemFilePath());
			sourceNode = createNode("vectorfilereader", attrs);
			fileToSourceNodeMap.put(file, sourceNode);
		}
		return sourceNode;
	}

	private Node createFilterNode(String operation, List<Param> params) {
		StringMap attrs = new StringMap();
		for (Param param : params)
			attrs.set(param.getName(), param.getValue());
		return createNode(operation, attrs);
	}

	private Node createSinkNode() {
		return createNode("arraybuilder", EMPTY_ATTRS);
	}

	private void connect(Port outputPort, Node node, String inputPortName) {
		dataflowManager.connect(outputPort, node.nodeType().getPort(node, inputPortName));
	}

	private Port getPort(long id) {
		return idToPortMap.get(id);
	}

	private Port getPort(Node node, String portName) {
		return node.nodeType().getPort(node, portName);
	}

	private void addSourceNodes(IDList idlist) {
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addSourceNode(id);
		}
	}

	private Node addSourceNode(long id) {
		// close the port
		if (getPort(id) != null) {
			Node node = addSinkNode(id);
			outputNodes.remove(node);
		}
		// create new port
		Node node = getOrCreateSourceNode(id);
		VectorFileReaderNode reader = VectorFileReaderNode.cast(node);
		Port port = reader.addVector(resultfileManager.getVector(id));
		idToPortMap.put(id, port);
		return node;
	}

	private Node addFilterNode(long id, String operation, List<Param> params) {
		Port port = getPort(id);
		Node filterNode = createFilterNode(operation, params);
		if (filterNode != null) {
			connect(port, filterNode, "in");
			idToPortMap.put(id, getPort(filterNode, "out"));
		}
		return filterNode;
	}

	private Node addSinkNode(long id) {
		Node sinkNode = createSinkNode();
		Port port = getPort(id);
		connect(port, sinkNode, "in");
		idToPortMap.remove(id);
		if (chartDisplayedIds == null || contains(chartDisplayedIds, id))
			outputNodes.add(sinkNode);
		return sinkNode;
	}

	// XXX add this to IDList
	private static boolean contains(IDList idlist, long id) {
		for (int i = 0; i < idlist.size(); ++i)
			if (idlist.get(i) == id)
				return true;
		return false;
	}


	private void removeSources(IDList ids) {
		for (int i = 0; i < ids.size(); ++i)
			removeNodes(ids.get(i));
	}

	private void removeNodes(long id) {

	}

	private IDList select(IDList source, List<SelectDeselectOp> filters) {
		// if no select, then interpret it as "select all" -- so we add an 
		// artificial "Select All" node into filters[] before proceeding
		if (filters.size() == 0 || filters.get(0) instanceof Deselect) {
			Select selectAll = ScaveModelFactory.eINSTANCE.createSelect();
			selectAll.setType(ResultType.VECTOR_LITERAL);
			filters = new ArrayList<SelectDeselectOp>(filters);
			filters.add(0, selectAll);
		}

		// now, actually evaluate the select/deselect stuff on the IDList
		return DatasetManager.select(source, filters, resultfileManager, ResultType.VECTOR_LITERAL);
	}

	private IDList select(IDList source, AddDiscardOp op) {
		return DatasetManager.select(source, op, resultfileManager);
	}
}

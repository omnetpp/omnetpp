package org.omnetpp.scave2.model;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.collections.ListUtils;
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
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave.model.Deselect;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ScaveModelFactory;
import org.omnetpp.scave.model.Select;
import org.omnetpp.scave.model.SelectDeselectOp;
import org.omnetpp.scave.model.util.ScaveModelSwitch;

/**
 * Builds a dataflow network for a dataset.
 *
 * @author tomi
 */
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
		
		ScaveModelSwitch modelSwitch = new ScaveModelSwitch() {
			private boolean finished;

			/**
			 * Adds "vectorfilereader" nodes for ids in the base dataset.
			 */
			public Object caseDataset(Dataset dataset) {
				// add sources for each id in the base dataset
				Dataset baseDataset = dataset.getBasedOn();
				if (baseDataset != null) {
					IDList idlist = DatasetManager.getIDListFromDataset(resultfileManager, baseDataset, null);
					addSourceNodes(idlist);
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
				addSourceNodes(select(null, add));
				return this;
			}
			
			/**
			 * Removes "vectorfilereader" ports (and filter nodes connected to it)
			 * of the ids selected by the discard operation.
			 */
			public Object caseDiscard(Discard discard) {
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
	 * Adds an arraybuilder node for each open ports.
	 */
	public void close() {
		StringMap attrs = new StringMap();
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
		Node node = null;
		if (factory.exists(typeName)) {
			NodeType nodeType = factory.getNodeType(typeName);
			node = nodeType.create(dataflowManager, attrs);
		}
		return node;
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
	
	private IDList getIDs() {
		Set<Long> keys = idToPortMap.keySet();
		return IDList.fromArray(keys.toArray(new Long[keys.size()]));
	}
	
	private void addSourceNodes(IDList idlist) {
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addSourceNode(id);
		}
	}
	
	private void addSourceNode(long id) {
		// close the port
		if (getPort(id) != null)
			addSinkNode(id);
		// create new port
		Node node = getOrCreateSourceNode(id);
		String portName = Integer.toString(resultfileManager.getVector(id).getVectorId());
		idToPortMap.put(id, getPort(node, portName));
	}
	
	private void addFilterNode(long id, String operation, List<Param> params) {
		Port port = getPort(id);
		Node filterNode = createFilterNode(operation, params);
		if (filterNode != null) {
			connect(port, filterNode, "in");
			idToPortMap.put(id, getPort(filterNode, "out"));
		}
	}
	
	private void addSinkNode(long id) {
		Node sinkNode = createSinkNode();
		Port port = getPort(id);
		connect(port, sinkNode, "in");
		idToPortMap.remove(id);
		if (chartDisplayedIds == null || contains(chartDisplayedIds, id))
			outputNodes.add(sinkNode);
	}
	
	// XXX add this to IDList
	private static boolean contains(IDList idlist, long id) {
		for (int i = 0; i < idlist.size(); ++i)
			if (idlist.get(i) == id)
				return true;
		return false;
	}
	
	
	private void removeSources(IDList ids) {
		// TODO: not supported yet
	}

	private IDList select(IDList source, List<SelectDeselectOp> filters) {
		// if no select, then interpret it as "select all"
		if (filters.size() == 0 || filters.get(0) instanceof Deselect) {
			Select selectAll = ScaveModelFactory.eINSTANCE.createSelect(); 
			filters = new ArrayList<SelectDeselectOp>(filters);
			filters.add(0, selectAll);
		}
		
		return DatasetManager.select(source, filters, resultfileManager, DatasetType.VECTOR_LITERAL);
	}
	
	private IDList select(IDList source, AddDiscardOp op) {
		return DatasetManager.select(source, op, resultfileManager, DatasetType.VECTOR_LITERAL);
	}
}

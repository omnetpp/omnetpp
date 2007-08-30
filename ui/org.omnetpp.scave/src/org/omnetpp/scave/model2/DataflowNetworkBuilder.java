package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.Port;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engine.IndexedVectorFileReaderNode;
import org.omnetpp.scave.engine.VectorResult;
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
// TODO: find identifier for a computed result (computed flag in id?)
public class DataflowNetworkBuilder {
	
	private static final StringMap EMPTY_ATTRS = new StringMap();

	/*
	 * Inner representation of the dataflow graph.
	 */
	private List<SinkNode> sinkNodes = new ArrayList<SinkNode>();
	private Map<ResultFile,SourceNode> fileToSourceNodeMap = new HashMap<ResultFile,SourceNode>();
	private Map<Long,PortWrapper> idToOutputPortMap = new LinkedHashMap<Long,PortWrapper>();

	class NodeWrapper
	{
		Node node;
		String type;
		StringMap attrs;
		List<PortWrapper> inPorts = new ArrayList<PortWrapper>();
		List<PortWrapper> outPorts = new ArrayList<PortWrapper>();
		
		public NodeWrapper(String type, StringMap attrs) {
			this.type = type;
			this.attrs = attrs;
		}
		
		public void addInputPort(PortWrapper port) {
			port.owner = this;
			inPorts.add(port);
		}
		
		public void addOutputPort(PortWrapper port) {
			port.owner = this;
			outPorts.add(port);
		}
		
		/**
		 * Notifies the node that one of its input port got connected.
		 */
		public void connected(PortWrapper in) {
		}
		
		/**
		 * Creates the port in the dataflowManager.
		 * <p>
		 * Override this method if the node expects extra information
		 * for the port which is not encoded in the port name (e.g. inputfilereader nodes).
		 * <p>
		 * Note: do not use the returned object after a new port
		 *       added to the node. It contains a pointer which
		 *       becomes invalid!
		 */
		public Port createPort(PortWrapper port) {
			return node.nodeType().getPort(node, port.name);
		}
	}
	
	class SourceNode extends NodeWrapper
	{
		ResultFile resultFile;
		
		public SourceNode(ResultFile resultFile) {
			super("indexedvectorfilereader", null);
			this.resultFile = resultFile;
			this.attrs = new StringMap();
			this.attrs.set("filename", resultFile.getFileSystemFilePath());
			fileToSourceNodeMap.put(resultFile, this);
		}
		
		public PortWrapper addPort(long id) {
			int vectorId = resultfileManager.getVector(id).getVectorId();
			PortWrapper port = new PortWrapper(id, ""+vectorId);
			addOutputPort(port);
			idToOutputPortMap.put(id, port);
			return port;
		}
		
		public PortWrapper getOutputPort(long id) {
			for (PortWrapper port : outPorts)
				if (port.id == id)
					return port;
			return null;
		}
		
		public void removeOutputPort(long id) {
			for (int i = 0; i < outPorts.size(); ++i)
				if (outPorts.get(i).id == id) {
					outPorts.remove(i);
					break;
				}
		}
		
		@Override
		public Port createPort(PortWrapper port) {
			IndexedVectorFileReaderNode reader = IndexedVectorFileReaderNode.cast(node);
			return reader.addVector(resultfileManager.getVector(port.id));
		}
	}
	
	class FilterNode extends NodeWrapper
	{
		PortWrapper inPort;
		PortWrapper outPort;
		int computationID;
		
		public FilterNode(String type, List<Param> params) {
			super(type, EMPTY_ATTRS);
			attrs = new StringMap();
			for (Param param : params)
				attrs.set(param.getName(), param.getValue());
			computationID = calculateComputationID();
			addInputPort(inPort = new PortWrapper(-1, "in"));
			addOutputPort(outPort = new PortWrapper(-1, "out"));
			
		}

		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			outPort.id = resultfileManager.getComputedVector(computationID, inPort.id);
			if (outPort.id == -1) {
				VectorResult vector = resultfileManager.getVector(inPort.id);
				String name = String.format("%s(%s)", type, vector.getName());
				outPort.id = resultfileManager.addComputedVector(name, computationID, inPort.id);
			}
			if (!(inPort.channel.out.owner instanceof TeeNode)) // XXX
				idToOutputPortMap.remove(inPort.id);
			idToOutputPortMap.put(outPort.id, outPort);
		}
		
		private int calculateComputationID() {
			String str = type;
			StringVector attrNames = attrs.keys();
			long size = attrNames.size();
			for (int i = 0; i < size; ++i) {
				String name = attrNames.get(i);
				str += attrs.get(name);
			}
			return str.hashCode(); // XXX should use the string as id
		}
	}
	
	class TeeNode extends NodeWrapper
	{
		PortWrapper inPort;
		PortWrapper outPort1;
		PortWrapper outPort2;
		
		public TeeNode() {
			super("tee", EMPTY_ATTRS);
			addInputPort(inPort = new PortWrapper(-1, "in"));
			addOutputPort(outPort1 = new PortWrapper(-1, "next-out"));
			addOutputPort(outPort2 = new PortWrapper(-1, "next-out"));
		}
		
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			outPort1.id = inPort.id;
			outPort2.id = inPort.id;
			idToOutputPortMap.put(outPort1.id, outPort1);
		}
	}
	
	class SinkNode extends NodeWrapper
	{
		PortWrapper inPort;
		
		public SinkNode() {
			super("arraybuilder", EMPTY_ATTRS);
			addInputPort(inPort = new PortWrapper(-1, "in"));
			sinkNodes.add(this);
		}
		
		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			idToOutputPortMap.remove(in.id);
		}
	}
	
	class PortWrapper {
		long id;
		String name;
		NodeWrapper owner;
		ChannelWrapper channel;
		
		public PortWrapper(long id, String name) {
			this.id = id;
			this.name = name;
		}
	}
	
	class ChannelWrapper {
		PortWrapper out;
		PortWrapper in;
		
		public ChannelWrapper(PortWrapper out, PortWrapper in) {
			this.out = out;
			this.in = in;
			out.channel = this;
			in.channel = this;
			
			in.id = out.id;
			in.owner.connected(in);
		}
	}

	private ResultFileManager resultfileManager;

	public DataflowNetworkBuilder(ResultFileManager resultfileManager) {
		this.resultfileManager = resultfileManager;
	}

	/**
	 * Builds a dataflow network for reading the data of the given vectors.
	 */
	public DataflowManager build(IDList idlist) {
		for (int i = 0; i < (int)idlist.size(); ++i) {
			addSourceNode(idlist.get(i));
		}
		close();
		return buildNetwork();
	}
	
	/**
	 * Builds a dataflow network for reading the data of {@code dataset} at 
	 * the given dataset item.
	 */
	public DataflowManager build(Dataset dataset, DatasetItem target, boolean createDataflowManager) {
		buildInternal(dataset, target);
		close();
		return createDataflowManager ? buildNetwork() : null;
	}
	
	public IDList getDisplayedIDs() {
		IDList idlist = new IDList();
		for (SinkNode node : sinkNodes)
			idlist.add(node.inPort.id);
		return idlist;
	}
	
	/**
	 * Returns the arraybuilder nodes.
	 */
	public List<Node> getArrayBuilders() {
		List<Node> result = new ArrayList<Node>();
		for (SinkNode node : sinkNodes)
			result.add(node.node);
		return result;
	}

	private void buildInternal(Dataset dataset, final DatasetItem target) {

		ScaveModelSwitch<Object> modelSwitch = new ScaveModelSwitch<Object>() {
			private boolean finished;

			/**
			 * Adds "vectorfilereader" nodes for ids in the base dataset.
			 */
			public Object caseDataset(Dataset dataset) {
				// build network for the base dataset first
				Dataset baseDataset = dataset.getBasedOn();
				if (baseDataset != null) {
					buildInternal(baseDataset, null);
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
					addFilterNode(idlist.get(i), apply.getOperation(), apply.getParams());
				}
				return this;
			}

			public Object caseCompute(Compute compute) {
				IDList idlist = select(getIDs(), compute.getFilters());
				for (int i = 0; i < idlist.size(); ++i) {
					TeeNode teeNode = addTeeNode(idlist.get(i), compute.getOperation(), compute.getParams());
					
					//addFilterNode(teeNode.outPort2.id, compute.getOperation(), compute.getParams());
				}
				return this;
			}

			/**
			 * Filters the output according to the chart's filters when the chart is
			 * the target of the dataflow network.
			 */
			public Object caseChart(Chart chart) {
				if (chart == target) {
					// discard everything not on the chart
					IDList idlist = getIDs();
					idlist.substract(select(idlist, chart.getFilters()));
					removeSources(idlist);
				}
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
	
	private IDList getIDs() {
		Set<Long> keys = idToOutputPortMap.keySet();
		return IDList.fromArray(keys.toArray(new Long[keys.size()]));
	}
	
	/**
	 * Adds an arraybuilder node for each open ports.
	 */
	private void close() {
		IDList idlist = getIDs();
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addSinkNode(id);
		}
	}
	
	/**
	 * Builds the network in the DataflowManager.
	 */
	private DataflowManager buildNetwork() {
		DataflowManager dataflowManager = new DataflowManager();
		
		try {

			for (SinkNode sinkNode : sinkNodes) {
				
				// create the arraybuilder node
				Assert.isLegal(NodeTypeRegistry.instance().exists(sinkNode.type),
						"Unknown node type: " + sinkNode.type);
				NodeType nodeType = NodeTypeRegistry.instance().getNodeType(sinkNode.type);
				sinkNode.node = nodeType.create(dataflowManager, sinkNode.attrs);
				
				ChannelWrapper channel = sinkNode.inPort.channel;
				// follow the channels backwards
				// until a source node or a previously created node reached
				while (channel != null) {
					NodeWrapper newNode = channel.out.owner.node == null ? channel.out.owner : null;

					// create new node
					if (newNode != null) {
						Assert.isLegal(NodeTypeRegistry.instance().exists(newNode.type),
											"Unknown node type: " + newNode.type);
						nodeType = NodeTypeRegistry.instance().getNodeType(newNode.type);
						newNode.node = nodeType.create(dataflowManager, newNode.attrs);
					}

					// create ports and connect them
					Port inPort = channel.in.owner.createPort(channel.in);
					Port outPort = channel.out.owner.createPort(channel.out); 
					dataflowManager.connect(outPort, inPort);

					if (newNode != null && newNode.inPorts.size() > 0) {
						Assert.isTrue(newNode.inPorts.size() == 1); // TODO multiple input nodes
						channel = newNode.inPorts.get(0).channel;
					}
					else // channel already created or reached source node
						break;
				}
			}
			return dataflowManager;
		} catch (Exception e) {
			ScavePlugin.logError(e);
			if (dataflowManager != null) {
				dataflowManager.delete();
				dataflowManager = null;
			}
			return null;
		}
	}

	/*=========================
	 *          Helpers
	 *=========================*/
	private SourceNode getOrCreateSourceNode(long id) {
		ResultFile file = resultfileManager.getVector(id).getFileRun().getFile();
		SourceNode sourceNode = fileToSourceNodeMap.get(file);
		if (sourceNode == null)
			sourceNode = new SourceNode(file);
		return sourceNode;
	}

	private void connect(PortWrapper outputPort, PortWrapper inPort) {
		new ChannelWrapper(outputPort, inPort);
	}

	private PortWrapper getOutputPort(long id) {
		return idToOutputPortMap.get(id);
	}

	private void addSourceNodes(IDList idlist) {
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addSourceNode(id);
		}
	}

	private SourceNode addSourceNode(long id) {
		if (getOutputPort(id) != null) {
			removeSource(id);
		}
		// create new port
		SourceNode node = getOrCreateSourceNode(id);
		node.addPort(id);
		return node;
	}
	
	private TeeNode addTeeNode(long id, String operation, List<Param> params) {
		PortWrapper port = getOutputPort(id);
		TeeNode teeNode = new TeeNode();
		connect(port, teeNode.inPort);
		addFilterNode(teeNode.outPort2, operation, params);
		return teeNode;
	}
	
	private FilterNode addFilterNode(long id, String operation, List<Param> params) {
		PortWrapper port = getOutputPort(id);
		return addFilterNode(port, operation, params);
	}
	

	private FilterNode addFilterNode(PortWrapper port, String operation, List<Param> params) {
		FilterNode filterNode = new FilterNode(operation, params);
		connect(port, filterNode.inPort);
		return filterNode;
	}

	private SinkNode addSinkNode(long id) {
		SinkNode sinkNode = new SinkNode();
		PortWrapper port = getOutputPort(id);
		connect(port, sinkNode.inPort);
		return sinkNode;
	}

	private void removeSources(IDList ids) {
		for (int i = 0; i < ids.size(); ++i)
			removeSource(ids.get(i));
	}

	private void removeSource(long id) {
		VectorResult vector = resultfileManager.getVector(id);
		ResultFile file = vector.getFileRun().getFile();
		SourceNode sourceNode = fileToSourceNodeMap.get(file);
		PortWrapper outPort = sourceNode.getOutputPort(id);
		sourceNode.removeOutputPort(id);
		while (outPort != null && outPort.channel != null) {
			PortWrapper inPort = outPort.channel.in;
			if (inPort.owner instanceof FilterNode)
				outPort = ((FilterNode)inPort.owner).outPort;
			else
				outPort = null;
		}
		if (sourceNode.outPorts.isEmpty()) {
			fileToSourceNodeMap.remove(file);
		}
		idToOutputPortMap.remove(id);
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

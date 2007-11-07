package org.omnetpp.scave.model2;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.IndexedVectorFileReaderNode;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.Port;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.model.Add;
import org.omnetpp.scave.model.AddDiscardOp;
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Group;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;
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
		
		public PortWrapper getIntputPortFor(PortWrapper outputPort)
		{
			Assert.isTrue(inPorts.size() <= 1);
			return inPorts.size() == 0 ? null : inPorts.get(0);
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
		ProcessingOp operation;
		
		public FilterNode(ProcessingOp operation) {
			super(operation.getOperation(), EMPTY_ATTRS);
			attrs = new StringMap();
			for (Param param : operation.getParams())
				attrs.set(param.getName(), param.getValue());
			this.operation = operation;
			addInputPort(inPort = new PortWrapper(-1, "in"));
			addOutputPort(outPort = new PortWrapper(-1, "out"));
			
		}

		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			//System.out.format("Connecting %x%n", inPort.id);
			outPort.id = DatasetManager.ensureComputedResultItem(operation, inPort.id, resultfileManager); 

			PortWrapper out = inPort.channel.out;  
			if (!(out.owner instanceof TeeNode) || out == ((TeeNode)out.owner).outPort1) { // XXX
				//System.out.format("Removing port %x from map%n", inPort.id);
				idToOutputPortMap.remove(inPort.id);
			}
			idToOutputPortMap.put(outPort.id, outPort);
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
	
	class XYPlotNode extends NodeWrapper
	{
		PortWrapper xPort;
		
		public XYPlotNode() {
			super("xyplot", EMPTY_ATTRS);
			addInputPort(xPort = new PortWrapper(-1, "x"));
		}
		
		public PortWrapper addPort() {
			int index = outPorts.size();
			PortWrapper yPort = new PortWrapper(-1, "y"+index);
			PortWrapper outPort = new PortWrapper(-1, "out"+index);
			addInputPort(yPort);
			addOutputPort(outPort);
			return yPort;
		}

		@Override
		public void connected(PortWrapper in) {
			if (in == xPort) {
				idToOutputPortMap.remove(in.id);
			}
			else {
				int index = inPorts.indexOf(in) - 1;
				Assert.isTrue(index >= 0);
				PortWrapper outPort = outPorts.get(index);
				outPort.id = in.id;
				idToOutputPortMap.put(in.id, outPort);
			}
		}

		@Override
		public Port createPort(PortWrapper port) {
			int index = outPorts.indexOf(port);
			if (index >= 0)
				super.createPort(inPorts.get(index+1));
			return super.createPort(port);
		}

		@Override
		public PortWrapper getIntputPortFor(PortWrapper outPort) {
			int index = outPorts.indexOf(outPort);
			Assert.isLegal(index >= 0);
			return inPorts.get(index + 1);
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
		boolean created;
		
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
		for (SinkNode node : sinkNodes) {
			Assert.isTrue(node.inPort.id != -1);
			idlist.add(node.inPort.id);
		}
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
				if (apply.getOperation() != null) {
					IDList idlist = select(getIDs(), apply.getFilters());
					for (int i = 0; i < idlist.size(); ++i) {
						addFilterNode(idlist.get(i), apply);
					}
				}
				return this;
			}

			public Object caseCompute(Compute compute) {
				if (compute.getOperation() != null) {
					IDList idlist = select(getIDs(), compute.getFilters());
					for (int i = 0; i < idlist.size(); ++i) {
						addComputeNode(idlist.get(i), compute);
					}
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
			 * Filters the output according to the chart's filters when the chart is
			 * the target of the dataflow network.
			 * Select x data and add xyplotnodes for them.
			 * Associated y data differ only by module/name.
			 */
			public Object caseScatterChart(ScatterChart chart) {
				if (chart == target) {
					// discard everything not on the chart
					IDList idlist = getIDs();
					IDList displayedIds = select(idlist, chart.getFilters());
					idlist.substract(displayedIds);
					removeSources(idlist);

					// select x data
					String xDataPattern = chart.getXDataPattern();
					if (!StringUtils.isEmpty(xDataPattern)) {
						
						IDList xData = resultfileManager.filterIDList(displayedIds, xDataPattern);
						for (int i = 0; i < xData.size(); ++i) {
							long id = xData.get(i);
							ResultItem item = resultfileManager.getItem(id);
							FileRunList fileruns = new FileRunList();
							fileruns.add(item.getFileRun());
							IDList yData = resultfileManager.filterIDList(displayedIds, fileruns, "", "");
							yData.substract(id);
							
							addXYPlotNode(id, yData);
						}
					}
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
				
				// follow the channels backwards
				// until a source node or a previously created node reached
				Queue<ChannelWrapper> pendingChannels = new ArrayDeque<ChannelWrapper>();
				pendingChannels.offer(sinkNode.inPort.channel);
				
				while (!pendingChannels.isEmpty()) {
					ChannelWrapper channel = pendingChannels.poll();
					
					NodeWrapper fromNode = channel.out.owner;
					NodeWrapper toNode = channel.in.owner;
					
					// create fromNode if needed
					if ((fromNode.node == null)) {
						Assert.isLegal(NodeTypeRegistry.instance().exists(fromNode.type),
											"Unknown node type: " + fromNode.type);
						nodeType = NodeTypeRegistry.instance().getNodeType(fromNode.type);
						fromNode.node = nodeType.create(dataflowManager, fromNode.attrs);
					}

					// create ports and connect them
					if (!channel.created) {
						dataflowManager.connect(fromNode.createPort(channel.out), toNode.createPort(channel.in));
						channel.created = true;
	
						// continue with the corresponding input port's channel
						for (PortWrapper inPort : fromNode.inPorts)
							pendingChannels.offer(inPort.channel);
					}
						
//					PortWrapper inPort = fromNode.getIntputPortFor(channel.out);
//					channel = inPort != null ? inPort.channel : null;
				}
			}
			return dataflowManager;
		} 
		catch (RuntimeException e) {
		    // clean up before re-throwing exception
			if (dataflowManager != null)
				dataflowManager.delete();
			throw e;
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
		ResultItem vector = resultfileManager.getVector(id);
		Assert.isTrue(!vector.getComputed(), "Tried to read a computed vector from file.");
		
		if (getOutputPort(id) != null) {
			removeSource(id);
		}
		// create new port
		SourceNode node = getOrCreateSourceNode(id);
		node.addPort(id);
		return node;
	}
	
	private TeeNode addComputeNode(long id, ProcessingOp operation) {
		PortWrapper port = getOutputPort(id);
		TeeNode teeNode = new TeeNode();
		connect(port, teeNode.inPort);
		addFilterNode(teeNode.outPort2, operation);
		return teeNode;
	}
	
	private FilterNode addFilterNode(long id, ProcessingOp operation) {
		return addFilterNode(getOutputPort(id), operation);
	}

	private FilterNode addFilterNode(PortWrapper port, ProcessingOp operation) {
		FilterNode filterNode = new FilterNode(operation);
		connect(port, filterNode.inPort);
		return filterNode;
	}

	private SinkNode addSinkNode(long id) {
		SinkNode sinkNode = new SinkNode();
		PortWrapper port = getOutputPort(id);
		connect(port, sinkNode.inPort);
		return sinkNode;
	}
	
	private XYPlotNode addXYPlotNode(long xid, IDList yids) {
		XYPlotNode xyplotNode = new XYPlotNode();
		PortWrapper port = getOutputPort(xid);
		connect(port, xyplotNode.xPort);
		for (int i = 0; i < yids.size(); ++i) {
			port = getOutputPort(yids.get(i));
			connect(port, xyplotNode.addPort());
		}
		return xyplotNode;
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
		return DatasetManager.select(source, filters, resultfileManager, ResultType.VECTOR_LITERAL);
	}

	private IDList select(IDList source, AddDiscardOp op) {
		return DatasetManager.select(source, op, resultfileManager);
	}
}

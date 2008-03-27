package org.omnetpp.scave.model2;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
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
import org.omnetpp.scave.engineext.IndexFile;
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
	
	private static final boolean debug = false;

	/*
	 * Inner representation of the dataflow graph.
	 */
	private List<SinkNode> sinkNodes = new ArrayList<SinkNode>();
	private List<ArrayBuilderNode> arrayBuilderNodes = new ArrayList<ArrayBuilderNode>();
	private Map<ResultFile,ReaderNode> fileToReaderNodeMap = new HashMap<ResultFile,ReaderNode>();
	private Map<Long,PortWrapper> idToOutputPortMap = new LinkedHashMap<Long,PortWrapper>();
	private ResultFileManager resultfileManager;
	private List<IStatus> warnings = new ArrayList<IStatus>();

	abstract class NodeWrapper
	{
		List<PortWrapper> inPorts = new ArrayList<PortWrapper>();
		List<PortWrapper> outPorts = new ArrayList<PortWrapper>();
		boolean created;
		
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
		
		public void disconnected(PortWrapper out) {
		}
		
		public PortWrapper getIntputPortFor(PortWrapper outputPort)
		{
			Assert.isTrue(inPorts.size() <= 1);
			return inPorts.size() == 0 ? null : inPorts.get(0);
		}

		/**
		 * Creates the nodes in the dataflowManager. 
		 */
		public abstract void createNodes(DataflowManager dataflowManager);
		
		/**
		 * Creates the port in the dataflowManager.
		 * <p>
		 * Note: do not use the returned object after a new port
		 *       added to the node. It contains a pointer which
		 *       becomes invalid!
		 */
		public abstract Port createPort(PortWrapper port);
	}
	
	class SimpleNode extends NodeWrapper
	{
		Node node;
		String type;
		StringMap attrs;
		
		public SimpleNode(String type, StringMap attrs) {
			this.type = type;
			this.attrs = attrs;
		}
		
		public void createNodes(DataflowManager manager) {
			Assert.isLegal(NodeTypeRegistry.instance().exists(type),
					"Unknown node type: " + type);
			NodeType nodeType = NodeTypeRegistry.instance().getNodeType(type);
			node = nodeType.create(manager, attrs);
		}
		
		public Port createPort(PortWrapper port) {
			return node.nodeType().getPort(node, port.name);
		}
	}
	
	abstract class CompoundNode extends NodeWrapper
	{
	}
	
	class ReaderNode extends SimpleNode
	{
		ResultFile resultFile;
		
		public ReaderNode(ResultFile resultFile) {
			super("indexedvectorfilereader", null);
			this.resultFile = resultFile;
			this.attrs = new StringMap();
			this.attrs.set("filename", resultFile.getFileSystemFilePath());
			fileToReaderNodeMap.put(resultFile, this);
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
		
		public void disconnected(PortWrapper out) {
			if (idToOutputPortMap.get(out.id) == out)
				idToOutputPortMap.remove(out.id);
			outPorts.remove(out);
			out.owner = null;
			if (outPorts.isEmpty())
				fileToReaderNodeMap.remove(resultFile);
		}
		
		@Override
		public Port createPort(PortWrapper port) {
			IndexedVectorFileReaderNode reader = IndexedVectorFileReaderNode.cast(node);
			return reader.addVector(resultfileManager.getVector(port.id));
		}
		
		public String toString() {
			return String.format("Reader[%s]", resultFile.getFileName());
		}
	}
	
	class ApplyNode extends SimpleNode
	{
		PortWrapper inPort;
		PortWrapper outPort;
		ProcessingOp operation;
		int outVectorId;
		
		public ApplyNode(ProcessingOp operation, int outVectorId) {
			super(operation.getOperation(), EMPTY_ATTRS);
			attrs = new StringMap();
			for (Param param : operation.getParams())
				attrs.set(param.getName(), param.getValue());
			this.operation = operation;
			this.outVectorId = outVectorId;
			addInputPort(inPort = new PortWrapper(-1, "in"));
			addOutputPort(outPort = new PortWrapper(-1, "out"));
			
		}

		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			//System.out.format("Connecting %x%n", inPort.id);
			outPort.id = DatasetManager.ensureComputedResultItem(operation, inPort.id, outVectorId, resultfileManager, warnings); 
			idToOutputPortMap.remove(inPort.id);
			idToOutputPortMap.put(outPort.id, outPort);
		}

		@Override
		public void disconnected(PortWrapper out) {
			Assert.isTrue(out == outPort);
			if (idToOutputPortMap.get(out.id) == out) {
				idToOutputPortMap.remove(out.id);
			}
			if (inPort.channel != null) {
				idToOutputPortMap.put(inPort.id, inPort.channel.out);
				disconnect(inPort.channel);
			}
		}
		
		public String toString() {
			return String.format("Apply[%s]", operation.getOperation());
		}
	}
	
	class ComputeNode extends CompoundNode
	{
		PortWrapper inPort;
		PortWrapper outPort1; // computed
		PortWrapper outPort2; // orig
		ProcessingOp operation;
		int outVectorId;
		Node teeNode, filterNode;
		boolean disconnected1, disconnected2;
		
		public ComputeNode(Compute operation, int outVectorId) {
			this.operation = operation;
			this.outVectorId = outVectorId;
			addInputPort(inPort = new PortWrapper(-1, "in"));
			addOutputPort(outPort1 = new PortWrapper(-1, "out1"));
			addOutputPort(outPort2 = new PortWrapper(-1, "out2"));
		}

		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			//System.out.format("Connecting %x%n", inPort.id);
			outPort1.id = DatasetManager.ensureComputedResultItem(operation, inPort.id, outVectorId, resultfileManager, warnings); 
			outPort2.id = inPort.id;
			idToOutputPortMap.put(outPort1.id, outPort1);
			idToOutputPortMap.put(outPort2.id, outPort2);
		}

		@Override
		public void disconnected(PortWrapper out) {
			if (out == outPort1) {
				disconnected1 = true;
				idToOutputPortMap.remove(outPort1.id);
				idToOutputPortMap.put(inPort.id, inPort.channel.out);
				if (outPort2.channel != null)
					connect(inPort.channel.out, outPort2.channel.in);
			}
			else if (out == outPort2) {
				disconnected2 = true;
				idToOutputPortMap.remove(outPort2.id);
			}
			if (disconnected1 && disconnected2) {
				idToOutputPortMap.put(inPort.id, inPort.channel.out);
				disconnect(inPort.channel);
			}
		}

		public void createNodes(DataflowManager manager) {
			Assert.isLegal(NodeTypeRegistry.instance().exists(operation.getOperation()),
					"Unknown node type: " + operation.getOperation());
			
			NodeTypeRegistry registry = NodeTypeRegistry.instance();
			StringMap attrs = new StringMap();
			for (Param param : operation.getParams())
				attrs.set(param.getName(), param.getValue());
			filterNode = registry.getNodeType(operation.getOperation()).create(manager, attrs);
			
			if (outPort2.channel != null) {
				teeNode = registry.getNodeType("tee").create(manager, EMPTY_ATTRS);
				Port out = teeNode.nodeType().getPort(teeNode, "next-out");
				Port in = filterNode.nodeType().getPort(filterNode, "in");
				manager.connect(out, in);
			}
		}

		@Override
		public Port createPort(PortWrapper port) {
			if (port == inPort) {
				if (teeNode != null)
					return teeNode.nodeType().getPort(teeNode, "in");
				else
					return filterNode.nodeType().getPort(filterNode, "in");
			}
			else if (port == outPort1)
				return filterNode.nodeType().getPort(filterNode, "out");
			else if (port == outPort2)
				return teeNode.nodeType().getPort(teeNode, "next-out");
			else
				Assert.isLegal(false);
			return null;
		}
		
		public String toString() {
			return String.format("Compute[%s]", operation.getOperation());
		}
	}
	
	
	class XYPlotNode extends SimpleNode
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
		
		private void removePorts(int index) {
			PortWrapper out = outPorts.remove(index);
			PortWrapper in = inPorts.remove(index + 1);
			out.owner = in.owner = null;
			for (int i = index; i < outPorts.size(); ++i) {
				outPorts.get(i).name = ("out"+i);
				inPorts.get(i+1).name = ("y"+i);
			}
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
		public void disconnected(PortWrapper out) {
			int index = outPorts.indexOf(out);
			Assert.isLegal(index >= 0);
			PortWrapper inPort = inPorts.get(index + 1);
			removePorts(index);
			
			if (idToOutputPortMap.get(out.id) == out) {
				idToOutputPortMap.remove(out.id);
			}
			if (inPort.channel != null) {
				idToOutputPortMap.put(inPort.id, inPort.channel.out);
				disconnect(inPort.channel);
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
	
	class SinkNode extends SimpleNode
	{
		public SinkNode(String type, StringMap attrs) {
			super(type, attrs);
			sinkNodes.add(this);
		}
	}
	
	class ArrayBuilderNode extends SinkNode
	{
		PortWrapper inPort;
		
		public ArrayBuilderNode() {
			super("arraybuilder", EMPTY_ATTRS);
			addInputPort(inPort = new PortWrapper(-1, "in"));
			arrayBuilderNodes.add(this);
		}
		
		@Override
		public void connected(PortWrapper in) {
			Assert.isTrue(in == inPort);
			idToOutputPortMap.remove(in.id);
		}
		
		public String toString() {
			return String.format("ArrayBuilder[%s]", inPort.id);
		}
	}
	
	class VectorFileWriterNode extends SinkNode
	{
		public VectorFileWriterNode(String fileName) {
			super("indexedvectorfilewriter", new StringMap());
			attrs.set("filename", fileName);
			attrs.set("indexfilename", IndexFile.getIndexFileName(fileName));
			attrs.set("blocksize", "32768");
			attrs.set("fileheader", "# computed vector file");
		}
		
		public PortWrapper addInputPort(long id, int vectorId, String module, String vector, String columns) {
			String name = String.format("%d %s %s %s",
					vectorId,
					StringUtils.quoteStringIfNeeded(module),
					StringUtils.quoteStringIfNeeded(vector),
					columns);
			PortWrapper inPort = new PortWrapper(id, name); 
			addInputPort(inPort);
			return inPort;
		}
		
		public String toString() {
			return String.format("Writer[%s]", attrs.get("filename"));
		}
	}
	
	static class PortWrapper {
		long id;
		String name;
		NodeWrapper owner;
		ChannelWrapper channel;
		
		public PortWrapper(long id, String name) {
			this.id = id;
			this.name = name;
		}
		
		public String toString() {
			return String.format("%s:%s(%s)", owner, name, id);
		}
	}
	
	static class ChannelWrapper {
		PortWrapper out;
		PortWrapper in;
		boolean created;
		
		public ChannelWrapper(PortWrapper out, PortWrapper in) {
			this.out = out;
			this.in = in;
			out.channel = this;
			in.channel = this;
			
			in.id = out.id;
		}
	}

	public DataflowNetworkBuilder(ResultFileManager resultfileManager) {
		this.resultfileManager = resultfileManager;
	}

	/**
	 * Builds a dataflow network for reading the data of the given vectors.
	 */
	public DataflowManager build(IDList idlist) {
		long start = System.currentTimeMillis();
		try {
			warnings.clear();
			for (int i = 0; i < (int)idlist.size(); ++i) {
				addReaderNode(idlist.get(i));
			}
			addArrayBuilderNodes();
			return buildNetwork();
		}
		finally {
			System.out.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
		}
	}
	
	/**
	 * Builds a dataflow network for reading the data of {@code dataset} at 
	 * the given dataset item.
	 */
	public DataflowManager build(Dataset dataset, DatasetItem target, boolean createDataflowManager) {
		long start = System.currentTimeMillis();
		try {
			warnings.clear();
			buildInternal(dataset, target);
			addArrayBuilderNodes();
			return createDataflowManager ? buildNetwork() : null;
		}
		finally {
			System.out.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
		}
	}
	
	/**
	 * Builds a dataflow network for writing the computed vectors into an indexed
	 * vector file.
	 */
	public DataflowManager build(Dataset dataset, DatasetItem target, String fileName) {
		long start = System.currentTimeMillis();
		try {
			warnings.clear();
			buildInternal(dataset, target);
			addVectorFileWriterNode(fileName);
			return buildNetwork();
		}
		finally {
			System.out.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
		}
	}
	
	public IDList getDisplayedIDs() {
		IDList idlist = new IDList();
		for (ArrayBuilderNode node : arrayBuilderNodes) {
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
		for (ArrayBuilderNode node : arrayBuilderNodes)
			result.add(node.node);
		return result;
	}
	
	public List<IStatus> getWarnings() {
		return warnings;
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
					addReaderNodes(select(null, add));
				return this;
			}

			/**
			 * Removes "vectorfilereader" ports (and filter nodes connected to it)
			 * of the ids selected by the discard operation.
			 */
			public Object caseDiscard(Discard discard) {
				if (discard.getType()==ResultType.VECTOR_LITERAL)
					removeOutputPorts(select(getIDs(), discard));
				return this;
			}

			/**
			 * Adds a filter node for each port selected by the apply operation.
			 */
			public Object caseApply(Apply apply) {
				if (apply.getOperation() != null) {
					IDList idlist = select(getIDs(), apply.getFilters());
					for (int i = 0; i < idlist.size(); ++i) {
						addApplyNode(idlist.get(i), apply, i);
					}
				}
				return this;
			}

			public Object caseCompute(Compute compute) {
				if (compute.getOperation() != null) {
					IDList idlist = select(getIDs(), compute.getFilters());
					for (int i = 0; i < idlist.size(); ++i) {
						addComputeNode(idlist.get(i), compute, i);
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
					removeOutputPorts(idlist);
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
					removeOutputPorts(idlist);

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
					
					if (debug) {
						System.out.format("Network after %s%n", object);
						dumpNetwork();
					}
				}
				return result;
			}

			private IDList select(IDList source, List<SelectDeselectOp> filters) {
				return DatasetManager.select(source, filters, resultfileManager, ResultType.VECTOR_LITERAL);
			}

			private IDList select(IDList source, AddDiscardOp op) {
				return DatasetManager.select(source, op, resultfileManager);
			}
		};

		modelSwitch.doSwitch(dataset);
	}
	
	private IDList getIDs() {
		Set<Long> keys = idToOutputPortMap.keySet();
		return IDList.fromArray(keys.toArray(new Long[keys.size()]));
	}
	
	/**
	 * Builds the network in the DataflowManager.
	 */
	private DataflowManager buildNetwork() {
		DataflowManager dataflowManager = new DataflowManager();
		
		try {

			for (SinkNode sinkNode : sinkNodes) {
				
				// create the arraybuilder node
				if (debug) System.out.format("Creating: %s%n", sinkNode);
				sinkNode.createNodes(dataflowManager);
				
				// follow the channels backwards
				// until a reader node or a previously created node reached
				Queue<ChannelWrapper> pendingChannels = new LinkedList<ChannelWrapper>();
				for (PortWrapper inPort : sinkNode.inPorts)
					pendingChannels.offer(inPort.channel);
				
				while (!pendingChannels.isEmpty()) {
					ChannelWrapper channel = pendingChannels.poll();
					
					NodeWrapper fromNode = channel.out.owner;
					NodeWrapper toNode = channel.in.owner;
					
					// create fromNode if needed
					if (!fromNode.created) {
						if (debug) System.out.format("Creating: %s%n", fromNode);
						fromNode.createNodes(dataflowManager);
						fromNode.created = true;
					}

					// create ports and connect them
					if (!channel.created) {
						if (debug) System.out.format("Connecting: %s and %s%n", fromNode, toNode);
						dataflowManager.connect(fromNode.createPort(channel.out), toNode.createPort(channel.in));
						channel.created = true;
	
						// continue with the corresponding input port's channel
						for (PortWrapper inPort : fromNode.inPorts)
							pendingChannels.offer(inPort.channel);
					}
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
	private ReaderNode getOrCreateReaderNode(long id) {
		ResultFile file = resultfileManager.getVector(id).getFileRun().getFile();
		ReaderNode readerNode = fileToReaderNodeMap.get(file);
		if (readerNode == null)
			readerNode = new ReaderNode(file);
		return readerNode;
	}

	private void connect(PortWrapper outPort, PortWrapper inPort) {
		if (debug) System.out.format("Connecting %s --> %s%n", outPort, inPort);
		new ChannelWrapper(outPort, inPort);
		inPort.owner.connected(inPort);
	}
	
	private void disconnect(ChannelWrapper channel) {
		if (debug) System.out.format("Disconnecting %s --> %s%n", channel.out, channel.in);
		PortWrapper out = channel.out;
		channel.in.channel = channel.out.channel = null;
		channel.in = channel.out = null;
		out.owner.disconnected(out);
	}

	private PortWrapper getOutputPort(long id) {
		return idToOutputPortMap.get(id);
	}

	private void addReaderNodes(IDList idlist) {
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addReaderNode(id);
		}
	}

	private ReaderNode addReaderNode(long id) {
		ResultItem vector = resultfileManager.getVector(id);
		Assert.isTrue(!vector.isComputed(), "Tried to read a computed vector from file.");
		
		if (getOutputPort(id) != null) {
			removeOutputPort(id);
		}
		// create new port
		ReaderNode node = getOrCreateReaderNode(id);
		node.addPort(id);
		return node;
	}
	
	private ComputeNode addComputeNode(long id, Compute operation, int outVectorId) {
		ComputeNode computeNode = new ComputeNode(operation, outVectorId);
		connect(getOutputPort(id), computeNode.inPort);
		return computeNode;
	}

	private ApplyNode addApplyNode(long id, ProcessingOp operation, int outVectorId) {
		ApplyNode applyNode = new ApplyNode(operation, outVectorId);
		connect(getOutputPort(id), applyNode.inPort);
		return applyNode;
	}

	/**
	 * Adds an arraybuilder node for each open ports.
	 */
	private void addArrayBuilderNodes() {
		IDList idlist = getIDs();
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			addArrayBuilderNode(id);
		}
	}
	
	private ArrayBuilderNode addArrayBuilderNode(long id) {
		ArrayBuilderNode sinkNode = new ArrayBuilderNode();
		PortWrapper port = getOutputPort(id);
		connect(port, sinkNode.inPort);
		return sinkNode;
	}
	
	private VectorFileWriterNode addVectorFileWriterNode(String fileName) {
		IDList idlist = getIDs();
		if (idlist.isEmpty())
			return null;
		VectorFileWriterNode writer = null;
		for (int i = 0; i < idlist.size(); ++i) {
			long id = idlist.get(i);
			VectorResult vector = resultfileManager.getVector(id);
			if (vector.isComputed()) {
				if (writer == null)
					writer =  new VectorFileWriterNode(fileName);
				PortWrapper inPort = writer.addInputPort(id, vector.getVectorId(), vector.getModuleName(), vector.getName(), vector.getColumns());
				PortWrapper outPort = getOutputPort(id);
				connect(outPort, inPort);
			}
			else {
				removeOutputPort(id);
			}
		}
		return writer;
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

	private void removeOutputPorts(IDList idlist) {
		for (int i = 0; i < idlist.size(); ++i)
			removeOutputPort(idlist.get(i));
	}
	
	private void removeOutputPort(long id) {
		PortWrapper outPort = idToOutputPortMap.get(id);
		if (outPort != null) {
			if (debug) System.out.format("Disconnecting %s%n", outPort);
			outPort.owner.disconnected(outPort);
		}
	}
	
	private void dumpNetwork() {
		for (ReaderNode reader : fileToReaderNodeMap.values()) {
			dumpNode(reader, "");
		}
	}
	
	private void dumpNode(NodeWrapper node, String indent) {
		System.out.format("%s%s%n", indent, node);
		for (PortWrapper port : node.outPorts) {
			if (port.channel != null) {
				dumpNode(port.channel.in.owner, indent+"  ");
			}
			else
				System.out.format("%s<null>%n", indent+"  ");
		}
	}
}

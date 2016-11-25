/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

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
import org.omnetpp.common.Debug;
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
import org.omnetpp.scave.model.Apply;
import org.omnetpp.scave.model.Chart;
import org.omnetpp.scave.model.Compute;
import org.omnetpp.scave.model.Dataset;
import org.omnetpp.scave.model.DatasetItem;
import org.omnetpp.scave.model.Discard;
import org.omnetpp.scave.model.Param;
import org.omnetpp.scave.model.ProcessingOp;
import org.omnetpp.scave.model.ResultType;
import org.omnetpp.scave.model.ScatterChart;

/**
 * Builds a dataflow network for a dataset.
 *
 * @author tomi
 */
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

        public void removeInputPort(int index) {
            Assert.isLegal(0 <= index && index < inPorts.size());
            removeInputPort(inPorts.get(index));
        }

        public void removeOutputPort(int index) {
            Assert.isLegal(0 <= index && index < outPorts.size());
            removeInputPort(outPorts.get(index));
        }

        public void removeInputPort(PortWrapper port) {
            Assert.isTrue(port.owner == this);
            boolean removed = inPorts.remove(port);
            Assert.isTrue(removed);
            port.owner = null;
        }

        public void removeOutputPort(PortWrapper port) {
            Assert.isTrue(port.owner == this);
            boolean removed = outPorts.remove(port);
            Assert.isTrue(removed);
            port.owner = null;
        }

        /**
         * Notifies the node that one of its input port got connected.
         */
        public void connected(PortWrapper in) {
        }

        public void disconnected(PortWrapper out) {
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

        protected boolean allInputPortsConnected() {
            for (PortWrapper inPort : inPorts)
                if (inPort.id == -1)
                    return false;
            return true;
        }

        protected IDList getInputIDs() {
            IDList idlist = new IDList();
            for (PortWrapper inPort : inPorts) {
                Assert.isTrue(inPort.id != -1);
                idlist.add(inPort.id);
            }
            return idlist;
        }
    }

    class SimpleNode extends NodeWrapper
    {
        Node node;
        String type;
        StringMap attrs;

        public SimpleNode(String type, StringMap attrs) {
            Assert.isNotNull(type);
            this.type = type;
            this.attrs = attrs;
        }

        public void createNodes(DataflowManager manager) {
            Assert.isLegal(NodeTypeRegistry.getInstance().exists(type),
                    "Unknown node type: " + type);
            NodeType nodeType = NodeTypeRegistry.getInstance().getNodeType(type);
            node = nodeType.create(manager, attrs);
        }

        public Port createPort(PortWrapper port) {
            return node.getNodeType().getPort(node, port.name);
        }
    }

    abstract class CompoundNode extends NodeWrapper
    {
    }

    class ReaderNode extends SimpleNode
    {
        ResultFile resultFile;

        public ReaderNode(ResultFile resultFile) {
            super("vectorreaderbyfiletype", null);
            this.resultFile = resultFile;
            this.attrs = new StringMap();
            this.attrs.set("filename", resultFile.getFileSystemFilePath());
            this.attrs.set("allowindexing", "true");
            fileToReaderNodeMap.put(resultFile, this);
        }

        public PortWrapper addPort(long id) {
            int vectorId = resultfileManager.getVector(id).getVectorId();
            PortWrapper port = new PortWrapper(""+vectorId);
            port.id = id;
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
            removeOutputPort(out);
            if (outPorts.isEmpty())
                fileToReaderNodeMap.remove(resultFile);
        }

        @Override
        public Port createPort(PortWrapper port) {
            return node.getNodeType().getPort(node, port.name);
        }

        public String toString() {
            return String.format("Reader[%s]", resultFile.getFileName());
        }
    }

    class ApplyNode extends SimpleNode
    {
        ProcessingOp operation;
        int outVectorId;

        public ApplyNode(ProcessingOp operation, int outVectorId) {
            super(operation.getOperation(), EMPTY_ATTRS);
            attrs = new StringMap();
            for (Param param : operation.getParams())
                attrs.set(param.getName(), param.getValue());
            this.operation = operation;
            this.outVectorId = outVectorId;
            addOutputPort(new PortWrapper("out"));

        }

        public PortWrapper nextInPort() {
            PortWrapper inPort;
            if (ScaveModelUtil.isFilterOperation(operation)) {
                Assert.isTrue(inPorts.isEmpty());
                inPort = new PortWrapper("in");
            }
            else {
                inPort = new PortWrapper("next-in");
            }
            addInputPort(inPort);
            return inPort;
        }

        public PortWrapper getOutputPort() {
            return outPorts.get(0);
        }

        @Override
        public void connected(PortWrapper in) {
            Assert.isTrue(inPorts.contains(in));
            //Debug.format("Connecting %x%n", inPort.id);
            idToOutputPortMap.remove(in.id);
            PortWrapper outPort = getOutputPort();
            if (outPort.id == -1 && allInputPortsConnected()) {
                IDList inputs = getInputIDs();
                outPort.id = DatasetManager.ensureComputedResultItem(operation, inputs, outVectorId, resultfileManager, warnings);
                idToOutputPortMap.put(outPort.id, outPort);
            }
        }

        @Override
        public void disconnected(PortWrapper out) {
            Assert.isTrue(out == getOutputPort());
            for (PortWrapper inPort : inPorts) {
                if (inPort.channel != null) {
                    idToOutputPortMap.put(inPort.id, inPort.channel.out);
                    disconnect(inPort.channel);
                }
            }
        }

        public String toString() {
            return String.format("Apply[%s,%x]", operation.getOperation(), hashCode());
        }
    }

    class ComputeNode extends CompoundNode
    {
        ProcessingOp operation;
        int computedVectorId;
        Node computeNode;
        List<Node> teeNodes = new ArrayList<Node>();

        public ComputeNode(Compute operation, int computedVectorId) {
            Assert.isLegal(operation != null && operation.getOperation() != null);
            this.operation = operation;
            this.computedVectorId = computedVectorId;
            addOutputPort(new PortWrapper("out1"));
        }

        public PortWrapper nextInPort() {
            PortWrapper inPort;
            if (ScaveModelUtil.isFilterOperation(operation)) {
                Assert.isTrue(inPorts.isEmpty());
                inPort = new PortWrapper("in");
            }
            else {
                inPort = new PortWrapper("next-in");
            }
            addInputPort(inPort);
            addOutputPort(new PortWrapper("out"+(outPorts.size()+1)));
            return inPort;
        }

        public PortWrapper getComputedOutputPort() {
            return outPorts.get(0);
        }

        public PortWrapper getOutPortFor(PortWrapper inPort) {
            int index = inPorts.indexOf(inPort);
            Assert.isTrue(0 <= index && index + 1 < outPorts.size());
            return outPorts.get(index + 1);
        }

        @Override
        public void connected(PortWrapper in) {
            Assert.isTrue(inPorts.contains(in));
            //Debug.format("Connecting %x%n", in.id);
            PortWrapper computedOutPort = getComputedOutputPort();
            if (computedOutPort.id == -1 && allInputPortsConnected()) {
                IDList inputs = getInputIDs();
                computedOutPort.id = DatasetManager.ensureComputedResultItem(operation, inputs, computedVectorId, resultfileManager, warnings);
                idToOutputPortMap.put(computedOutPort.id, computedOutPort);
            }

            PortWrapper outPort = getOutPortFor(in);
            outPort.id = in.id;
            idToOutputPortMap.put(outPort.id, outPort);
        }

        @Override
        public void disconnected(PortWrapper out) {
            int index;
            PortWrapper computedOutPort = getComputedOutputPort();
            if (out == computedOutPort) {
                for (int i = 0; i < inPorts.size(); ++i) {
                    PortWrapper inPort = inPorts.get(i);
                    PortWrapper outPort = outPorts.get(i + 1);
                    idToOutputPortMap.put(inPort.id, inPort.channel.out);
                    if (outPort.id == -1) {
                        disconnect(inPort.channel);
                    }
                    else if (outPort.channel != null) {
                        connect(inPort.channel.out, outPort.channel.in);
                    }
                }
            }
            else if ((index = outPorts.indexOf(out)) >= 1) {
                if (computedOutPort.id == -1) {
                    PortWrapper inPort = inPorts.get(index - 1);
                    idToOutputPortMap.put(inPort.id, inPort.channel.out);
                    disconnect(inPort.channel);
                }
            }
        }

        public void createNodes(DataflowManager manager) {
            Assert.isLegal(NodeTypeRegistry.getInstance().exists(operation.getOperation()),
                    "Unknown node type: " + operation.getOperation());

            NodeTypeRegistry registry = NodeTypeRegistry.getInstance();
            StringMap attrs = new StringMap();
            for (Param param : operation.getParams())
                attrs.set(param.getName(), param.getValue());
            computeNode = registry.getNodeType(operation.getOperation()).create(manager, attrs);

            for (int i = 0; i < inPorts.size(); ++i) {
                PortWrapper inPort = inPorts.get(i);
                PortWrapper outPort = getOutPortFor(inPort);
                if (outPort.channel != null) {
                    Node teeNode = registry.getNodeType("tee").create(manager, EMPTY_ATTRS);
                    teeNodes.add(teeNode);
                    Port out = teeNode.getNodeType().getPort(teeNode, "next-out");
                    Port in = computeNode.getNodeType().getPort(computeNode, inPort.name);
                    manager.connect(out, in);
                }
                else
                    teeNodes.add(null);
            }
        }

        @Override
        public Port createPort(PortWrapper port) {
            int index;
            if ((index = inPorts.indexOf(port)) >= 0) {
                Assert.isTrue(index < teeNodes.size());
                Node teeNode = teeNodes.get(index);
                if (teeNode != null)
                    return teeNode.getNodeType().getPort(teeNode, "in");
                else
                    return computeNode.getNodeType().getPort(computeNode, port.name);
            }
            else if (port == getComputedOutputPort())
                return computeNode.getNodeType().getPort(computeNode, "out");
            else if ((index = outPorts.indexOf(port)) >= 0) {
                Node teeNode = teeNodes.get(index - 1);
                return teeNode.getNodeType().getPort(teeNode, "next-out");
            }
            else
                Assert.isLegal(false);
            return null;
        }

        public String toString() {
            return String.format("Compute[%s,%x]", operation.getOperation(), hashCode());
        }
    }


    class XYPlotNode extends SimpleNode
    {
        PortWrapper xPort;

        public XYPlotNode() {
            super("xyplot", EMPTY_ATTRS);
            addInputPort(xPort = new PortWrapper("x"));
        }

        public PortWrapper addPort() {
            int index = outPorts.size();
            PortWrapper yPort = new PortWrapper("y"+index);
            PortWrapper outPort = new PortWrapper("out"+index);
            addInputPort(yPort);
            addOutputPort(outPort);
            return yPort;
        }

        private void removePorts(int index) {
            removeOutputPort(index);
            removeInputPort(index + 1);
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
            addInputPort(inPort = new PortWrapper("in"));
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

        public PortWrapper addInputPort(int vectorId, String module, String vector, String columns) {
            String name = String.format("%d %s %s %s",
                    vectorId,
                    StringUtils.quoteStringIfNeeded(module),
                    StringUtils.quoteStringIfNeeded(vector),
                    columns);
            PortWrapper inPort = new PortWrapper(name);
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

        public PortWrapper(String name) {
            this.id = -1;
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
        resultfileManager.checkReadLock();
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
            if (debug)
                Debug.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
        }
    }

    /**
     * Builds a dataflow network for reading the data of {@code dataset} at
     * the given dataset item.
     */
    public DataflowManager build(Dataset dataset, DatasetItem target, boolean createDataflowManager) {
        resultfileManager.checkReadLock();
        long start = System.currentTimeMillis();
        try {
            warnings.clear();
            buildInternal(dataset, target);
            addArrayBuilderNodes();
            return createDataflowManager ? buildNetwork() : null;
        }
        finally {
            if (debug)
                Debug.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
        }
    }

    /**
     * Builds a dataflow network for writing the computed vectors into an indexed
     * vector file.
     */
    public DataflowManager build(Dataset dataset, DatasetItem target, String fileName) {
        resultfileManager.checkReadLock();
        long start = System.currentTimeMillis();
        try {
            warnings.clear();
            buildInternal(dataset, target);
            addVectorFileWriterNode(fileName);
            return buildNetwork();
        }
        finally {
            if (debug)
                Debug.format("build dataflow network: %dms%n", System.currentTimeMillis() - start);
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

        new NetworkBuilder(target).doSwitch(dataset);

        if (debug) {
            Debug.println("Dataflow network");
            dumpNetwork();
        }
    }

    private class NetworkBuilder extends ProcessDatasetSwitch {

        public NetworkBuilder(EObject target) {
            super(target, false, resultfileManager);
        }

        /**
         * Adds "vectorfilereader" nodes for ids selected by the add operation.
         */
        public Object caseAdd(Add add) {
            if (add.getType()==ResultType.VECTOR_LITERAL) {
                addReaderNodes(select(null, add));
            }
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
                IDList idlist = select(getIDs(), apply.getFilters(), ResultType.VECTOR_LITERAL);
                List<IDList> groups = DatasetManager.groupIDs(apply, idlist, manager);
                addApplyNodes(groups, apply);
            }
            return this;
        }

        public Object caseCompute(Compute compute) {
            if (compute.getOperation() != null) {
                IDList idlist = select(getIDs(), compute.getFilters(), ResultType.VECTOR_LITERAL);
                List<IDList> groups = DatasetManager.groupIDs(compute, idlist, manager);
                addComputeNodes(groups, compute);
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
                idlist.subtract(select(idlist, chart.getFilters(), ResultType.VECTOR_LITERAL));
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
                IDList displayedIds = select(idlist, chart.getFilters(), ResultType.VECTOR_LITERAL);
                idlist.subtract(displayedIds);
                removeOutputPorts(idlist);

                // select x data
                String xDataPattern = chart.getXDataPattern();
                if (!StringUtils.isEmpty(xDataPattern)) {

                    IDList xData = manager.filterIDList(displayedIds, xDataPattern);
                    for (int i = 0; i < xData.size(); ++i) {
                        long id = xData.get(i);
                        ResultItem item = manager.getItem(id);
                        FileRunList fileruns = new FileRunList();
                        fileruns.add(item.getFileRun());
                        IDList yData = manager.filterIDList(displayedIds, fileruns, "", "");
                        yData.subtract(id);

                        addXYPlotNode(id, yData);
                    }
                }
            }
            return this;
        }

        public Object defaultCase(EObject object) {
            return this; // do nothing
        }

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
                if (debug) Debug.format("Creating: %s%n", sinkNode);
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
                        if (debug) Debug.format("Creating: %s%n", fromNode);
                        fromNode.createNodes(dataflowManager);
                        fromNode.created = true;
                    }

                    // create ports and connect them
                    if (!channel.created) {
                        if (debug) Debug.format("Connecting: %s and %s%n", fromNode, toNode);
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
        if (debug) Debug.format("Connecting %s --> %s%n", outPort, inPort);
        Assert.isTrue(outPort.id != -1, "Output port id is not set when connecting.");
        new ChannelWrapper(outPort, inPort);
        inPort.owner.connected(inPort);
    }

    private void disconnect(ChannelWrapper channel) {
        if (debug) Debug.format("Disconnecting %s --> %s%n", channel.out, channel.in);
        PortWrapper out = channel.out;
        channel.in.channel = channel.out.channel = null;
        channel.in = channel.out = null;
        disconnect(out);
    }

    private void disconnect(PortWrapper outPort) {
        if (idToOutputPortMap.get(outPort.id) == outPort) {
            idToOutputPortMap.remove(outPort.id);
        }
        outPort.id = -1;
        outPort.owner.disconnected(outPort);
    }

    private PortWrapper getOutputPort(long id) {
        PortWrapper outPort = idToOutputPortMap.get(id);
        Assert.isTrue(outPort == null || outPort.id == id);
        return outPort;
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

    private void addComputeNodes(List<IDList> groups, Compute operation) {
        for (int i = 0; i < groups.size(); ++i) {
            addComputeNode(groups.get(i), operation, i);
        }
    }

    private ComputeNode addComputeNode(IDList idlist, Compute operation, int outVectorId) {
        ComputeNode computeNode = new ComputeNode(operation, outVectorId);
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            connect(getOutputPort(id), computeNode.nextInPort());
        }
        return computeNode;
    }

    private void addApplyNodes(List<IDList> groups, Apply operation) {
        for (int i = 0; i < groups.size(); ++i) {
            addApplyNode(groups.get(i), operation, i);
        }
    }

    private ApplyNode addApplyNode(IDList idlist, Apply operation, int outVectorId) {
        ApplyNode applyNode = new ApplyNode(operation, outVectorId);
        for (int i = 0; i < idlist.size(); ++i) {
            long id = idlist.get(i);
            connect(getOutputPort(id), applyNode.nextInPort());
        }
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
                PortWrapper inPort = writer.addInputPort(vector.getVectorId(), vector.getModuleName(), vector.getName(), vector.getColumns());
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
            if (debug) Debug.format("Disconnecting %s%n", outPort);
            disconnect(outPort);
        }
    }

    private void dumpNetwork() {
        for (ReaderNode reader : fileToReaderNodeMap.values()) {
            dumpNode(reader, "");
        }
    }

    private void dumpNode(NodeWrapper node, String indent) {
        Debug.format("%s%s%n", indent, node);
        for (PortWrapper port : node.outPorts) {
            if (port.channel != null) {
                dumpNode(port.channel.in.owner, indent+"  ");
            }
            else
                Debug.format("%s<null>%n", indent+"  ");
        }
    }
}

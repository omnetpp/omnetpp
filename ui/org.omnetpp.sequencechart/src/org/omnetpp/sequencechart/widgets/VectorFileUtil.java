package org.omnetpp.sequencechart.widgets;

import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.Port;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorFileReaderNode;
import org.omnetpp.scave.engine.XYArray;


/**
 * Makes it possible for SequenceChart to read data from output vector files, without 
 * depending on the Scave plugin.
 * 
 * @author Andras
 */
public class VectorFileUtil {
	// all functions are static
	private VectorFileUtil() {
	}

	public static XYArray getDataOfVector(ResultFileManager resultfileManager, long id) {
		return getDataOfVector(resultfileManager, id, false);
	}

	/**
	 * Returns data from an output vector given with its ID.
	 */
	public static XYArray getDataOfVector(ResultFileManager resultfileManager, long id, boolean includeEventNumbers) {
		// we'll build a data-flow network consisting of a source and a sink node, and run it. 
		DataflowManager dataflowManager = new DataflowManager();

		// create a reader node for the given vector id
		ResultFile file = resultfileManager.getVector(id).getFileRun().getFile();
		StringMap attrs = new StringMap();
		attrs.set("filename", file.getFileSystemFilePath());
		VectorFileReaderNode readerNode = VectorFileReaderNode.cast(createNode(dataflowManager, "vectorfilereader", attrs));
		Port port = readerNode.addVector(resultfileManager.getVector(id));

		// and an array builder as sink 
		StringMap stringMap = new StringMap();
		if (includeEventNumbers)
			stringMap.set("collecteventnumbers", "true");
		Node arrayBuilderNode = createNode(dataflowManager, "arraybuilder", stringMap);
		dataflowManager.connect(port, arrayBuilderNode.nodeType().getPort(arrayBuilderNode, "in"));

		// run the data-flow network
		long startTime = System.currentTimeMillis();
		try {
			dataflowManager.execute();
		}
		finally {
			dataflowManager.delete(); // close vector file
		}
		System.out.println("data-flow network: "+(System.currentTimeMillis()-startTime)+" ms");

		// and return the array
		XYArray result = arrayBuilderNode.getArray();
		return result;
	}

	private static Node createNode(DataflowManager dataflowManager, String typeName, StringMap attrs) {
		NodeTypeRegistry factory = NodeTypeRegistry.instance();
		if (!factory.exists(typeName))
			throw new IllegalArgumentException("unknown node type: " + typeName);
		NodeType nodeType = factory.getNodeType(typeName);
		return nodeType.create(dataflowManager, attrs);
	}

}

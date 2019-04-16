/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.Port;
import org.omnetpp.scave.engine.ResultFile;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;

public class VectorDataLoader {
    private static final boolean debug = false;

    public static XYVector[] getDataOfVectors(ResultFileManager manager, IDList idlist, IProgressMonitor progressMonitor) {
        long startTime = System.currentTimeMillis();
        DataflowManager dataflowManager = new DataflowManager();
        String readerNodeTypeName = "vectorreaderbyfiletype";
        NodeTypeRegistry registry = NodeTypeRegistry.getInstance();
        NodeType readerNodeType = registry.getNodeType(readerNodeTypeName);
        Assert.isNotNull(readerNodeType, "unknown node type " + readerNodeTypeName);

        ResultFileList filteredVectorFileList = manager.getUniqueFiles(idlist);
        Map<ResultFile, Node> vectorFileReaders = new HashMap<>();
        for (int i = 0; i < (int)filteredVectorFileList.size(); i++) {
            ResultFile resultFile = filteredVectorFileList.get(i);
            StringMap attrs = new StringMap();
            attrs.set("filename", resultFile.getFileSystemFilePath());
            attrs.set("allowindexing", "true");
            Node readerNode = readerNodeType.create(dataflowManager, attrs);
            vectorFileReaders.put(resultFile, readerNode);
        }

        NodeType arrayBuilderNodeType = registry.getNodeType("arraybuilder");
        Assert.isNotNull(arrayBuilderNodeType, "unknown node type arraybuilder");

        List<Node> arrayBuilders = new ArrayList<>();

        for (int i = 0; i < idlist.size(); i++) {
            // create a port for each vector on its reader node
            VectorResult vector = manager.getVector(idlist.get(i));
            String portName = "" + vector.getVectorId();
            Node readerNode = vectorFileReaders.get(vector.getFile());
            Assert.isNotNull(readerNode);
            Port outPort = readerNodeType.getPort(readerNode, portName);

            // create writer node(s) and connect them
            Node arrayBuilderNode = arrayBuilderNodeType.create(dataflowManager, new StringMap());
            dataflowManager.connect(outPort, arrayBuilderNodeType.getPort(arrayBuilderNode, "in"));
            arrayBuilders.add(arrayBuilderNode);
        }

        XYArray[] arrays = null;
        if (dataflowManager != null) {
            if (debug)
                dataflowManager.dump();
            try {
                arrays = executeDataflowNetwork(dataflowManager, arrayBuilders, progressMonitor);
            }
            finally {
                dataflowManager.delete();
                dataflowManager = null;
            }
        }

        // convert native XYArrays to pure Java XYVectors
        XYVector[] vectors = new XYVector[idlist.size()];
        if (arrays != null) { //TODO ??? check how it can be null
            Assert.isTrue(arrays.length == idlist.size());
            for (int i = 0; i < arrays.length; i++)
                vectors[i] = new XYVector(arrays[i]);
        }

        Debug.println("getDataOfVectors() took " + (System.currentTimeMillis()-startTime) + "ms");

        return vectors;
    }

    private static XYArray[] executeDataflowNetwork(DataflowManager manager, List<Node> arrayBuilders, IProgressMonitor monitor) {
        long startTime = System.currentTimeMillis();
        if (arrayBuilders.size() > 0) // XXX DataflowManager crashes when there are no sinks
            manager.execute(monitor);
        if (debug)
            Debug.println("execute dataflow network: "+(System.currentTimeMillis()-startTime)+" ms");

        XYArray[] result = new XYArray[arrayBuilders.size()];
        for (int i = 0; i < result.length; ++i)
            result[i] = arrayBuilders.get(i).getArray();
        return result;
    }
}

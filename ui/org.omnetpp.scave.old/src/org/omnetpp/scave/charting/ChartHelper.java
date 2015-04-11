/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import java.util.HashMap;

import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.xy.XYDataset;

import org.omnetpp.scave.engine.DataflowManager;
import org.omnetpp.scave.engine.FileList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.Node;
import org.omnetpp.scave.engine.NodeType;
import org.omnetpp.scave.engine.NodeTypeRegistry;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.VectorResult;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.plugin.ScavePlugin;

public class ChartHelper {
    /**
     * Convert an IDList to a JFreeChart CategoryDataset.
     */
    public static CategoryDataset createChartWithRunsOnXAxis(IDList idlist) {
        DefaultCategoryDataset ds = new DefaultCategoryDataset();
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;

        int sz = (int)idlist.size();
        for (int i=0; i<sz; i++) {
            ScalarResult d = resultFileManager.getScalar(idlist.get(i));
            ds.addValue(d.getValue(),
                    d.getRun().getFileAndRunName(),
                    d.getModuleName()+"\n"+d.getName());
        }
        return ds;
    }

    /**
     * Produces JFreeChart dataset for a line chart from a series of
     * output vectors, by running the data-flow engine.
     */
    private static XYArray[] executeDataflowNetwork(IDList vecs) {
        // prepare
        ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
        DataflowManager net = new DataflowManager();
        NodeTypeRegistry factory = NodeTypeRegistry.instance();

        // create VectorFileReader nodes
        HashMap fileNameToNode = new HashMap();
        NodeType vectorFileReaderType = factory.getNodeType("vectorfilereader");
        FileList files = resultFileManager.getFiles();
        int sz = (int)files.size();
        for (int i=0; i<sz; i++) { // FIXME we should only create reader for files that occur in vecs[]
            StringMap args = new StringMap();
            String fileName = files.get(i).getFilePath();
            args.set("filename", fileName);
            Node readerNode = vectorFileReaderType.create(net, args);
            fileNameToNode.put(fileName, readerNode);
        }

        // create network
        NodeType arrayBuilderType = factory.getNodeType("arraybuilder");
        Node [] arrayBuilderNodes = new Node[(int)vecs.size()];
        for (int i=0; i<(int)vecs.size(); i++) {
            VectorResult vec = resultFileManager.getVector(vecs.get(i));
            if (true) {
                // no filter: connect directly to an ArrayBuilder
                StringMap args = new StringMap();
                Node arrayNode = arrayBuilderType.create(net, args);
                arrayBuilderNodes[i] = arrayNode;
                String fileName = vec.getRun().getFile().getFilePath();
                Node readerNode = (Node)fileNameToNode.get(fileName);
                net.connect(
                        vectorFileReaderType.getPort(readerNode, ""+vec.getVectorId()),
                        arrayBuilderType.getPort(arrayNode, "in"));
                args.set("filename", fileName);
            }
            else {
                // FIXME filters: not impl...
/*
                # create and connect filter, then arraybuilder
                set filtername $vec($id,filter)
                set filter [opp_createnode $net $filtername]
                opp_connect $vecfilenodes($vec($id,fname)) $vec($id,vecid) $filter in

                set arraybuilder($id) [opp_createnode $net arraybuilder]
                opp_connect $filter out $arraybuilder($id) in

 */
            }
        }

        // run the netwrork
        net.dump();
        net.execute();

        // extract results
        XYArray[] array = new XYArray[arrayBuilderNodes.length];
        for (int i=0; i<arrayBuilderNodes.length; i++)
            array[i] = arrayBuilderNodes[i].getArray();
        return array;
    }

    public static XYDataset createXYDataSet(IDList vecs) {
        XYArray[] array = executeDataflowNetwork(vecs);
        return new OutputVectorDataset(array);
    }
}

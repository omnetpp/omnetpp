/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import org.omnetpp.common.Debug;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.InterruptedFlag;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYArrayVector;

public class VectorDataLoader {
    public static boolean debug = Debug.isChannelEnabled("vectordataloader");

    public static XYVector[] getDataOfVectors(ResultFileManager manager, IDList idlist, double simTimeStart, double simTimeEnd, InterruptedFlag interruptedFlag) {

        if (debug)
            Debug.println("getting data of vectors");

        XYVector[] vectors = new XYVector[idlist.size()];

        XYArrayVector out = ScaveEngine.readVectorsIntoArrays2(manager, idlist, false, false, 512 * 1024 * 1024, simTimeStart, simTimeEnd, interruptedFlag);

        if (debug)
            Debug.println("converting vector data");

        for (int i = 0; i < out.size(); ++i) {
            XYArray xyArray = out.get(i);
            vectors[i] = new XYVector(xyArray.length());
            for (int k = 0; k < xyArray.length(); ++k) {
                vectors[i].x[k] = xyArray.getX(k);
                vectors[i].y[k] = xyArray.getY(k);
            }
        }

        if (debug)
            Debug.println("vector data loaded, cleaning up memory");

        out.delete();
        out = null;
        System.gc();
        ScaveEngine.malloc_trim();

        if (debug)
            Debug.println("vector data memory cleanup done");

        return vectors;
    }
}

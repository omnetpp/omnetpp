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
import org.omnetpp.scave.engine.XYArrayVector;

public class VectorDataLoader {
    public static boolean debug = Debug.isChannelEnabled("vectordataloader");

    public static XYArrayVector getDataOfVectors(ResultFileManager manager, IDList idlist, double simTimeStart, double simTimeEnd, InterruptedFlag interruptedFlag) {

        System.gc();
        ScaveEngine.malloc_trim();

        long availableSystemMemoryBytes = ScaveEngine.getAvailableMemoryBytes();

        if (debug) {
            Debug.println("getting data of vectors");
            Debug.println("available system memory is: " + ( availableSystemMemoryBytes / 1024 / 1024 ) + " MiB");
        }

        // vector data never touches the JVM heap now, but we add a generous safety margin,
        // because on Windows and Mac, we will have to copy the data instead of simply sharing it
        long memoryLimitBytes = availableSystemMemoryBytes / 2;

        if (debug)
            Debug.println("memory limit MiB: " + memoryLimitBytes / 1024 / 1024);

        XYArrayVector out = ScaveEngine.readVectorsIntoArrays2(manager, idlist, false, false, memoryLimitBytes, simTimeStart, simTimeEnd, interruptedFlag);

        if (debug)
            Debug.println("vector data loaded");

        return out;
    }
}

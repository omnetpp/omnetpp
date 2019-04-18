/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.dataset;

import org.eclipse.core.runtime.IProgressMonitor;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYArrayVector;

public class VectorDataLoader {
    public static XYVector[] getDataOfVectors(ResultFileManager manager, IDList idlist, IProgressMonitor progressMonitor) {

        XYVector[] vectors = new XYVector[idlist.size()];

        XYArrayVector out = ScaveEngine.readVectorsIntoArrays(manager, idlist);

        for (int i = 0; i < out.size(); ++i) {
            XYArray xyArray = out.get(i);
            vectors[i] = new XYVector(xyArray.length());
            for (int k = 0; k < xyArray.length(); ++k) {
                vectors[i].x[k] = xyArray.getX(k);
                vectors[i].y[k] = xyArray.getY(k);
            }
        }
        return vectors;
    }
}

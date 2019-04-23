/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import static org.junit.Assert.assertEquals;

import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ScaveEngine;
import org.omnetpp.scave.engine.XYArray;
import org.omnetpp.scave.engine.XYArrayVector;


/**
 * Makes it possible for SequenceChart to read data from output vector files, without
 * depending on the Scave plugin.
 *
 * @author Andras
 */
public class VectorFileUtil { // TODO deduplicate with VectorDataLoader
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

        IDList idList = new IDList();
        idList.add(id);

        XYArrayVector out = ScaveEngine.readVectorsIntoArrays(resultfileManager, idList);

        assertEquals(out.size(), 1);

        return out.get(0);
    }
}

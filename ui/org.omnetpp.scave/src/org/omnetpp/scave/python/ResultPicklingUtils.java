/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.python;

import java.nio.ByteBuffer;

public class ResultPicklingUtils {

    public static double[] bytesToDoubles(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
        double[] doubles = new double[bytes.length / 8];
        byteBuffer.asDoubleBuffer().get(doubles);
        return doubles;
    }
}

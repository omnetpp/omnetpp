/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.jfree.data.DomainOrder;
import org.jfree.data.general.AbstractSeriesDataset;
import org.jfree.data.xy.XYDataset;

import org.omnetpp.scave.engine.XYArray;

public class OutputVectorDataset extends AbstractSeriesDataset implements XYDataset {

    private static final long serialVersionUID = -3852560945270511274L;

    private XYArray[] array;

    public OutputVectorDataset(XYArray[] a) {
        array = a;
    }

    public DomainOrder getDomainOrder() {
        return DomainOrder.NONE;
    }

    public int getItemCount(int arg0) {
        return array[arg0].length();
    }

    public Number getX(int arg0, int arg1) {
        return new Double(array[arg0].getX(arg1));
    }

    public double getXValue(int arg0, int arg1) {
        return array[arg0].getX(arg1);
    }

    public Number getY(int arg0, int arg1) {
        return new Double(array[arg0].getY(arg1));
    }

    public double getYValue(int arg0, int arg1) {
        return array[arg0].getY(arg1);
    }

    public int getSeriesCount() {
        return array.length;
    }

    public Comparable getSeriesKey(int arg0) {
        return "series-"+arg0;
    }

}

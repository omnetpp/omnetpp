/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.ScaveModelFactory;


/**
 * Create a BarChart..
 */
public class NewHistogramChartAction extends NewChartObjectAction {
    public NewHistogramChartAction() {
        super(ScaveModelFactory.eINSTANCE.createHistogramChart());
        setText("New Histogram Chart");
        setImageDescriptor(ScavePlugin.getImageDescriptor("icons/full/etool16/newhistogramchart.png"));
    }
}

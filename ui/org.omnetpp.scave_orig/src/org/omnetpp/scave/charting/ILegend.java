/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting;

import org.eclipse.swt.graphics.Color;
import org.omnetpp.scave.charting.plotter.IChartSymbol;

public interface ILegend {

    void clearItems();

    void addItem(Color color, String text, IChartSymbol symbol, boolean drawLine);
}

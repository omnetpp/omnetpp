/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.charting.properties;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;

/**
 * Defines defaults for the chart. Used by both the chart widgets
 * and the property sheet.
 *
 * @author tomi, andras
 */
public class PlotDefaults {
    //TODO turn these into properties!!!
    public static final Color DEFAULT_INSETS_BACKGROUND_COLOR = Display.getDefault().getSystemColor(SWT.COLOR_WIDGET_BACKGROUND);
    public static final Color DEFAULT_INSETS_LINE_COLOR = ColorFactory.BLACK;
    public static final Color DEFAULT_BAR_OUTLINE_COLOR = ColorFactory.GREY80;
    public static final Color DEFAULT_BAR_BASELINE_COLOR = ColorFactory.GREY80;
}

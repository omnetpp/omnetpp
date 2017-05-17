/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;

/**
 * Opens the ChartSheet object on the Charts page.
 */
public class GotoChartSheetDefinitionAction extends GotoItemDefinitionAction {
    public GotoChartSheetDefinitionAction() {
        setText("Go to Chart Definition");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_GOTOITEMDEFINITION));
    }
}

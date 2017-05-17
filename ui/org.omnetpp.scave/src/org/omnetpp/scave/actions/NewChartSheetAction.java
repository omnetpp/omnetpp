/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.ScaveModelFactory;


/**
 * Create a ChartSheet.
 */
public class NewChartSheetAction extends NewAnalysisItemAction {
    public NewChartSheetAction(boolean withEditDialog) {
        super(ScaveModelFactory.eINSTANCE.createChartSheet(), withEditDialog);
        setText("New Chart Sheet");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_NEWCHARTSHEET));
    }
}

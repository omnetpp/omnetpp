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
 * Create a Dataset.
 */
public class NewDatasetAction extends NewAnalysisItemAction {
    public NewDatasetAction(boolean withEditDialog) {
        super(ScaveModelFactory.eINSTANCE.createDataset(), withEditDialog);
        setText("New Dataset");
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_NEWDATASET));
    }
}

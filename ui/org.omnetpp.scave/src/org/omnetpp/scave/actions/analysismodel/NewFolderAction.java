/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions.analysismodel;

import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.model.Folder;


public class NewFolderAction extends NewAnalysisItemAction {

    public NewFolderAction() {
        super(new Folder("New Folder"));
        setText("New Folder");
        setImageDescriptor(ScavePlugin.getImageDescriptor( ScaveImages.IMG_ETOOL16_NEWFOLDER));
    }
}

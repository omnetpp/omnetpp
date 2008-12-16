/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.
  
  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.ui.misc;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NEDResourcesPlugin;
import org.omnetpp.ned.model.INEDElement;

/**
 * Represents a Hyperlink that points to a NED type
 *
 * @author rhornig
 */
public class NEDHyperlink implements IHyperlink {

    IRegion region;         // the region covered by the hyperlink
    INEDElement target;      // the ned element which should be opened
    /**
     * @param reg The region covered by the link
     * @param element The target NED element that should be opened
     */
    public NEDHyperlink(IRegion reg, INEDElement element) {
        region = reg;
        target = element;
    }

    public IRegion getHyperlinkRegion() {
        return region;
    }

    public String getHyperlinkText() {
        return null;
    }

    public String getTypeLabel() {
        return null;
    }

    public void open() {
        // open the ned element in text mode
        NEDResourcesPlugin.openNEDElementInEditor(target, IGotoNedElement.Mode.TEXT);
    }

}

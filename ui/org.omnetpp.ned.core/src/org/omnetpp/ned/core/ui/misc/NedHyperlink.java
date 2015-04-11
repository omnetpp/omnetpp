/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.core.ui.misc;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.omnetpp.ned.core.IGotoNedElement;
import org.omnetpp.ned.core.NedResourcesPlugin;
import org.omnetpp.ned.model.INedElement;

/**
 * Represents a Hyperlink that points to a NED type
 *
 * @author rhornig
 */
public class NedHyperlink implements IHyperlink {

    IRegion region;         // the region covered by the hyperlink
    INedElement target;      // the ned element which should be opened
    String text;

    /**
     * @param reg The region covered by the link
     * @param element The target NED element that should be opened
     */
    public NedHyperlink(IRegion reg, INedElement element) {
        region = reg;
        target = element;
    }

    /**
     * @param reg The region covered by the link
     * @param element The target NED element that should be opened
     * @param text The text shown when there are several target for the same region
     */
    public NedHyperlink(IRegion reg, INedElement element, String text) {
        region = reg;
        target = element;
        this.text = text;
    }

    public IRegion getHyperlinkRegion() {
        return region;
    }

    public String getHyperlinkText() {
        return text;
    }

    public String getTypeLabel() {
        return null;
    }

    public void open() {
        // open the ned element in text mode
        NedResourcesPlugin.openNedElementInEditor(target, IGotoNedElement.Mode.TEXT);
    }

}

package org.omnetpp.ned.misc;

import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.hyperlink.IHyperlink;
import org.omnetpp.ned.model.NEDElement;
import org.omnetpp.ned.resources.IGotoNedElement;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Represents a Hyperlink that points to a NED type
 * @author rhornig
 */
public class NEDHyperlink implements IHyperlink {

    IRegion region;         // the region covered by the hyperlink
    NEDElement target;      // the ned elemnt which should be opened 
    /**
     * @param reg The region covered by the link
     * @param element The target NED element that should be opened
     */
    public NEDHyperlink(IRegion reg, NEDElement element) {
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

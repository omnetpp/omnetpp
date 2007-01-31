package org.omnetpp.ned.editor.graph.actions;

import org.eclipse.gef.ui.actions.ZoomComboContributionItem;
import org.eclipse.ui.IPartService;

public class NEDZoomComboContributionItem extends ZoomComboContributionItem {

    public NEDZoomComboContributionItem(IPartService partService, String initString) {
        super(partService, initString);
    }

    public NEDZoomComboContributionItem(IPartService partService, String[] initStrings) {
        super(partService, initStrings);
    }

    public NEDZoomComboContributionItem(IPartService partService) {
        super(partService);
    }
    
    @Override
    public boolean isEnabled() {
        return false;
    }

}

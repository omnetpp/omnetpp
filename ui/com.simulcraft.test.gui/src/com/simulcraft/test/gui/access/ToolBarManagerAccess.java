package com.simulcraft.test.gui.access;

import org.eclipse.jface.action.IContributionItem;
import org.eclipse.jface.action.IToolBarManager;

public class ToolBarManagerAccess
    extends Access
{
    protected IToolBarManager toolBarManager;
    
    public ToolBarManagerAccess(IToolBarManager toolBarManager) {
        this.toolBarManager = toolBarManager;
    }
    
    public ToolItemAccess findToolItem() {
        for (IContributionItem contributionItem : toolBarManager.getItems()) {
            contributionItem.getId();
        }

//        return new ToolItemAccess();
        return null;
    }
}

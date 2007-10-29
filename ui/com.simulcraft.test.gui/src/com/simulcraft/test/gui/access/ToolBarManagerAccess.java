package com.simulcraft.test.gui.access;

import org.eclipse.jface.action.IToolBarManager;
import org.eclipse.swt.widgets.ToolBar;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.core.UIStep;

public class ToolBarManagerAccess
    extends Access
{
    protected IToolBarManager toolBarManager;
    
    public ToolBarManagerAccess(IToolBarManager toolBarManager) {
        this.toolBarManager = toolBarManager;
    }
    
    @UIStep
    public ToolBarAccess getToolBar() {
        return new ToolBarAccess((ToolBar)ReflectionUtils.getFieldValue(toolBarManager, "toolBar"));
    }
}

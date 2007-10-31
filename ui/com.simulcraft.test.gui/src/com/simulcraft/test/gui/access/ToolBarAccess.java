package com.simulcraft.test.gui.access;

import org.eclipse.swt.widgets.ToolBar;
import org.eclipse.swt.widgets.ToolItem;
import org.omnetpp.common.util.IPredicate;

import com.simulcraft.test.gui.core.UIStep;

public class ToolBarAccess
    extends CompositeAccess
{
    public ToolBarAccess(ToolBar toolBar) {
        super(toolBar);
    }
    
    @Override
    public ToolBar getControl() {
        return (ToolBar)widget;
    }
    
    @UIStep
    public ToolItemAccess findToolItem(final String text) {
        return new ToolItemAccess((ToolItem)findObject(getControl().getItems(), new IPredicate() {
            public boolean matches(Object object) {
                return ((ToolItem)object).getText().matches(text);
            }
        }));
    }

    @UIStep
    @Override
    public ToolItemAccess findToolItemWithTooltip(final String text) {
        return new ToolItemAccess((ToolItem)findObject(getControl().getItems(), new IPredicate() {
            public boolean matches(Object object) {
            	String tooltipText = ((ToolItem)object).getToolTipText(); 
                return tooltipText != null && tooltipText.matches(text);
            }
        }));
    }
}

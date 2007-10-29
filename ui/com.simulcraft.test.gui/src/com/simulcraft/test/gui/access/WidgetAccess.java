package com.simulcraft.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.widgets.Widget;

import com.simulcraft.test.gui.core.UIStep;

public class WidgetAccess
	extends ClickableAccess 
{
	protected Widget widget;

	public WidgetAccess(Widget widget) {
		Assert.assertTrue(widget != null);
		this.widget = widget;
	}

	public Widget getWidget() {
		return widget;
	}
	
	@UIStep
	public void assertDisposed() {
	    Assert.assertTrue("control or widget not yet disposed", getWidget().isDisposed());
	}

	@UIStep
    public void assertNotDisposed() {
        Assert.assertTrue("control or widget disposed", !getWidget().isDisposed());
    }
}

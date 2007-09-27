package com.simulcraft.test.gui.util;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.common.util.IPredicate;

public class Predicate {
	
	public static final String WIDGET_ID = "com.simulcraft.test.gui.WidgetID";

	public static IPredicate hasID(final String id) {
		Assert.isLegal(id != null);
		
		return new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Widget) {
					Widget widget = (Widget)object;
					Object widgetID = widget.getData(WIDGET_ID);
					return id.equals(widgetID);
				}
				return false;
			}
		};
	}
}

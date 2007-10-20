package com.simulcraft.test.gui.util;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Item;
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
	
	public static IPredicate itemWithText(final String text) {
		Assert.isLegal(text != null);
		
		return new IPredicate() {
			public boolean matches(Object object) {
				return object instanceof Item &&
						((Item)object).getText().matches(text);
			}
		};
	}
	
	public static IPredicate buttonWithText(final String text) {
	    return new IPredicate() {
	        public boolean matches(Object object) {
	            if (object instanceof Button) {
	                Button button = (Button)object;
	                return button.getText().replace("&", "").matches(text);
	            }
	            return false;
	        }
	    };
	}
}

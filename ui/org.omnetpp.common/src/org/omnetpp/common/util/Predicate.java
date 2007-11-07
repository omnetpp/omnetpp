package org.omnetpp.common.util;

import org.eclipse.core.runtime.Assert;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Item;
import org.eclipse.swt.widgets.Widget;

public class Predicate {
	
	public static final String WIDGET_ID = "com.simulcraft.test.gui.WidgetID";
    
    public static IPredicate and(final IPredicate ... args) {
        return new IPredicate() {
            public boolean matches(Object object) {
                for (IPredicate arg : args)
                    if (!arg.matches(object))
                        return false;

                return true;
            }
        };
    }

    public static IPredicate or(final IPredicate ... args) {
        return new IPredicate() {
            public boolean matches(Object object) {
                for (IPredicate arg : args)
                    if (arg.matches(object))
                        return true;

                return false;
            }
        };
    }

    public static IPredicate not(final IPredicate arg) {
        return new IPredicate() {
            public boolean matches(Object object) {
                return !arg.matches(object);
            }
        };
    }

    public static IPredicate instanceOf(final Class<?> clazz) {
        return new IPredicate() {
            public boolean matches(Object object) {
                return clazz.isInstance(object);
            }
            
            public String toString() {
                return "an instance of " + clazz.getSimpleName();
            }
        };
    }

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

    public static IPredicate visible() {
        return new IPredicate() {
            public boolean matches(Object object) {
                return object instanceof Control && ((Control)object).isVisible();
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

package org.omnetpp.common.ui;

import java.lang.reflect.Field;

import org.eclipse.jface.action.ActionContributionItem;
import org.eclipse.jface.action.IAction;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Widget;
import org.omnetpp.common.CommonPlugin;

/**
 * Unfortunately, ActionContributionItem only displays the icon on the button 
 * and not the text (MODE_FORCE_TEXT has no effect for Buttons). Eclipse 3.3M7.
 * 
 * TODO remove this class when https://bugs.eclipse.org/bugs/show_bug.cgi?id=187956 gets fixed.
 *
 * @author Andras
 */
public class ActionContributionItem2 extends ActionContributionItem {

	public ActionContributionItem2(IAction action) {
		super(action);
	}

	public ActionContributionItem2(IAction action, int mode) {
		super(action);
		setMode(mode);
	}

	public void update(String propertyName) {
		super.update(propertyName);

		// set the text
		Widget widget = getWidget();
		boolean textChanged = propertyName == null || propertyName.equals(IAction.TEXT);
		if (widget instanceof Button && textChanged && (getMode() & MODE_FORCE_TEXT) != 0) {
			((Button)widget).setText(getAction().getText());
		}
	}

	/**
	 * Base class doesn't have getWidget(), and the field is private, so we need to 
	 * get it with reflection here.
	 */
	protected Widget getWidget() {
		try {
			Field declaredField = ActionContributionItem.class.getDeclaredField("widget");
			declaredField.setAccessible(true);
			return (Widget)declaredField.get(this);
		} 
		catch (Exception e) {
			CommonPlugin.logError(e);
			return null;
		}
	}
}

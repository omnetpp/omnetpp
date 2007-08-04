package org.omnetpp.test.gui.access;

import java.util.List;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;

public class CompositeAccess<T extends Composite> extends ControlAccess<T>
{
	public CompositeAccess(T composite) {
		super(composite);
	}

	public T getComposite() {
		return widget;
	}
	
	public Control findChildControl(Class<? extends Control> clazz) {
		return findDescendantControl(getComposite(), clazz);
	}

	public Control findChildControl(IPredicate predicate) {
		return findDescendantControl(getComposite(), predicate);
	}

	public List<Control> collectChildControls(IPredicate predicate) {
		return collectDescendantControls(getComposite(), predicate);
	}
}

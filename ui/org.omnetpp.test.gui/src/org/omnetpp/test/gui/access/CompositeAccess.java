package org.omnetpp.test.gui.access;

import java.util.List;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Text;
import org.omnetpp.test.gui.InUIThread;

public class CompositeAccess<T extends Composite> extends ControlAccess<T>
{
	public CompositeAccess(T composite) {
		super(composite);
	}

	public T getComposite() {
		return widget;
	}

	@InUIThread
	public Control findChildControl(Class<? extends Control> clazz) {
		return findDescendantControl(getComposite(), clazz);
	}

	@InUIThread
	public Control findChildControl(IPredicate predicate) {
		return findDescendantControl(getComposite(), predicate);
	}

	@InUIThread
	public List<Control> collectChildControls(IPredicate predicate) {
		return collectDescendantControls(getComposite(), predicate);
	}

	@InUIThread
	public ButtonAccess findButtonWithLabel(final String label) {
		return new ButtonAccess((Button)findDescendantControl(getComposite(), new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Button) {
					Button button = (Button)object;
					return button.getText().replace("&", "").matches(label);
				}
				return false;
			}
		}));
	}

	@InUIThread
	public TextAccess findTextNearLabel(final String label) {
		Label labelControl = (Label)findDescendantControl(getComposite(), new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Label) {
					Label labelControl = (Label)object;

					return labelControl.getText().replace("&", "").matches(label);
				}

				return false;
			}
		});

		// TODO: should consider layout
		return new TextAccess((Text)findDescendantControl(labelControl.getParent(), Text.class));
	}

}

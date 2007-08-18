package org.omnetpp.test.gui.access;

import java.util.List;

import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.test.gui.InUIThread;

public class CompositeAccess<T extends Composite> extends ControlAccess<T>
{
	public CompositeAccess(T composite) {
		super(composite);
	}

	public Composite getComposite() {
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
	public LabelAccess findLabel(final String label) {
		Label labelControl = (Label)findDescendantControl(getComposite(), new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Label) {
					Label labelControl = (Label)object;
					return labelControl.getText().replace("&", "").matches(label);
				}
				return false;
			}
		});
		return new LabelAccess(labelControl);
	}

	@InUIThread
	public TreeAccess findTree() {
		return new TreeAccess((Tree)findDescendantControl(getComposite(), Tree.class));
	}

	@InUIThread
	public TableAccess findTable() {
		return new TableAccess((Table)findDescendantControl(getComposite(), Table.class));
	}

	@InUIThread
	public StyledTextAccess findStyledText() {
		return new StyledTextAccess((StyledText) Access.findDescendantControl(getComposite(), StyledText.class));
	}

	@InUIThread
	public TextAccess findTextAfterLabel(String label) {
		final LabelAccess labelAccess = findLabel(label);
		return new TextAccess((Text)labelAccess.findNextControl(Text.class));
	}

	@InUIThread
	public ComboAccess findComboAfterLabel(String label) {
		final LabelAccess labelAccess = findLabel(label);
		return new ComboAccess((Combo)labelAccess.findNextControl(Combo.class));
	}

}

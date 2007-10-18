package com.simulcraft.test.gui.access;

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

import com.simulcraft.test.gui.core.InUIThread;


public class CompositeAccess extends ControlAccess
{
	public CompositeAccess(Composite composite) {
		super(composite);
	}

    @Override
	public Composite getControl() {
		return (Composite)widget;
	}

	@InUIThread
	public Control findDescendantControl(Class<? extends Control> clazz) {
		return findDescendantControl(getControl(), clazz);
	}

	@InUIThread
	public Control findDescendantControl(IPredicate predicate) {
		return findDescendantControl(getControl(), predicate);
	}

	@InUIThread
	public List<Control> collectDescendantControls(IPredicate predicate) {
		return collectDescendantControls(getControl(), predicate);
	}

	@InUIThread
	public ButtonAccess findButtonWithLabel(final String label) {
		return new ButtonAccess((Button)findDescendantControl(getControl(), new IPredicate() {
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
	public ButtonAccess tryToFindButtonWithLabel(final String label) {
		List<Control> buttons = collectDescendantControls(getControl(), new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Button) {
					Button button = (Button)object;
					return button.getText().replace("&", "").matches(label);
				}
				return false;
			}
		});
		Button button = (Button)atMostOneObject(buttons);
		return button != null ? (ButtonAccess)createAccess(button) : null;
	}

	@InUIThread
	public LabelAccess findLabel(final String label) {
		Label labelControl = (Label)findDescendantControl(getControl(), new IPredicate() {
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
		return new TreeAccess((Tree)findDescendantControl(getControl(), Tree.class));
	}

	@InUIThread
	public TableAccess findTable() {
		return new TableAccess((Table)findDescendantControl(getControl(), Table.class));
	}

	@InUIThread
	public StyledTextAccess findStyledText() {
		return new StyledTextAccess((StyledText) Access.findDescendantControl(getControl(), StyledText.class));
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

	@InUIThread
	public TreeAccess findTreeAfterLabel(String label) {
		final LabelAccess labelAccess = findLabel(label);
		return new TreeAccess((Tree)labelAccess.findNextControl(Tree.class));
	}
}

package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.swt.widgets.Text;

public class ShellAccess extends ControlAccess<Shell>
{
	public ShellAccess(Shell shell) {
		super(shell);
	}
	
	public Shell getShell() {
		return widget;
	}

	public ButtonAccess findButtonWithLabel(final String label) {
		return new ButtonAccess((Button)findDescendantControl(getShell(), new IPredicate() {
			public boolean matches(Object object) {
				if (object instanceof Button) {
					Button button = (Button)object;
					
					return button.getText().replace("&", "").matches(label);
				}
				
				return false;
			}
		}));
	}

	public TextAccess findTextNearLabel(final String label) {
		Label labelControl = (Label)findDescendantControl(getShell(), new IPredicate() {
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

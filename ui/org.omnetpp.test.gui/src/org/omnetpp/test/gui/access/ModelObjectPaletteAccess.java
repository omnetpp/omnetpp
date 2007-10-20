package org.omnetpp.test.gui.access;

import org.eclipse.swt.custom.ScrolledComposite;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.omnetpp.common.util.IPredicate;
import org.omnetpp.common.util.ReflectionUtils;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class ModelObjectPaletteAccess extends CompositeAccess {
	
	// button labels
	public static final String
		DATASET_BUTTON = "Dataset.*",
		ADD_BUTTON = "Add.*",
		DISCARD_BUTTON = "Discard.*",
		APPLY_BUTTON = "Apply.*",
		COMPUTE_BUTTON = "Compute.*",
		BAR_CHART_BUTTON = "Bar.*[cC]hart.*",
		LINE_CHART_BUTTON = "Line.*[cC]hart.*",
		HISTOGRAM_CHART_BUTTON = "Histogram.*[cC]hart.*",
		SCATTER_CHART_BUTTON = "Scatter.*[cC]hart.*",
		CHARTSHEET_BUTTON = "Chart.*[sS]heet.*";

	

	public ModelObjectPaletteAccess(Composite composite) {
		super(composite);
	}
	
	@Override
	public ScrolledComposite getControl() {
		return (ScrolledComposite)super.getControl();
	}

	public CompositeAccess getComposite() {
		return (CompositeAccess)createAccess(getControl().getContent());
	}
	
	@InUIThread
	public CompositeAccess getButton(final String label) {
		return (CompositeAccess)createAccess(getComposite().findDescendantControl(
						new IPredicate() {
							public boolean matches(Object object) {
								if (object instanceof Canvas) {
									// use reflection instead of depending on ToolButton is exported
									String lbl = (String)ReflectionUtils.invokeMethod(((Canvas)object), "getText");
									return lbl != null && lbl.matches(label);
								}
								return false;
							}
							
						}));
	}
	
	public void revealButton(String label) {
		revealButton(getButton(label));
	}
	
	@InUIThread
	public void revealButton(CompositeAccess button) {
		getControl().setOrigin(button.getControl().getLocation());
	}
	
	@NotInUIThread
	public void clickButton(String label) {
		CompositeAccess button = getButton(label);
		revealButton(button);
		button.click();
	}
}

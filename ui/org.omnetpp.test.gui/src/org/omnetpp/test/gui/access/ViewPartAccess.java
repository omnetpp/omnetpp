package org.omnetpp.test.gui.access;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.ui.IViewPart;
import org.eclipse.ui.internal.ViewSite;
import org.omnetpp.test.gui.InUIThread;

public class ViewPartAccess extends CompositeAccess<Composite>
{
	protected IViewPart viewPart;

	public ViewPartAccess(IViewPart viewPart) {
		super((Composite)((ViewSite)viewPart.getSite()).getPane().getControl());
		this.viewPart = viewPart;
	}

	public IViewPart getViewPart() {
		return viewPart;
	}
	
	@InUIThread
	public void activateWithMouseClick() {
		clickCTabItem(findDescendantCTabItemByLabel(getComposite().getParent(), viewPart.getSite().getRegisteredName()));
	}
}

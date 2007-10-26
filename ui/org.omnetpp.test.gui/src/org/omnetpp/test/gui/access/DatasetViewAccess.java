package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.StackLayout;
import org.eclipse.ui.IViewPart;
import org.omnetpp.scave.TestSupport;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.ToolItemAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.core.InUIThread;
import com.simulcraft.test.gui.core.NotInUIThread;

public class DatasetViewAccess extends ViewPartAccess {

	public DatasetViewAccess(IViewPart viewPart) {
		super(viewPart);
	}
	
	@InUIThread
	public CompositeAccess getMainPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_MAIN_PANEL_ID);
	}
	
	@InUIThread
	public CompositeAccess getActivePanel() {
		CompositeAccess mainPanel = getMainPanel();
		Assert.assertTrue(mainPanel.getControl().getLayout() instanceof StackLayout);
		StackLayout layout = (StackLayout)mainPanel.getControl().getLayout();
		return (CompositeAccess)createAccess(layout.topControl);
	}
	
	@InUIThread
	public CompositeAccess getScalarsPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_SCALARS_PANEL_ID);
	}
	
	@InUIThread
	public CompositeAccess getVectorsPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_VECTORS_PANEL_ID);
	}
	
	@InUIThread
	public boolean isScalarsPanelActivated() {
		return getActivePanel() != null && getActivePanel().getControl() == getScalarsPanel().getControl();
	}
	
	@InUIThread
	public boolean isVectorsPanelActivated() {
		return getActivePanel() != null && getActivePanel().getControl() == getVectorsPanel().getControl();
	}

	@InUIThread
	public void assertScalarsPanelActivated() {
		Assert.assertTrue("Scalars panel is not active", isScalarsPanelActivated());
	}
	
	@InUIThread
	public void assertVectorsPanelActivated() {
		Assert.assertTrue("Vectors panel is not active", isVectorsPanelActivated());
	}
	
	@InUIThread
	public void activateScalarsPanel() {
		getShowScalarsButton().click();
	}
	
	@InUIThread
	public void activateVectorsPanel() {
		getShowVectorsButton().click();
	}
	
	@NotInUIThread
	public void ensureScalarsPanelActivated() {
		if (!isScalarsPanelActivated())
			activateScalarsPanel();
	}
	
	@NotInUIThread
	public void ensureVectorsPanelActivated() {
		if (!isVectorsPanelActivated())
			activateVectorsPanel();
	}
	
	@InUIThread
	public TableAccess getScalarsTable() {
		return getScalarsPanel().findTable();
	}
	
	@InUIThread
	public TableAccess getVectorsTable() {
		return getVectorsPanel().findTable();
	}
	
	@InUIThread
	protected ToolItemAccess findToolButton(String tooltip) {
		return getViewToolbar().findToolItemWithToolTip(tooltip);
	}
	
	public ToolItemAccess getShowScalarsButton() {
		return findToolButton(".*[sS]calar.*");
	}
	
	public ToolItemAccess getShowVectorsButton() {
		return findToolButton(".*[vV]ector.*");
	}
}

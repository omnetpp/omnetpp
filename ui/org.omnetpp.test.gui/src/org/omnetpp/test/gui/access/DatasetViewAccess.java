package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.StackLayout;
import org.eclipse.ui.IViewPart;
import org.omnetpp.scave.TestSupport;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.ToolItemAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.core.UIStep;
import com.simulcraft.test.gui.core.InBackgroundThread;

public class DatasetViewAccess extends ViewPartAccess {

	public DatasetViewAccess(IViewPart viewPart) {
		super(viewPart);
	}
	
	@UIStep
	public CompositeAccess getMainPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_MAIN_PANEL_ID);
	}
	
	@UIStep
	public CompositeAccess getActivePanel() {
		CompositeAccess mainPanel = getMainPanel();
		Assert.assertTrue(mainPanel.getControl().getLayout() instanceof StackLayout);
		StackLayout layout = (StackLayout)mainPanel.getControl().getLayout();
		return (CompositeAccess)createAccess(layout.topControl);
	}
	
	@UIStep
	public CompositeAccess getScalarsPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_SCALARS_PANEL_ID);
	}
	
	@UIStep
	public CompositeAccess getVectorsPanel() {
		return (CompositeAccess)getComposite().findControlWithID(TestSupport.DATASET_VIEW_VECTORS_PANEL_ID);
	}
	
	@UIStep
	public boolean isScalarsPanelActivated() {
		return getActivePanel() != null && getActivePanel().getControl() == getScalarsPanel().getControl();
	}
	
	@UIStep
	public boolean isVectorsPanelActivated() {
		return getActivePanel() != null && getActivePanel().getControl() == getVectorsPanel().getControl();
	}

	@UIStep
	public void assertScalarsPanelActivated() {
		Assert.assertTrue("Scalars panel is not active", isScalarsPanelActivated());
	}
	
	@UIStep
	public void assertVectorsPanelActivated() {
		Assert.assertTrue("Vectors panel is not active", isVectorsPanelActivated());
	}
	
	@UIStep
	public void activateScalarsPanel() {
		getShowScalarsButton().click();
	}
	
	@UIStep
	public void activateVectorsPanel() {
		getShowVectorsButton().click();
	}
	
	@InBackgroundThread
	public void ensureScalarsPanelActivated() {
		if (!isScalarsPanelActivated())
			activateScalarsPanel();
	}
	
	@InBackgroundThread
	public void ensureVectorsPanelActivated() {
		if (!isVectorsPanelActivated())
			activateVectorsPanel();
	}
	
	@UIStep
	public TableAccess getScalarsTable() {
		return getScalarsPanel().findTable();
	}
	
	@UIStep
	public TableAccess getVectorsTable() {
		return getVectorsPanel().findTable();
	}
	
	@UIStep
	protected ToolItemAccess findToolButton(String tooltip) {
		return getViewToolbar().findToolItemWithTooltip(tooltip);
	}
	
	public ToolItemAccess getShowScalarsButton() {
		return findToolButton(".*[sS]calar.*");
	}
	
	public ToolItemAccess getShowVectorsButton() {
		return findToolButton(".*[vV]ector.*");
	}
}

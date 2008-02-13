package org.omnetpp.test.gui.access;

import junit.framework.Assert;

import org.eclipse.swt.custom.StackLayout;
import org.eclipse.ui.IViewPart;
import org.omnetpp.scave.TestSupport;

import com.simulcraft.test.gui.access.CompositeAccess;
import com.simulcraft.test.gui.access.TableAccess;
import com.simulcraft.test.gui.access.ToolItemAccess;
import com.simulcraft.test.gui.access.ViewPartAccess;
import com.simulcraft.test.gui.core.InBackgroundThread;
import com.simulcraft.test.gui.core.UIStep;

public class DatasetViewAccess extends ViewPartAccess {
	
	// column names
	public static final String
		DIRECTORY = "Dir.*",
		FILE_NAME = "File.*",
		CONFIG_NAME = "Config.*",
		RUN_NUMBER = "Run num.*",
		RUN_ID = "Run [iI][dD].*",
		MODULE = "Module.*",
		NAME = "Name.*",
		EXPERIMENT = "Experiment.*",
		MEASUREMENT = "Measurement.*",
		REPLICATION = "Replication.*",
		VALUE = "Value.*",
		COUNT = "Count.*",
		MEAN = "Mean.*",
		STDDEV = "St.*[dD]ev.*",
		MIN = "Min.*",
		MAX = "Max.*";

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
	public TableAccess getActiveTable() {
		if (isScalarsPanelActivated())
			return getScalarsTable();
		else if (isVectorsPanelActivated())
			return getVectorsTable();
		else
			return null;
	}
	
	@UIStep
	protected ToolItemAccess findToolButton(String tooltip) {
		return getViewToolbar().findToolItemWithTooltip(tooltip);
	}
	
	@UIStep
	public void sortByTableColumn(String columnName, int direction) {
		getActiveTable().getTableColumn(columnName).sort(direction);
	}
	
	public ToolItemAccess getShowScalarsButton() {
		return findToolButton(".*[sS]calar.*");
	}
	
	public ToolItemAccess getShowVectorsButton() {
		return findToolButton(".*[vV]ector.*");
	}
}

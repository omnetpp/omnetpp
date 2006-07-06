package org.omnetpp.scave2.editors.datatable;

import static org.omnetpp.scave2.model.RunAttribute.EXPERIMENT;
import static org.omnetpp.scave2.model.RunAttribute.MEASUREMENT;
import static org.omnetpp.scave2.model.RunAttribute.REPLICATION;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.SelectionAdapter;
import org.eclipse.swt.events.SelectionEvent;
import org.eclipse.swt.events.SelectionListener;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.forms.widgets.FormToolkit;
import org.omnetpp.scave.engine.FileRunList;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileList;
import org.omnetpp.scave.engine.RunList;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.engineext.ResultFileManagerEx;
import org.omnetpp.scave.model.DatasetType;
import org.omnetpp.scave2.model.FilterHints;
import org.omnetpp.scave2.model.FilterParams;

/**
 * Displays a data table of vectors/scalars/histograms with filter
 * comboboxes.
 * 
 * This class is reusable, which means it only knows that it has to 
 * display an IDList belonging to a particular ResultFileManager,
 * and has absolutely no reference to the editor, or EMF model objects,
 * or any widgets outside -- nothing.
 * 
 * The user is responsible to keep contents up-to-date in case
 * ResultFileManager or IDList contents change. Refreshing can be
 * done by calling setIDList(). 
 *  
 * @author andras
 */
//XXX tables and filter should include the experiment, measurement, replication fields as well
//XXX filter should include expressions ("where load>10")
//XXX make filter panel foldable?
public class FilteredDataPanel extends Composite {
	private FilteringPanel filterPanel;
	private DataTable table;
	private IDList idlist; // the unfiltered data list
	private FormToolkit formToolkit = null;   //  @jve:decl-index=0:visual-constraint=""
	
	public FilteredDataPanel(Composite parent, int style, int type) {
		super(parent, style);
		initialize(type);
		configureFilterPanel();
	}

	public FilteringPanel getFilterPanel() {
		return filterPanel;
	}

	public DataTable getTable() {
		return table;
	}
	
	public void setIDList(IDList idlist) {
		this.idlist = idlist;
		updateFilterCombos();
		runFilter();
	}

	public IDList getIDList() {
		return idlist;
	}

	public void setResultFileManager(ResultFileManagerEx manager) {
		table.setResultFileManager(manager);
	}

	public ResultFileManagerEx getResultFileManager() {
		return table.getResultFileManager();
	}

	protected void initialize(int type) {
		setLayout(new GridLayout(1, false));
		filterPanel = new FilteringPanel(this, SWT.NONE);
		filterPanel.setLayoutData(new GridData(SWT.FILL, SWT.BEGINNING, true, false));
		table = new DataTable(this, SWT.MULTI, type);
		table.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
	}

	protected void configureFilterPanel() {
		SelectionListener selectionListener = new SelectionAdapter() {
			public void widgetSelected(SelectionEvent e) {
				runFilter();
			}
			public void widgetDefaultSelected(SelectionEvent e) {
				runFilter();
			}
		};
			
		// when the filter button gets pressed, update the table
		filterPanel.getFilterButton().addSelectionListener(selectionListener);
		filterPanel.getFileNameCombo().addSelectionListener(selectionListener);
		filterPanel.getRunNameCombo().addSelectionListener(selectionListener);
		filterPanel.getExperimentNameCombo().addSelectionListener(selectionListener);
		filterPanel.getMeasurementNameCombo().addSelectionListener(selectionListener);
		filterPanel.getReplicationNameCombo().addSelectionListener(selectionListener);
		filterPanel.getModuleNameCombo().addSelectionListener(selectionListener);
		filterPanel.getNameCombo().addSelectionListener(selectionListener);
	}

	protected void updateFilterCombos() {
		filterPanel.setFilterHints(getFilterHints());
	}
	
	public FilterHints getFilterHints() {
		FilterHints hints = new FilterHints();
		ResultFileManagerEx manager = table.getResultFileManager();
		ResultFileList fileList = manager.getUniqueFiles(idlist);
		RunList runList = manager.getUniqueRuns(idlist);
		
		hints.setFileNameHints(getFileNameFilterHints(fileList));
		hints.setRunNameHints(getRunNameFilterHints(runList));
		hints.setModuleNameHints(manager.getModuleFilterHints(idlist).toArray());
		hints.setDataNameHints(manager.getNameFilterHints(idlist).toArray());
		hints.setExperimentNameHints(getFilterHintsForRunAttribute(runList, EXPERIMENT));
		hints.setMeasurementNameHints(getFilterHintsForRunAttribute(runList, MEASUREMENT));
		hints.setReplicationNameHints(getFilterHintsForRunAttribute(runList, REPLICATION));
		
		return hints;
	}
	
	private static String[] getFileNameFilterHints(ResultFileList fileList) {
		String[] hints = new String[(int)fileList.size() + 1];
		hints[0] = "*";
		for (int i = 0; i < fileList.size(); ++i)
			hints[i+1] = fileList.get(i).getFilePath();
		return hints;
	}
	
	private static String[] getRunNameFilterHints(RunList runList) {
		StringVector hints = new StringVector();
		hints.add("*");
		for (int i = 0; i < runList.size(); ++i) {
			String runName = runList.get(i).getRunName();
			if (runName.length() > 0)
				hints.add(runName);
		}
		return hints.toArray();
	}
	
	private String[] getFilterHintsForRunAttribute(RunList runList, String attrName) {
		ResultFileManagerEx manager = table.getResultFileManager();
		StringVector values = manager.getUniqueAttributeValues(runList, attrName);
		String[] filterHints = new String[(int)values.size() + 1];
		filterHints[0] = "*";
		for (int i = 0; i < values.size(); ++i)
			filterHints[i+1] = values.get(i);
		return filterHints;
	}
	
	protected void runFilter() {
		// run the filter on the unfiltered IDList, and set the result to the table
		ResultFileManagerEx manager = table.getResultFileManager();
		FilterParams params = getFilterParams();
		ResultFileList fileList = params.getFileNamePattern().length() > 0 ?
				manager.filterFileList(manager.getFiles(), params.getFileNamePattern()) : null;
		StringMap attrs = new StringMap();
		addAttribute(attrs, EXPERIMENT, params.getExperimentNamePattern());
		addAttribute(attrs, MEASUREMENT, params.getMeasurementNamePattern());
		addAttribute(attrs, REPLICATION, params.getReplicationNamePattern());
		String runNamePattern = params.getRunNamePattern().length() > 0 ? params.getRunNamePattern() : "*";
		RunList runList = params.getRunNamePattern().length() > 0 || attrs.size() > 0 ?
				manager.filterRunList(manager.getRuns(), runNamePattern, attrs) : null;
		FileRunList fileRunFilter = manager.getFileRuns(fileList, runList);
		IDList filteredIDList = manager.filterIDList(idlist,
				fileRunFilter, params.getModuleNamePattern(), params.getDataNamePattern());
		
		table.setIDList(filteredIDList);

		filterPanel.setFilterText(getFilterParams().toString());
	}
	
	private static void addAttribute(StringMap attrs, String name, String value) {
		if (value != null && value.length() > 0)
			attrs.set(name, value);
	}

	public FilterParams getFilterParams() {
		return filterPanel.getFilterParams();
	}
	
	public void setFilterParams(FilterParams params) {
		filterPanel.setFilterParams(params);
		runFilter();
	}

	/**
	 * This method initializes formToolkit	
	 * 	
	 * @return org.eclipse.ui.forms.widgets.FormToolkit	
	 */
	private FormToolkit getFormToolkit() {
		if (formToolkit == null) {
			formToolkit = new FormToolkit(Display.getCurrent());
		}
		return formToolkit;
	}
}

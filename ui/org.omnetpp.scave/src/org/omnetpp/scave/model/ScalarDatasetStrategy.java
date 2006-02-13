package org.omnetpp.scave.model;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.widgets.Composite;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.panel.FilterPanelComposite;
import org.omnetpp.scave.panel.ScalarPanelComposite;
import org.omnetpp.scave.panel.ScalarTableContentSorter;
import org.omnetpp.scave.panel.ScalarTableLabelProvider;
import org.omnetpp.scave.panel.TableContentSorter;
import org.omnetpp.scave.plugin.ScavePlugin;

public class ScalarDatasetStrategy implements IDatasetStrategy {

	public FilterPanel createFilterPanel(Composite parent, int style) {
		return new FilterPanel(parent, style, this);
	}

	public FilterPanelComposite createFilterPanelComposite(Composite parent, int style) {
			return new ScalarPanelComposite(parent, style);
	}

	public ITableLabelProvider createFilterPanelLabelProvider() {
		return new ScalarTableLabelProvider();
	}

	public IDList getAll() {
		ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
		return resultFileManager.getAllScalars();
	}

	public File loadResultFile(String fileName) {
		ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
		return resultFileManager.loadScalarFile(fileName);
	}

	public boolean isCompatible(IDList idList) {
		return idList.areAllScalars();
	}

	public TableContentSorter createTableSorter() {
		return new ScalarTableContentSorter();
	}
}

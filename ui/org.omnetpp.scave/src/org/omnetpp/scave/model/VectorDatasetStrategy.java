package org.omnetpp.scave.model;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.widgets.Composite;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.panel.FilterPanelComposite;
import org.omnetpp.scave.panel.TableContentSorter;
import org.omnetpp.scave.panel.VectorPanelComposite;
import org.omnetpp.scave.panel.VectorTableContentSorter;
import org.omnetpp.scave.panel.VectorTableLabelProvider;
import org.omnetpp.scave.plugin.ScavePlugin;


public class VectorDatasetStrategy implements IDatasetStrategy {

	public FilterPanel createFilterPanel(Composite parent, int style) {
		return new FilterPanel(parent, style, this);
	}

	public FilterPanelComposite createFilterPanelComposite(Composite parent, int style) {
		return new VectorPanelComposite(parent, style);
	}

	public ITableLabelProvider createFilterPanelLabelProvider() {
		return new VectorTableLabelProvider();
	}

	public IDList getAll() {
		ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
		return resultFileManager.getAllVectors();
	}

	public File loadResultFile(String fileName) {
		ResultFileManager resultFileManager = ScavePlugin.getDefault().resultFileManager;
		return resultFileManager.loadVectorFile(fileName);
	}

	public boolean isCompatible(IDList idList) {
		return idList.areAllVectors();
	}

	public TableContentSorter createTableSorter() {
		return new VectorTableContentSorter();
	}

}

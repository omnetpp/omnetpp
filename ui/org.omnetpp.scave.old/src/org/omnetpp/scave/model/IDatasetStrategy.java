/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.model;

import org.eclipse.jface.viewers.ITableLabelProvider;
import org.eclipse.swt.widgets.Composite;
import org.jfree.chart.JFreeChart;

import org.omnetpp.scave.engine.File;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.panel.FilterPanel;
import org.omnetpp.scave.panel.FilterPanelComposite;
import org.omnetpp.scave.panel.TableContentSorter;


/**
 * Abstraction layer to hide differences between handling scalars and vectors.
 */
public interface IDatasetStrategy {
    /**
     * Creates a FilterPanel for scalars or for vectors.
     */
    public FilterPanel createFilterPanel(Composite parent, int style);

    /**
     * Creates a ScalarPanelComposite or VectorPanelComposite.
     */
    public FilterPanelComposite createFilterPanelComposite(Composite parent, int style);

    /**
     * Creates a ScalarTableLabelProvider or VectorTableLabelProvider.
     */
    public ITableLabelProvider createFilterPanelLabelProvider();

    /**
     * Creates a ScalarTableContentSorter or VectorTableContentSorter.
     */
    public TableContentSorter createTableSorter();

    /**
     * Invokes ResultFileManager.getAllScalars() or getAllVectors().
     */
    public IDList getAll();

    /**
     * Invokes ResultFileManager.loadScalarFile() or loadVectorFile().
     */
    public File loadResultFile(String fileName);

    public boolean isCompatible(IDList idList);

    public JFreeChart createEmptyChart();

    public void updateDataset(JFreeChart chart, IDList idlist);
}

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.editors;


import org.omnetpp.scave.model.IDatasetStrategy;
import org.omnetpp.scave.model.ScalarDatasetStrategy;


/**
 * Editor for scalar datasets.
 */
public class ScalarChartEditor extends DatasetEditor {

    private static IDatasetStrategy strategy = new ScalarDatasetStrategy();

    public ScalarChartEditor() {
        super(strategy,
            new IDatasetEditorPage[] {
                new FilterPanelPage(strategy),
                new ChartPage(strategy)
            });
    }
}

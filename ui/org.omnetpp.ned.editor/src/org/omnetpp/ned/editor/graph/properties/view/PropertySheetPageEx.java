package org.omnetpp.ned.editor.graph.properties.view;

import org.eclipse.ui.views.properties.PropertySheetPage;
import org.eclipse.ui.views.properties.PropertySheetSorter;

/**
 * @author rhornig
 * Special property sheet page which overrides the sorter algorithm. 
 * This may be a bug in eclipse (ie. setSorter is declared a protected)
 */
public class PropertySheetPageEx extends PropertySheetPage {

    @Override
    public void setSorter(PropertySheetSorter sorter) {
        super.setSorter(sorter);
    }
}

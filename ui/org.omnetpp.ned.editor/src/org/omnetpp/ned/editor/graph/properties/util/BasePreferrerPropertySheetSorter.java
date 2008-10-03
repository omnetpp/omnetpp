package org.omnetpp.ned.editor.graph.properties.util;

import org.eclipse.ui.views.properties.PropertySheetSorter;

/**
 * This class shorts the properties almost alphabetically EXCEPT it places
 * the "Base" category at the beginning of the sheet.
 *
 * @author rhornig
 */
public class BasePreferrerPropertySheetSorter extends PropertySheetSorter {

    @Override
    public int compareCategories(String categoryA, String categoryB) {
        if ("Base".equalsIgnoreCase(categoryA))
            return -1;
        if ("Base".equalsIgnoreCase(categoryB))
            return 1;
        // otherwise use the original sorting
        return super.compareCategories(categoryA, categoryB);
    }
}

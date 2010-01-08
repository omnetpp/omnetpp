/**
 *
 */
package org.omnetpp.scave.actions;

import java.util.Arrays;

import org.eclipse.jface.action.Action;
import org.omnetpp.scave.editors.datatable.DataTree;

public class PredefinedLevels1Action extends Action {
    private final DataTree dataTree;

    public PredefinedLevels1Action(String text, int style, DataTree dataTree) {
        super(text, style);
        this.dataTree = dataTree;
    }

    @Override
    public boolean isChecked() {
        setChecked(Arrays.equals(dataTree.getContentProvider().getLevels(), dataTree.getContentProvider().getPredefinedLevels1()));
        return super.isChecked();
    }

    @SuppressWarnings("unchecked")
    @Override
    public void run() {
        if (!isChecked()) {
            dataTree.getContentProvider().setLevels(dataTree.getContentProvider().getPredefinedLevels1());
            dataTree.refresh();
        }
    }
}
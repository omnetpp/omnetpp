/**
 *
 */
package org.omnetpp.scave.actions;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.Action;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.ResultFileManagerTreeContentProvider;

public class FlatModuleTreeAction extends Action {
    private final DataTree dataTree;

    public FlatModuleTreeAction(String text, int style, DataTree dataTree) {
        super(text, style);
        this.dataTree = dataTree;
    }

    @Override
    public boolean isChecked() {
        setChecked(ArrayUtils.indexOf(dataTree.getContentProvider().getLevels(), ResultFileManagerTreeContentProvider.ModulePathNode.class) != -1);
        return super.isChecked();
    }

    @SuppressWarnings("unchecked")
    @Override
    public void run() {
        ResultFileManagerTreeContentProvider contentProvider = dataTree.getContentProvider();
        Class[] levels = contentProvider.getLevels().clone();
        Class oldClass = isChecked() ? ResultFileManagerTreeContentProvider.ModulePathNode.class : ResultFileManagerTreeContentProvider.ModuleNameNode.class;
        Class newClass = !isChecked() ? ResultFileManagerTreeContentProvider.ModulePathNode.class : ResultFileManagerTreeContentProvider.ModuleNameNode.class;
        int index = ArrayUtils.indexOf(levels, oldClass);
        if (index != -1)
            levels[index] = newClass;
        dataTree.setLevels(levels);
    }
}
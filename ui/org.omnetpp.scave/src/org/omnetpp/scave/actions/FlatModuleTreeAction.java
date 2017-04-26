/**
 *
 */
package org.omnetpp.scave.actions;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.Action;
import org.omnetpp.scave.ScaveImages;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.DataTreeContentProvider;

/**
 *
 * @author levy
 */
//FIXME forditva mukodik!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
public class FlatModuleTreeAction extends Action {
    private final DataTree dataTree;

    public FlatModuleTreeAction(DataTree dataTree) {
        super("Flat Module Tree", Action.AS_CHECK_BOX);
        setImageDescriptor(ScavePlugin.getImageDescriptor(ScaveImages.IMG_ETOOL16_FLATMODULETREE));
        this.dataTree = dataTree;
    }

    @Override
    public boolean isChecked() {
        setChecked(ArrayUtils.indexOf(dataTree.getContentProvider().getLevels(), DataTreeContentProvider.ModulePathNode.class) != -1);
        return super.isChecked();
    }

    @Override
    @SuppressWarnings("rawtypes")
    public void run() {
        boolean flat = isChecked();
        DataTreeContentProvider contentProvider = dataTree.getContentProvider();
        Class[] levels = contentProvider.getLevels().clone();
        Class oldClass = flat ? DataTreeContentProvider.ModulePathNode.class : DataTreeContentProvider.ModuleNameNode.class;
        Class newClass = !flat ? DataTreeContentProvider.ModulePathNode.class : DataTreeContentProvider.ModuleNameNode.class;
        int index = ArrayUtils.indexOf(levels, oldClass);
        if (index != -1)
            levels[index] = newClass;
        dataTree.setLevels(levels);
    }
}
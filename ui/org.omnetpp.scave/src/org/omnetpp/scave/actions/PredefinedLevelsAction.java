/*--------------------------------------------------------------*
  Copyright (C) 2006-2020 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.actions;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.jface.action.Action;
import org.omnetpp.scave.editors.datatable.DataTree;
import org.omnetpp.scave.editors.datatable.DataTreeContentProvider;

@SuppressWarnings("rawtypes")
public class PredefinedLevelsAction extends Action {
    private final DataTree dataTree;

    private Class[] levels;

    public PredefinedLevelsAction(String text, DataTree dataTree, Class[] levels) {
        super(text, Action.AS_RADIO_BUTTON);
        this.dataTree = dataTree;
        this.levels = levels;
    }

    @Override
    public boolean isChecked() {
        setChecked(isLevelsEqualsIgnoreModuleNameLevel(dataTree.getContentProvider().getLevels(), levels));
        return super.isChecked();
    }

    @Override
    public void run() {
        if (!isChecked()) {
            DataTreeContentProvider contentProvider = dataTree.getContentProvider();
            Class[] currentLevels = contentProvider.getLevels();
            int index = ArrayUtils.indexOf(currentLevels, DataTreeContentProvider.ModuleNameNode.class);
            if (index == -1)
                dataTree.setLevels(levels);
            else {
                Class[] clonedLevels = levels.clone();
                index = ArrayUtils.indexOf(clonedLevels, DataTreeContentProvider.ModulePathNode.class);
                if (index != -1)
                    clonedLevels[index] = DataTreeContentProvider.ModuleNameNode.class;
                dataTree.setLevels(clonedLevels);
            }
        }
    }

    public static boolean isLevelsEqualsIgnoreModuleNameLevel(Class[] levels1, Class[] levels2) {
        if (levels1.length != levels2.length)
            return false;
        for (int i = 0; i < levels1.length; i++)
            if (!toModulePathNodeClass(levels1[i]).equals(toModulePathNodeClass(levels2[i])))
                return false;
        return true;
    }

    private static Class toModulePathNodeClass(Class clazz) {
        if (clazz.equals(DataTreeContentProvider.ModuleNameNode.class))
            return DataTreeContentProvider.ModulePathNode.class;
        else
            return clazz;
    }
}
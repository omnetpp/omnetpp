package com.simulcraft.test.gui.recorder.object;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.swt.widgets.MenuItem;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class MenuItemRecognizer extends ObjectRecognizer {
    public MenuItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof MenuItem) {
            MenuItem item = (MenuItem)uiObject;

            List<String> path = new ArrayList<String>();
            path.add(item.getText());
            MenuItem currentItem = item;
            while (currentItem.getParent().getParentItem() != null) {
                path.add(0, currentItem.getParent().getParentItem().getText());
                currentItem = currentItem.getParent().getParentItem();
            }

            // FIXME code not really good (e.g. parent is some widget NOT a shell)
            // an EventRecognizer should produce: control.chooseFromContextMenu(menuPath)
            String menuPath = StringUtils.join(path, "|");
            return makeSeq(item.getParent().getShell(), expr("findMenuItemByPath("+quote(menuPath)+")", 0.8, item));
        }
        return null;
    }
}
package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.TreeItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TreeItemRecognizer extends ObjectRecognizer {
    public TreeItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof TreeItem) {
            TreeItem item = (TreeItem)uiObject;
            //FIXME check label uniquely identifies item
            return makeSeq(item.getParent(), expr("findTreeItemByContent("+quote(item.getText())+")", 0.8, item));
        }
        return null;
    }
}
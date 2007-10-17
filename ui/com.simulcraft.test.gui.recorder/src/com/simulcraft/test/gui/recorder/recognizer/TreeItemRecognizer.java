package com.simulcraft.test.gui.recorder.recognizer;

import java.util.List;

import org.eclipse.swt.widgets.TreeItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TreeItemRecognizer extends ObjectRecognizer {
    public TreeItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        if (uiObject instanceof TreeItem) {
            TreeItem item = (TreeItem)uiObject;
            //FIXME check label uniquely identifies item
            return chain(recorder.identifyObject(item.getParent()), "findTreeItemByContent("+quote(item.getText())+")", 0.8);
        }
        return null;
    }
}
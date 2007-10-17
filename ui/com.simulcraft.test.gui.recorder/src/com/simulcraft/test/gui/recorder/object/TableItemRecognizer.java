package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.TableItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TableItemRecognizer extends ObjectRecognizer {
    public TableItemRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof TableItem) {
            TableItem item = (TableItem)uiObject;
            //FIXME check label uniquely identifies item
            return chainO(item.getParent(), "findTableItemByContent("+quote(item.getText())+")", 0.8);
        }
        return null;
    }
}
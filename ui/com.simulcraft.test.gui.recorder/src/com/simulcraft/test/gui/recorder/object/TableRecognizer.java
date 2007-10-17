package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TableRecognizer extends ObjectRecognizer {
    public TableRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence identifyObject(Object uiObject) {
        if (uiObject instanceof Table) {
            Table table = (Table)uiObject;
            if (findDescendantControl(table.getShell(), Table.class) == uiObject)
                return chainO(table.getShell(), "findTable()", 0.8);
        }
        return null;
    }
}
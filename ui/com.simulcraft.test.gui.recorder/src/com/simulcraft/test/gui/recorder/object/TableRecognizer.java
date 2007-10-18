package com.simulcraft.test.gui.recorder.object;

import org.eclipse.swt.widgets.Composite;
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
            Composite container = findContainer(table);
            if (findDescendantControl(container, Table.class) == uiObject)
                return makeSeq(container, expr("findTable()", 0.8, table));
        }
        return null;
    }
}
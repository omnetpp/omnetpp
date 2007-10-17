package com.simulcraft.test.gui.recorder.object;

import java.util.List;

import org.eclipse.swt.widgets.Table;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaExpr;

public class TableRecognizer extends ObjectRecognizer {
    public TableRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public List<JavaExpr> identifyObject(Object uiObject) {
        if (uiObject instanceof Table) {
            Table table = (Table)uiObject;
            if (findDescendantControl(table.getShell(), Table.class) == uiObject)
                return chain(recorder.identifyObject(table.getShell()), "findTable()", 0.8); // should be lower than TableItemRecognizer
        }
        return null;
    }
}
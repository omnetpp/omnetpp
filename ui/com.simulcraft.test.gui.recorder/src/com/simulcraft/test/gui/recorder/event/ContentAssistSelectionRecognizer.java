package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableItem;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;
import com.simulcraft.test.gui.recorder.object.ContentAssistPopupRecognizer;

public class ContentAssistSelectionRecognizer extends EventRecognizer {
    public ContentAssistSelectionRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.widget instanceof Table && (e.type == SWT.MouseDoubleClick) && e.button == 1) {
            Table table = (Table) e.widget;
            if (ContentAssistPopupRecognizer.isContentAssist(table) && getSelectedText(table) != null)
                return makeMethodCall(table, expr("chooseWithMouse("+quoteRegexText(getSelectedText(table))+")", 0.7, null));
        }
        if (e.widget instanceof Table && e.type == SWT.KeyDown && e.keyCode == SWT.CR) {
            //FIXME the following does not work, because the focusControl is the text NOT the content assist popup
            Table table = (Table) e.widget;
            if (ContentAssistPopupRecognizer.isContentAssist(table) && getSelectedText(table) != null)
                return makeMethodCall(table, expr("chooseWithKeyboard("+quoteRegexText(getSelectedText(table))+")", 0.7, null));
        }
        return null;
    }

    protected String getSelectedText(Table table) {
        TableItem[] selection = table.getSelection();
        return selection.length==0 ? null : selection[0].getText();
    }
   
}


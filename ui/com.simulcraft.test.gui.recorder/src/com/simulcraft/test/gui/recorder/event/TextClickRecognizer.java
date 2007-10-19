package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Text;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class TextClickRecognizer extends EventRecognizer {
    public TextClickRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        //FIXME do Combo and whatnot as well
        if (e.widget instanceof Text && (e.type == SWT.MouseDown || e.type == SWT.MouseDoubleClick) && e.button == 1) { // left click into a StyledText
            Text text = (Text) e.widget;
            return makeSeq(text, expr("clickRelative("+e.x+", "+e.y+")", 0.5, null));
        }
        return null;
    }
}


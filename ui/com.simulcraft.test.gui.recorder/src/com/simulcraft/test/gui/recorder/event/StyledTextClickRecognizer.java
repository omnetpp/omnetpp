package com.simulcraft.test.gui.recorder.event;

import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.widgets.Event;
import org.omnetpp.common.util.StringUtils;

import com.simulcraft.test.gui.recorder.GUIRecorder;
import com.simulcraft.test.gui.recorder.JavaSequence;

public class StyledTextClickRecognizer extends EventRecognizer {
    public StyledTextClickRecognizer(GUIRecorder recorder) {
        super(recorder);
    }

    public JavaSequence recognizeEvent(Event e) {
        if (e.widget instanceof StyledText && (e.type == SWT.MouseDown || e.type == SWT.MouseDoubleClick) && e.button == 1) { // left click into a StyledText
            StyledText styledText = (StyledText) e.widget;
            
            int offset;
            try {
                offset = styledText.getOffsetAtLocation(new Point(e.x, e.y));
            } catch (IllegalArgumentException ex) {
                //XXX no character at (x,y) -- what to do?
                return makeMethodCall(uiObject(e), expr("clickRelative("+e.x+", "+e.y+")", 0.5, null));
            }

            int lineNumber = styledText.getLineAtOffset(offset);
            int lineBegOffset = styledText.getOffsetAtLine(lineNumber);
            String linePrefix = styledText.getTextRange(lineBegOffset, offset-lineBegOffset);

            if (e.type == SWT.MouseDown) {
                //XXX the methods generated here don't exist on StyledText yet
                if (StringUtils.countMatches(styledText.getText(), linePrefix)==1)
                    return makeMethodCall(styledText, expr("clickAfter("+quoteRegexText(linePrefix)+")", 0.5, null));
                else
                    return makeMethodCall(styledText, expr("clickAtLineAndOffset("+lineNumber+", "+(offset-lineBegOffset)+")", 0.5, null));
            }
            else if (e.type == SWT.MouseDoubleClick) {
                //XXX the methods generated here don't exist on StyledText yet
                if (StringUtils.countMatches(styledText.getText(), linePrefix)==1)
                    return makeMethodCall(styledText, expr("doubleClickAfter("+quoteRegexText(linePrefix)+")", 0.5, null));
                else
                    return makeMethodCall(styledText, expr("doubleClickAtLineAndOffset("+lineNumber+", "+(offset-lineBegOffset)+")", 0.5, null));
            }
        }
        return null;
    }
}


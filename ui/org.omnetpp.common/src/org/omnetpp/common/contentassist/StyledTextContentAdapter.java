package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.IControlContentAdapter;
import org.eclipse.jface.fieldassist.IControlContentAdapter2;
import org.eclipse.swt.custom.StyledText;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Control;

/**
 * A copy of JFace's TextContentAdapter, adapted for StyledText widgets.
 * 
 * @author andras
 */
public class StyledTextContentAdapter implements IControlContentAdapter,
        IControlContentAdapter2, IControlContentAdapterEx {

    @Override
    public String getControlContents(Control control) {
        return ((StyledText) control).getText();
    }

    @Override
    public void setControlContents(Control control, String text, int cursorPosition) {
        System.out.println("StyledTextContentAdapter.setControlContents(" + control + ", '" + text + "', cursorPos=" + cursorPosition + ")");
        ((StyledText) control).setText(text);
        ((StyledText) control).setSelection(cursorPosition, cursorPosition);
    }

    @Override
    public void insertControlContents(Control control, String text, int cursorPosition) {
        System.out.println("StyledTextContentAdapter.insertControlContents(" + control + ", '" + text + "', cursorPos=" + cursorPosition + ")");
        Point selection = ((StyledText) control).getSelection();
        ((StyledText) control).insert(text);
        // Insert will leave the cursor at the end of the inserted text. If this is not what we wanted, reset the selection.
        if (cursorPosition < text.length())
            ((StyledText) control).setSelection(selection.x + cursorPosition, selection.x + cursorPosition);
    }

    @Override
    public int getCursorPosition(Control control) {
        return ((StyledText) control).getCaretOffset();
    }

    @Override
    public Rectangle getInsertionBounds(Control control) {
        StyledText text = (StyledText) control;
        Point caretOrigin = text.getLocationAtOffset(text.getCaretOffset());
        // We fudge the y pixels due to problems with getCaretLocation. See https://bugs.eclipse.org/bugs/show_bug.cgi?id=52520
        return new Rectangle(caretOrigin.x + text.getClientArea().x, caretOrigin.y + text.getClientArea().y, 1, text.getLineHeight());
    }

    @Override
    public void setCursorPosition(Control control, int position) {
        System.out.println("StyledTextContentAdapter.setCursorPosition(" + control + ", " + position + ")");
        ((StyledText) control).setSelection(new Point(position, position));
    }

    @Override
    public Point getSelection(Control control) {
        return ((StyledText) control).getSelection();
    }

    @Override
    public void setSelection(Control control, Point range) {
        ((StyledText) control).setSelection(range);
    }

    public void replaceControlContents(Control control, int start, int end, String text, int cursorPosition) {
        System.out.println("StyledTextContentAdapter.replaceControlContents(" + control + ", start=" + start + ", end=" + end + ", '" + text + "', " + cursorPosition + ")");
        StyledText textControl = (StyledText)control;
        textControl.setSelection(start, end);
        textControl.insert(text);
        textControl.setSelection(start + cursorPosition, start + cursorPosition);
    }
    
}

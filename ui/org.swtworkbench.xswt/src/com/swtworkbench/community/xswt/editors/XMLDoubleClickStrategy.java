package com.swtworkbench.community.xswt.editors;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextDoubleClickStrategy;
import org.eclipse.jface.text.ITextViewer;

public class XMLDoubleClickStrategy implements ITextDoubleClickStrategy {
    protected ITextViewer fText;

    public void doubleClicked(ITextViewer part) {
        int pos = part.getSelectedRange().x;

        if (pos < 0) {
            return;
        }
        this.fText = part;

        if (!(selectComment(pos)))
            selectWord(pos);
    }

    protected boolean selectComment(int caretPos) {
        IDocument doc = this.fText.getDocument();
        try {
            int pos = caretPos;
            char c = ' ';

            while (pos >= 0) {
                c = doc.getChar(pos);
                if (c == '\\') {
                    pos -= 2;
                }
                else {
                    if (c == '\r')
                        break;
                    if (c == '"')
                        break;
                    --pos;
                }
            }
            if (c != '"') {
                return false;
            }
            int startPos = pos;

            pos = caretPos;
            int length = doc.getLength();
            c = ' ';

            while (pos < length) {
                c = doc.getChar(pos);
                if (c == '\r')
                    break;
                if (c == '"')
                    break;
                ++pos;
            }
            if (c != '"') {
                return false;
            }
            int endPos = pos;

            int offset = startPos + 1;
            int len = endPos - offset;
            this.fText.setSelectedRange(offset, len);
            return true;
        }
        catch (BadLocationException localBadLocationException) {
        }
        return false;
    }

    protected boolean selectWord(int caretPos) {
        IDocument doc = this.fText.getDocument();
        try {
            char c;
            int pos = caretPos;

            while (pos >= 0) {
                c = doc.getChar(pos);
                if (!(Character.isJavaIdentifierPart(c)))
                    break;
                --pos;
            }

            int startPos = pos;

            pos = caretPos;
            int length = doc.getLength();

            while (pos < length) {
                c = doc.getChar(pos);
                if (!(Character.isJavaIdentifierPart(c)))
                    break;
                ++pos;
            }

            int endPos = pos;
            selectRange(startPos, endPos);
            return true;
        }
        catch (BadLocationException localBadLocationException) {
        }
        return false;
    }

    private void selectRange(int startPos, int stopPos) {
        int offset = startPos + 1;
        int length = stopPos - offset;
        this.fText.setSelectedRange(offset, length);
    }
}
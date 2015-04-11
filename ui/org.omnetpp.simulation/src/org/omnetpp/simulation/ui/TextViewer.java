/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.simulation.ui;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.ISelectionChangedListener;
import org.eclipse.jface.viewers.ISelectionProvider;
import org.eclipse.swt.SWT;
import org.eclipse.swt.SWTError;
import org.eclipse.swt.custom.ST;
import org.eclipse.swt.dnd.Clipboard;
import org.eclipse.swt.dnd.DND;
import org.eclipse.swt.dnd.TextTransfer;
import org.eclipse.swt.dnd.Transfer;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Cursor;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontMetrics;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Canvas;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Event;
import org.eclipse.swt.widgets.Listener;
import org.eclipse.swt.widgets.ScrollBar;
import org.omnetpp.common.ui.SelectionProvider;


/**
 * For efficient viewing of a large amount of text. We use this instead of
 * StyledText plus a custom ITextViewerContent class, because StyledTextRenderer
 * is very slow. Its throughput is about 20..30 events/sec, while this class
 * has about 200..300 (counting with average 2 lines/event). Also, this class
 * has very low and constant memory consumption (StyledTextRenderer consumes
 * minimum 8 bytes per line, see lineWidth[] and lineHeight[] arrays).
 *
 * @author Andras
 */
//TODO widget should always add an extra newline to the content!!!!
//TODO provider should provide lines WITHOUT newline!!!
//TODO Ctrl+A should work!
//TODO Ctrl+C should prompt the user if content is too large, e.g. >10MB (MEssageDialog.openConfirm())
//TODO cannot move cursor to linestart below content!
//TODO asserts on empty content!
//TODO Module Output view-nak kovetnie kellene az uj kiirasokat (scroll to end), ha eleve a vegen allt a cursor
//TODO minor glitches with word selection (esp with single-letter words)
//TODO revealCaret doesn't scroll horizontally
//TODO finish horiz scrolling!
//TODO implement selectionprovider stuff
public class TextViewer extends Canvas implements ISelectionProvider {
    private static final Cursor CURSOR_IBEAM = new Cursor(Display.getDefault(), SWT.CURSOR_IBEAM); //TODO into some shared resource file
    protected ITextViewerContentProvider content;
    protected ITextChangeListener textChangeListener;
    protected Font font;
    protected Color backgroundColor;
    protected Color foregroundColor;
    protected Color selectionBackgroundColor;
    protected Color selectionForegroundColor;
    protected int leftMargin = 0;
    protected int lineHeight, averageCharWidth; // measured from font
    protected boolean isMonospaceFont;
    protected int topLineIndex;
    protected int topLineY = 0; // Y coordinate of where top edge of line topLineIndex gets painted (range: -(lineHeight-1)..0)
    protected int horizontalScrollOffset; // in pixels
    protected int caretLineIndex, caretColumn;  // caretColumn may be greater than line length! ()
    protected int selectionAnchorLineIndex, selectionAnchorColumn; // selection is between anchor and caret
    protected Map<Integer,Integer> keyActionMap = new HashMap<Integer, Integer>(); // key: keycode, value: ST.xxx constants
    protected Listener listener;
    protected int clickCount;
    protected Clipboard clipboard;
    protected ISelectionProvider selectionProvider = new SelectionProvider();
    protected Runnable caretBlinkTimer;
    protected boolean caretShown = true; // during blinking
    protected int autoScrollDirection = SWT.NULL;   // the direction of autoscrolling (up, down, right, left)
    protected int autoScrollDistance = 0; // currently unused
    protected Runnable autoScrollTimer = null;

    protected static final int MAX_CLIPBOARD_SIZE = 100*1024*1024; // 100 MiB
    protected static final int CARET_BLINK_DELAY = 500;
    protected static final int V_SCROLL_RATE = 50;
    protected static final int H_SCROLL_RATE = 10;

    protected final static boolean IS_CARBON, IS_GTK, IS_MOTIF;
    static {
        String platform = SWT.getPlatform();
        IS_CARBON = "carbon".equals(platform);
        IS_GTK = "gtk".equals(platform);
        IS_MOTIF = "motif".equals(platform);
    }


    public TextViewer(Composite parent, int style) {
        super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL | SWT.DOUBLE_BUFFERED | SWT.NO_BACKGROUND);

        backgroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_BACKGROUND);
        foregroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_FOREGROUND);
        selectionBackgroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_SELECTION);
        selectionForegroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);

        //setFont(JFaceResources.getTextFont());
        //setFont(JFaceResources.getDefaultFont());
        //setFont(new Font(getDisplay(), "Courier New", 8, SWT.NORMAL));
        setFont(new Font(getDisplay(), "Arial", 8, SWT.NORMAL));

        setCursor(CURSOR_IBEAM);

        clipboard = new Clipboard(getDisplay());

        textChangeListener = new ITextChangeListener() {
            //@Override
            public void textChanged(ITextViewerContentProvider textViewer) {
                contentChanged();
            }
        };

        installListeners();

        createKeyBindings();

        // cursor blinking
        caretBlinkTimer = new Runnable() {
            public void run() {
                if (!isDisposed()) {
                    caretShown = !caretShown;
                    redraw();
                    Display.getCurrent().timerExec(CARET_BLINK_DELAY, this);
                }
            }
        };
        caretShown = false;  // gets shown on a FocusIn event
    }

    protected void installListeners() {
        listener = new Listener() {
            public void handleEvent(Event event) {
                switch (event.type) {
                    case SWT.Dispose: handleDispose(event); break;
                    case SWT.KeyDown: handleKeyDown(event); break;
                    case SWT.KeyUp: handleKeyUp(event); break;
                    case SWT.MouseDown: handleMouseDown(event); break;
                    case SWT.MouseUp: handleMouseUp(event); break;
                    case SWT.MouseMove: handleMouseMove(event); break;
                    case SWT.MouseWheel: handleMouseWheel(event); break;
                    case SWT.Paint: handlePaint(event); break;
                    case SWT.Resize: handleResize(event); break;
                    case SWT.FocusIn: handleFocusIn(event); break;
                    case SWT.FocusOut: handleFocusOut(event); break;
                }
            }
        };
        addListener(SWT.Dispose, listener);
        addListener(SWT.KeyDown, listener);
        addListener(SWT.KeyUp, listener);
        addListener(SWT.MouseDown, listener);
        addListener(SWT.MouseUp, listener);
        addListener(SWT.MouseMove, listener);
        addListener(SWT.MouseWheel, listener);
        addListener(SWT.Paint, listener);
        addListener(SWT.Resize, listener);
        addListener(SWT.FocusIn, listener);
        addListener(SWT.FocusOut, listener);

        ScrollBar verticalBar = getVerticalBar();
        if (verticalBar != null) {
            verticalBar.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    handleVerticalScroll(event);
                }
            });
        }
        ScrollBar horizontalBar = getHorizontalBar();
        if (horizontalBar != null) {
            horizontalBar.addListener(SWT.Selection, new Listener() {
                public void handleEvent(Event event) {
                    handleHorizontalScroll(event);
                }
            });
        }
    }

    // code from StyledText
    protected void createKeyBindings() {
        int nextKey = SWT.ARROW_RIGHT;
        int previousKey = SWT.ARROW_LEFT;

        // Navigation
        setKeyBinding(SWT.ARROW_UP, ST.LINE_UP);
        setKeyBinding(SWT.ARROW_DOWN, ST.LINE_DOWN);
        if (IS_CARBON) {
            setKeyBinding(previousKey | SWT.MOD1, ST.LINE_START);
            setKeyBinding(nextKey | SWT.MOD1, ST.LINE_END);
            setKeyBinding(SWT.HOME, ST.TEXT_START);
            setKeyBinding(SWT.END, ST.TEXT_END);
            setKeyBinding(SWT.ARROW_UP | SWT.MOD1, ST.TEXT_START);
            setKeyBinding(SWT.ARROW_DOWN | SWT.MOD1, ST.TEXT_END);
            setKeyBinding(nextKey | SWT.MOD3, ST.WORD_NEXT);
            setKeyBinding(previousKey | SWT.MOD3, ST.WORD_PREVIOUS);
        } else {
            setKeyBinding(SWT.HOME, ST.LINE_START);
            setKeyBinding(SWT.END, ST.LINE_END);
            setKeyBinding(SWT.HOME | SWT.MOD1, ST.TEXT_START);
            setKeyBinding(SWT.END | SWT.MOD1, ST.TEXT_END);
            setKeyBinding(nextKey | SWT.MOD1, ST.WORD_NEXT);
            setKeyBinding(previousKey | SWT.MOD1, ST.WORD_PREVIOUS);
        }
        setKeyBinding(SWT.PAGE_UP, ST.PAGE_UP);
        setKeyBinding(SWT.PAGE_DOWN, ST.PAGE_DOWN);
        setKeyBinding(SWT.PAGE_UP | SWT.MOD1, ST.WINDOW_START);
        setKeyBinding(SWT.PAGE_DOWN | SWT.MOD1, ST.WINDOW_END);
        setKeyBinding(nextKey, ST.COLUMN_NEXT);
        setKeyBinding(previousKey, ST.COLUMN_PREVIOUS);

        // Selection
        setKeyBinding('A' | SWT.MOD1, ST.SELECT_ALL);
        setKeyBinding(SWT.ARROW_UP | SWT.MOD2, ST.SELECT_LINE_UP);
        setKeyBinding(SWT.ARROW_DOWN | SWT.MOD2, ST.SELECT_LINE_DOWN);
        if (IS_CARBON) {
            setKeyBinding(previousKey | SWT.MOD1 | SWT.MOD2, ST.SELECT_LINE_START);
            setKeyBinding(nextKey | SWT.MOD1 | SWT.MOD2, ST.SELECT_LINE_END);
            setKeyBinding(SWT.HOME | SWT.MOD2, ST.SELECT_TEXT_START);
            setKeyBinding(SWT.END | SWT.MOD2, ST.SELECT_TEXT_END);
            setKeyBinding(SWT.ARROW_UP | SWT.MOD1 | SWT.MOD2, ST.SELECT_TEXT_START);
            setKeyBinding(SWT.ARROW_DOWN | SWT.MOD1 | SWT.MOD2, ST.SELECT_TEXT_END);
            setKeyBinding(nextKey | SWT.MOD2 | SWT.MOD3, ST.SELECT_WORD_NEXT);
            setKeyBinding(previousKey | SWT.MOD2 | SWT.MOD3, ST.SELECT_WORD_PREVIOUS);
        } else  {
            setKeyBinding(SWT.HOME | SWT.MOD2, ST.SELECT_LINE_START);
            setKeyBinding(SWT.END | SWT.MOD2, ST.SELECT_LINE_END);
            setKeyBinding(SWT.HOME | SWT.MOD1 | SWT.MOD2, ST.SELECT_TEXT_START);
            setKeyBinding(SWT.END | SWT.MOD1 | SWT.MOD2, ST.SELECT_TEXT_END);
            setKeyBinding(nextKey | SWT.MOD1 | SWT.MOD2, ST.SELECT_WORD_NEXT);
            setKeyBinding(previousKey | SWT.MOD1 | SWT.MOD2, ST.SELECT_WORD_PREVIOUS);
        }
        setKeyBinding(SWT.PAGE_UP | SWT.MOD2, ST.SELECT_PAGE_UP);
        setKeyBinding(SWT.PAGE_DOWN | SWT.MOD2, ST.SELECT_PAGE_DOWN);
        setKeyBinding(SWT.PAGE_UP | SWT.MOD1 | SWT.MOD2, ST.SELECT_WINDOW_START);
        setKeyBinding(SWT.PAGE_DOWN | SWT.MOD1 | SWT.MOD2, ST.SELECT_WINDOW_END);
        setKeyBinding(nextKey | SWT.MOD2, ST.SELECT_COLUMN_NEXT);
        setKeyBinding(previousKey | SWT.MOD2, ST.SELECT_COLUMN_PREVIOUS);

        // Clipboard
        setKeyBinding('C' | SWT.MOD1, ST.COPY);
    }

    // code from StyledText
    public void setKeyBinding(int key, int action) {
        checkWidget();
        int modifierValue = key & SWT.MODIFIER_MASK;
        char keyChar = (char)(key & SWT.KEY_MASK);
        if (Character.isLetter(keyChar)) {
            // make the keybinding case insensitive by adding it
            // in its upper and lower case form
            char ch = Character.toUpperCase(keyChar);
            int newKey = ch | modifierValue;
            if (action == SWT.NULL) {
                keyActionMap.remove(new Integer(newKey));
            } else {
                keyActionMap.put(new Integer(newKey), new Integer(action));
            }
            ch = Character.toLowerCase(keyChar);
            newKey = ch | modifierValue;
            if (action == SWT.NULL) {
                keyActionMap.remove(new Integer(newKey));
            } else {
                keyActionMap.put(new Integer(newKey), new Integer(action));
            }
        } else {
            if (action == SWT.NULL) {
                keyActionMap.remove(new Integer(key));
            } else {
                keyActionMap.put(new Integer(key), new Integer(action));
            }
        }
    }

    // code from StyledText
    public int getKeyBinding(int key) {
        checkWidget();
        Integer action = (Integer) keyActionMap.get(new Integer(key));
        return action == null ? SWT.NULL : action.intValue();
    }

    private static int clip(int lower, int upper, int x) {
        if (x < lower) x = lower;
        if (x > upper) x = upper;
        return x;
    }

    // code from StyledText
    protected void handleDispose(Event event) {
        removeListener(SWT.Dispose, listener);
        notifyListeners(SWT.Dispose, event);
        event.type = SWT.None;
        if (content != null)
            content.removeTextChangeListener(textChangeListener);
        clipboard.dispose();
        clipboard = null;
    }

    protected void handleKeyDown(Event event) {
        handleKey(event);
    }

    protected void handleKeyUp(Event event) {
        // nothing to do
    }

    // code from StyledText
    protected void handleKey(Event event) {
        int action;
        if (event.keyCode != 0) {
            // special key pressed (e.g., F1)
            action = getKeyBinding(event.keyCode | event.stateMask);
        } else {
            // character key pressed
            action = getKeyBinding(event.character | event.stateMask);
            if (action == SWT.NULL) {
                // see if we have a control character
                if ((event.stateMask & SWT.CTRL) != 0 && (event.character >= 0) && event.character <= 31) {
                    // get the character from the CTRL+char sequence, the control
                    // key subtracts 64 from the value of the key that it modifies
                    int c = event.character + 64;
                    action = getKeyBinding(c | event.stateMask);
                }
            }
        }
        if (action != SWT.NULL) {
            invokeAction(action);
        }
    }

    protected void invokeAction(int action) {
        switch (action) {
            // Navigation
            case ST.LINE_UP: doLineUp(false); break;
            case ST.LINE_DOWN: doLineDown(false); break;
            case ST.LINE_START: doLineStart(false); break;
            case ST.LINE_END: doLineEnd(false); break;
            case ST.COLUMN_PREVIOUS: doCursorPrevious(false); break;
            case ST.COLUMN_NEXT:  doCursorNext(false); break;
            case ST.PAGE_UP: doPageUp(false); break;
            case ST.PAGE_DOWN: doPageDown(false); break;
            case ST.WORD_PREVIOUS: doWordPrevious(false); break;
            case ST.WORD_NEXT: doWordNext(false); break;
            case ST.TEXT_START: doContentStart(false); break;
            case ST.TEXT_END: doContentEnd(false); break;
            case ST.WINDOW_START: doPageStart(false); break;
            case ST.WINDOW_END: doPageEnd(false); break;
            // Selection
            case ST.SELECT_ALL: selectAll(); break;
            case ST.SELECT_LINE_UP: doLineUp(true); break;
            case ST.SELECT_LINE_DOWN: doLineDown(true); break;
            case ST.SELECT_LINE_START: doLineStart(true); break;
            case ST.SELECT_LINE_END: doLineEnd(true); break;
            case ST.SELECT_COLUMN_PREVIOUS: doCursorPrevious(true); break;
            case ST.SELECT_COLUMN_NEXT:  doCursorNext(true); break;
            case ST.SELECT_PAGE_UP: doPageUp(true); break;
            case ST.SELECT_PAGE_DOWN: doPageDown(true); break;
            case ST.SELECT_WORD_PREVIOUS: doWordPrevious(true); break;
            case ST.SELECT_WORD_NEXT: doWordNext(true); break;
            case ST.SELECT_TEXT_START: doContentStart(true); break;
            case ST.SELECT_TEXT_END: doContentEnd(true); break;
            case ST.SELECT_WINDOW_START: doPageStart(true); break;
            case ST.SELECT_WINDOW_END: doPageEnd(true); break;
            // Modification
            case ST.COPY: copy(); break;
        }
        revealCaret();
        adjustScrollbars();
        restartCaretBlinking();  // make cursor always visible while user holds down arrow keys
        redraw();
    }

    /**
     * Scroll the caret into view
     */
    protected void revealCaret() {
        if (caretLineIndex < topLineIndex)
            topLineIndex = caretLineIndex;
        if (caretLineIndex >= topLineIndex + getNumVisibleLines())
            topLineIndex = caretLineIndex - getNumVisibleLines() + 1;
        topLineIndex = clip(0, Math.max(0, content.getLineCount()-getNumVisibleLines()), topLineIndex);

        // if caret is in the top or bottom line, view that line fully
        if (caretLineIndex==topLineIndex)
            alignTopLine();
        else if (caretLineIndex==topLineIndex+getNumVisibleLines()-1)
            alignBottomLine();

        // TODO with columns too
//        if (caretColumn < )
//            topLineIndex = caretLineIndex;
//        if (caretLineIndex >= topLineIndex + getNumVisibleLines())
//            topLineIndex = caretLineIndex - getNumVisibleLines() + 1;
    }

    protected void doLineUp(boolean select) {
        caretLineIndex = Math.max(0, caretLineIndex-1);
        if (!select) clearSelection();
    }

    protected void doLineDown(boolean select) {
        caretLineIndex = clip(0, content.getLineCount()-1, caretLineIndex+1);
        if (!select) clearSelection();
    }

    protected void doCursorPrevious(boolean select) {
        caretColumn = Math.min(caretColumn, content.getLineText(caretLineIndex).length());
        if (caretColumn > 0)
            caretColumn--;
        else if (caretLineIndex > 0) {
            caretLineIndex--;
            caretColumn = content.getLineText(caretLineIndex).length();
        }
        if (!select) clearSelection();
    }

    protected void doCursorNext(boolean select) {
        if (caretColumn < content.getLineText(caretLineIndex).length())
            caretColumn++;
        else if (caretLineIndex < content.getLineCount()-1) {
            caretLineIndex++;
            caretColumn = 0;
        }
        if (!select) clearSelection();
    }

    protected void doPageUp(boolean select) {
        int pageLines = Math.max(1, getNumVisibleLines()-1);
        caretLineIndex = Math.max(0, caretLineIndex - pageLines);
        topLineIndex = clip(0, Math.max(0,content.getLineCount()-getNumVisibleLines()), topLineIndex - pageLines);
        if (!select) clearSelection();
    }

    protected void doPageDown(boolean select) {
        int pageLines = Math.max(1, getNumVisibleLines()-1);
        int lastLineIndex = content.getLineCount()-1;
        caretLineIndex = Math.min(lastLineIndex, caretLineIndex + pageLines);
        topLineIndex = clip(0, Math.max(0,content.getLineCount()-getNumVisibleLines()), topLineIndex + pageLines);
        if (!select) clearSelection();
    }

    protected void doLineStart(boolean select) {
        caretColumn = 0;
        if (!select) clearSelection();
    }

    protected void doLineEnd(boolean select) {
        caretColumn = content.getLineText(caretLineIndex).length();
        if (!select) clearSelection();
    }

    protected void doWordPrevious(boolean select) {
        String line = content.getLineText(caretLineIndex);
        int pos = caretColumn;
        if (pos == 0) {
            // go to end of previous line
            if (caretLineIndex > 0) {
                caretLineIndex--;
                caretColumn = content.getLineText(caretLineIndex).length();
            }
        } else {
            // go to start of current or previous word
            pos--; // move at least one character
            while (pos > 0 && !isWordChar(line.charAt(pos-1)))
                pos--;
            while (pos > 0 && isWordChar(line.charAt(pos-1)))
                pos--;
            caretColumn = pos;
        }
        if (!select) clearSelection();
    }

    protected void doWordNext(boolean select) {
        String line = content.getLineText(caretLineIndex);
        int pos = caretColumn;
        if (pos == line.length()) {
            // go to start of next line
            if (caretLineIndex < content.getLineCount()-1) {
                caretLineIndex++;
                caretColumn = 0;
            }
        } else {
            // go to end of current or next word
            pos++; // move at least one character
            while (pos < line.length() && isWordChar(line.charAt(pos)))
                pos++;
            while (pos < line.length() && !isWordChar(line.charAt(pos)))
                pos++;
            caretColumn = pos;
        }
        if (!select) clearSelection();
    }

    protected void doPageStart(boolean select) {
        caretLineIndex = topLineIndex;
        caretColumn = 0;
        if (!select) clearSelection();
    }

    protected void doPageEnd(boolean select) {
        caretLineIndex = Math.min(content.getLineCount()-1, topLineIndex + getNumVisibleLines());
        caretColumn = content.getLineText(caretLineIndex).length();
        if (!select) clearSelection();
    }

    protected void doContentStart(boolean select) {
        caretLineIndex = 0;
        caretColumn = 0;
        if (!select) clearSelection();
    }

    protected void doContentEnd(boolean select) {
        caretLineIndex = content.getLineCount()-1;
        caretColumn = content.getLineText(caretLineIndex).length();
        if (!select) clearSelection();
    }

    protected void selectAll() {
        selectionAnchorLineIndex = 0;
        selectionAnchorColumn = 0;
        caretLineIndex = content.getLineCount()-1;
        caretColumn = content.getLineText(caretLineIndex).length();
    }

    protected void clearSelection() {
        selectionAnchorLineIndex = caretLineIndex;
        selectionAnchorColumn = caretColumn;
    }

    /**
     * Copies the selected text to the <code>DND.CLIPBOARD</code> clipboard.
     */
    public void copy() {
        copy(DND.CLIPBOARD, MAX_CLIPBOARD_SIZE);
    }

    /**
     * Copies the selected text to the specified clipboard.  The text will be put in the
     * clipboard in plain text format
     * <p>
     * The clipboardType is  one of the clipboard constants defined in class
     * <code>DND</code>.  The <code>DND.CLIPBOARD</code>  clipboard is
     * used for data that is transferred by keyboard accelerator (such as Ctrl+C/Ctrl+V)
     * or by menu action.  The <code>DND.SELECTION_CLIPBOARD</code>
     * clipboard is used for data that is transferred by selecting text and pasting
     * with the middle mouse button.
     *
     * @return true if clipboard was set, false if not
     */
    public boolean copy(int clipboardType, int approximateMaxSize) {
        if (clipboardType != DND.CLIPBOARD && clipboardType != DND.SELECTION_CLIPBOARD)
            return false;

        Pos start = getSelectionStart();
        Pos end = getSelectionEnd();
        if (start.equals(end))
            return false;

        try {
            String text = getTextRange(start, end, approximateMaxSize);
            if (text == null)
                throw new RuntimeException("Selected text too large");
            setClipboardContent(text, clipboardType);
            return true;
        }
        catch (SWTError error) {
            // Copy to clipboard failed. This happens when another application
            // is accessing the clipboard while we copy. Ignore the error.
            if (error.code == DND.ERROR_CANNOT_SET_CLIPBOARD)
                return false;
            throw error;
        }
    }

    public String getTextRange(Pos start, Pos end) {
        return getTextRange(start, end, -1);
    }

    /**
     * Returns the text range, or null if its size exceeds approximateMaxSize.
     */
    public String getTextRange(Pos start, Pos end, int approximateMaxSize) {
        //TODO assert numbers in range
        if (end.lessThan(start))
            return "";
        if (start.line == end.line)
            return content.getLineText(start.line).substring(start.column, end.column);

        StringBuilder buffer = new StringBuilder();
        for (int line = start.line; line <= end.line; line++) {
            String txt = content.getLineText(line);
            if (line == start.line)
                txt = txt.substring(start.column);
            else if (line == end.line)
                txt = txt.substring(0, end.column);
            buffer.append(txt);
            if (!txt.endsWith("\n") && line != end.line)
                buffer.append("\n");
            if (approximateMaxSize != -1 && buffer.length() >= approximateMaxSize)
                return null;
        }
        return buffer.toString();
    }

    // based on StyledText
    protected void setClipboardContent(String text, int clipboardType) {
        if (clipboardType == DND.SELECTION_CLIPBOARD && !(IS_MOTIF || IS_GTK)) return;
        TextTransfer plainTextTransfer = TextTransfer.getInstance();
        Object[] data = new Object[]{text};
        Transfer[] types = new Transfer[]{plainTextTransfer};
        clipboard.setContents(data, types, clipboardType);
    }

    protected boolean isWordChar(char ch) {
        return Character.isLetterOrDigit(ch) || ch=='_' || ch=='@';
    }

    protected void handleMouseDown(Event event) {
        clickCount = event.count;
        Point lineColumn = getLineColumnAt(event.x, event.y);
        caretLineIndex = clip(0, content.getLineCount()-1, lineColumn.y);
        caretColumn = clip(0, content.getLineText(caretLineIndex).length(), lineColumn.x);

        if (clickCount == 1) {
            boolean select = (event.stateMask & SWT.MOD2) != 0;
            if (!select)
                clearSelection();
        }
        else if (clickCount % 2 == 0) {
            // double click - select word
            doWordPrevious(false);
            doWordNext(true);
        }
        else {
            // triple click - select full line
            selectionAnchorLineIndex = caretLineIndex;
            selectionAnchorColumn = 0;
            if (caretLineIndex < content.getLineCount()-1) {
                caretLineIndex++;
                caretColumn = 0;
            } else {
                caretColumn = content.getLineText(caretLineIndex).length();
            }
        }
        revealCaret();
        redraw();
    }

    protected void handleMouseUp(Event event) {
        clickCount = 0;
        stopAutoScroll();
    }

    protected void handleMouseMove(Event event) {
        if (clickCount > 0) {
            Point lineColumn = getLineColumnAt(event.x, event.y);
            caretLineIndex = clip(0, content.getLineCount()-1, lineColumn.y);
            caretColumn = clip(0, content.getLineText(caretLineIndex).length(), lineColumn.x);
            redraw();
            doAutoScroll(event); // start/stop autoscrolling as needed
        }
    }

    protected void handleMouseWheel(Event event) {
        // when scrolling down (count<0), bottom line should be entirely visible;
        // when scrolling up (count>0), the top line should be entirely visible.
        // actual scrolling seems to be taken care of automatically (by ScrolledComposite?)
        if (event.count > 0 || content.getLineCount() < getNumVisibleLines())
            alignTopLine();
        else {
            alignBottomLine();
        }
    }

    protected void alignTopLine() {
        topLineY = 0;
    }

    protected void alignBottomLine() {
        Rectangle r = getClientArea();
        topLineY = r.height % lineHeight;
        if (topLineY > 0)
            topLineY -= lineHeight;
    }

    /**
     * A mouse move event has occurred.  See if we should start autoscrolling.  If
     * the move position is outside of the client area, initiate autoscrolling.
     * Otherwise, we've moved back into the widget so end autoscrolling.
     */
    protected void doAutoScroll(Event event) {
        Rectangle clientArea = getClientArea();
        if (event.y > clientArea.height)
            startOrRefineAutoScroll(SWT.DOWN, event.y - clientArea.height);
        else if (event.y < 0)
            startOrRefineAutoScroll(SWT.UP, -event.y);
        else if (event.x < 0)
            startOrRefineAutoScroll(ST.COLUMN_PREVIOUS, event.x);
        else if (event.x > clientArea.width)
            startOrRefineAutoScroll(ST.COLUMN_NEXT, event.x - clientArea.width);
        else
            stopAutoScroll();
    }

    protected void startOrRefineAutoScroll(final int direction, int distance) {
        autoScrollDistance = distance;

        if (autoScrollDirection == direction)
            return; // already autoscrolling in the given direction, do nothing

        // we use a periodic timer that simulates the user hitting up/down etc keys
        if (autoScrollTimer == null) {
            // create timer if not yet exists
            autoScrollTimer = new Runnable() {
                public void run() {
                    if (!isDisposed() && autoScrollDirection != SWT.NULL) {
                        // note: we could make use of use autoScrollDistance here
                        if (autoScrollDirection == SWT.UP) {
                            invokeAction(ST.SELECT_LINE_UP);
                        }
                        else if (autoScrollDirection == SWT.DOWN) {
                            invokeAction(ST.SELECT_LINE_DOWN);
                        }
                        else if (autoScrollDirection == ST.COLUMN_NEXT) {
                            if (caretColumn < content.getLineText(caretLineIndex).length())
                                invokeAction(ST.SELECT_COLUMN_NEXT);
                        }
                        else if (autoScrollDirection == ST.COLUMN_PREVIOUS) {
                            if (caretColumn > 0)
                                invokeAction(ST.SELECT_COLUMN_PREVIOUS);
                        }
                        int rate = autoScrollDirection==SWT.UP || autoScrollDirection==SWT.DOWN ? V_SCROLL_RATE : H_SCROLL_RATE;
                        Display.getCurrent().timerExec(rate, this);
                    }
                }
            };
        }

        // restart timer
        autoScrollDirection = direction;
        int rate = autoScrollDirection==SWT.UP || autoScrollDirection==SWT.DOWN ? V_SCROLL_RATE : H_SCROLL_RATE;
        Display.getCurrent().timerExec(-1, autoScrollTimer);
        Display.getCurrent().timerExec(rate, autoScrollTimer);
    }

    protected void stopAutoScroll() {
        if (autoScrollTimer != null)
            Display.getCurrent().timerExec(-1, autoScrollTimer);
    }

    protected void handleResize(Event event) {
        configureScrollbars();
        adjustScrollbars();
        redraw();
    }

    protected void handleFocusIn(Event event) {
        restartCaretBlinking();
    }

    protected void handleFocusOut(Event event) {
        hideCaret();
        stopAutoScroll(); // in any case
    }

    protected void restartCaretBlinking() {
        // start or re-start caret blinking
        caretShown = true;
        Display.getCurrent().timerExec(-1, caretBlinkTimer); // cancels timer if running
        Display.getCurrent().timerExec(CARET_BLINK_DELAY, caretBlinkTimer);
    }

    protected void hideCaret() {
        caretShown = false;
        Display.getCurrent().timerExec(-1, caretBlinkTimer); // cancels timer if running
        redraw();
    }

    public void setContentProvider(ITextViewerContentProvider content) {
        if (this.content != null)
            this.content.removeTextChangeListener(textChangeListener);
        this.content = content;
        if (content != null)
            content.addTextChangeListener(textChangeListener);

        contentChanged();
    }

    public ITextViewerContentProvider getContentProvider() {
        return content;
    }

    public void refresh() {
        contentChanged();
    }

    protected void contentChanged() {
        Assert.isTrue(content.getLineCount() > 0, "content must be at least one line");

        // adjust caret and selection line index
        caretLineIndex = clip(0, content.getLineCount()-1, caretLineIndex);
        selectionAnchorLineIndex = clip(0, content.getLineCount()-1, selectionAnchorLineIndex);

        // NOTE: for performance reasons, DO NOT ADJUST COLUMN INDICES!!! An out-of-bounds
        // column index causes no problem, but to clip it to the line length would require
        // us to ask the content provide for the line -- at that can be very COSTLY if
        // the line index is far from the currently displayed line range. (The underlying
        // content provider was not designed for random access, it takes O(n) time to
        // return a random line!!!)
        //
        // So, do NOT do this:
        //caretColumn = clip(0, content.getLine(caretLineIndex).length(), caretColumn);
        //selectionAnchorColumn = clip(0, content.getLine(caretLineIndex).length(), selectionAnchorColumn);

        configureScrollbars();
        revealCaret();
        adjustScrollbars();
        redraw();
    }

    public void setFont(Font font) {
        this.font = font;

        GC gc = new GC(Display.getCurrent());
        gc.setFont(font);
        FontMetrics fontMetrics = gc.getFontMetrics();
        lineHeight = fontMetrics.getHeight();
        averageCharWidth = fontMetrics.getAverageCharWidth();

        final String wideText = "wgh8";
        final String narrowText = "1l;.";
        Assert.isTrue(wideText.length() == narrowText.length());
        isMonospaceFont = gc.textExtent(wideText).x == gc.textExtent(narrowText).x;

        gc.dispose();

        redraw();
    }

    public Font getFont() {
        return font;
    }

    public void setBackgroundColor(Color backgroundColor) {
        this.backgroundColor = backgroundColor;
        redraw();
    }

    public Color getBackgroundColor() {
        return backgroundColor;
    }

    public void setForegroundColor(Color foregroundColor) {
        this.foregroundColor = foregroundColor;
        redraw();
    }

    public Color getForegroundColor() {
        return foregroundColor;
    }

    public void setTopLineIndex(int topLineIndex) {
        this.topLineIndex = topLineIndex;
        adjustScrollbars(); // this will fix the value if it was out of range
        redraw();
    }

    public int getTopLineIndex() {
        return topLineIndex;
    }

    /**
     * This is the efficient way to set the caret position.
     */
    public void setCaretPosition(int lineIndex, int column) {
        caretLineIndex = clip(0, lineIndex, content.getLineCount()-1);
        caretColumn = Math.max(0, column);  // do NOT clip to line length! content provider may be O(n)
        clearSelection();
        redraw();
    }

    /**
     * Returns the caret position.
     */
    public Pos getCaretPosition() {
        int clippedCaretColumn = clip(0, content.getLineText(caretLineIndex).length(), caretColumn);
        return new Pos(caretLineIndex, clippedCaretColumn);
    }

    public void setSelection(int startLineIndex, int startColumn, int endLineIndex, int endColumn) {
        //XXX assert ranges!
        selectionAnchorLineIndex = startLineIndex;
        selectionAnchorColumn = startColumn;
        caretLineIndex = endLineIndex;
        caretColumn = endColumn;
        redraw();
    }

    public Pos getSelectionAnchor() {
        int clippedSelectionAnchorColumn = clip(0, content.getLineText(selectionAnchorLineIndex).length(), selectionAnchorColumn);
        return new Pos(selectionAnchorLineIndex, clippedSelectionAnchorColumn);
    }

    public void setSelectionAnchor(int lineIndex, int column) {
        selectionAnchorLineIndex = lineIndex;  //XXX assert range!
        selectionAnchorColumn = column;        //XXX assert range!
        redraw();
    }

    public Pos getSelectionStart() {
        Pos anchor = getSelectionAnchor();
        Pos caret = getCaretPosition();
        return anchor.lessThan(caret) ? anchor : caret;
    }

    public Pos getSelectionEnd() {
        Pos anchor = getSelectionAnchor();
        Pos caret = getCaretPosition();
        return anchor.lessThan(caret) ? caret : anchor;
    }

    public int getLineHeight() {
        return lineHeight;
    }

    public int getNumVisibleLines() {
        // count partially visible lines as well
        return  (getClientArea().height + lineHeight - 1) / lineHeight;
    }

    public int getNumVisibleColumns() {
        // count partially visible columns as well
        return  (getClientArea().width + averageCharWidth - 1) / averageCharWidth;
    }

    public Point getLineColumnAt(int x, int y) {
        int lineIndex = topLineIndex + (y-topLineY) / lineHeight;
        lineIndex = clip(0, content.getLineCount()-1, lineIndex);

        int column;
        if (isMonospaceFont) {
            column = (x - leftMargin + horizontalScrollOffset) / averageCharWidth;
            column = Math.max(0, column);
        }
        else {
            // linear search is good enough (this method is only called on mouse clicks, and gc.textExtent() is fast)
            GC gc = new GC(Display.getCurrent());
            gc.setFont(font);
            String line = content.getLineText(lineIndex);
            column = line.length(); // in case loop falls through
            for (int i = 0; i < line.length(); i++) {
                if (gc.textExtent(line.substring(0, i+1)).x > x + horizontalScrollOffset + 2) {
                    column = i;
                    break;
                }
            }
            gc.dispose();
        }
        return new Point(column, lineIndex);
    }

    protected void handlePaint(Event e) {
        GC gc = e.gc;
        gc.setBackground(backgroundColor);
        gc.setForeground(foregroundColor);
        gc.setFont(font);

        Rectangle r = getClientArea();
        gc.fillRectangle(r.x, r.y, r.width, r.height);

        int numLines = content.getLineCount();
        int numVisibleLines = getNumVisibleLines();

        // note: if the following asserts are triggered, odds are that content has changed without a call to our contentChanged() method
        Assert.isTrue(topLineIndex >= 0 && topLineIndex <= Math.max(0,numLines-numVisibleLines));
        Assert.isTrue(numLines==0 || (caretLineIndex >= 0 && caretLineIndex < numLines));

        int x = leftMargin - horizontalScrollOffset;
        int lineIndex = topLineIndex;
        int startY = topLineY;

        // draw the lines
        for (int y = startY; y < r.y+r.height && lineIndex < numLines; y += lineHeight) {
            drawLine(gc, lineIndex++, x, y);
        }
    }

    protected void drawLine(GC gc, int lineIndex, int x, int y) {
        // draw the line in the specified color
        String line = content.getLineText(lineIndex);
        Color color = content.getLineColor(lineIndex);
        if (color == null)
            gc.drawString(line, x, y);
        else {
            gc.setForeground(color);
            gc.drawString(line, x, y);
            gc.setForeground(foregroundColor);
        }

        // if there is selection, draw it
        if (selectionAnchorLineIndex!=caretLineIndex || selectionAnchorColumn!=caretColumn) {
            int selStartLine = caretLineIndex;
            int selStartColumn = caretColumn;
            int selEndLine = selectionAnchorLineIndex;
            int selEndColumn = selectionAnchorColumn;
            if (selStartLine > selEndLine || (selStartLine==selEndLine && selStartColumn > selEndColumn)) {
                // swap so that start < end
                int tmp = selStartLine; selStartLine = selEndLine; selEndLine = tmp;
                tmp = selStartColumn; selStartColumn = selEndColumn; selEndColumn = tmp;
            }

            // if lineIndex is in the selected region, redraw the selected part
            if (lineIndex >= selStartLine && lineIndex <= selEndLine) {
                int startColumn = lineIndex==selStartLine ? selStartColumn : 0;
                int endColumn = lineIndex==selEndLine ? selEndColumn : line.length();
                int startColOffset = gc.textExtent(line.substring(0, startColumn)).x;
                String selection = line.substring(startColumn, endColumn);

                gc.setBackground(selectionBackgroundColor);
                gc.setForeground(selectionForegroundColor);
                gc.drawText(selection, x + startColOffset, y);
                gc.setBackground(backgroundColor);
                gc.setForeground(foregroundColor);
            }
        }

        if (lineIndex == caretLineIndex && caretShown) {
            // draw caret
            String linePrefix = caretColumn >= line.length() ? line : line.substring(0, caretColumn);
            int caretX = x + gc.textExtent(linePrefix).x;
            gc.drawRectangle(caretX, y, 1, lineHeight-1);
        }
    }

    protected void configureScrollbars() {
        int numVisibleLines = getNumVisibleLines();
        ScrollBar vsb = getVerticalBar();
        vsb.setValues(vsb.getSelection(), 0, content.getLineCount(), numVisibleLines, 1, numVisibleLines);

        int maxWidth = 200*averageCharWidth; //FIXME
        int width = getClientArea().width;
        ScrollBar hsb = getHorizontalBar();
        hsb.setValues(hsb.getSelection(), 0, maxWidth, width, 4, width);
    }

    protected void handleVerticalScroll(Event event) {
        topLineIndex = getVerticalBar().getSelection();
        redraw();
    }

    protected void handleHorizontalScroll(Event event) {
        horizontalScrollOffset = getHorizontalBar().getSelection();
        redraw();
    }

    /**
     * Fix topLineIndex and adjust scrollbar selections.
     */
    protected void adjustScrollbars() {
        topLineIndex = clip(0, Math.max(0,content.getLineCount()-getNumVisibleLines()), topLineIndex);
        getVerticalBar().setSelection(topLineIndex);
        getHorizontalBar().setSelection(horizontalScrollOffset);
    }

    //@Override
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.addSelectionChangedListener(listener);
    }

    //@Override
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.removeSelectionChangedListener(listener);
    }

    //@Override
    public void setSelection(ISelection selection) {
        selectionProvider.setSelection(selection);  //FIXME synchronize this with the text selection!!!! (it does nothing as it is!!)
    }

    //@Override
    public ISelection getSelection() {
        return selectionProvider.getSelection();  //FIXME synchronize this with the text selection!!!! (it does nothing as it is!!)
    }

}

package org.omnetpp.runtimeenv.widgets;

import java.util.HashMap;
import java.util.Map;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.resource.JFaceResources;
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
//TODO cursor should be solid while moving (restart timer on any key/mouse/textchange event)
//TODO minor glitches with word selection (esp with single-letter words)
//TODO drag-autoscroll
//TODO try if it works with proportional font
//TODO finish horiz scrolling!
//TODO implement selectionprovider stuff
//TODO when resizing vertically, content moves strangely (scrolls twice the resize speed, and jumps back every 8 pixels)
public class TextViewer extends Canvas implements ISelectionProvider {
    protected TextViewerContent content;
    protected TextChangeListener textChangeListener;
    protected Font font;
    protected Color backgroundColor;
    protected Color foregroundColor;
    protected Color selectionBackgroundColor;
    protected Color selectionForegroundColor;
    protected int leftMargin = 0;
    protected int lineHeight, averageCharWidth; // measured from font
    protected int topLineIndex;
    protected boolean alignTop = true; // whether to align the top or the bottom line to window edge
    protected int horizontalScrollOffset; // in pixels
    protected boolean caretShown = true; // during blinking
    protected int caretLineIndex, caretColumn;  // caretColumn may be greater than line length!
    protected int selectionAnchorLineIndex, selectionAnchorColumn; // selection is between anchor and caret
    protected Map<Integer,Integer> keyActionMap = new HashMap<Integer, Integer>(); // key: keycode, value: ST.xxx constants
    protected Listener listener;
    protected int clickCount;
    protected Clipboard clipboard;
    protected ISelectionProvider selectionProvider = new SelectionProvider();

    protected static final int MAX_CLIPBOARD_SIZE = 10*1024*1024; //10Meg

    final static boolean IS_CARBON, IS_GTK, IS_MOTIF;
    static {
        String platform = SWT.getPlatform();
        IS_CARBON = "carbon".equals(platform);
        IS_GTK = "gtk".equals(platform);
        IS_MOTIF = "motif".equals(platform);
    }

    
    public TextViewer(Composite parent, int style) {
        super(parent, style | SWT.H_SCROLL | SWT.V_SCROLL | SWT.NO_REDRAW_RESIZE | SWT.DOUBLE_BUFFERED | SWT.NO_BACKGROUND);

        backgroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_BACKGROUND);
        foregroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_FOREGROUND);
        selectionBackgroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_SELECTION);
        selectionForegroundColor = getDisplay().getSystemColor(SWT.COLOR_LIST_SELECTION_TEXT);

        setFont(JFaceResources.getTextFont());
        clipboard = new Clipboard(getDisplay());
        
        textChangeListener = new TextChangeListener() {
            @Override
            public void textChanged(TextViewerContent textViewer) {
                contentChanged();
            }
        };

        installListeners();
        
        createKeyBindings();
        
        // cursor blinking
        //XXX disable if not focused
        Display.getCurrent().timerExec(500, new Runnable() {
            @Override
            public void run() {
                if (!isDisposed()) {
                    caretShown = !caretShown;
                    redraw();
                    Display.getCurrent().timerExec(500, this);
                }
            }
        });
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
        caretColumn = Math.min(caretColumn, content.getLine(caretLineIndex).length());
        if (caretColumn > 0) 
            caretColumn--;
        else if (caretLineIndex > 0) {
            caretLineIndex--;
            caretColumn = content.getLine(caretLineIndex).length();
        }
        if (!select) clearSelection();
    }

    protected void doCursorNext(boolean select) {
        if (caretColumn < content.getLine(caretLineIndex).length())
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
        caretColumn = content.getLine(caretLineIndex).length();
        if (!select) clearSelection();
    }
    
    protected void doWordPrevious(boolean select) {
        String line = content.getLine(caretLineIndex);
        int pos = caretColumn;
        if (pos == 0) {
            // go to end of previous line
            if (caretLineIndex > 0) {
                caretLineIndex--;
                caretColumn = content.getLine(caretLineIndex).length();
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
        String line = content.getLine(caretLineIndex);
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
        caretColumn = content.getLine(caretLineIndex).length();
        if (!select) clearSelection();
    }

    protected void doContentStart(boolean select) {
        caretLineIndex = 0;
        caretColumn = 0;
        if (!select) clearSelection();
    }

    protected void doContentEnd(boolean select) {
        caretLineIndex = content.getLineCount()-1;
        caretColumn = content.getLine(caretLineIndex).length();
        if (!select) clearSelection();
    }

    protected void selectAll() {
        selectionAnchorLineIndex = 0;
        selectionAnchorColumn = 0;
        caretLineIndex = content.getLineCount()-1;
        caretColumn = content.getLine(caretLineIndex).length();
    }

    protected void clearSelection() {
        selectionAnchorLineIndex = caretLineIndex;
        selectionAnchorColumn = caretColumn;
    }

    /**
     * Copies the selected text to the <code>DND.CLIPBOARD</code> clipboard.
     */
    public void copy() {
        copy(DND.CLIPBOARD);
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
     */
    public void copy(int clipboardType) {
        if (clipboardType != DND.CLIPBOARD && clipboardType != DND.SELECTION_CLIPBOARD) return;
        Point selection = getSelectionRange();
        int length = selection.y - selection.x;
        if (length > 0) {
            try {
                String text = getTextRange(selection.x, length);
                setClipboardContent(text, clipboardType);
            } 
            catch (SWTError error) {
                // Copy to clipboard failed. This happens when another application 
                // is accessing the clipboard while we copy. Ignore the error.
                if (error.code != DND.ERROR_CANNOT_SET_CLIPBOARD) {
                    throw error;
                }
            }
        }
    }

    protected String getTextRange(int offset, int length) {
        offset = clip(0, content.getCharCount()-1, offset);
        length = clip(0, content.getCharCount()-offset, length);
        boolean truncate = length > MAX_CLIPBOARD_SIZE;
        if (truncate) length = MAX_CLIPBOARD_SIZE; 
        int startLine = content.getLineAtOffset(offset);
        StringBuilder b = new StringBuilder();
        b.append(content.getLine(startLine).substring(offset-content.getOffsetAtLine(startLine))).append('\n');
        int line = startLine+1;
        while (b.length() < length) {
            Assert.isTrue(line < content.getLineCount());
            b.append(content.getLine(line++)).append('\n');
        }
        return b.substring(0, length) + (truncate ? "... [truncated]" : "");
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
        caretColumn = clip(0, content.getLine(caretLineIndex).length(), lineColumn.x);

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
                caretColumn = content.getLine(caretLineIndex).length();
            }
        }
        redraw();
    }

    protected void handleMouseUp(Event event) {
        clickCount = 0;
    }

    protected void handleMouseMove(Event event) {
        if (clickCount > 0) {
            Point lineColumn = getLineColumnAt(event.x, event.y);
            caretLineIndex = clip(0, content.getLineCount()-1, lineColumn.y);
            caretColumn = clip(0, content.getLine(caretLineIndex).length(), lineColumn.x);
            redraw();
        }
    }

    protected void handleMouseWheel(Event event) {
    	// when scrolling down, bottom line should be entirely visible; 
    	// when scrolling up, the top line should be entirely visible.
    	// actual scrolling seems to be taken care of automatically (by ScrolledComposite?) 
    	alignTop = event.count > 0;
    }

    protected void handleResize(Event event) {
        configureScrollbars();
        adjustScrollbars();
        redraw();
    }

    public void setContent(TextViewerContent content) {
        if (this.content != null)
            this.content.removeTextChangeListener(textChangeListener);
        this.content = content;
        if (content != null)
            content.addTextChangeListener(textChangeListener);

        contentChanged();
    }

    public TextViewerContent getContent() {
        return content;
    }
    
    public void refresh() {
        contentChanged();
    }
    
    protected void contentChanged() {
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
     * Sets the caret offset. CAUTION: Due to O(n) content provider, this method 
     * is inefficient if the caret is off-screen by several lines. 
     */
    public void setCaretOffset(int offset) {
        offset = clip(0, content.getCharCount(), offset);
        caretLineIndex = content.getLineAtOffset(offset);
        caretColumn = offset - content.getOffsetAtLine(caretLineIndex);
        clearSelection();
        redraw();
    }

    /** 
     * Returns the caret offset. CAUTION: Due to O(n) content provider, this method 
     * is inefficient if the caret is off-screen by several lines. 
     */
    public int getCaretOffset() {
        return content.getOffsetAtLine(caretLineIndex) + Math.min(caretColumn, content.getLine(caretLineIndex).length());
    }

    public void setSelection(int startLineIndex, int startColumn, int endLineIndex, int endColumn) {
        selectionAnchorLineIndex = startLineIndex;
        selectionAnchorColumn = startColumn;
        caretLineIndex = endLineIndex;
        caretColumn = endColumn;
        redraw();
    }

    public int getSelectionAnchor() {
        return content.getOffsetAtLine(selectionAnchorLineIndex) + Math.min(selectionAnchorColumn, content.getLine(selectionAnchorLineIndex).length());
    }

    public void setSelectionAnchor(int offset) {
        offset = clip(0, content.getCharCount(), offset);
        selectionAnchorLineIndex = content.getLineAtOffset(offset);
        selectionAnchorColumn = offset - content.getOffsetAtLine(selectionAnchorLineIndex);
        redraw();
    }
    
    public Point getSelectionRange() {
        int a = getSelectionAnchor();
        int b = getCaretOffset();
        return a < b ? new Point(a,b) : new Point(b,a);
    }

    public int getLineHeight() {
        return lineHeight;
    }

    public int getNumVisibleLines() {
        return  (getClientArea().height + lineHeight - 1) / lineHeight;
    }

    public int getNumVisibleColumns() {
        return  (getClientArea().width + averageCharWidth - 1) / averageCharWidth;
    }

    public Point getLineColumnAt(int x, int y) {
        int lineIndex = topLineIndex + y / lineHeight;
        int column = (x - leftMargin + horizontalScrollOffset) / averageCharWidth; //XXX this only works for monospace fonts
        lineIndex = clip(0, content.getLineCount()-1, lineIndex);
        column = Math.max(0, column);
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

        Assert.isTrue(topLineIndex >= 0 && topLineIndex <= Math.max(0,numLines-numVisibleLines));
        Assert.isTrue(caretLineIndex >= 0 && caretLineIndex < numLines);
        
        if (!alignTop && caretLineIndex==topLineIndex)
            alignTop = true;
        if (alignTop && caretLineIndex==topLineIndex+numVisibleLines-1)
            alignTop = false;
        
        int startY = r.x + (alignTop ? 0 : -(r.height % lineHeight));
        
        int x = leftMargin - horizontalScrollOffset;
        
        // draw the lines
        int lineIndex = topLineIndex;
        for (int y = startY; y < r.y+r.height && lineIndex < numLines; y += lineHeight) {
            drawLine(gc, lineIndex++, x, y);
        }
    }

    protected void drawLine(GC gc, int lineIndex, int x, int y) {
        // draw the line in the specified color
		String line = "**UNDEF**";
    	try { 
    		line = content.getLine(lineIndex);
    	} catch (Exception e) { //XXX only for debugging!!
    		System.out.println("EXCEPTION in TextViewer.drawLine(): line=" + lineIndex + "; numLines=" + getContent().getLineCount() + " -- " + e.getMessage());
    	}
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

    @Override
    public void addSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.addSelectionChangedListener(listener);
    }

    @Override
    public void removeSelectionChangedListener(ISelectionChangedListener listener) {
        selectionProvider.removeSelectionChangedListener(listener);
    }

    @Override
    public void setSelection(ISelection selection) {
        selectionProvider.setSelection(selection);  //FIXME synchronize this with the text selection!!!! (it does nothing as it is!!)
    }

    @Override
    public ISelection getSelection() {
        return selectionProvider.getSelection();  //FIXME synchronize this with the text selection!!!! (it does nothing as it is!!)
    }

}

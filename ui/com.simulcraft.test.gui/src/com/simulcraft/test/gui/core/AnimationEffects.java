package com.simulcraft.test.gui.core;

import com.simulcraft.test.gui.access.Access;
import com.simulcraft.test.gui.access.ClickableAccess;

import org.eclipse.jface.internal.text.html.BrowserInformationControl;
import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

import org.omnetpp.common.color.ColorFactory;

// Notes for Linux/GTK:
//   - XOR mode doesn't work   
//   - can only draw on ONE control, EXLUDING its children! ie. drawing on a Shell
//     will only draw on a few empty pixel strips not covered by children... setting clipRect doesn't help
// Workarounds: 
//   - XOR: save bg image and restore it?
//   - clipping: draw on cursorControl (AND maybe all its parents and siblings??? crazy!)
//

public class AnimationEffects  {
    private static final String HTML_PROLOG =
        "<html><head><style CHARSET=\"ISO-8859-1\" TYPE=\"text/css\">\n" +
        "html   { font-family: 'Tahoma',sans-serif; font-size: 16pt; font-style: normal; font-weight: normal; }\n" +
        "pre    { font-family: monospace; }\n" +
        "body   { overflow: auto; margin: 10px;}\n" +
        "</style></head>\n" +
        "<body text=\"#000000\" bgcolor=\"#cde1f9\">\n";
    private static final String HTML_EPILOG =
        "</body></html>\n";
    
    public static void displayTextBox(String text, int delayMillis) {
    	displayTextBox(text, ColorFactory.BLACK, 16, delayMillis);
    }

    public static void displayError(Throwable error, int delayMillis) {
    	displayTextBox(error.toString(), ColorFactory.RED, 12, delayMillis);
    }

    public static void displayTextBox(String text, Color textColor, int fontSize, int delayMillis) {
    	if (PlatformUtils.isWindows)
    		displayTextBox1(text, textColor, fontSize, delayMillis); // draw directly on the display
    	else
    		displayTextBox2(text, textColor, fontSize, delayMillis);  // draw on a temporary shell
    }

    public static void displayTextBox1(String text, Color textColor, int fontSize, int delayMillis) {
        // choose font, calculate position and size
        Display display = Display.getCurrent();
        GC gc = new GC(display);
        Font font = new Font(display, "Arial", fontSize, SWT.NORMAL);
        gc.setFont(font);
        Point textExtent = gc.textExtent(text);

        //Point p = getPositionNearFocusWidget();
        Point p = display.getCursorLocation();  

        Rectangle r = new Rectangle(p.x + 10, p.y + 10, textExtent.x + 20, textExtent.y + 10);

        // save original background
        Image savedBackground = new Image(display, r.width, r.height);
        gc.copyArea(savedBackground, r.x, r.y);
        gc.setClipping(r);

        // draw text box
        gc.setBackground(ColorFactory.LIGHT_YELLOW);
        gc.setForeground(ColorFactory.BLACK);
        gc.setLineWidth(2);
        gc.fillRectangle(r);
        gc.drawRectangle(r);
        gc.setForeground(textColor);
        gc.drawText(text, r.x + r.width/2 - textExtent.x/2, r.y + r.height/2 - textExtent.y/2);

        // wait
        try { Thread.sleep(Access.rescaleTime(delayMillis)); } catch (InterruptedException e) { }

        // restore background
        gc.drawImage(savedBackground, r.x, r.y);

        // dispose
        savedBackground.dispose();
        font.dispose();
        gc.dispose();
    }

    public static void displayTextBox2(String text, Color textColor, int fontSize, int delayMillis) {
    	System.out.println("SHOWING: " + text); Display.getCurrent().beep();
    	
        Shell shell = new Shell(SWT.NO_TRIM | SWT.ON_TOP);
        Display display = Display.getCurrent();

        //Point p = getPositionNearFocusWidget();
        Point p = display.getCursorLocation();  
        shell.setLocation(p.x + 20, p.y + 10);
        
        // choose font, calculate position and size
        GC gc = new GC(shell);
        Font font = new Font(display, "Arial", fontSize, SWT.NORMAL);
        gc.setFont(font);
        Point textExtent = gc.textExtent(text);
        shell.setSize(textExtent.x, textExtent.y);
        shell.open();

        Rectangle r = new Rectangle(1, 1, textExtent.x-2, textExtent.y-2);

        // draw text box
        gc.setBackground(ColorFactory.LIGHT_YELLOW);
        gc.setForeground(ColorFactory.BLACK);
        gc.setLineWidth(2);
        gc.fillRectangle(r);
        gc.drawRectangle(r);
        gc.setForeground(textColor);
        gc.drawText(text, r.x + r.width/2 - textExtent.x/2, r.y + r.height/2 - textExtent.y/2);
        font.dispose();
        gc.dispose();

        // wait
        try { Thread.sleep(Access.rescaleTime(delayMillis)); } catch (InterruptedException e) { }

        // dispose
        shell.dispose();
    }

    @SuppressWarnings("deprecation")
    public static void animateClick(int x, int y) {
        // draw inflating red circle 
        GC gc = new GC(Display.getCurrent());
        gc.setForeground(ColorFactory.CYAN); // complement of red
        gc.setLineWidth(2);
        gc.setXORMode(true); // won't work on Mac
        for (int r = 2; r < 25; r++) {
            gc.drawOval(x-r, y-r, 2*r, 2*r);
            try { Thread.sleep(Access.rescaleTime(5)); } catch (InterruptedException e) { break; }
            gc.drawOval(x-r, y-r, 2*r, 2*r);
        }
    }

    public static void beginAnimateDragDrop(int x1, int y1, int x2, int y2) {
        // note: normal drag&drop animation (the SWT one) is missing because we don't call readAndDispatch() between posting mouse events
        drawXorLine(x1, y1, x2, y2);
        displayTextBox("dragging...", 500);
        ClickableAccess.setMouseMoveDurationMillis(ClickableAccess.getMouseMoveDurationMillis()*2);
    }

    public static void endAnimateDragDrop(int x1, int y1, int x2, int y2) {
        ClickableAccess.setMouseMoveDurationMillis(ClickableAccess.getMouseMoveDurationMillis()/2);
        drawXorLine(x1, y1, x2, y2);
        displayTextBox("drop", 300);
        animateClick(x2, y2);
    }

    @SuppressWarnings("deprecation")
    private static void drawXorLine(int x1, int y1, int x2, int y2) {
        // draw red line 
        GC gc = new GC(Display.getCurrent());
        gc.setForeground(ColorFactory.CYAN); // complement of red
        gc.setLineWidth(2);
        gc.setXORMode(true); // won't work on Mac
        gc.drawLine(x1, y1, x2, y2);
    }

    @InBackgroundThread
    public static void showMessage(String msg, Rectangle bounds, int delayMillis) {
        showMessage(msg, bounds.x, bounds.y, bounds.width, bounds.height, delayMillis);
    }

    @InBackgroundThread
    public static void showMessage(String msg, int x, int y, int w, int h, int delayMillis) {
            IInformationControl informationControl = showInformationControl(addHTMLStyleSheet(msg), x, y, w, h);
            try {
                Thread.sleep(Access.rescaleTime(delayMillis));
            } catch (InterruptedException e) {}
            disposeInformationControl(informationControl);
    }

    @UIStep
    private static void disposeInformationControl(IInformationControl informationControl) {
        informationControl.dispose();
    }

    @UIStep
    private static IInformationControl showInformationControl(String msg, int x, int y, int w, int h) {
        IInformationControl informationControl = getInformationPresenterControlCreator().createInformationControl(Display.getCurrent().getActiveShell());
        informationControl.setSizeConstraints(800, 600);
        informationControl.setInformation(msg);
        informationControl.setSize(w, h);
        informationControl.setLocation(new Point(x,y));
        
        informationControl.setVisible(true);
        informationControl.setFocus();
        return informationControl;
    }

    private static IInformationControlCreator getInformationPresenterControlCreator() {
        return new IInformationControlCreator() {
            @SuppressWarnings("restriction")
            public IInformationControl createInformationControl(Shell parent) {
                // for more info, see JavadocHover class in JDT
                int shellStyle = SWT.RESIZE | SWT.TOOL;
                int style = SWT.NONE;
                if (BrowserInformationControl.isAvailable(parent))
                    return new BrowserInformationControl(parent, shellStyle, style);
                else
                    return new DefaultInformationControl(parent, shellStyle, style, new HTMLTextPresenter(false));
            }
        };
    }

    /**
     * Wraps an HTML formatted string with a stylesheet for hover display
     * @param htmlText
     * @return
     */
    public static String addHTMLStyleSheet(String htmlText) {
        return htmlText != null ? HTML_PROLOG + htmlText + HTML_EPILOG : null;
    }

}


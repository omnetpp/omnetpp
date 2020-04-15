package org.omnetpp.common.util;

import java.awt.Color;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;

import javax.swing.text.AbstractDocument.AbstractElement;
import javax.swing.text.AttributeSet;
import javax.swing.text.BadLocationException;
import javax.swing.text.Element;
import javax.swing.text.SimpleAttributeSet;
import javax.swing.text.Style;
import javax.swing.text.StyleConstants;
import javax.swing.text.html.HTML;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLDocument.RunElement;
import javax.swing.text.html.HTMLEditorKit;
import javax.swing.text.html.StyleSheet;

import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.SWT;
import org.eclipse.swt.custom.ST;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.GC;
import org.eclipse.swt.graphics.GlyphMetrics;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.omnetpp.common.CommonCorePlugin;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.swt.custom.Bullet;
import org.omnetpp.common.swt.custom.PaintObjectEvent;
import org.omnetpp.common.swt.custom.PaintObjectListener;
import org.omnetpp.common.swt.custom.StyleRange;
import org.omnetpp.common.swt.custom.StyledText;

/**
 * This utility class converts HTML pages into StyledText widget content.
 * It understands a subset of HTML 3.2, the following tags are supported
 * (unknown tags are ignored):
 *  - b, i, u
 *  - br, p, div
 *  - h1, h2, .. h9
 *  - ul, ol, li
 *  - code, tt, pre
 *  - font
 *  - img
 *
 * Using different font families, font sizes, foreground and background colors,
 * and vertical spacing are also supported. Style can be applied via a limited
 * set of CSS rules and direct style attributes. CSS selectors support only
 * element types and simple class attributes. The conversion takes a map of images
 * that can be referred from img tags
 *
 * The conversion is done using the standard Java HTML parser found in Swing.
 * Note: you must set the initial font on the StyledText before converting the
 * HTML content. Otherwise vertical spacing will be wrong (due to font ascent/descent)
 *
 * @author levy
 */
public class HTMLUtils {
    /**
     * WORKAROUND: document.getForeground()/getBackground() methods return "black"
     * when no color was specified, instead of returning null. As a workaround,
     * we detect the absence of explicit colors by adding a low-priority rule
     * to the stylesheet with this FALLBACK_COLOR -- when the methods return
     * that value, we take it as null.
     */
    private final static Color FALLBACK_COLOR = new Color(1, 2, 3);

    /**
     * For resolving "img" tags. Its get() method receives the value of the "src" attribute,
     * and should return the image for it (or null if could not be resolved).
     */
    public interface IImageProvider {
        Image get(String name);
    }

    /**
     * This font must be set on the StyledText before it is actually shown. See above.
     */
    public static Font getInitialFont() {
        return new Font(Display.getDefault(), translateFontFamily("Monospaced"), 1, SWT.NORMAL);
    }

    public static void htmlToStyledText(String htmlText, StyledText styledText) {
        htmlToStyledText(new StringReader(htmlText), styledText, (IImageProvider)null);
    }

    public static void htmlToStyledText(String htmlText, StyledText styledText, Map<String, Image> imageMap) {
        htmlToStyledText(new StringReader(htmlText), styledText, imageMap);
    }

    public static void htmlToStyledText(String htmlText, StyledText styledText, IImageProvider imageProvider) {
        htmlToStyledText(new StringReader(htmlText), styledText, imageProvider);
    }

    public static void htmlToStyledText(InputStream inputStream, StyledText styledText) {
        htmlToStyledText(new InputStreamReader(inputStream), styledText, (IImageProvider)null);
    }

    public static void htmlToStyledText(InputStream inputStream, StyledText styledText, Map<String, Image> imageMap) {
        htmlToStyledText(new InputStreamReader(inputStream), styledText, imageMap);
    }

    public static void htmlToStyledText(InputStream inputStream, StyledText styledText, IImageProvider imageProvider) {
        htmlToStyledText(new InputStreamReader(inputStream), styledText, imageProvider);
    }

    public static void htmlToStyledText(Reader reader, StyledText styledText) {
        htmlToStyledText(reader, styledText, (IImageProvider)null);
    }

    public static void htmlToStyledText(Reader reader, StyledText styledText, Map<String, Image> imageMap) {
        htmlToStyledText(reader, styledText, (String name) -> imageMap.get(name));
    }

    /**
     * Converts an HTML 3.2 page and appends it as content to the given StyledText.
     *
     * @param htmlText the content of the page (HTML 3.2 subset)
     * @param styledText the widget where the content will be appended
     * @param imageProvider maps from names (to be used as img tag sources) to images
     */
    public static void htmlToStyledText(Reader reader, StyledText styledText, IImageProvider imageProvider) {
        try {
            HTMLEditorKit editorKit = new HTMLEditorKit();
            HTMLDocument document = (HTMLDocument)editorKit.createDefaultDocument();
            StyleSheet styles = document.getStyleSheet();
            // KLUDGE: add fallback rule to stylesheet, see FALLBACK_COLOR's comment
            String fallbackColor = ColorFactory.asString(FALLBACK_COLOR);
            styles.addRule("body, p, h1, h2, h3, h4, h5, h6, pre, code, ol, ul, dl, li, td, th, blockquote { color: " + fallbackColor + "; background-color: " + fallbackColor + "; }");
            editorKit.read(reader, document, 0);
            for (Element rootElement : document.getRootElements())
                htmlToStyledTextRecursive(new Context(), rootElement, styledText, imageProvider);
            // KLUDGE: for correct tooltip background/foreground colors
            styledText.setColorLock(true);
        }
        catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    private static void htmlToStyledTextRecursive(Context context, Element element, StyledText styledText, IImageProvider imageProvider) throws BadLocationException {
        String stringName = element.getName();
        HTMLDocument document = (HTMLDocument)element.getDocument();
        if (element instanceof RunElement) {
            if (stringName.equals("br"))
                styledText.append("\n");
            else if (stringName.equals("img")) {
                if (imageProvider != null) {
                    int startOffset = styledText.getCharCount();
                    Image image = imageProvider.get(getAttributeValue(element, "src"));
                    if (image == null)
                        image = CommonCorePlugin.getCachedImage("icons/unknown.png");
                    final Image finalImage = image;
                    Rectangle rectangle = image.getBounds();
                    // images are recognized in the text flow by this special unicode character
                    styledText.append("\uFFFC");
                    StyleRange styleRange = new StyleRange();
                    styleRange.start = startOffset;
                    styleRange.length = 1;
                    styleRange.metrics = new GlyphMetrics(rectangle.height, 0, rectangle.width);
                    styledText.setStyleRange(styleRange);
                    // add a separate listener for each image to paint it
                    styledText.addPaintObjectListener(new PaintObjectListener() {
                        @Override
                        public void paintObject(PaintObjectEvent event) {
                            GC gc = event.gc;
                            StyleRange style = event.style;
                            if (startOffset == style.start) {
                                int x = event.x;
                                int y = event.y + event.ascent - style.metrics.ascent;
                                gc.drawImage(finalImage, x, y);
                            }
                        }
                    });
                }
            }
            else {
                int startOffset = element.getStartOffset();
                int endOffset = element.getEndOffset();
                int length = endOffset - startOffset;
                String text = document.getText(startOffset, length);
                if (styledText.getCharCount() != 0 || !text.trim().isEmpty()) {
                    int charCount = styledText.getCharCount();
                    int lineCount = styledText.getLineCount();
                    styledText.append(text);
                    if (context.isPreformatted)
                        styledText.setLineWrapIndent(lineCount - 1, styledText.getLineCount() - lineCount + 1, -1);
                    AttributeSet attributeSet = getMergedAttributes(element);
                    Color foregroundColor = document.getForeground(attributeSet);
                    Color backgroundColor = document.getBackground(attributeSet);
                    // KLUDGE: If we get back FALLBACK_COLOR, no color was specified in the stylesheet or the document
                    if (foregroundColor != null && foregroundColor.equals(FALLBACK_COLOR))
                        foregroundColor = null;
                    if (backgroundColor != null && backgroundColor.equals(FALLBACK_COLOR))
                        backgroundColor = null;
                    StyleRange styleRange = new StyleRange(charCount, length, ColorFactory.asColor(foregroundColor), ColorFactory.asColor(backgroundColor));
                    styleRange.font = getFont(document.getFont(attributeSet));
                    styleRange.underline = Boolean.TRUE.equals(attributeSet.getAttribute(StyleConstants.Underline));
                    styledText.setStyleRange(styleRange);
                }
            }
        }
        else if (element instanceof AbstractElement) {
            AbstractElement abstractElement = (AbstractElement)element;
//            abstractElement.dump(System.out, 0);
            AttributeSet attributeSet = getMergedAttributes(element);
            Object name = attributeSet.getAttribute(StyleConstants.NameAttribute);
            // before processing tag
            if (stringName.equals("ul") || stringName.equals("ol")) {
                StyleRange styleRange = new StyleRange();
                styleRange.metrics = new GlyphMetrics(0, 0, (context.bullets.size() + 1) * 30);
                styleRange.font = getFont(document.getFont(attributeSet));
                Bullet bullet;
                if (stringName.equals("ul"))
                    bullet = new Bullet(ST.BULLET_DOT, styleRange);
                else {
                    bullet = new Bullet(ST.BULLET_NUMBER | ST.BULLET_TEXT, styleRange);
                    bullet.text = ".";
                }
                context.bullets.add(bullet);
            }
            else if (stringName.equals("li")) {
                int level = context.bullets.size() - 1;
                Bullet bullet = context.bullets.get(level);
                styledText.setLineWrapIndent(styledText.getLineCount() - 1, 1, (level + 1) * 30);
                styledText.setLineBullet(styledText.getLineCount() - 1, 1, bullet);
            }
            else if (stringName.matches("h[0-9]"))
                context.beginOffsets.add(styledText.getCharCount());
            else if (stringName.matches("pre"))
                context.isPreformatted = true;
            else if (stringName.matches("body")) {
                Color foregroundColor = document.getForeground(attributeSet);
                // KLUDGE: If we get back FALLBACK_COLOR, no color was specified in the stylesheet or the document
                if (foregroundColor != null && !foregroundColor.equals(FALLBACK_COLOR))
                    styledText.setForeground(ColorFactory.asColor(foregroundColor));
                Color backgroundColor = document.getBackground(attributeSet);
                if (backgroundColor != null && !backgroundColor.equals(FALLBACK_COLOR))
                    styledText.setBackground(ColorFactory.asColor(backgroundColor));
            }
            // handle space above
            boolean realTag = name instanceof HTML.Tag && name != HTML.Tag.CONTENT && name != HTML.Tag.IMPLIED;
            Object spaceAbove = attributeSet.getAttribute(StyleConstants.SpaceAbove);
            if (realTag && spaceAbove instanceof Number)
                insertVerticalSpacing(((Number)spaceAbove).intValue(), styledText);
            // recurse into tag
            for (int i = 0; i < abstractElement.getElementCount(); i++)
                htmlToStyledTextRecursive(context, abstractElement.getElement(i), styledText, imageProvider);
            // handle space below
            Object spaceBelow = attributeSet.getAttribute(StyleConstants.SpaceBelow);
            if (realTag && spaceBelow instanceof Number)
                insertVerticalSpacing(((Number)spaceBelow).intValue(), styledText);
            // after processing tag
            if (stringName.equals("ul") || stringName.equals("ol"))
                context.bullets.remove(context.bullets.size() - 1);
            else if (stringName.matches("pre"))
                context.isPreformatted = false;
        }
    }

    private static void insertVerticalSpacing(int height, StyledText styledText) {
        if (height > 0) {
            // insert an almost empty line that act as vertical spacing
            int startOffset = styledText.getCharCount();
            styledText.append(" \n");
            int endOffset = styledText.getCharCount();
            StyleRange styleRange = new StyleRange(startOffset, endOffset - startOffset, null, null);
            // TODO: why is this / 2?
            styleRange.font = getFont("Serif", height / 2, SWT.NORMAL);
            styledText.setStyleRange(styleRange);
        }
    }

    /**
     * Merges the element's attributes with the attributes of its element type
     * (e.g. h1, p) and the attributes of its class.
     */
    private static AttributeSet getMergedAttributes(Element element) {
        HTMLDocument document = (HTMLDocument)element.getDocument();
        StyleSheet styleSheet = document.getStyleSheet();
        SimpleAttributeSet resultAttributeSet = new SimpleAttributeSet(element.getAttributes());
        // get the style for the element class and merge
        String elementClass = getElementClass(element);
        SimpleAttributeSet elementClassAttributeSet = null;
        if (elementClass != null) {
            Style elementClassStyle = styleSheet.getRule("." + elementClass);
            if (elementClassStyle != null) {
                elementClassAttributeSet = new SimpleAttributeSet(elementClassStyle);
                resultAttributeSet.setResolveParent(elementClassAttributeSet);
            }
        }
        // get the style for the element type and merge
        String elementType = getElementType(element);
        if (elementType != null) {
            Style elementTypeStyle = styleSheet.getRule(elementType);
            if (elementTypeStyle != null) {
                if (elementClassAttributeSet != null)
                    elementClassAttributeSet.setResolveParent(elementTypeStyle);
                else
                    resultAttributeSet.setResolveParent(elementTypeStyle);
            }
        }
        return resultAttributeSet;
    }

    private static String getElementType(Element element) {
        while (element != null) {
            AttributeSet currentAttributeSet = element.getAttributes();
            Object name = currentAttributeSet.getAttribute(StyleConstants.NameAttribute);
            if (name instanceof HTML.Tag && name != HTML.Tag.CONTENT && name != HTML.Tag.IMPLIED)
                return element.getName();
            // KLUDGE: workaround for code and tt elements, they seem to be parsed incorrectly
            Enumeration<?> attributeNames = currentAttributeSet.getAttributeNames();
            while (attributeNames.hasMoreElements()) {
                Object attributeName = attributeNames.nextElement();
                if (attributeName.toString().equals("code") || attributeName.toString().equals("tt"))
                    return attributeName.toString();
            }
            element = element.getParentElement();
        }
        return null;
    }

    private static String getAttributeValue(Element element, String attribute) {
        // KLUDGE: workaround that we can't get an attribute directly
        AttributeSet attributeSet = element.getAttributes();
        Enumeration<?> attributeNames = attributeSet.getAttributeNames();
        while (attributeNames.hasMoreElements()) {
            Object attributeName = attributeNames.nextElement();
            if (attributeName.toString().equals(attribute))
                return (String)attributeSet.getAttribute(attributeName);
        }
        return null;
    }

    /**
     * Figuring out the element class is not trivial, because this parser
     * supports only HTML 3.2
     */
    private static String getElementClass(Element element) {
        while (element != null) {
            AttributeSet currentAttributeSet = element.getAttributes();
            Object name = currentAttributeSet.getAttribute(StyleConstants.NameAttribute);
            if (name instanceof HTML.Tag && name != HTML.Tag.CONTENT && name != HTML.Tag.IMPLIED) {
                Enumeration<?> attributeNames = currentAttributeSet.getAttributeNames();
                while (attributeNames.hasMoreElements()) {
                    Object attributeName = attributeNames.nextElement();
                    if (attributeName.toString().equals("class"))
                        return (String)currentAttributeSet.getAttribute(attributeName);
                }
            }
            element = element.getParentElement();
        }
        return null;
    }

    private static Font getFont(java.awt.Font font) {
        Display display = Display.getDefault();
        FontData fontData = new FontData();
        fontData.setName(translateFontFamily(font.getFamily()));
        int style = SWT.NORMAL;
        if ((font.getStyle() & java.awt.Font.BOLD) != 0)
            style |= SWT.BOLD;
        if ((font.getStyle() & java.awt.Font.ITALIC) != 0)
            style |= SWT.ITALIC;
        fontData.setStyle(style);
        // convert the font size (in pt for AWT) to height in pixels for SWT
        // TODO: this code was copied so I left it as it was, but
        // TODO: both AWT and SWT speaks in pt, so what?
        int height = (int) Math.round(font.getSize() * 72.0 / display.getDPI().y);
        fontData.setHeight(height);
        // TODO: check that dispose does't really have to be called, because
        // fonts are only used in StyleRange instances and the documentation of
        // the base class that is called TextStyle, says that dispose doesn't
        // have to be called
        return new Font(display, fontData);
    }

    private static Font getFont(String fontName, int height, int style) {
        Display display = Display.getDefault();
        // TODO: wrt. dispose see above
        return new Font(display, fontName, height, SWT.NORMAL);
    }

    /**
     * Convert symbolic font names.
     */
    private static String translateFontFamily(String family) {
        if (Platform.isRunning()) {
            String os = Platform.getOS();
            if (os.equals(Platform.OS_MACOSX)) {
                if (family.equals("Serif"))
                    family = "Times New Roman";
                else if (family.equals("SansSerif"))
                    family = "Lucida Grande";
                else if (family.equals("Monospaced"))
                    family = "Monaco";
            }
            else if (os.equals(Platform.OS_WIN32)) {
                if (family.equals("Monospaced"))
                    family = "Courier New";
            }
            else if (os.equals(Platform.OS_LINUX)) {
                if (family.equals("Monospaced"))
                    family = "Monospace";
            }
        }
        else {
            // fallback to linux settings when the platform is not running (testing)
            if (family.equals("Monospaced"))
                family = "Monospace";
        }
        return family;
    }

    /**
     * Intermediate state during converting HTML to styled text content.
     */
    private static class Context {
        public ArrayList<Integer> beginOffsets = new ArrayList<Integer>();
        public ArrayList<Bullet> bullets = new ArrayList<Bullet>();
        public boolean isPreformatted;
    }

    public static void main(String[] args) {
        Display display = new Display();
//        for (FontData fontData : display.getFontList(null, true))
//            System.out.println(fontData.name);
        Shell shell = new Shell(display);
        shell.setLayout(new FillLayout());
        Image image = new Image(display, 100, 100);
        GC gc = new GC(image);
        gc.setBackground(ColorFactory.WHITE);
        gc.fillRectangle(0, 0, 100, 100);
        gc.setBackground(ColorFactory.BLACK);
        gc.fillOval(0, 0, 100, 100);
        Map<String, Image> imageMap = new HashMap<String, Image>();
        imageMap.put("0", image);
        StyledText styledText = new StyledText(shell, SWT.V_SCROLL | SWT.H_SCROLL | SWT.WRAP | SWT.READ_ONLY);
        styledText.setFont(getInitialFont());
        String htmlText =
                "<html>" +
                "<head><style type='text/css'>" +
                ".someclass { color: purple; }" +
                "h1 { color: #ff0000; margin-top: 2px; margin-bottom: 64px; }" +
                "h2 { color: #cc0000; margin-bottom: 32px; }" +
                "h3 { color: #880000; margin-bottom: 16px; }" +
                "h4 { color: #440000; margin-bottom: 8px; }" +
                "p { color: green; font-size: 1.5em; margin-bottom: 8px; }" +
                "li { color: #880000; }" +
                "code { color: blue; font-size: 1.2em; }" +
                "tt { color: #000088; font-size: 1.2em; }" +
                "body { color: #000000; background-color: #ffffe1; " +
                "</style></head>" +
                "<body>" +
                "<b>bold text </b>" +
                "<i>italic text </i>" +
                "<b><i>bold and intalic text </i></b>" +
                "<u>underlined text </u>" +
                "<div class='someclass'>some text with class </div>" +
                "<br/>" +
                "<font color='red'>red foreground </font>" +
                "<font style='background-color: red'>red background </font>" +
                "<br/>" +
                "<font size='2'>small font </font>" +
                "<font size='5'>medium font </font>" +
                "<font size='10'>large font </font>" +
                "<br/>" +
                "<font face='Serif'>text with serif font </font>" +
                "<font face='SansSerif'>text with sansserif font </font>" +
                "<font face='Monospace'>text with monospace font </font>" +
                "<h1>header 1</h1>" +
                "<h2>header 2</h2>" +
                "<h3>header 3</h3>" +
                "<h4>header 4</h4>" +
                "a line break in<br/>a paragraph." +
                "<p>first paragraph - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</p>" +
                "<p>second paragraph - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</p>" +
                "<code>public static void main(String args[]) {}</code>" +
                "<tt>monospaced teletype text</tt>" +
                "<pre>preformatted text including a\ncouple of line breaks\nto show how they behave</pre>" +
                "<div>some unicode: \u271A \u2716 \u25CF \u25B2 \u25C6 \u25FE</div>" +
                "<ul><li>unordered list item 1 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                    "<li>unordered list item 2 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                    "<li>unordered nested sublist<ul>" +
                        "<li>nested unordered list item 1 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                        "<li>nested unordered list item 2 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li></ul></li>" +
                    "<li>unordered list item 3 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li></ul>" +
                "<ol><li>ordered list item 1 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                    "<li>ordered list item 2 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                    "<li>ordered nested sublist<ol>" +
                        "<li>nested ordered list item 1 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li>" +
                        "<li>nested ordered list item 2 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li></ol></li>" +
                    "<li>ordered list item 3 - Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum adipiscing tortor diam, eu semper massa. Morbi non dui lectus, ac commodo nulla. Duis mauris leo, ultricies vitae ultrices ac, egestas interdum nisl.</li></ol>" +
                "an inline circle <img src='0'/> image" +
                "</body></html>";
        htmlToStyledText(htmlText, styledText, imageMap);
        shell.open();
        shell.setSize(styledText.computeSize(800, -1));
        while (!shell.isDisposed())
            if (!display.readAndDispatch())
                display.sleep();
        display.dispose();
    }
}

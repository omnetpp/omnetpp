package com.swtworkbench.community.xswt.contrib;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.text.MessageFormat;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.swt.SWT;
import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.FillLayout;
import org.eclipse.swt.layout.FormAttachment;
import org.eclipse.swt.layout.FormData;
import org.eclipse.swt.layout.FormLayout;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.layout.RowData;
import org.eclipse.swt.layout.RowLayout;
import org.eclipse.swt.widgets.Button;
import org.eclipse.swt.widgets.Combo;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Group;
import org.eclipse.swt.widgets.Label;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Sash;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.TableColumn;
import org.eclipse.swt.widgets.TableItem;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.Widget;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.SAXException;

import com.swtworkbench.community.xswt.dataparser.parsers.color.CSSColors;
import com.swtworkbench.community.xswt.dataparser.parsers.color.IColorManager;

@SuppressWarnings({"rawtypes", "unchecked"})
public class XSWT_bobfoster {
    private IColorManager colorManager;
    private static Map mapColors;
    private static final Map mapStyles;
    private Map map = new HashMap();
    private LinkedList idRefs;

    static {
        Field field;
        int iModifiers;
        mapColors = new HashMap();

        Field[] fields = CSSColors.class.getDeclaredFields();

        for (int i = 0; i < fields.length; ++i) {
            field = fields[i];

            if (RGB.class.equals(field.getType())) {
                iModifiers = field.getModifiers();

                if ((!(Modifier.isPublic(iModifiers))) || (!(Modifier.isStatic(iModifiers))) || (!(Modifier.isFinal(iModifiers))))
                    continue;
                try {
                    mapColors.put(field.getName(), field.get(null));
                }
                catch (IllegalAccessException localIllegalAccessException1) {
                }
            }
        }

        mapStyles = new TreeMap();

        fields = SWT.class.getDeclaredFields();

        for (int i = 0; i < fields.length; ++i) {
            field = fields[i];

            if (Integer.TYPE.equals(field.getType())) {
                iModifiers = field.getModifiers();

                if ((!(Modifier.isPublic(iModifiers))) || (!(Modifier.isStatic(iModifiers))) || (!(Modifier.isFinal(iModifiers))))
                    continue;
                try {
                    mapStyles.put(field.getName(), field.get(null));
                }
                catch (IllegalAccessException localIllegalAccessException2) {
                }
            }
        }
    }

    public static Map create(Composite parent, InputStream inputStream) {
        return create(parent, inputStream, null, null, null);
    }

    public static Map create(Composite parent, String uri) {
        return create(parent, uri, null, null, null);
    }

    public static Map create(Composite parent, InputStream inputStream, IColorManager colorManager, ErrorHandler errorHandler,
            EntityResolver entityResolver) {
        try {
            DocumentBuilder documentBuilder = getDocumentBuilder(errorHandler, entityResolver);
            if (documentBuilder != null) {
                Document document = documentBuilder.parse(inputStream);
                return new XSWT_bobfoster(parent, document.getDocumentElement(), colorManager).map;
            }
        }
        catch (ParserConfigurationException eParserConfiguration) {
            System.err.println(eParserConfiguration);
        }
        catch (SAXException eSAX) {
            System.err.println(eSAX);
        }
        catch (IOException eIO) {
            System.err.println(eIO);
        }

        return null;
    }

    public static Map create(Composite parent, String uri, IColorManager colorManager, ErrorHandler errorHandler,
            EntityResolver entityResolver) {
        try {
            DocumentBuilder documentBuilder = getDocumentBuilder(errorHandler, entityResolver);
            if (documentBuilder != null) {
                Document document = documentBuilder.parse(uri);
                return new XSWT_bobfoster(parent, document.getDocumentElement(), colorManager).map;
            }
        }
        catch (ParserConfigurationException eParserConfiguration) {
            System.err.println(eParserConfiguration);
        }
        catch (SAXException eSAX) {
            System.err.println(eSAX);
        }
        catch (IOException eIO) {
            System.err.println(eIO);
        }

        return null;
    }

    private static DocumentBuilder getDocumentBuilder(ErrorHandler errorHandler, EntityResolver entityResolver)
            throws ParserConfigurationException {
        DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
        documentBuilder.setErrorHandler(errorHandler);
        documentBuilder.setEntityResolver(entityResolver);
        return documentBuilder;
    }

    private static boolean isChildElement(Node child, Node parent) {
        return ((child != null) && (child.getNodeType() == 1) && (child.getParentNode() != null) && (child.getParentNode()
                .equals(parent)));
    }

    private Color parseColor(Display display, String string) {
        RGB rgb = (RGB) mapColors.get(string.trim());
        if (rgb != null)
            return this.colorManager.getColor(display, rgb);
        return this.colorManager.getColor(display, parseRGB(string));
    }

    private static String[] parseTokenArray(String string) {
        StringTokenizer stringTokenizer = new StringTokenizer(string, " \t\r\n");
        int tokens = stringTokenizer.countTokens();
        String[] tokenArray = new String[tokens];

        for (int i = 0; i < tokens; ++i) {
            tokenArray[i] = stringTokenizer.nextToken();
        }
        return tokenArray;
    }

    private static int[] parseIntArray(String string) {
        StringTokenizer stringTokenizer = new StringTokenizer(string, " \t\r\n");
        int tokens = stringTokenizer.countTokens();
        int[] intArray = new int[tokens];

        for (int i = 0; i < tokens; ++i)
            try {
                intArray[i] = Integer.parseInt(stringTokenizer.nextToken().trim());
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        return intArray;
    }

    private static Point parsePoint(String string) {
        int[] intArray = parseIntArray(string);
        int length = intArray.length;
        return new Point((length >= 1) ? intArray[0] : 0, (length >= 2) ? intArray[1] : 0);
    }

    private static RGB parseRGB(String string) {
        int[] intArray = parseIntArray(string);
        int length = intArray.length;
        int red = (length >= 1) ? intArray[0] : 0;
        int green = (length >= 2) ? intArray[1] : 0;
        int blue = (length >= 3) ? intArray[1] : 0;

        if (red < 0)
            red = 0;
        else if (red > 255) {
            red = 255;
        }
        if (green < 0)
            green = 0;
        else if (green > 255) {
            green = 255;
        }
        if (blue < 0)
            blue = 0;
        else if (blue > 255) {
            blue = 255;
        }
        return new RGB(red, green, blue);
    }

    private static int parseStyle(String string) {
        if ((string == null) || (string.length() == 0))
            return 0;

        int style = 0;
        StringTokenizer stringTokenizer = new StringTokenizer(string, " \t\r\n");

        while (stringTokenizer.hasMoreTokens()) {
            String token = stringTokenizer.nextToken();
            Integer value = (Integer) mapStyles.get(token);

            if (value != null) {
                style |= value.intValue();
            }
        }
        return style;
    }

    private LinkedList getIdRefs() {
        if (this.idRefs == null)
            this.idRefs = new LinkedList();
        return this.idRefs;
    }

    private void addIdRef(String idRef, Object object) {
        getIdRefs().add(new IdRef(idRef, object));
    }

    private void resolveIdrefs() {
        if (this.idRefs == null)
            return;
        Iterator ids = this.idRefs.iterator();
        while (ids.hasNext()) {
            IdRef ref = (IdRef) ids.next();

            Control control = (Control) this.map.get(ref.idRef);
            if (control != null) {
                if (ref.refer instanceof FormAttachment) {
                    ((FormAttachment) ref.refer).control = control;
                }
            }
            else
                missingIdRefError(ref.idRef);
        }
    }

    private void missingIdRefError(String string) {
        System.err.println(MessageFormat.format("Unresolved ID reference {0}", new Object[] { string }));
    }

    private XSWT_bobfoster(Composite parent, Element element, IColorManager colorManager) {
        setColorManager(parent, colorManager);

        if ("XSWT_bobfoster".equals(element.getTagName())) {
            processXswt(parent, element);
            resolveIdrefs();
        }
    }

    private void setColorManager(Composite parent, IColorManager colorManager) {
        this.colorManager = colorManager;
        if (colorManager == null) {
            this.colorManager = new InternalColorManager();
            parent.addDisposeListener(new DisposeListener() {
                public void widgetDisposed(DisposeEvent e) {
                    XSWT_bobfoster.this.colorManager.dispose();
                }
            });
        }
    }

    private void processXswt(Composite parent, Element element) {
        processChildControls(parent, element);
    }

    private void processChildControls(Composite parent, Element element) {
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if (isChildElement(nodeChild, element))
                process(parent, (Element) nodeChild);
        }
    }

    private Control process(Composite parent, Element element) {
        Control result = null;
        String tagName = element.getTagName();
        int style = processStyle(element.getAttribute("style"));

        if ("button".equals(tagName)) {
            result = new Button(parent, style);
            processButton((Button) result, element);
        }
        else if ("label".equals(tagName)) {
            result = new Label(parent, style);
            processLabel((Label) result, element);
        }
        else if ("progressBar".equals(tagName)) {
            result = new ProgressBar(parent, style);
            processProgressBar((ProgressBar) result, element);
        }
        else if ("sash".equals(tagName)) {
            result = new Sash(parent, style);
            processSash((Sash) result, element);
        }
        else if ("scale".equals(tagName)) {
            result = new Scale(parent, style);
            processScale((Scale) result, element);
        }
        else if ("composite".equals(tagName)) {
            result = new Composite(parent, style);
            processComposite((Composite) result, element);
        }
        else if ("combo".equals(tagName)) {
            result = new Combo(parent, style);
            processCombo((Combo) result, element);
        }
        else if ("group".equals(tagName)) {
            result = new Group(parent, style);
            processGroup((Group) result, element);
        }
        else if ("tabFolder".equals(tagName)) {
            result = new TabFolder(parent, style);
            processTabFolder((TabFolder) result, element);
        }
        else if ("table".equals(tagName)) {
            result = new Table(parent, style);
            processTable((Table) result, element);
        }
        else if ("tree".equals(tagName)) {
            result = new Tree(parent, style);
            processTree((Tree) result, element);
        }
        else if ("list".equals(tagName)) {
            result = new List(parent, style);
            processList((List) result, element);
        }
        else if ("text".equals(tagName)) {
            result = new Text(parent, style);
            processText((Text) result, element);
        }

        return result;
    }

    private int processStyle(String string) {
        return ((string != null) ? parseStyle(string) : 0);
    }

    private void processWidget(Widget widget, Element element) {
        String attributeId = element.getAttribute("id");

        if ((attributeId != null) && (attributeId.length() > 0)) {
            widget.setData("id", attributeId);
            this.map.put(attributeId, widget);
        }

        processData(widget, element);
    }

    private void processData(Widget widget, Element element) {
        NodeList nodeListData = element.getElementsByTagName("data");

        for (int i = 0; i < nodeListData.getLength(); ++i) {
            Element elementData = (Element) nodeListData.item(i);

            if (elementData.getParentNode().equals(element)) {
                String attributeKey = elementData.getAttribute("key");
                String attributeValue = elementData.getAttribute("value");

                if (attributeKey == null)
                    widget.setData(attributeValue);
                else
                    widget.setData(attributeKey, attributeValue);
            }
        }
    }

    private void processControl(Control control, Element element) {
        processWidget(control, element);
        String attributeBackground = element.getAttribute("background");

        if ((attributeBackground != null) && (attributeBackground.length() > 0)) {
            control.setBackground(parseColor(control.getDisplay(), attributeBackground));
        }
        String attributeEnabled = getBooleanAttribute(element, "enabled");

        if (attributeEnabled != null) {
            control.setEnabled(attributeEnabled == "true");
        }
        String attributeFocus = getBooleanAttribute(element, "focus");

        if ((attributeFocus != null) && (attributeFocus == "true")) {
            control.setFocus();
        }

        String attributeForeground = element.getAttribute("foreground");

        if ((attributeForeground != null) && (attributeForeground.length() > 0)) {
            control.setForeground(parseColor(control.getDisplay(), attributeForeground));
        }
        String attributeLocation = element.getAttribute("location");

        if ((attributeLocation != null) && (attributeLocation.length() > 0)) {
            control.setLocation(parsePoint(attributeLocation));
        }
        String attributeSize = element.getAttribute("size");

        if ((attributeSize != null) && (attributeSize.length() > 0)) {
            control.setSize(parsePoint(attributeSize));
        }
        String attributeToolTipText = element.getAttribute("toolTipText");

        if ((attributeToolTipText != null) && (attributeToolTipText.length() > 0)) {
            control.setToolTipText(attributeToolTipText);
        }
        String attributeVisible = getBooleanAttribute(element, "visible");

        if (attributeVisible != null) {
            control.setVisible(attributeVisible == "true");
        }
        processFont(control, element);

        processLayoutData(control, element);
    }

    private void processFont(Control control, Element element) {
        NodeList nodeListFont = element.getChildNodes();

        for (int i = 0; i < nodeListFont.getLength(); ++i) {
            Node nodeFont = nodeListFont.item(i);

            if ((isElement(nodeFont)) && ("font".equals(nodeFont.getNodeName()))) {
                Element elementFont = (Element) nodeFont;
                LinkedList descriptions = new LinkedList();

                processFontDescription(descriptions, control, elementFont);
                processFontDescriptions(descriptions, control, elementFont);

                if (descriptions.size() > 0) {
                    FontData[] data = (FontData[]) descriptions.toArray(new FontData[descriptions.size()]);
                    control.setFont(new Font(control.getDisplay(), data));
                }
            }
        }
    }

    private void processFontDescriptions(LinkedList descriptions, Control control, Element element) {
        NodeList nodeListFontData = element.getChildNodes();

        for (int i = 0; i < nodeListFontData.getLength(); ++i) {
            Node nodeFontData = nodeListFontData.item(i);

            if ((isElement(nodeFontData)) && ("fontData".equals(nodeFontData.getNodeName())))
                processFontDescription(descriptions, control, (Element) nodeFontData);
        }
    }

    private void processFontDescription(LinkedList descriptions, Control control, Element element) {
        String fontName = getAttribute(element, "name");
        int fontHeight = getIntAttribute(element, "height");
        String fontStyle = getAttribute(element, "style");
        int style = (fontStyle == null) ? 0 : parseStyle(fontStyle);

        if ((fontName == null) || (fontHeight == -2147483648) || (fontStyle == null)) {
            Font font = control.getFont();
            if (font == null)
                return;
            FontData[] oldData = font.getFontData();
            if ((oldData == null) || (oldData.length == 0))
                return;
            FontData oldFontData = oldData[0];

            if (fontName == null)
                fontName = oldFontData.getName();
            if (fontHeight == -2147483648)
                fontHeight = oldFontData.getHeight();
            if (fontStyle == null) {
                style = oldFontData.getStyle();
            }
        }
        descriptions.add(new FontData(fontName, fontHeight, style));
    }

    private void processLayoutData(Control control, Element element) {
        NodeList nodeListLayoutData = element.getChildNodes();

        for (int i = 0; i < nodeListLayoutData.getLength(); ++i) {
            Node nodeLayoutData = nodeListLayoutData.item(i);

            if (isElement(nodeLayoutData)) {
                String name = nodeLayoutData.getNodeName();
                if ("gridData".equals(name))
                    processGridLayoutData(control, (Element) nodeLayoutData);
                else if ("rowData".equals(name))
                    processRowLayoutData(control, (Element) nodeLayoutData);
                else if ("tableData".equals(name))
                    processFormLayoutData(control, (Element) nodeLayoutData);
            }
        }
    }

    private void processFormLayoutData(Control control, Element element) {
        FormData formData = new FormData();

        String attributeHeight = getAttribute(element, "height");
        if ((attributeHeight != null) && (attributeHeight.length() > 0))
            try {
                formData.height = Integer.parseInt(attributeHeight);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeWidth = getAttribute(element, "width");
        if ((attributeWidth != null) && (attributeWidth.length() > 0))
            try {
                formData.width = Integer.parseInt(attributeWidth);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        NodeList nodeListAttachments = element.getChildNodes();

        for (int i = 0; i < nodeListAttachments.getLength(); ++i) {
            Node nodeLayoutAttachment = nodeListAttachments.item(i);

            if (isElement(nodeLayoutAttachment)) {
                String name = nodeLayoutAttachment.getNodeName();
                if ("bottom".equals(name))
                    formData.bottom = processAttachment((Element) nodeLayoutAttachment);
                else if ("left".equals(name))
                    formData.left = processAttachment((Element) nodeLayoutAttachment);
                else if ("right".equals(name))
                    formData.right = processAttachment((Element) nodeLayoutAttachment);
                else if ("top".equals(name))
                    formData.top = processAttachment((Element) nodeLayoutAttachment);
            }
        }
    }

    private FormAttachment processAttachment(Element element) {
        FormAttachment attachment = null;

        String attributeOffset = getAttribute(element, "offset");

        int offset = 0;

        if ((attributeOffset != null) && (attributeOffset.length() > 0))
            try {
                offset = Integer.parseInt(attributeOffset);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeControl = getAttribute(element, "control");
        if (attributeControl != null) {
            String attributeAlignment = getAttribute(element, "alignment");

            int alignment = -1;

            if (attributeAlignment != null) {
                Object style = mapStyles.get(attributeAlignment);
                if (style instanceof Integer) {
                    alignment = ((Integer) style).intValue();
                }
            }
            Control control = (Control) this.map.get(attributeControl);

            attachment = new FormAttachment(control, offset, alignment);

            if (control == null)
                addIdRef(attributeControl, attachment);
        }
        else {
            String attributeNumerator = getAttribute(element, "numerator");
            String attributeDenominator = getAttribute(element, "denominator");

            int numerator = 0;
            int denominator = 100;

            if ((attributeNumerator != null) && (attributeNumerator.length() > 0))
                try {
                    numerator = Integer.parseInt(attributeNumerator);
                }
                catch (NumberFormatException localNumberFormatException2) {
                }
            if ((attributeDenominator != null) && (attributeDenominator.length() > 0))
                try {
                    denominator = Integer.parseInt(attributeDenominator);
                }
                catch (NumberFormatException localNumberFormatException3) {
                }
            attachment = new FormAttachment(numerator, denominator, offset);
        }

        return attachment;
    }

    private void processGridLayoutData(Control control, Element element) {
        GridData layoutData = new GridData();
        String attributeGrabExcessHorizontalSpace = getBooleanAttribute(element, "grabExcessHorizontalSpace");

        if (attributeGrabExcessHorizontalSpace != null) {
            layoutData.grabExcessHorizontalSpace = (attributeGrabExcessHorizontalSpace == "true");
        }
        String attributeGrabExcessVerticalSpace = getBooleanAttribute(element, "grabExcessVerticalSpace");

        if (attributeGrabExcessVerticalSpace != null) {
            layoutData.grabExcessVerticalSpace = (attributeGrabExcessVerticalSpace == "true");
        }
        String attributeHeightHint = element.getAttribute("heightHint");

        if (attributeHeightHint != null)
            try {
                layoutData.heightHint = Integer.parseInt(attributeHeightHint);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeHorizontalAlignment = element.getAttribute("horizontalAlignment");

        if ("BEGINNING".equals(attributeHorizontalAlignment))
            layoutData.horizontalAlignment = 1;
        else if ("CENTER".equals(attributeHorizontalAlignment))
            layoutData.horizontalAlignment = 2;
        else if ("END".equals(attributeHorizontalAlignment))
            layoutData.horizontalAlignment = 3;
        else if ("FILL".equals(attributeHorizontalAlignment)) {
            layoutData.horizontalAlignment = 4;
        }
        String attributeHorizontalIndent = element.getAttribute("horizontalIndent");

        if (attributeHorizontalIndent != null)
            try {
                layoutData.horizontalIndent = Integer.parseInt(attributeHorizontalIndent);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeHorizontalSpan = element.getAttribute("horizontalSpan");

        if (attributeHorizontalSpan != null)
            try {
                layoutData.horizontalSpan = Integer.parseInt(attributeHorizontalSpan);
            }
            catch (NumberFormatException localNumberFormatException3) {
            }
        String attributeVerticalAlignment = element.getAttribute("verticalAlignment");

        if ("BEGINNING".equals(attributeVerticalAlignment))
            layoutData.verticalAlignment = 1;
        else if ("CENTER".equals(attributeVerticalAlignment))
            layoutData.verticalAlignment = 2;
        else if ("END".equals(attributeVerticalAlignment))
            layoutData.verticalAlignment = 3;
        else if ("FILL".equals(attributeVerticalAlignment)) {
            layoutData.verticalAlignment = 4;
        }
        String attributeVerticalSpan = element.getAttribute("verticalSpan");

        if (attributeVerticalSpan != null)
            try {
                layoutData.verticalSpan = Integer.parseInt(attributeVerticalSpan);
            }
            catch (NumberFormatException localNumberFormatException4) {
            }
        String attributeWidthHint = element.getAttribute("widthHint");

        if (attributeWidthHint != null)
            try {
                layoutData.widthHint = Integer.parseInt(attributeWidthHint);
            }
            catch (NumberFormatException localNumberFormatException5) {
            }
        control.setLayoutData(layoutData);
    }

    private void processRowLayoutData(Control control, Element element) {
        int height = 0;
        int width = 0;
        String attributeHeight = element.getAttribute("height");

        if (attributeHeight != null)
            try {
                height = Integer.parseInt(attributeHeight);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeWidth = element.getAttribute("width");

        if (attributeWidth != null)
            try {
                width = Integer.parseInt(attributeWidth);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        control.setLayoutData(new RowData(width, height));
    }

    private void processButton(Button button, Element element) {
        processControl(button, element);
        button.setAlignment(processStyle(element.getAttribute("alignment")));
        String attributeImage = element.getAttribute("image");

        if ((attributeImage != null) && (attributeImage.length() > 0))
            try {
                button.setImage(new Image(button.getDisplay(), attributeImage));
            }
            catch (Exception localException) {
            }
        String attributeSelection = getBooleanAttribute(element, "selection");

        if (attributeSelection != null) {
            button.setSelection(attributeSelection == "true");
        }
        String attributeText = element.getAttribute("text");

        if ((attributeText != null) && (attributeText.length() > 0))
            button.setText(attributeText);
    }

    private void processLabel(Label label, Element element) {
        processControl(label, element);
        label.setAlignment(processStyle(element.getAttribute("alignment")));
        String attributeImage = element.getAttribute("image");

        if ((attributeImage != null) && (attributeImage.length() > 0))
            try {
                label.setImage(new Image(label.getDisplay(), attributeImage));
            }
            catch (Exception localException) {
            }
        String attributeText = element.getAttribute("text");

        if ((attributeText != null) && (attributeText.length() > 0))
            label.setText(attributeText);
    }

    private void processProgressBar(ProgressBar progressBar, Element element) {
        processControl(progressBar, element);
        String attributeMaximum = element.getAttribute("maximum");

        if (attributeMaximum != null)
            try {
                progressBar.setMaximum(Integer.parseInt(attributeMaximum));
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMinimum = element.getAttribute("minimum");

        if (attributeMinimum != null)
            try {
                progressBar.setMinimum(Integer.parseInt(attributeMinimum));
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeSelection = element.getAttribute("selection");

        if (attributeSelection == null)
            return;
        try {
            progressBar.setSelection(Integer.parseInt(attributeSelection));
        }
        catch (NumberFormatException localNumberFormatException3) {
        }
    }

    private void processSash(Sash sash, Element element) {
        processControl(sash, element);
    }

    private void processScale(Scale scale, Element element) {
        processControl(scale, element);
        String attributeIncrement = element.getAttribute("increment");

        if (attributeIncrement != null)
            try {
                scale.setIncrement(Integer.parseInt(attributeIncrement));
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMaximum = element.getAttribute("maximum");

        if (attributeMaximum != null)
            try {
                scale.setMaximum(Integer.parseInt(attributeMaximum));
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeMinimum = element.getAttribute("minimum");

        if (attributeMinimum != null)
            try {
                scale.setMinimum(Integer.parseInt(attributeMinimum));
            }
            catch (NumberFormatException localNumberFormatException3) {
            }
        String attributePageIncrement = element.getAttribute("pageIncrement");

        if (attributePageIncrement != null)
            try {
                scale.setPageIncrement(Integer.parseInt(attributePageIncrement));
            }
            catch (NumberFormatException localNumberFormatException4) {
            }
        String attributeSelection = element.getAttribute("selection");

        if (attributeSelection == null)
            return;
        try {
            scale.setSelection(Integer.parseInt(attributeSelection));
        }
        catch (NumberFormatException localNumberFormatException5) {
        }
    }

    private void processScrollable(Scrollable scrollable, Element element) {
        processControl(scrollable, element);
    }

    private void processComposite(Composite composite, Element element) {
        processScrollable(composite, element);
        processLayout(composite, element);
        processChildren(composite, element);
        processTabList(composite, element);
    }

    private void processTabList(Composite composite, Element element) {
        String tabList = element.getAttribute("tabList");

        String[] idRefs = parseTokenArray(tabList);
        LinkedList list = new LinkedList();

        for (int i = 0; i < idRefs.length; ++i) {
            String idRef = idRefs[i];
            Control control = (Control) this.map.get(idRef);
            if (control != null)
                list.add(control);
            else {
                missingIdRefError(idRef);
            }
        }
        if (list.size() > 0)
            composite.setTabList((Control[]) list.toArray(new Control[list.size()]));
    }

    private void processLayout(Composite composite, Element element) {
        NodeList nodeListLayout = element.getChildNodes();

        for (int i = 0; i < nodeListLayout.getLength(); ++i) {
            Node nodeLayout = nodeListLayout.item(i);

            if (isElement(nodeLayout)) {
                String name = nodeLayout.getNodeName();
                if ("fillLayout".equals(name))
                    processFillLayout(composite, (Element) nodeLayout);
                else if ("gridLayout".equals(name))
                    processGridLayout(composite, (Element) nodeLayout);
                else if ("rowLayout".equals(name))
                    processRowLayout(composite, (Element) nodeLayout);
                else if ("formLayout".equals(name))
                    processFormLayout(composite, (Element) nodeLayout);
            }
        }
    }

    private void processFormLayout(Composite composite, Element element) {
        FormLayout formLayout = new FormLayout();

        String attributeMarginHeight = getAttribute(element, "marginHeight");
        if ((attributeMarginHeight != null) && (attributeMarginHeight.length() > 0))
            try {
                formLayout.marginHeight = Integer.parseInt(attributeMarginHeight);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMarginWidth = getAttribute(element, "marginWidth");
        if ((attributeMarginWidth != null) && (attributeMarginWidth.length() > 0))
            try {
                formLayout.marginWidth = Integer.parseInt(attributeMarginWidth);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        composite.setLayout(formLayout);
    }

    private void processFillLayout(Composite composite, Element element) {
        FillLayout layout = new FillLayout();
        layout.type = processStyle(element.getAttribute("type"));
        composite.setLayout(layout);
    }

    private void processGridLayout(Composite composite, Element element) {
        GridLayout layout = new GridLayout();
        String attributeHorizontalSpacing = element.getAttribute("horizontalSpacing");

        if (attributeHorizontalSpacing != null)
            try {
                layout.horizontalSpacing = Integer.parseInt(attributeHorizontalSpacing);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMakeColumnsEqualWidth = getBooleanAttribute(element, "makeColumnsEqualWidth");

        if (attributeMakeColumnsEqualWidth != null) {
            layout.makeColumnsEqualWidth = (attributeMakeColumnsEqualWidth == "true");
        }
        String attributeMarginHeight = element.getAttribute("marginHeight");

        if (attributeMarginHeight != null)
            try {
                layout.marginHeight = Integer.parseInt(attributeMarginHeight);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeMarginWidth = element.getAttribute("marginWidth");

        if (attributeMarginWidth != null)
            try {
                layout.marginWidth = Integer.parseInt(attributeMarginWidth);
            }
            catch (NumberFormatException localNumberFormatException3) {
            }
        String attributeNumColumns = element.getAttribute("numColumns");

        if (attributeNumColumns != null)
            try {
                layout.numColumns = Integer.parseInt(attributeNumColumns);
            }
            catch (NumberFormatException localNumberFormatException4) {
            }
        String attributeVerticalSpacing = element.getAttribute("verticalSpacing");

        if (attributeVerticalSpacing != null)
            try {
                layout.verticalSpacing = Integer.parseInt(attributeVerticalSpacing);
            }
            catch (NumberFormatException localNumberFormatException5) {
            }
        composite.setLayout(layout);
    }

    private void processRowLayout(Composite composite, Element element) {
        RowLayout layout = new RowLayout();
        String attributeFill = getBooleanAttribute(element, "fill");

        if (attributeFill != null)
            ;
        String attributeJustify = getBooleanAttribute(element, "justify");

        if (attributeJustify != null) {
            layout.justify = (attributeJustify == "true");
        }
        String attributeMarginBottom = element.getAttribute("marginBottom");

        if (attributeMarginBottom != null)
            try {
                layout.marginBottom = Integer.parseInt(attributeMarginBottom);
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMarginLeft = element.getAttribute("marginLeft");

        if (attributeMarginLeft != null)
            try {
                layout.marginLeft = Integer.parseInt(attributeMarginLeft);
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeMarginRight = element.getAttribute("marginRight");

        if (attributeMarginRight != null)
            try {
                layout.marginRight = Integer.parseInt(attributeMarginRight);
            }
            catch (NumberFormatException localNumberFormatException3) {
            }
        String attributeMarginTop = element.getAttribute("marginTop");

        if (attributeMarginTop != null)
            try {
                layout.marginTop = Integer.parseInt(attributeMarginTop);
            }
            catch (NumberFormatException localNumberFormatException4) {
            }
        String attributePack = getBooleanAttribute(element, "pack");

        if (attributePack != null) {
            layout.pack = (attributePack == "true");
        }
        String attributeSpacing = element.getAttribute("spacing");

        if (attributeSpacing != null)
            try {
                layout.spacing = Integer.parseInt(attributeSpacing);
            }
            catch (NumberFormatException localNumberFormatException5) {
            }
        layout.type = processStyle(element.getAttribute("type"));
        String attributeWrap = getBooleanAttribute(element, "wrap");

        if (attributeWrap != null) {
            layout.wrap = (attributeWrap == "true");
        }
        composite.setLayout(layout);
    }

    private void processChildren(Composite composite, Element element) {
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isElement(nodeChild)) && ("children".equals(nodeChild.getNodeName())))
                processChildControls(composite, (Element) nodeChild);
        }
    }

    private void processCombo(Combo combo, Element element) {
        processComposite(combo, element);
        String attributeSelection = element.getAttribute("selection");

        if ((attributeSelection != null) && (attributeSelection.length() > 0)) {
            int[] intArray = parseIntArray(attributeSelection);
            int length = intArray.length;

            if (length == 1)
                combo.select(intArray[0]);
            else if (length == 2) {
                combo.setSelection(new Point(intArray[0], intArray[1]));
            }
        }
        String attributeText = element.getAttribute("text");

        if ((attributeText != null) && (attributeText.length() > 0)) {
            combo.setText(attributeText);
        }
        String attributeTextLimit = element.getAttribute("textLimit");

        if (attributeTextLimit != null)
            try {
                combo.setTextLimit(Integer.parseInt(attributeTextLimit));
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isChildElement(nodeChild, element)) && ("items".equals(((Element) nodeChild).getTagName()))) {
                Element elementItems = (Element) nodeChild;
                NodeList nodeListChildren2 = elementItems.getChildNodes();

                for (int j = 0; j < nodeListChildren2.getLength(); ++j) {
                    Node nodeChild2 = nodeListChildren2.item(j);

                    if ((isChildElement(nodeChild2, elementItems)) && ("item".equals(((Element) nodeChild2).getTagName()))) {
                        Element elementItem = (Element) nodeChild2;
                        String attributeValue = elementItem.getAttribute("value");

                        if (attributeValue != null)
                            combo.add(attributeValue);
                    }
                }
            }
        }
    }

    private void processGroup(Group group, Element element) {
        processComposite(group, element);
        String attributeText = element.getAttribute("text");

        if ((attributeText != null) && (attributeText.length() > 0))
            group.setText(attributeText);
    }

    private void processTabFolder(TabFolder tabFolder, Element element) {
        processComposite(tabFolder, element);
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isChildElement(nodeChild, element)) && ("items".equals(((Element) nodeChild).getTagName()))) {
                Element elementItems = (Element) nodeChild;
                NodeList nodeListChildren2 = elementItems.getChildNodes();

                for (int j = 0; j < nodeListChildren2.getLength(); ++j) {
                    Node nodeChild2 = nodeListChildren2.item(j);

                    if ((isChildElement(nodeChild2, elementItems)) && ("item".equals(((Element) nodeChild2).getTagName()))) {
                        Element elementItem = (Element) nodeChild2;
                        TabItem tabItem = new TabItem(tabFolder, processStyle(elementItem.getAttribute("style")));
                        String attributeImage = elementItem.getAttribute("image");

                        if ((attributeImage != null) && (attributeImage.length() > 0))
                            try {
                                tabItem.setImage(new Image(tabItem.getDisplay(), attributeImage));
                            }
                            catch (Exception localException) {
                            }
                        String attributeText = elementItem.getAttribute("text");

                        if ((attributeText != null) && (attributeText.length() > 0)) {
                            tabItem.setText(attributeText);
                        }
                        String attributeToolTipText = elementItem.getAttribute("toolTipText");

                        if ((attributeToolTipText != null) && (attributeToolTipText.length() > 0)) {
                            tabItem.setToolTipText(attributeToolTipText);
                        }
                        NodeList nodeListChildren3 = elementItem.getChildNodes();

                        for (int k = 0; k < nodeListChildren3.getLength(); ++k) {
                            Node nodeChild3 = nodeListChildren3.item(k);

                            if ((isChildElement(nodeChild3, elementItem))
                                    && ("control".equals(((Element) nodeChild3).getTagName()))) {
                                Element elementControl = (Element) nodeChild3;
                                NodeList nodeListChildren4 = elementControl.getChildNodes();

                                for (int l = 0; l < nodeListChildren4.getLength(); ++l) {
                                    Node nodeChild4 = nodeListChildren4.item(l);

                                    if (isChildElement(nodeChild4, elementControl)) {
                                        tabItem.setControl(process(tabFolder, (Element) nodeChild4));
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        String attributeSelection = element.getAttribute("selection");

        if (attributeSelection == null)
            return;
        try {
            tabFolder.setSelection(Integer.parseInt(attributeSelection));
        }
        catch (NumberFormatException localNumberFormatException) {
        }
    }

    private void processTable(Table table, Element element) {
        processControl(table, element);
        String attributeHeaderVisible = getBooleanAttribute(element, "headerVisible");

        if (attributeHeaderVisible != null) {
            table.setHeaderVisible(attributeHeaderVisible == "true");
        }
        String attributeLinesVisible = getBooleanAttribute(element, "linesVisible");

        if (attributeLinesVisible != null) {
            table.setLinesVisible(attributeLinesVisible == "true");
        }
        boolean[] packColumns = processColumns(table, element);

        processRows(table, element);

        String attributeSelection = element.getAttribute("selection");

        if ((attributeSelection != null) && (attributeSelection.length() > 0)) {
            int[] intArray = parseIntArray(attributeSelection);
            int length = intArray.length;

            if (length == 1)
                table.setSelection(intArray[0]);
            else if (length == 2) {
                table.setSelection(intArray[0], intArray[1]);
            }
        }
        String attributeTopIndex = element.getAttribute("topIndex");

        if (attributeTopIndex != null)
            try {
                table.setTopIndex(Integer.parseInt(attributeTopIndex));
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        TableColumn[] columns = table.getColumns();
        for (int i = 0; i < columns.length; ++i)
            if (packColumns[i])
                columns[i].pack();
    }

    private boolean[] processColumns(Table table, Element element) {
        LinkedList list = new LinkedList();
        boolean[] result = new boolean[0];

        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isElement(nodeChild)) && ("columns".equals(nodeChild.getNodeName()))) {
                NodeList nodeListColumns = ((Element) nodeChild).getChildNodes();

                list.clear();

                for (int j = 0; j < nodeListColumns.getLength(); ++j) {
                    Node nodeColumn = nodeListColumns.item(j);

                    if ((isElement(nodeColumn)) && ("tableColumn".equals(nodeColumn.getNodeName()))) {
                        Element elementColumn = (Element) nodeColumn;

                        TableColumn tableColumn = new TableColumn(table, processStyle(element.getAttribute("style")));

                        int alignment = getIntAttribute(elementColumn, "alignment");
                        if (alignment != -2147483648) {
                            tableColumn.setAlignment(alignment);
                        }
                        Image attributeImage = getImageAttribute(elementColumn, table, "image");
                        if (attributeImage != null) {
                            tableColumn.setImage(attributeImage);
                        }
                        String attributePack = getBooleanAttribute(elementColumn, "pack");
                        list.add(((attributePack != null) && (attributePack == "true")) ? Boolean.TRUE : Boolean.FALSE);

                        String attributeResizable = getBooleanAttribute(elementColumn, "resizable");
                        if (attributeResizable != null) {
                            tableColumn.setResizable(attributeResizable == "true");
                        }
                        String attributeText = getAttribute(elementColumn, "text");
                        if (attributeText != null) {
                            tableColumn.setText(attributeText);
                        }
                        int width = getIntAttribute(elementColumn, "width");
                        if (width != -2147483648) {
                            tableColumn.setWidth(width);
                        }
                    }
                }
                result = new boolean[list.size()];
                Iterator it = list.iterator();
                for (int j = 0; it.hasNext(); ++j)
                    result[j] = ((Boolean) it.next()).booleanValue();
                break;
            }
        }

        return result;
    }

    private void processRows(Table table, Element element) {
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);
            String childName = nodeChild.getNodeName();

            if ((isElement(nodeChild)) && ("items".equals(childName))) {
                NodeList nodeListRows = ((Element) nodeChild).getChildNodes();

                for (int j = 0; j < nodeListRows.getLength(); ++j) {
                    Node nodeRow = nodeListRows.item(j);
                    String rowName = nodeRow.getNodeName();

                    if ((isElement(nodeRow)) && ("tableItem".equals(rowName)))
                        processRow(table, (Element) nodeRow);
                }
            }
        }
    }

    private boolean isElement(Node node) {
        return (node.getNodeType() == 1);
    }

    private void processRow(Table table, Element rowElement) {
        TableItem item = new TableItem(table, parseStyle(rowElement.getAttribute("style")));

        String background = getAttribute(rowElement, "background");
        if (background != null) {
            Color bgcolor = parseColor(table.getDisplay(), background);
            item.setBackground(bgcolor);
        }

        String foreground = getAttribute(rowElement, "foreground");
        if (foreground != null) {
            Color fgcolor = parseColor(table.getDisplay(), foreground);
            item.setForeground(fgcolor);
        }

        String checked = getBooleanAttribute(rowElement, "checked");
        if (checked != null) {
            item.setChecked(checked == "true");
        }
        String grayed = getBooleanAttribute(rowElement, "grayed");
        if (grayed != null) {
            item.setGrayed(grayed == "true");
        }
        String imageIndent = getAttribute(rowElement, "imageIndent");
        if (imageIndent != null) {
            try {
                int indent = Integer.parseInt(imageIndent);
                item.setImageIndent(indent);
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        }
        processItemData(item, rowElement);
    }

    private void processItemData(TableItem item, Element element) {
        NodeList nodeListColumn = element.getChildNodes();

        int i = 0;
        for (int n = 0; i < nodeListColumn.getLength(); ++i) {
            Node nodeColumn = nodeListColumn.item(i);

            if ((isElement(nodeColumn)) && ("data".equals(nodeColumn.getNodeName()))) {
                Element elementColumn = (Element) nodeColumn;

                Image attributeImage = getImageAttribute(elementColumn, item, "image");
                if (attributeImage != null) {
                    item.setImage(attributeImage);
                }
                String attributeText = getAttribute(elementColumn, "text");
                if ((attributeText != null) && (attributeText.length() > 0)) {
                    item.setText(n, attributeText);
                }
                ++n;
            }
        }
    }

    private Image getImageAttribute(Element element, Widget widget, String attributeName) {
        Image image = null;
        String attributeImage = getAttribute(element, attributeName);
        if (attributeImage != null)
            try {
                image = new Image(widget.getDisplay(), attributeImage);
            }
            catch (Exception localException) {
            }
        return image;
    }

    private int getIntAttribute(Element element, String attributeName) {
        String attributeValue = element.getAttribute(attributeName);
        int result = -2147483648;
        if ((attributeValue != null) && (attributeValue.length() > 0))
            try {
                result = Integer.parseInt(attributeValue.trim());
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        return result;
    }

    private String getAttribute(Element element, String attributeName) {
        String value = element.getAttribute(attributeName);
        return (((value != null) && (value.length() > 0)) ? value.trim() : null);
    }

    private String getBooleanAttribute(Element element, String attributeName) {
        String value = getAttribute(element, attributeName);
        if ((value != null) && (value.length() > 0)) {
            if (("1".equals(value)) || ("true".equals(value)))
                value = "true";
        }
        else
            value = null;
        return value;
    }

    private void processTree(Tree tree, Element element) {
        processComposite(tree, element);
    }

    private void processList(List list, Element element) {
        processScrollable(list, element);
        String attributeSelection = element.getAttribute("selection");

        if ((attributeSelection != null) && (attributeSelection.length() > 0)) {
            int[] intArray = parseIntArray(attributeSelection);
            int length = intArray.length;

            if (length == 1)
                list.setSelection(intArray[0]);
            else if (length == 2) {
                list.setSelection(intArray[0], intArray[1]);
            }
        }
        String attributeTopIndex = element.getAttribute("topIndex");

        if (attributeTopIndex != null)
            try {
                list.setTopIndex(Integer.parseInt(attributeTopIndex));
            }
            catch (NumberFormatException localNumberFormatException) {
            }
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isChildElement(nodeChild, element)) && ("items".equals(((Element) nodeChild).getTagName()))) {
                Element elementItems = (Element) nodeChild;
                NodeList nodeListChildren2 = elementItems.getChildNodes();

                for (int j = 0; j < nodeListChildren2.getLength(); ++j) {
                    Node nodeChild2 = nodeListChildren2.item(j);

                    if ((isChildElement(nodeChild2, elementItems)) && ("item".equals(((Element) nodeChild2).getTagName()))) {
                        Element elementItem = (Element) nodeChild2;
                        String attributeValue = elementItem.getAttribute("value");

                        if (attributeValue != null)
                            list.add(attributeValue);
                    }
                }
            }
        }
    }

    private void processText(Text text, Element element) {
        processScrollable(text, element);
        String attributeDoubleClickEnabled = getBooleanAttribute(element, "doubleClickEnabled");

        if (attributeDoubleClickEnabled != null) {
            text.setDoubleClickEnabled(attributeDoubleClickEnabled == "true");
        }
        String attributeEchoChar = element.getAttribute("echoChar");

        if ((attributeEchoChar != null) && (attributeEchoChar.length() == 1)) {
            text.setEchoChar(attributeEchoChar.charAt(0));
        }
        String attributeEditable = getBooleanAttribute(element, "editable");

        if (attributeEditable != null) {
            text.setEditable(attributeEditable == "true");
        }
        String attributeSelection = element.getAttribute("selection");

        if ((attributeSelection != null) && (attributeSelection.length() > 0)) {
            int[] intArray = parseIntArray(attributeSelection);
            int length = intArray.length;

            if (length == 1)
                text.setSelection(intArray[0]);
            else if (length == 2) {
                text.setSelection(intArray[0], intArray[1]);
            }
        }
        String attributeTabs = element.getAttribute("tabs");

        if (attributeTabs != null)
            try {
                text.setTabs(Integer.parseInt(attributeTabs));
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeText = element.getAttribute("text");

        if ((attributeText != null) && (attributeText.length() > 0)) {
            text.setText(attributeText);
        }
        String attributeTextLimit = element.getAttribute("textLimit");

        if (attributeTextLimit != null)
            try {
                text.setTextLimit(Integer.parseInt(attributeTextLimit));
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeTopIndex = element.getAttribute("topIndex");

        if (attributeTopIndex == null)
            return;
        try {
            text.setTopIndex(Integer.parseInt(attributeTopIndex));
        }
        catch (NumberFormatException localNumberFormatException3) {
        }
    }

    private void processSlider(Slider slider, Element element) {
        processControl(slider, element);
        String attributeIncrement = element.getAttribute("increment");

        if (attributeIncrement != null)
            try {
                slider.setIncrement(Integer.parseInt(attributeIncrement));
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        String attributeMaximum = element.getAttribute("maximum");

        if (attributeMaximum != null)
            try {
                slider.setMaximum(Integer.parseInt(attributeMaximum));
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        String attributeMinimum = element.getAttribute("minimum");

        if (attributeMinimum != null)
            try {
                slider.setMinimum(Integer.parseInt(attributeMinimum));
            }
            catch (NumberFormatException localNumberFormatException3) {
            }
        String attributePageIncrement = element.getAttribute("pageIncrement");

        if (attributePageIncrement != null)
            try {
                slider.setPageIncrement(Integer.parseInt(attributePageIncrement));
            }
            catch (NumberFormatException localNumberFormatException4) {
            }
        String attributeSelection = element.getAttribute("selection");

        if (attributeSelection != null)
            try {
                slider.setSelection(Integer.parseInt(attributeSelection));
            }
            catch (NumberFormatException localNumberFormatException5) {
            }
        String attributeThumb = element.getAttribute("thumb");

        if (attributeThumb == null)
            return;
        try {
            slider.setThumb(Integer.parseInt(attributeThumb));
        }
        catch (NumberFormatException localNumberFormatException6) {
        }
    }

    private static class InternalColorManager implements IColorManager {
        Map colors = new HashMap();

        public void dispose() {
            Iterator cit = this.colors.values().iterator();
            while (cit.hasNext())
                ((Color) cit.next()).dispose();
            this.colors.clear();
        }

        public Color getColor(Display display, RGB rgb) {
            Color color = (Color) this.colors.get(rgb);
            if (color == null) {
                color = new Color(display, rgb);
                this.colors.put(rgb, color);
            }
            return color;
        }
    }

    private static class IdRef {
        public Object refer;
        public String idRef;

        public IdRef(String idRef, Object refer) {
            this.idRef = idRef;
            this.refer = refer;
        }
    }
}
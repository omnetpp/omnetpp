package com.swtworkbench.community.xswt.contrib;

import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Modifier;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.TreeMap;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.layout.FillLayout;
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
import org.eclipse.swt.widgets.Layout;
import org.eclipse.swt.widgets.List;
import org.eclipse.swt.widgets.ProgressBar;
import org.eclipse.swt.widgets.Sash;
import org.eclipse.swt.widgets.Scale;
import org.eclipse.swt.widgets.Scrollable;
import org.eclipse.swt.widgets.Slider;
import org.eclipse.swt.widgets.TabFolder;
import org.eclipse.swt.widgets.TabItem;
import org.eclipse.swt.widgets.Table;
import org.eclipse.swt.widgets.Text;
import org.eclipse.swt.widgets.Tree;
import org.eclipse.swt.widgets.Widget;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class XSWT_orig {
    private static final Map mapStyles = new TreeMap();

    private Map map = new HashMap();

    static {
        Field[] fields = SWT.class.getDeclaredFields();

        for (int i = 0; i < fields.length; ++i) {
            Field field = fields[i];

            if (Integer.TYPE.equals(field.getType())) {
                int iModifiers = field.getModifiers();

                if ((!(Modifier.isPublic(iModifiers))) || (!(Modifier.isStatic(iModifiers))) || (!(Modifier.isFinal(iModifiers))))
                    continue;
                try {
                    mapStyles.put(field.getName(), field.get(null));
                }
                catch (IllegalAccessException localIllegalAccessException) {
                }
            }
        }
    }

    public static Map create(Composite parent, InputStream inputStream) {
        DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
        try {
            DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
            Document document = documentBuilder.parse(inputStream);
            return new XSWT_orig(parent, document.getDocumentElement()).map;
        }
        catch (ParserConfigurationException eParserConfiguration) {
            System.err.println(eParserConfiguration);
        }
        catch (SAXException eSAX) {
            System.err.println(eSAX);
        }
        catch (IOException e) {
            System.err.println(e);
        }

        return null;
    }

    public static Map create(Composite parent, String uri) {
        DocumentBuilderFactory documentBuilderFactory = DocumentBuilderFactory.newInstance();
        try {
            DocumentBuilder documentBuilder = documentBuilderFactory.newDocumentBuilder();
            Document document = documentBuilder.parse(uri);
            return new XSWT_orig(parent, document.getDocumentElement()).map;
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

    private static boolean isChildElement(Node child, Node parent) {
        return ((child != null) && (child.getNodeType() == 1) && (child.getParentNode() != null) && (child.getParentNode()
                .equals(parent)));
    }

    private static Color parseColor(Display display, String string) {
        return new Color(display, parseRGB(string));
    }

    private static Font parseFont(Display display, String string) {
        String name = null;
        int height = 10;
        int style = 0;
        StringTokenizer stringTokenizer = new StringTokenizer(string, ",");

        if (stringTokenizer.hasMoreTokens()) {
            name = stringTokenizer.nextToken().trim();
        }
        if (stringTokenizer.hasMoreTokens())
            try {
                height = Integer.parseInt(stringTokenizer.nextToken().trim());
            }
            catch (NumberFormatException localNumberFormatException1) {
            }
        if (stringTokenizer.hasMoreTokens())
            try {
                style = Integer.parseInt(stringTokenizer.nextToken().trim());
            }
            catch (NumberFormatException localNumberFormatException2) {
            }
        return (((name != null) && (height >= 0)) ? new Font(display, name, height, style) : null);
    }

    private static int[] parseIntArray(String string) {
        StringTokenizer stringTokenizer = new StringTokenizer(string, ",");
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
        int style = 0;
        StringTokenizer stringTokenizer = new StringTokenizer(string, "|");

        while (stringTokenizer.hasMoreTokens()) {
            String token = stringTokenizer.nextToken().trim();
            Integer value = (Integer) mapStyles.get(token);

            if (value != null) {
                style |= value.intValue();
            }
        }
        return style;
    }

    private XSWT_orig(Composite parent, Element element) {
        process(parent, element);
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

        if (attributeId != null) {
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
        String attributeEnabled = element.getAttribute("enabled");

        if ((attributeEnabled != null) && (attributeEnabled.length() > 0)) {
            control.setEnabled(Boolean.valueOf(attributeEnabled).booleanValue());
        }
        String attributeFocus = element.getAttribute("focus");

        if ((attributeFocus != null) && (attributeFocus.length() > 0) && (Boolean.TRUE.equals(Boolean.valueOf(attributeFocus)))) {
            control.setFocus();
        }
        String attributeFont = element.getAttribute("font");

        if ((attributeFont != null) && (attributeFont.length() > 0)) {
            control.setFont(parseFont(control.getDisplay(), attributeFont));
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
        String attributeVisible = element.getAttribute("visible");

        if ((attributeVisible != null) && (attributeVisible.length() > 0)) {
            control.setVisible(Boolean.valueOf(attributeVisible).booleanValue());
        }
        processLayoutData(control, element);
    }

    private void processLayoutData(Control control, Element element) {
        NodeList nodeListLayoutData = element.getElementsByTagName("layoutData");

        for (int i = 0; i < nodeListLayoutData.getLength(); ++i) {
            Element elementLayoutData = (Element) nodeListLayoutData.item(i);

            if (elementLayoutData.getParentNode().equals(element)) {
                Composite parent = control.getParent();
                Layout layout = parent.getLayout();

                if (layout instanceof GridLayout)
                    processGridLayoutData(control, elementLayoutData);
                else if (layout instanceof RowLayout)
                    processRowLayoutData(control, elementLayoutData);
            }
        }
    }

    private void processGridLayoutData(Control control, Element element) {
        GridData layoutData = new GridData();
        String attributeGrabExcessHorizontalSpace = element.getAttribute("grabExcessHorizontalSpace");

        if ((attributeGrabExcessHorizontalSpace != null) && (attributeGrabExcessHorizontalSpace.length() > 0)) {
            layoutData.grabExcessHorizontalSpace = Boolean.valueOf(attributeGrabExcessHorizontalSpace).booleanValue();
        }
        String attributeGrabExcessVerticalSpace = element.getAttribute("grabExcessVerticalSpace");

        if ((attributeGrabExcessVerticalSpace != null) && (attributeGrabExcessVerticalSpace.length() > 0)) {
            layoutData.grabExcessVerticalSpace = Boolean.valueOf(attributeGrabExcessVerticalSpace).booleanValue();
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
        String attributeSelection = element.getAttribute("selection");

        if ((attributeSelection != null) && (attributeSelection.length() > 0)) {
            button.setSelection(Boolean.valueOf(attributeSelection).booleanValue());
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
    }

    private void processLayout(Composite composite, Element element) {
        NodeList nodeListLayout = element.getElementsByTagName("layout");

        for (int i = 0; i < nodeListLayout.getLength(); ++i) {
            Element elementLayout = (Element) nodeListLayout.item(i);

            if (elementLayout.getParentNode().equals(element)) {
                String attributeType = elementLayout.getAttribute("class");

                if ("fillLayout".equals(attributeType))
                    processFillLayout(composite, elementLayout);
                else if ("gridLayout".equals(attributeType))
                    processGridLayout(composite, elementLayout);
                else if ("rowLayout".equals(attributeType))
                    processRowLayout(composite, elementLayout);
            }
        }
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
        String attributeMakeColumnsEqualWidth = element.getAttribute("makeColumnsEqualWidth");

        if ((attributeMakeColumnsEqualWidth != null) && (attributeMakeColumnsEqualWidth.length() > 0)) {
            layout.makeColumnsEqualWidth = Boolean.valueOf(attributeMakeColumnsEqualWidth).booleanValue();
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
        element.getAttribute("fill");

        String attributeJustify = element.getAttribute("justify");

        if ((attributeJustify != null) && (attributeJustify.length() > 0)) {
            layout.justify = Boolean.valueOf(attributeJustify).booleanValue();
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
        String attributePack = element.getAttribute("pack");

        if ((attributePack != null) && (attributePack.length() > 0)) {
            layout.pack = Boolean.valueOf(attributePack).booleanValue();
        }
        String attributeSpacing = element.getAttribute("spacing");

        if (attributeSpacing != null)
            try {
                layout.spacing = Integer.parseInt(attributeSpacing);
            }
            catch (NumberFormatException localNumberFormatException5) {
            }
        layout.type = processStyle(element.getAttribute("type"));
        String attributeWrap = element.getAttribute("wrap");

        if ((attributeWrap != null) && (attributeWrap.length() > 0)) {
            layout.wrap = Boolean.valueOf(attributeWrap).booleanValue();
        }
        composite.setLayout(layout);
    }

    private void processChildren(Composite composite, Element element) {
        NodeList nodeListChildren = element.getChildNodes();

        for (int i = 0; i < nodeListChildren.getLength(); ++i) {
            Node nodeChild = nodeListChildren.item(i);

            if ((isChildElement(nodeChild, element)) && ("children".equals(((Element) nodeChild).getTagName()))) {
                Element elementChildren = (Element) nodeChild;
                NodeList nodeListChildren2 = elementChildren.getChildNodes();

                for (int j = 0; j < nodeListChildren2.getLength(); ++j) {
                    Node nodeChild2 = nodeListChildren2.item(j);

                    if (isChildElement(nodeChild2, elementChildren))
                        process(composite, (Element) nodeChild2);
                }
            }
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
        String attributeSelection = element.getAttribute("selection");

        if (attributeSelection != null)
            try {
                tabFolder.setSelection(Integer.parseInt(attributeSelection));
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
    }

    private void processTable(Table table, Element element) {
        processComposite(table, element);
        String attributeHeaderVisible = element.getAttribute("headerVisible");

        if ((attributeHeaderVisible != null) && (attributeHeaderVisible.length() > 0)) {
            table.setHeaderVisible(Boolean.valueOf(attributeHeaderVisible).booleanValue());
        }
        String attributeLinesVisible = element.getAttribute("linesVisible");

        if ((attributeLinesVisible != null) && (attributeLinesVisible.length() > 0)) {
            table.setLinesVisible(Boolean.valueOf(attributeLinesVisible).booleanValue());
        }
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

        if (attributeTopIndex == null)
            return;
        try {
            table.setTopIndex(Integer.parseInt(attributeTopIndex));
        }
        catch (NumberFormatException localNumberFormatException) {
        }
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
        String attributeDoubleClickEnabled = element.getAttribute("doubleClickEnabled");

        if ((attributeDoubleClickEnabled != null) && (attributeDoubleClickEnabled.length() > 0)) {
            text.setDoubleClickEnabled(Boolean.valueOf(attributeDoubleClickEnabled).booleanValue());
        }
        String attributeEchoChar = element.getAttribute("echoChar");

        if ((attributeEchoChar != null) && (attributeEchoChar.length() == 1)) {
            text.setEchoChar(attributeEchoChar.charAt(0));
        }
        String attributeEditable = element.getAttribute("editable");

        if ((attributeEditable != null) && (attributeEditable.length() > 0)) {
            text.setEditable(Boolean.valueOf(attributeEditable).booleanValue());
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
}
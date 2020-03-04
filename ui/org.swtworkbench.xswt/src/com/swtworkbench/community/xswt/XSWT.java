package com.swtworkbench.community.xswt;

import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

import org.eclipse.swt.events.DisposeEvent;
import org.eclipse.swt.events.DisposeListener;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.graphics.Device;
import org.eclipse.swt.graphics.Font;
import org.eclipse.swt.graphics.Image;
import org.eclipse.swt.graphics.Point;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.swt.graphics.Rectangle;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Widget;
import org.kxml2.io.KXmlParser;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;

import com.swtworkbench.community.xswt.codegen.CodeGenerator;
import com.swtworkbench.community.xswt.codegens.CharacterCodeGenerator;
import com.swtworkbench.community.xswt.codegens.FontCodeGenerator;
import com.swtworkbench.community.xswt.codegens.ImageCodeGenerator;
import com.swtworkbench.community.xswt.codegens.IntArrayCodeGenerator;
import com.swtworkbench.community.xswt.codegens.ObjectStubCodeGenerator;
import com.swtworkbench.community.xswt.codegens.PointCodeGenerator;
import com.swtworkbench.community.xswt.codegens.RGBCodeGenerator;
import com.swtworkbench.community.xswt.codegens.RectangleCodeGenerator;
import com.swtworkbench.community.xswt.codegens.StringArrayCodeGenerator;
import com.swtworkbench.community.xswt.codegens.StringCodeGenerator;
import com.swtworkbench.community.xswt.dataparser.DataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.BooleanDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.ByteArrayDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.CharacterDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.DeviceDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.DoubleDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.FloatDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.FontDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.ImageDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.IntArrayDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.IntDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.LongDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.PointDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.RGBDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.RectangleDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.StringArrayDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.StringBufferDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.StringDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.WidgetDataParser;
import com.swtworkbench.community.xswt.dataparser.parsers.color.ColorDataParser;
import com.swtworkbench.community.xswt.layoutbuilder.ILayoutBuilder;
import com.swtworkbench.community.xswt.layoutbuilder.ObjectStub;
import com.swtworkbench.community.xswt.layoutbuilder.SWTLayoutBuilder;
import com.swtworkbench.community.xswt.metalogger.Logger;

public class XSWT {
    public static final String XSWT_NS = "http://sweet_swt.sf.net/xswt";
    private static Map customNSRegistry = new HashMap();

    private HashMap fixupTable = new HashMap();

    public ClassBuilder classBuilder = ClassBuilder.getDefault();
    public static ILayoutBuilder layoutBuilder = new SWTLayoutBuilder();
    protected WidgetDataParser widgetDataParser = new WidgetDataParser();


    static {
        DataParser.registerDataParser(Character.TYPE, new CharacterDataParser());
        DataParser.registerDataParser(Boolean.TYPE, new BooleanDataParser());
        DataParser.registerDataParser(Integer.TYPE, new IntDataParser());
        DataParser.registerDataParser(Double.TYPE, new DoubleDataParser());
        DataParser.registerDataParser(Long.TYPE, new LongDataParser());
        DataParser.registerDataParser(Float.TYPE, new FloatDataParser());

        DataParser.registerDataParser(String.class, new StringDataParser());
        DataParser.registerDataParser(String[].class, new StringArrayDataParser());
        DataParser.registerDataParser(StringBuffer.class, new StringBufferDataParser());
        DataParser.registerDataParser(int[].class, new IntArrayDataParser());
        DataParser.registerDataParser(byte[].class, new ByteArrayDataParser());
        DataParser.registerDataParser(RGB.class, new RGBDataParser());
        DataParser.registerDataParser(Point.class, new PointDataParser());
        DataParser.registerDataParser(Rectangle.class, new RectangleDataParser());
        DataParser.registerDataParser(Image.class, new ImageDataParser());
        DataParser.registerDataParser(Font.class, new FontDataParser());
        DataParser.registerDataParser(Color.class, new ColorDataParser());
        DataParser.registerDataParser(Device.class, new DeviceDataParser());

        CodeGenerator.registerGenerator(Character.TYPE, new CharacterCodeGenerator());

        CodeGenerator.registerGenerator(String.class, new StringCodeGenerator());
        CodeGenerator.registerGenerator(Font.class, new FontCodeGenerator());
        CodeGenerator.registerGenerator(Image.class, new ImageCodeGenerator());
        CodeGenerator.registerGenerator(int[].class, new IntArrayCodeGenerator());
        CodeGenerator.registerGenerator(ObjectStub.class, new ObjectStubCodeGenerator());
        CodeGenerator.registerGenerator(Point.class, new PointCodeGenerator());
        CodeGenerator.registerGenerator(Rectangle.class, new RectangleCodeGenerator());
        CodeGenerator.registerGenerator(RGB.class, new RGBCodeGenerator());
        CodeGenerator.registerGenerator(String[].class, new StringArrayCodeGenerator());
    }

    public static void registerNSHandler(String namespace, ICustomNSHandler handler) {
        customNSRegistry.put(namespace, handler);
    }

    public static void setExtraClassLoader(ClassLoader classLoader) {
        ClassBuilder.getDefault().setExtraClassLoader(classLoader);
    }

    public static ClassLoader getExtraClassLoader() {
        return ClassBuilder.getDefault().getExtraClassLoader();
    }

    public static Map create(Composite parent, String file, Class relativeTo) throws XSWTException, XmlPullParserException,
            IOException {
        return create(parent, relativeTo.getResource(file).openStream());
    }

    public static Map createl(Composite parent, String file, Class relativeTo) {
        try {
            return create(parent, relativeTo.getResource(file).openStream());
        }
        catch (Exception e) {
            Logger.log().error(e, "Unable to create XSWT layout: " + e.getMessage());
        }
        return null;
    }

    public static Map create(Composite parent, InputStream inputStream) throws XSWTException {
        XmlPullParserFactory factory;
        XmlPullParser parser = null;
        if (System.getProperty("org.xmlpull.v1.XmlPullParserFactory") != null) {
            try {
                factory = XmlPullParserFactory.newInstance();

                factory.setNamespaceAware(true);
                factory.setFeature("http://xmlpull.org/v1/doc/features.html#process-namespaces", true);
                parser = factory.newPullParser();
            }
            catch (XmlPullParserException e) {
                throw new XSWTException(e.toString());
            }
        }
        else
            parser = new KXmlParser();
        try {
            parser.setInput(inputStream, null);
            parser.setFeature("http://xmlpull.org/v1/doc/features.html#process-namespaces", true);
            parser.require(0, null, null);
            parser.nextToken();

            XSWT xswt = new XSWT();
            xswt.parse(parent, parser);

            return xswt.getWidgetMap();
        }
        catch (XSWTException e) {
            if (e.row == -1) {
                e.row = parser.getLineNumber();
                e.column = parser.getColumnNumber();
            }
            throw e;
        }
        catch (FileNotFoundException e) {
            Logger.log().debug(XSWT.class, e.toString());
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
        catch (XmlPullParserException e) {
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
        catch (IOException e) {
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
    }

    public static Map create(Composite parent, String uri) throws XSWTException {
        XmlPullParserFactory factory = null;
        XmlPullParser parser = null;
        if (System.getProperty("org.xmlpull.v1.XmlPullParserFactory") != null) {
            try {
                factory = XmlPullParserFactory.newInstance();

                factory.setNamespaceAware(true);
                factory.setFeature("http://xmlpull.org/v1/doc/features.html#process-namespaces", true);
                parser = factory.newPullParser();
            }
            catch (XmlPullParserException e) {
                throw new XSWTException(e.toString());
            }
        }
        else
            parser = new KXmlParser();
        try {
            parser.setInput(new FileReader(uri));
            parser.setFeature("http://xmlpull.org/v1/doc/features.html#process-namespaces", true);
            parser.require(0, null, null);
            parser.nextToken();

            XSWT xswt = new XSWT();
            xswt.parse(parent, parser);

            return xswt.getWidgetMap();
        }
        catch (XSWTException e) {
            if (e.row == -1) {
                e.row = parser.getLineNumber();
                e.column = parser.getColumnNumber();
            }
            throw e;
        }
        catch (FileNotFoundException e) {
            Logger.log().debug(XSWT.class, e.toString());
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
        catch (XmlPullParserException e) {
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
        catch (IOException e) {
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
    }

    public void parse(Composite parent, XmlPullParser parser) throws XSWTException {
        int eventType;
        if (parent != null) {
            parent.addDisposeListener(new DisposeListener() {
                public void widgetDisposed(DisposeEvent e) {
                    DataParser.dispose();
                }
            });
        }
        try {
            eventType = parser.getEventType();
            do {
                if (eventType == 1) {
                    throw new XSWTException("End of document reached abnormally", parser.getLineNumber(), parser
                            .getColumnNumber());
                }
                if ((eventType == 2) && (parser.getName().equals("xswt"))) {
                    Logger.log().debug(XSWT.class, "Start XSWT:" + parser.getNamespace() + ":" + parser.getName());
                    processTopLevelElement(parent, parser);
                }
                else if (eventType == 3) {
                    processEndElement(parent, parser);
                }

                eventType = parser.next();
            } while (eventType != 1);

            if (this.fixupTable.size() != 0)
                throw new XSWTException(missingIdRefError(), 0, 0);
        }
        catch (XmlPullParserException e1) {
            throw new XSWTException(e1.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
        catch (IOException e) {
            throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
        }
    }

    private void processEndElement(Composite parent, XmlPullParser parser) throws XSWTException {
        Logger.log().debug(XSWT.class, "End tag:" + parser.getNamespace() + ":" + parser.getName());
    }

    private void addUnresolvedIDRef(String attributeName, Object obj, String attributeValue, int row, int col) {
        FixupTableEntry entry = new FixupTableEntry();
        entry.control = obj;
        entry.colNumber = col;
        entry.rowNumber = row;
        entry.attributeName = attributeName;
        LinkedList id = (LinkedList) this.fixupTable.get(attributeValue);
        if (id == null) {
            LinkedList newList = new LinkedList();
            newList.add(entry);
            this.fixupTable.put(attributeValue, newList);
        }
        else {
            id.add(entry);
        }
    }

    private void resolveIdRefs(String idRef, Object object) throws XSWTException {
        if ((idRef == null) || (this.fixupTable.size() == 0)) {
            return;
        }
        LinkedList list = (LinkedList) this.fixupTable.get(idRef);
        if (list == null)
            return;
        for (int i = 0; i < list.size(); ++i) {
            FixupTableEntry entry = (FixupTableEntry) list.get(i);
            if (entry == null)
                continue;
            Logger.log().debug(
                    XSWT.class,
                    "Process unresolved reference (".concat(idRef).concat(") at Row:") + entry.rowNumber + " Col:"
                            + entry.colNumber);
            layoutBuilder.setProperty(entry.attributeName, entry.control, idRef);
        }

        this.fixupTable.remove(idRef);
    }

    private String missingIdRefError() {
        Iterator ids = this.fixupTable.keySet().iterator();
        String msg = "";
        while (ids.hasNext()) {
            String id = (String) ids.next();
            LinkedList list = (LinkedList) this.fixupTable.get(id);

            for (int i = 0; i < list.size(); ++i) {
                FixupTableEntry entry = (FixupTableEntry) list.get(i);
                if (entry != null) {
                    msg = msg + "\nFound unresolved reference: " + entry.attributeName + " at " + entry.toString();
                }
            }
        }
        this.fixupTable.clear();
        return msg;
    }

    private XSWT() {
        DataParser.registerDataParser(Widget.class, this.widgetDataParser);
    }

    public Map getWidgetMap() {
        return this.widgetDataParser.getWidgetMap();
    }

    private void processImports(Composite composite, XmlPullParser parser) throws XSWTException, XmlPullParserException,
            IOException {
        int level = parser.getDepth();
        while ((parser.next() != 3) || (parser.getDepth() != level)) {
            if (parser.getEventType() != 2)
                continue;
            if ((parser.getName() != null) && (parser.getName().equals("package"))) {
                String thePackage = parser.getAttributeValue(0);

                Logger.log().debug(XSWT.class, "Process package :" + thePackage);

                if (thePackage != null)
                    this.classBuilder.importPackage(thePackage);
                else
                    throw new XSWTException("Unable to get import name", parser.getLineNumber(), parser.getColumnNumber());
            }
        }
    }

    private void processTopLevelElement(Composite composite, XmlPullParser parser) throws XSWTException, XmlPullParserException,
            IOException {
        int level = parser.getDepth();
        while ((parser.next() != 3) || (parser.getDepth() != level)) {
            if (parser.getEventType() != 2)
                continue;
            String namespace = parser.getNamespace();
            if ((namespace != null) && (namespace.length() != 0) && (!("http://sweet_swt.sf.net/xswt".equals(namespace)))) {
                ICustomNSHandler handler = (ICustomNSHandler) customNSRegistry.get(namespace);

                if (handler != null)
                    handler.handleNamespace(parser, layoutBuilder);
                else
                    Logger.log().debug(XSWT.class, "No custom namespace handler for " + namespace);
            }
            else if ((namespace != null) && (namespace.length() != 0) && (namespace.equals("http://sweet_swt.sf.net/xswt"))) {
                processXSWTNode(composite, parser);
            }
            else {
                Object parent = composite;
                if (parent == null)
                    parent = new ObjectStub("Shell");
                processNodeProperty(parent, parser);
            }
        }
    }

    private void processXSWTNode(Composite composite, XmlPullParser parser) throws XSWTException, XmlPullParserException,
            IOException {
        if (parser.getName().equals("import")) {
            processImports(composite, parser);
        }
        else if (parser.getName().equals("children"))
            processChildControls(composite, parser);
    }

    private void processChildControls(Object parent, XmlPullParser parser) throws XmlPullParserException, IOException,
            XSWTException {
        int level = parser.getDepth();
        while ((parser.next() != 3) || (parser.getDepth() != level)) {
            if (parser.getEventType() != 2)
                continue;
            processChild(parent, parser);
        }
    }

    private Object processChild(Object parent, XmlPullParser parser) throws XmlPullParserException, IOException, XSWTException {
        Object result = null;
        String tagName = parser.getName();
        String style = parser.getAttributeValue("http://sweet_swt.sf.net/xswt", "style");

        String className = upperCaseFirstLetter(tagName);
        Logger.log().debug(XSWT.class, "Constructing (" + parser.getNamespace() + ") " + className);

        String widgetName = getWidgetName(parser);
        result = layoutBuilder.construct(className, parent, style, widgetName);
        if (widgetName != null) {
            if (this.widgetDataParser.get(widgetName) != null)
                throw new XSWTException("Duplicated widget ID found", parser.getLineNumber(), parser.getColumnNumber());
            this.widgetDataParser.put(widgetName, result);

            resolveIdRefs(widgetName, result);
        }
        processChildAttributes(result, parser);
        processSubNodes(result, parser);

        return result;
    }

    private String getWidgetName(XmlPullParser parser) {
        return parser.getAttributeValue("http://sweet_swt.sf.net/xswt", "id");
    }

    private static Class[] getParamTypes(Object[] args) {
        Class[] paramTypes = new Class[args.length];
        for (int i = 0; i < args.length; ++i) {
            paramTypes[i] = args[i].getClass();
        }
        return paramTypes;
    }

    public static Object invokei(Object receiver, String method, Object[] args) {
        Object result = null;
        try {
            Method methodCaller = receiver.getClass().getMethod(method, getParamTypes(args));
            result = methodCaller.invoke(receiver, args);
        }
        catch (Exception localException) {
        }
        return result;
    }

    private void processChildAttributes(Object obj, XmlPullParser parser) throws XSWTException {
        Object viewer = obj;
        Object viewedObject = invokei(obj, "getControl", new Object[0]);
        if (viewedObject != null) {
            obj = viewedObject;
            ((Widget) obj).setData("viewer", viewer);
        }

        int count = parser.getAttributeCount();
        for (int i = 0; i < count; ++i) {
            boolean recognized = false;

            recognized = processBuiltInAttr(obj, parser.getAttributeName(i), parser.getAttributeNamespace(i), parser
                    .getAttributeValue(i));
            if (recognized) {
                continue;
            }
            try {
                recognized = layoutBuilder.setProperty(parser.getAttributeName(i), obj, parser.getAttributeValue(i));
            }
            catch (XSWTException localXSWTException) {
                Logger.log().debug(XSWT.class, "Found unsolved reference:" + parser.getAttributeValue(i));
                addUnresolvedIDRef(parser.getAttributeName(i), obj, parser.getAttributeValue(i), parser.getLineNumber(), parser
                        .getColumnNumber());
                recognized = true;
            }
            if (recognized) {
                continue;
            }
            recognized = layoutBuilder.setField(parser.getAttributeName(i), obj, parser.getAttributeValue(i));
            if (recognized) {
                continue;
            }
            if (!(recognized))
                throw new XSWTException("Attribute not recognized: " + parser.getAttributeName(i), parser.getLineNumber(), parser
                        .getColumnNumber());
        }
    }

    private boolean processBuiltInAttr(Object obj, String nodeName, String namespace, String value) throws XSWTException {
        int index = nodeName.indexOf(58);
        nodeName = (index == -1) ? nodeName : nodeName.substring(index + 1);

        if ("http://sweet_swt.sf.net/xswt".equals(namespace)) {
            if (nodeName.endsWith("id")) {
                return true;
            }
            if ("p".equals(nodeName.substring(0, 1))) {
                return true;
            }

            if (nodeName.endsWith("style")) {
                return true;
            }

            if (nodeName.endsWith("class")) {
                return true;
            }

            if (nodeName.startsWith("id.")) {
                String key = nodeName.substring("id.".length());

                if (!(obj instanceof Widget)) {
                    throw new XSWTException("id. attribute is not applicable to objects of type: " + obj.getClass().getName());
                }
                Widget widget = (Widget) obj;
                widget.setData(key, value);
                return true;
            }

        }
        return false;
    }

    private void processSubNodes(Object obj, XmlPullParser parser) throws XmlPullParserException, IOException, XSWTException {
        Object viewedObject = invokei(obj, "getControl", new Object[0]);
        if (viewedObject != null) {
            obj = viewedObject;
        }

        int type = parser.getEventType();

        while (type != 3) {
            try {
                type = parser.next();
            }
            catch (IOException e) {
                Logger.log().debug(XSWT.class, e.toString());
                throw new XSWTException(e.toString(), parser.getLineNumber(), parser.getColumnNumber());
            }
            if (type == 3)
                return;
            if (type != 2)
                continue;
            if (parser.getName().equals("children")) {
                processChildControls(obj, parser);
            }
            else
                processNodeProperty(obj, parser);
        }
    }

    private void processNodeProperty(Object obj, XmlPullParser parser) throws XmlPullParserException, IOException, XSWTException {
        Class valueType = null;

        Method setter = null;
        Field field = null;

        Object value = null;

        String classAttrib = parser.getAttributeValue("http://sweet_swt.sf.net/xswt", "class");
        if (classAttrib != null) {
            String className = upperCaseFirstLetter(classAttrib);
            valueType = this.classBuilder.getClass(className);
        }

        String nodeName = parser.getName();
        setter = layoutBuilder.resolveAttributeSetMethod(obj, nodeName, valueType);
        if ((valueType == null) && (setter != null)) {
            Class[] paramTypes = setter.getParameterTypes();
            valueType = paramTypes[0];
        }

        if (setter == null) {
            String parentReferenceId = parser.getAttributeValue("http://sweet_swt.sf.net/xswt", "id");

            Method getParentReferenceMethod = layoutBuilder.resolveAttributeGetMethod(obj, nodeName);
            if (getParentReferenceMethod != null) {
                Object temp_parent = layoutBuilder.getProperty(getParentReferenceMethod, obj, null);
                if (parentReferenceId != null) {
                    if (this.widgetDataParser.get(parentReferenceId) != null)
                        throw new XSWTException("Duplicated widget ID found");
                    this.widgetDataParser.put(parentReferenceId, temp_parent);

                    resolveIdRefs(parentReferenceId, temp_parent);
                }

                if (temp_parent != null) {
                    processChildAttributes(temp_parent, parser);

                    processSubNodes(temp_parent, parser);
                }
                return;
            }
        }
        if (setter == null) {
            try {
                field = layoutBuilder.getClass(obj).getField(nodeName);
            }
            catch (Throwable localThrowable) {
            }
            if ((obj != null) && (field == null)) {
                throw new XSWTException("Property/method/field not found: " + nodeName);
            }

            if (valueType == null) {
                valueType = field.getType();
            }
        }

        Logger.log().debug(XSWT.class, "Field type: " + valueType.getName());
        value = constructValueObject(valueType, parser);

        processChildAttributes(value, parser);
        processSubNodes(value, parser);

        if (setter != null) {
            layoutBuilder.setProperty(setter, obj, value);
            return;
        }
        if (obj == null)
            return;
        layoutBuilder.setField(field, obj, value);
    }

    private Object constructValueObject(Class valueType, XmlPullParser parser) throws XSWTException {
        LinkedList argList = new LinkedList();

        int count = parser.getAttributeCount();
        for (int i = 0; i < count; ++i) {
            StringBuffer arg = new StringBuffer("p");
            arg.append(i);
            String argName = arg.toString();
            String value = parser.getAttributeValue("http://sweet_swt.sf.net/xswt", argName);

            if (value == null)
                break;
            argList.addLast(value);
        }

        return layoutBuilder.construct(valueType, argList);
    }

    protected Method resolvePropertySetter(Object obj, String name, Class type) {
        Method result = null;
        try {
            result = layoutBuilder.getClass(obj).getMethod(name, new Class[] { type });
        }
        catch (Throwable localThrowable) {
        }
        return result;
    }

    public static String upperCaseFirstLetter(String source) {
        StringBuffer buf = new StringBuffer(source.substring(0, 1).toUpperCase());
        buf.append(source.substring(1, source.length()));
        return buf.toString();
    }

    private class FixupTableEntry {
        public Object control;
        public String attributeName;
        public int rowNumber;
        public int colNumber;

        public String toString() {
            return "Row " + this.rowNumber + " Col " + this.colNumber;
        }
    }
}
package com.swtworkbench.community.xswt.layoutbuilder;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.StringTokenizer;

import com.swtworkbench.community.xswt.ClassBuilder;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.codegen.CodeGenerator;
import com.swtworkbench.community.xswt.dataparser.DataParser;

public class SWTSourceBuilder extends LayoutBuilder implements ILayoutBuilder {
    private StringBuffer source = new StringBuffer();

    public static String INDENT = "    ";

    protected void indent() {
        this.source.append(INDENT);
    }

    protected void emit(String source) {
        this.source.append(source);
    }

    protected void newline() {
        this.source.append("\n");
    }

    public String toString() {
        return this.source.toString();
    }

    public Object construct(Class<?> valueType, LinkedList<String> argList) throws XSWTException {
        ObjectStub stub = new ObjectStub(valueType.getName());
        indent();
        emit(stub.className);
        emit(" ");
        emit(stub.sourceName);
        emit(" = new ");
        emit(stub.className);
        emit("(");

        LayoutBuilder.ConstructorInfo constructorInfo = getConstructorInfo(valueType, argList);
        int i = 0;
        for (Iterator current = argList.iterator(); current.hasNext();) {
            String xswtSource = (String) current.next();
            Object value = constructorInfo.args[i];
            if (i > 0)
                emit(", ");
            emit(CodeGenerator.getDefault().getCode(value, xswtSource));
            ++i;
        }

        emit(");");
        newline();
        return stub;
    }

    public Object construct(String className, Object parent, String style, String name) throws XSWTException {
        ObjectStub parentStub;
        ObjectStub stub = new ObjectStub(className, name);

        if (parent instanceof ObjectStub)
            parentStub = (ObjectStub) parent;
        else {
            parentStub = new ObjectStub("Composite", "xswtParent");
        }
        indent();
        emit(stub.className);
        emit(" ");
        emit(stub.sourceName);
        emit(" = new ");
        emit(stub.className);
        emit("(");
        emit(parentStub.sourceName);
        emit(", ");
        emit(getStyle(style));
        emit(");");
        newline();
        return stub;
    }

    private String getStyle(String style) {
        if ((style == "") || (style == null)) {
            return "SWT.NULL";
        }
        StringBuffer result = new StringBuffer();
        StringTokenizer bits = new StringTokenizer(style, " |");
        int i = 0;
        while (bits.hasMoreTokens()) {
            if (i > 0)
                result.append(" | ");
            String bit = bits.nextToken();
            if (bit.indexOf(".") == -1) {
                result.append("SWT.");
            }
            result.append(bit);
            ++i;
        }

        return result.toString();
    }

    public void setField(Field field, Object receiver, Object value) throws XSWTException {
        ObjectStub stub = (ObjectStub) receiver;
        indent();
        emit(stub.sourceName);
        emit(".");
        emit(field.getName());
        emit(" = ");
        emit(CodeGenerator.getDefault().getCode(value, ""));
        emit(";");
        newline();
    }

    public boolean setField(String fieldName, Object receiver, String valueSource) throws XSWTException {
        try {
            Class<?> receiverClass = getClass(receiver);
            Field field = receiverClass.getField(fieldName);
            Object value = DataParser.parse(valueSource, field.getType());

            ObjectStub stub = (ObjectStub) receiver;
            indent();
            emit(stub.sourceName);
            emit(".");
            emit(field.getName());
            emit(" = ");
            emit(CodeGenerator.getDefault().getCode(value, valueSource));
            emit(";");
            newline();

            return true;
        }
        catch (Exception e) {
            throw new XSWTException("Unable to set field: " + fieldName, e);
        }
    }

    public void setProperty(Method setter, Object receiver, Object value) throws XSWTException {
        ObjectStub stub = (ObjectStub) receiver;
        indent();
        emit(stub.sourceName);
        emit(".");
        emit(setter.getName());
        emit("(");
        emit(CodeGenerator.getDefault().getCode(value, ""));
        emit(");");
        newline();
    }

    public boolean setProperty(String propertyName, Object receiver, String valueSource) throws XSWTException {
        try {
            Method setMethod = resolveAttributeSetMethod(receiver, propertyName, null);
            if (setMethod == null)
                return false;
            Object value = DataParser.parse(valueSource, setMethod.getParameterTypes()[0]);

            ObjectStub stub = (ObjectStub) receiver;
            indent();
            emit(stub.sourceName);
            emit(".");
            emit(setMethod.getName());
            emit("(");
            emit(CodeGenerator.getDefault().getCode(value, valueSource));
            emit(");");
            newline();

            return true;
        }
        catch (Exception e) {
            throw new XSWTException("Unable to set property: " + propertyName, e);
        }
    }

    public Class<?> getClass(Object obj) throws XSWTException {
        ObjectStub stub = (ObjectStub) obj;

        return ClassBuilder.getDefault().getClass(stub.className);
    }

    public Object getProperty(Method getter, Object receiver, Object value) throws XSWTException {
        try {
            ObjectStub stub = (ObjectStub) receiver;
            indent();
            ObjectStub stub2 = new ObjectStub(getter.getReturnType().getName());
            emit(getter.getReturnType().getName());
            emit(" " + stub2.sourceName + " = ");
            emit(stub.sourceName);
            emit(".");
            emit(getter.getName());
            emit(" ( ");

            if (value != null)
                emit(value.toString());
            emit(");");
            newline();

            return stub2;
        }
        catch (Exception e) {
            throw new XSWTException("Unable to get property: " + getter.getName(), e);
        }
    }
}
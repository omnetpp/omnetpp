package com.swtworkbench.community.xswt.layoutbuilder;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.LinkedList;

import org.eclipse.swt.widgets.Widget;

import com.swtworkbench.community.xswt.ClassBuilder;
import com.swtworkbench.community.xswt.StyleParser;
import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DataParser;

@SuppressWarnings("rawtypes")
public class GUIEditorLayoutBuilder extends LayoutBuilder implements ILayoutBuilder {
    public boolean setProperty(String propertyName, Object receiver, String valueSource) throws XSWTException {
        Method setMethod = resolveAttributeSetMethod(receiver, propertyName, null);
        if (setMethod == null)
            return false;
        try {
            Object value = DataParser.parse(valueSource, setMethod.getParameterTypes()[0]);

            setMethod.invoke(receiver, new Object[] { value });
        }
        catch (IllegalArgumentException e) {
            throw new XSWTException(e);
        }
        catch (IllegalAccessException e) {
            throw new XSWTException(e);
        }
        catch (InvocationTargetException e) {
            throw new XSWTException(e);
        }
        return true;
    }

    public void setProperty(Method setter, Object receiver, Object value) throws XSWTException {
        try {
            setter.invoke(receiver, new Object[] { value });
        }
        catch (Throwable t) {
            throw new XSWTException("Error calling setter " + setter.getName(), t);
        }
    }

    public boolean setField(String fieldName, Object receiver, String valueSource) throws XSWTException {
        Field field = null;
        try {
            field = receiver.getClass().getField(fieldName);
        }
        catch (Throwable localThrowable) {
        }
        if (field == null)
            return false;
        try {
            Object value = DataParser.parse(valueSource, field.getType());
            field.set(receiver, value);
        }
        catch (IllegalArgumentException e) {
            throw new XSWTException(e);
        }
        catch (IllegalAccessException e) {
            throw new XSWTException(e);
        }

        return true;
    }

    public void setField(Field field, Object receiver, Object value) throws XSWTException {
        try {
            field.set(receiver, value);
        }
        catch (Throwable t) {
            throw new XSWTException("Error setting field " + field.getName(), t);
        }
    }

    public Object construct(String className, Object parent, String style, String name) throws XSWTException {
        Object result = ClassBuilder.getDefault().constructControl(className, parent, StyleParser.parse(style));

        if ((name != null) && (result instanceof Widget)) {
            Widget widget = (Widget) result;
            widget.setData("Sweet_id", name);
        }
        return result;
    }

    public Object construct(Class valueType, LinkedList argList) throws XSWTException {
        if (argList.size() < 1) {
            try {
                return valueType.newInstance();
            }
            catch (Exception e) {
                throw new XSWTException("Unable to create a newInstance() of " + valueType.getName(), e);
            }

        }

        LayoutBuilder.ConstructorInfo constructorInfo = getConstructorInfo(valueType, argList);

        if ((constructorInfo.constructor == null) || (constructorInfo.args == null)) {
            throw new XSWTException("Unable to locate a constructor for type " + valueType.getName() + " with " + argList.size()
                    + " parameters.");
        }
        try {
            return constructorInfo.constructor.newInstance(constructorInfo.args);
        }
        catch (Exception e) {
            String paramTypeStr = "";
            if (constructorInfo.paramTypes != null) {
                paramTypeStr = "(" + constructorInfo.paramTypes[0].getName();
                for (int i = 1; i < constructorInfo.paramTypes.length; ++i) {
                    paramTypeStr = paramTypeStr + ", " + constructorInfo.paramTypes[i].getName();
                }
                paramTypeStr = paramTypeStr + ")";
            }
            throw new XSWTException("Unable to invoke constructor with parameters of type: " + paramTypeStr, e);
        }
    }

    public Class getClass(Object obj) throws XSWTException {
        return obj.getClass();
    }

    public Object getProperty(Method getter, Object receiver, Object value) throws XSWTException {
        return null;
    }
}
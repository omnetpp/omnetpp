package com.swtworkbench.community.xswt.layoutbuilder;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.LinkedList;

import com.swtworkbench.community.xswt.XSWTException;

@SuppressWarnings("rawtypes")
public abstract interface ILayoutBuilder {
    public abstract Object construct(String paramString1, Object paramObject, String paramString2, String paramString3)
            throws XSWTException;

    public abstract Object construct(Class paramClass, LinkedList paramLinkedList) throws XSWTException;

    public abstract boolean setProperty(String paramString1, Object paramObject, String paramString2) throws XSWTException;

    public abstract void setProperty(Method paramMethod, Object paramObject1, Object paramObject2) throws XSWTException;

    public abstract Object getProperty(Method paramMethod, Object paramObject1, Object paramObject2) throws XSWTException;

    public abstract boolean setField(String paramString1, Object paramObject, String paramString2) throws XSWTException;

    public abstract void setField(Field paramField, Object paramObject1, Object paramObject2) throws XSWTException;

    public abstract Method resolveAttributeSetMethod(Object paramObject, String paramString, Class paramClass);

    public abstract Method resolveAttributeGetMethod(Object paramObject, String paramString);

    public abstract Class getClass(Object paramObject) throws XSWTException;
}
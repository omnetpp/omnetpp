package com.swtworkbench.community.xswt.layoutbuilder;

import com.swtworkbench.community.xswt.XSWTException;
import com.swtworkbench.community.xswt.dataparser.DataParser;
import com.swtworkbench.community.xswt.metalogger.ILogger;
import com.swtworkbench.community.xswt.metalogger.Logger;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.util.Iterator;
import java.util.LinkedList;

public abstract class LayoutBuilder {
    protected ConstructorInfo getConstructorInfo(Class valueType, LinkedList argList) throws XSWTException {
        ConstructorInfo result = new ConstructorInfo();

        Constructor[] constructors = valueType.getConstructors();
        for (int i = 0; i < constructors.length; ++i) {
            result.constructor = constructors[i];

            result.paramTypes = result.constructor.getParameterTypes();
            if (argList.size() != result.paramTypes.length) {
                continue;
            }

            result.args = new Object[argList.size()];
            try {
                Iterator argIter = argList.iterator();
                for (int arg = 0; arg < result.args.length; ++arg) {
                    String argStr = (String) argIter.next();
                    result.args[arg] = DataParser.parse(argStr, result.paramTypes[arg]);
                }

            }
            catch (Exception e) {
                Logger.log().debug(LayoutBuilder.class, "Error in parsing " + valueType + ": " + e);

                result.constructor = null;
                result.args = null;
            }
        }

        return result;
    }

    protected String[] getSetMethodNames(String nodeName) {
        String[] result = new String[2];

        StringBuffer setterBuf = new StringBuffer("set");
        setterBuf.append(nodeName.substring(0, 1).toUpperCase());

        setterBuf.append(nodeName.substring(1));
        result[0] = setterBuf.toString();

        result[1] = nodeName;

        return result;
    }

    protected Method resolvePropertySetter(Object obj, String name, Class propertyType) {
        try {
            Method result = null;
            Method[] methods = getClass(obj).getMethods();
            for (int i = 0; i < methods.length; ++i) {
                if ((!(name.equals(methods[i].getName()))) || (methods[i].getParameterTypes().length != 1))
                    continue;
                if (propertyType == null) {
                    result = methods[i];
                    break;
                }

                if (methods[i].getParameterTypes()[0].isAssignableFrom(propertyType)) {
                    result = methods[i];
                    break;
                }

            }

            return result;
        }
        catch (Exception localException) {
        }
        return null;
    }

    public Method resolveAttributeSetMethod(Object obj, String methodName, Class propertyType) {
        Method setMethod = null;

        String[] setters = getSetMethodNames(methodName);
        for (int i = 0; i < setters.length; ++i) {
            setMethod = resolvePropertySetter(obj, setters[i], propertyType);
            if (setMethod != null)
                return setMethod;
        }
        return null;
    }

    public Method resolveAttributeGetMethod(Object obj, String methodName) {
        try {
            Method result = null;
            Method[] methods = getClass(obj).getMethods();
            for (int i = 0; i < methods.length; ++i) {
                if (methodName.equals(methods[i].getName())) {
                    methods[i].getReturnType().getName();
                    result = methods[i];
                    break;
                }
            }
            return result;
        }
        catch (Exception localException) {
        }
        return null;
    }

    public abstract Class getClass(Object paramObject) throws XSWTException;

    protected class ConstructorInfo {
        public Constructor constructor = null;
        public Class[] paramTypes = null;
        public Object[] args = null;
    }
}
package org.omnetpp.python;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Proxy;

import org.eclipse.swt.widgets.Display;

/**
 * This class can be used from Python to execute code in the display thread.
 */
public class DisplayThreadInvocationHandler implements InvocationHandler {
    private Object target = null;

    DisplayThreadInvocationHandler(Object target) {
        this.target = target;
    }

    @Override
    public Object invoke(Object object, Method method, Object[] args) throws Throwable {
        Exception[] exception = new Exception[1];
        Object[] result = new Object[1];
        Display.getDefault().syncExec(() -> {
            try {
                result[0] = method.invoke(target, args);
            }
            catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException e) {
                exception[0] = e;
            }
        });
        if (exception[0] != null)
            throw exception[0];
        return result[0];
    }

    public static Object wrap(Object object) {
        return Proxy.newProxyInstance(object.getClass().getClassLoader(),
                object.getClass().getInterfaces(),
                new DisplayThreadInvocationHandler(object));
    }
}

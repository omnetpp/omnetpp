package org.omnetpp.scave.computed;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

import org.apache.commons.lang3.ArrayUtils;

/**
 * Vector operations.
 *
 * @author andras
 */
//TODO register all methods in the VectorOp class; use it in DatasetManager.evaluate()
public class VectorOperation {
    private final String name;
    private final String signature;
    private final String description;
    private final Method method;

    VectorOperation(String name, String signature, Class<?> clazz, String methodName, String description) {
        this.name = name;
        this.signature = signature;
        this.description = description;
        this.method = findMethod(clazz, methodName);
    }

    private static Method findMethod(Class<?> clazz, String methodName) {
        Method method = null;
        for (Method m : clazz.getMethods()) {
            if (m.getName().equals(methodName)) {
                if (method != null)
                    throw new RuntimeException("Ambiguous method" + clazz.getSimpleName()+"." + methodName + ", overloaded methods not allowed here");
                method = m;
            }
        }
        if (method == null)
            throw new RuntimeException("No such method: " + clazz.getSimpleName()+"." + methodName);
        int modifiers = method.getModifiers();
        if (!Modifier.isPublic(modifiers) || !Modifier.isStatic(modifiers))
            throw new RuntimeException("Must be a public static method: " + clazz.getSimpleName()+"." + methodName);
        return method;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getSignature() {
        return signature;
    }
    
    public Method getMethod() {
        return method;
    }

    public Class<?>[] getArgTypes() {
        Class<?>[] methodArgTypes = method.getParameterTypes();
        return ArrayUtils.remove(methodArgTypes, 0);  // remove XYVector argument
    }

    public void apply(XYVector vector, Object[] args) throws IllegalAccessException, IllegalArgumentException, InvocationTargetException {
        Object[] methodArgs = ArrayUtils.add(args, 0, vector); // add the XYVector as first argument
        method.invoke(null, methodArgs);
    }
}

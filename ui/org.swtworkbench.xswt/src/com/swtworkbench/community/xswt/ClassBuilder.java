package com.swtworkbench.community.xswt;

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Map;

import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Decorations;
import org.eclipse.swt.widgets.Widget;

public class ClassBuilder {
    public static final String ERROR_NOT_CONTROL = "parent is not Control";
    public static final String ERROR_NOT_WIDGET = "parent is not Widget";
    public static final String ERROR_NOT_COMPOSITE = "parent is not Composite";
    public static final String ERROR_NOT_DECORATION = "parent is not Decoration";
    private static ClassBuilder builder = null;

    private LinkedList imports = new LinkedList();
    private ClassLoader classLoader = getClass().getClassLoader();  // instead of plain Class.forName() --Andras

    private Map resolvedClasses = new HashMap();

    public static ClassBuilder getDefault() {
        if (builder == null)
            builder = new ClassBuilder();
        return builder;
    }

    @SuppressWarnings("unchecked")
    public void importPackage(String packageName) {
        this.imports.addLast(packageName);
    }
    
    public void setClassLoader(ClassLoader classLoader) {
        this.classLoader = classLoader;
    }
    
    public ClassLoader getClassLoader() {
        return classLoader;
    }

    @SuppressWarnings("unchecked")
    public Class getClass(String className) throws XSWTException {
        Class result = null;

        result = (Class) this.resolvedClasses.get(className);
        if (result != null)
            return result;

        Iterator i = this.imports.iterator();
        while (i.hasNext()) {
            StringBuffer packageName = new StringBuffer((String) i.next());
            packageName.append(".");
            packageName.append(className);
            String fullyQualifiedName = packageName.toString();
            try {
                result = classLoader.loadClass(fullyQualifiedName);
            }
            catch (Exception localException) {
                result = null;
            }
            if (result != null) {
                StyleParser.registerClassConstants(result);
                return result;
            }
        }

        try {
            result = classLoader.loadClass(className);
        }
        catch (Throwable localThrowable) {
        }
        if (result != null) {
            StyleParser.registerClassConstants(result);
            return result;
        }

        throw new XSWTException("Unable to resolve class: " + className
                + "\nCheck the import node for the necessary package name");
    }

    @SuppressWarnings("unchecked")
    public Object constructControl(String className, Object parent, int style) throws XSWTException {
        try {
            Class klass = getClass(className);
            Constructor constructor = null;
            try {
                if (!(parent instanceof Decorations))
                    throw new Exception("parent is not Decoration");
                constructor = klass.getDeclaredConstructor(new Class[] { Decorations.class, Integer.TYPE });
            }
            catch (Exception localException1) {
                try {
                    if (!(parent instanceof Composite))
                        throw new Exception("parent is not Composite");
                    constructor = klass.getDeclaredConstructor(new Class[] { Composite.class, Integer.TYPE });
                }
                catch (Exception localException2) {
                    try {
                        if (!(parent instanceof Control))
                            throw new Exception("parent is not Control");
                        constructor = klass.getDeclaredConstructor(new Class[] { Control.class, Integer.TYPE });
                    }
                    catch (Exception localException3) {
                        try {
                            if (!(parent instanceof Widget))
                                throw new Exception("parent is not Widget");
                            constructor = klass.getDeclaredConstructor(new Class[] { Widget.class, Integer.TYPE });
                        }
                        catch (Exception localException4) {
                            try {
                                constructor = klass.getDeclaredConstructor(new Class[] { parent.getClass(), Integer.TYPE });
                            }
                            catch (Exception localException5) {
                                try {
                                    if (!(parent instanceof Control))
                                        throw new Exception("parent is not Control");
                                    constructor = klass.getDeclaredConstructor(new Class[] { Control.class });
                                }
                                catch (Exception localException6) {
                                    try {
                                        if (!(parent instanceof Widget))
                                            throw new Exception("parent is not Widget");
                                        constructor = klass.getDeclaredConstructor(new Class[] { Widget.class });
                                    }
                                    catch (Exception localException7) {
                                        constructor = klass.getDeclaredConstructor(new Class[] { parent.getClass() });
                                    }
                                }
                            }
                        }
                    }
                }

            }

            if (constructor.getParameterTypes().length == 2) {
                return constructor.newInstance(new Object[] { parent, new Integer(style) });
            }
            return constructor.newInstance(new Object[] { parent });
        }
        catch (Throwable t) {
            throw new XSWTException(t);
        }
    }

    public void dispose() {
        this.imports.clear();
        this.resolvedClasses.clear();
    }
}
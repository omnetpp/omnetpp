package com.swtworkbench.community.xswt;

import java.lang.reflect.Constructor;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.Platform;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Decorations;
import org.eclipse.swt.widgets.Widget;

@SuppressWarnings("rawtypes")
public class ClassBuilder {
    public static final String CLASSLOADER_EXTENSION_ID = "org.swtworkbench.xswt.classloaders";
    public static final String ERROR_NOT_CONTROL = "parent is not Control";
    public static final String ERROR_NOT_WIDGET = "parent is not Widget";
    public static final String ERROR_NOT_COMPOSITE = "parent is not Composite";
    public static final String ERROR_NOT_DECORATION = "parent is not Decoration";
    private static ClassLoader[] classLoaders; // class loaders from the eclipse configuration
    private LinkedList imports = new LinkedList();
    private ClassLoader extraClassLoader;
    private Map resolvedClasses = new HashMap();
    private static ClassBuilder builder = null;

    public static ClassBuilder getDefault() {
        if (builder == null)
            builder = new ClassBuilder();
        return builder;
    }

    @SuppressWarnings("unchecked")
    public void importPackage(String packageName) {
        this.imports.addLast(packageName);
    }

    public void setExtraClassLoader(ClassLoader classLoader) {
        this.extraClassLoader = classLoader;
    }

    public ClassLoader getExtraClassLoader() {
        return extraClassLoader;
    }

    protected ClassLoader[] getClassLoaders() {
        if (classLoaders == null) {
            List<ClassLoader> result = new ArrayList<ClassLoader>();
            result.add(getClass().getClassLoader());
            try {
                IConfigurationElement[] config = Platform.getExtensionRegistry().getConfigurationElementsFor(CLASSLOADER_EXTENSION_ID);
                for (IConfigurationElement e : config) {
                    String pluginId = e.getAttribute("pluginId");
                    String referenceClass = e.getAttribute("referenceClass");
                    ClassLoader classLoader = Platform.getBundle(pluginId).loadClass(referenceClass).getClassLoader();
                    result.add(classLoader);
                }
            } catch (Exception ex) {
                XswtPlugin.logError("Error getting ClassLoaders from the configuration", ex);
            }
            classLoaders = result.toArray(new ClassLoader[]{});
        }
        return classLoaders;
    }

    public Class getClass(String className) throws XSWTException {
        Class result = (Class) this.resolvedClasses.get(className);
        if (result != null)
            return result;

        for (ClassLoader classLoader : getClassLoaders()) {
            result = getClass(className, classLoader);
            if (result != null)
                return result;
        }
        if (extraClassLoader != null)
            result = getClass(className, extraClassLoader);
        if (result != null)
            return result;
        throw new XSWTException("Unable to resolve class: " + className + "\nCheck the import node for the necessary package name");
    }

    @SuppressWarnings("unchecked")
    protected Class getClass(String className, ClassLoader classLoader) throws XSWTException {
        Class result = null;

        Iterator i = this.imports.iterator();
        while (i.hasNext()) {
            String packageName = (String) i.next();
            String fullyQualifiedName = packageName + "." + className;
            try {
                result = classLoader.loadClass(fullyQualifiedName);
            } catch (Exception localException) { }
            if (result != null)
                break;
        }

        if (result == null) {
            try {
                result = classLoader.loadClass(className);
            } catch (Throwable localThrowable) { }
        }

        if (result != null) {
            StyleParser.registerClassConstants(result);
            resolvedClasses.put(className, result);
        }
        return result;
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
                return constructor.newInstance(new Object[] { parent, Integer.valueOf(style) });
            }
            return constructor.newInstance(new Object[] { parent });
        }
        catch (Throwable t) {
            throw new XSWTException(t);
        }
    }
}
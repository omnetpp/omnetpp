package org.omnetpp.common.wizard.support;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.omnetpp.common.util.ReflectionUtils;

import freemarker.ext.beans.HashAdapter;
import freemarker.template.TemplateModel;
import freemarker.template.utility.ClassUtil;



/**
 * Static utility methods for the FreeMarker templates, exposed to the user
 * via BeansWrapper.
 *
 * IMPORTANT: This class must be backward compatible across OMNeT++ versions
 * at all times. DO NOT REMOVE FUNCTIONS OR CHANGE THEIR SIGNATURES; add new methods
 * instead, if needed.
 *
 * @author Andras
 */
public class LangUtils {

    /**
     * Instantiates the given class with the default constructor.
     * (Provided because FreeMarker does not seem to accept the Class.newInstance() method)
     */
    public static Object newInstance(String className) {
        try {
            Class<?> clazz = ClassUtil.forName(className);
            return clazz.newInstance();
        }
        catch (Exception e) {
            throw new RuntimeException("Cannot instantiate "+className+": "+e.getClass().getSimpleName(), e);
        }
    }

    /**
     * Creates and returns a new mutable List object (currently ArrayList).
     */
    public static List<Object> newList() {
        return new ArrayList<Object>();
    }

    /**
     * Creates and returns a new mutable Map object (currently HashMap).
     */
    public static Map<Object, Object> newMap() {
        return new HashMap<Object, Object>();
    }

    /**
     * Creates and returns a new mutable Set object (currently HashSet).
     */
    public static Set<Object> newSet() {
        return new HashSet<Object>();
    }

    /**
     * Produces a user-friendly representation of the object. In case of
     * collections (lists, maps, etc), the representation is JSON-like.
     */
    @SuppressWarnings("rawtypes")
    public static String toString(Object object) {
        try {
            if (object == null)
                return "null";
            if (object instanceof String)
                return "\"" + (String)object + "\"";
            if (object instanceof Object[])
                return toString(Arrays.asList((Object[])object));
            if (object instanceof byte[])
                return toString(Arrays.asList((byte[])object));
            if (object instanceof char[])
                return toString(Arrays.asList((char[])object));
            if (object instanceof short[])
                return toString(Arrays.asList((short[])object));
            if (object instanceof int[])
                return toString(Arrays.asList((int[])object));
            if (object instanceof long[])
                return toString(Arrays.asList((long[])object));
            if (object instanceof float[])
                return toString(Arrays.asList((float[])object));
            if (object instanceof double[])
                return toString(Arrays.asList((double[])object));
            if (object instanceof HashAdapter) {
                TemplateModel templateModel = ((HashAdapter) object).getTemplateModel();
                if (templateModel.getClass().getName().equals("freemarker.ext.beans.StaticModel")) {
                    Class<?> clazz = (Class<?>) ReflectionUtils.getFieldValue(templateModel, "clazz");
                    return "wrapper for " + clazz.getName();
                }
                else {
                    return templateModel.toString(); // HashAdapter implements List but size() throws exception
                }
            }
            if (object instanceof List) {
                StringBuilder buffer = new StringBuilder();
                buffer.append("[");
                for (Object item : (List)object) {
                    if (buffer.length() > 1)
                        buffer.append(", ");
                    buffer.append(toString(item));
                }
                buffer.append("]");
                return buffer.toString();
            }
            if (object instanceof Set) {
                StringBuilder buffer = new StringBuilder();
                buffer.append("{");
                for (Object item : (Set)object) {
                    if (buffer.length() > 1)
                        buffer.append(", ");
                    buffer.append(toString(item));
                }
                buffer.append("}");
                return buffer.toString();
            }
            if (object instanceof Map) {
                StringBuilder buffer = new StringBuilder();
                buffer.append("{");
                for (Object key : ((Map)object).keySet()) {
                    if (buffer.length() > 1)
                        buffer.append(", ");
                    buffer.append(toString(key) + ": " + toString(((Map)object).get(key)));
                }
                buffer.append("}");
                return buffer.toString();
            }
            return object.toString();
        }
        catch (Exception e) {
            return object.getClass().toString() + "[toString() threw " + e.getClass().getSimpleName() + "]"; // hashCode() is unsafe!!
        }
    }

    /**
     * Returns the class of the given object. Provided because BeanWrapper seems to have
     * a problem with the getClass() method.
     */
    public static Class<?> getClass(Object object) {
        return object.getClass();
    }

    /**
     * Returns true if the object has a method with the given name.
     * Method args and return type are not taken into account.
     * Search is performed on the object's class and all super classes.
     */
    public static boolean hasMethod(Object object, String methodName) {
        for (Class<?> clazz = object.getClass(); clazz != null; clazz=clazz.getSuperclass())
            for (Method method : clazz.getDeclaredMethods())
                if (method.getName().equals(methodName))
                    return true;
        return false;
    }

    /**
     * Returns true if the object has a field with the given name.
     * Field type is not taken into account. Search is performed on
     * the object's class and all super classes.
     */
    public static boolean hasField(Object object, String fieldName) {
        for (Class<?> clazz = object.getClass(); clazz != null; clazz=clazz.getSuperclass())
            for (Field field : clazz.getDeclaredFields())
                if (field.getName().equals(fieldName))
                    return true;
        return false;
    }

    /**
     * Returns true if the given object is instance of (subclasses from or
     * implements) the given class or interface. To simplify usage, the class
     * or interface name is accepted both with and without the package name.
     */
    public static boolean instanceOf(Object object, String classOrInterfaceName) {
        Class<?> clazz = object.getClass();
        if (clazz.getName().equals(classOrInterfaceName) || clazz.getSimpleName().equals(classOrInterfaceName))
            return true;
        if (instanceOf(clazz.getSuperclass(), classOrInterfaceName))
            return true;
        for (Class<?> interfaceClazz : clazz.getInterfaces())
            if (instanceOf(interfaceClazz, classOrInterfaceName))
                return true;
        return false;
    }
}

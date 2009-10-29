package org.omnetpp.cdt.wizard.support;

import java.lang.reflect.Field;
import java.lang.reflect.Method;



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

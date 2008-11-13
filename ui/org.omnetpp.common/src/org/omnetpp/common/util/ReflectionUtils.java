package org.omnetpp.common.util;

import java.lang.annotation.Annotation;
import java.lang.reflect.AccessibleObject;
import java.lang.reflect.Constructor;
import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;

@SuppressWarnings("unchecked")
public class ReflectionUtils
{
	public static boolean isPublic( Method method )
	{
		return Modifier.isPublic( method.getModifiers() );
	}

	public static void setAccessible( AccessibleObject object )
	{
		if ( object.isAccessible() )
			return;
		object.setAccessible( true );
	}

	public static <T> T castObject( Class<T> klazz, Object object )
	{
		if ( klazz.isInstance( object ) )
			return (T) object;

		throw new AssertionError( object + " cannot be cast to Class[" + klazz.getName() + "]!" );
	}


	public static void assertAssignableFrom( Class<?> to, Class<?> from )
	{
	    if (!isAssignableFrom( to, from ))
	        throw new AssertionError( "Class[" + from.getName() + "] is not assignable to Class[" + to.getName() + "]!" );
	}

	public static boolean isAssignableFrom( Class<?> to, Class<?> from )
	{
		if ( to.isAssignableFrom( from ) )
			return true;

		if (to == Integer.TYPE && from.isAssignableFrom(Integer.class) ||
		    from == Integer.TYPE && to.isAssignableFrom(Integer.class))
		    return true;
		if (to == Long.TYPE && from.isAssignableFrom(Long.class) ||
		    from == Long.TYPE && to.isAssignableFrom(Long.class))
		    return true;
		if (to == Byte.TYPE && from.isAssignableFrom(Byte.class) ||
		    from == Byte.TYPE && to.isAssignableFrom(Byte.class))
		    return true;
		if (to == Short.TYPE && from.isAssignableFrom(Short.class) ||
		    from == Short.TYPE && to.isAssignableFrom(Short.class))
		    return true;
		if (to == Float.TYPE && from.isAssignableFrom(Float.class) ||
		    from == Float.TYPE && to.isAssignableFrom(Float.class))
		    return true;
		if (to == Double.TYPE && from.isAssignableFrom(Double.class) ||
		    from == Double.TYPE && to.isAssignableFrom(Double.class))
		    return true;
		if (to == Boolean.TYPE && from.isAssignableFrom(Boolean.class) ||
		    from == Boolean.TYPE && to.isAssignableFrom(Boolean.class))
		    return true;
		if (to == Character.TYPE && from.isAssignableFrom(Character.class) ||
		    from == Character.TYPE && to.isAssignableFrom(Character.class))
		    return true;
		if ( to.isArray() && from.isArray() )
			return isAssignableFrom( to.getComponentType(), from.getComponentType() );

		// TODO: implement other primitive types
		if (to.isPrimitive() || from.isPrimitive())
		    throw new UnsupportedOperationException("Not yet implemented for this primitive type");

		return false;
	}

	public static Method getMethod( Class klazz, String methodName, Class... arguments )
	{
		Method foundMethod = getMethodOrNullRec( klazz, methodName, arguments );

		if ( foundMethod == null )
			throw new AssertionError( "Class [" + klazz.getName() + "] does not have the appropriate Method [" + methodName + "]!" );

		return foundMethod;
	}

	public static Constructor getConstructor( Class klazz, Class... arguments )
	{
		Constructor[] constructors = klazz.getDeclaredConstructors();
		Constructor foundConstructor = null;

		_methodLoop: for ( int i = 0 ; i < constructors.length ; i++ ) {
			Class[] parameters = constructors[ i ].getParameterTypes();
			if ( parameters.length == ((arguments == null) ? 0 : arguments.length) ) {
				if ( arguments != null ) {
					for ( int j = 0 ; j < parameters.length ; j++ ) {
						if ( !isAssignableFrom(parameters[ j ], arguments[ j ] ) )
							continue _methodLoop;
					}
				}
				foundConstructor = constructors[ i ];
				break _methodLoop;
			}
		}

		if ( foundConstructor == null )
			throw new AssertionError( "Class [" + klazz.getName() + "] does not have the appropriate Constructor!" );

		return foundConstructor;
	}

	public static Field getField( Class klazz, String fieldName, boolean isStatic )
	{
		Field foundField = getFieldOrNullRec( klazz, fieldName, isStatic );

		if ( foundField == null )
			throw new AssertionError( "Class [" + klazz.getName() + "] does not have the appropriate Field [" + fieldName + "]!" );

		return foundField;
	}

	public static <T extends Annotation> T getAnnotation( Class klazz, Class<T> annotationClass )
	{
		T annotation = getAnnotationOrNullRec( klazz, annotationClass );

		if ( annotation == null )
			throw new AssertionError( "Class [" + klazz.getName() + "] does not have the Annotation [" + annotationClass.getName() + "]!" );

		return annotation;
	}

	private static <T extends Annotation> T getAnnotationOrNull( Class klazz, Class<T> annotationClass )
	{
		T annotation = (T) klazz.getAnnotation( annotationClass );
		return annotation;

//		Annotation[] annotations = klazz.getDeclaredAnnotations();
//		for ( int i = 0 ; i < annotations.length ; i++ ) {
//			Annotation annotation = annotations[ i ];
//			Debug.println( annotation.getClass().getName() );
//			if ( isAssignableFrom(annotation.getClass(), annotationClass ) )
//				return (T) annotation;
//		}
//		return null;
	}

	private static <T extends Annotation> T getAnnotationOrNullRec( Class klazz, Class<T> annotationClass )
	{
		if ( klazz == null || klazz.equals( Object.class ) )
            return null;

		Annotation annotation = getAnnotationOrNull( klazz, annotationClass );
		if ( annotation == null )
			annotation = getAnnotationOrNull( klazz.getSuperclass(), annotationClass );
		if ( annotation == null ) {
			Class[] interfaces = klazz.getInterfaces();
			for ( int i = 0 ; annotation == null && i < interfaces.length ; i++ ) {
				annotation = getAnnotationOrNullRec( klazz.getSuperclass(), annotationClass );
			}
		}
		return (T) annotation;
	}

	private static Method getMethodOrNull( Class klazz, String methodName, Class[] arguments )
	{
		Method[] methods = klazz.getDeclaredMethods();
		Method foundMethod = null;
		_methodLoop: for ( int i = 0 ; i < methods.length ; i++ ) {
			Method method = methods[ i ];

			if ( method.getName().equals( methodName ) ) {
				Class[] parameters = method.getParameterTypes();
				if ( parameters.length == ((arguments == null) ? 0 : arguments.length) ) {
					if ( arguments != null ) {
						for ( int j = 0 ; j < parameters.length ; j++ ) {
							if ( !isAssignableFrom(parameters[ j ], arguments[ j ] ) )
							    continue _methodLoop;
						}
					}
					foundMethod = method;
					break _methodLoop;
				}
			}
		}
		return foundMethod;
	}

	private static Method getMethodOrNullRec( Class klazz, String methodName, Class[] arguments )
	{
		if ( klazz == null || klazz.equals( Object.class ) )
            return null;

		Method foundMethod = getMethodOrNull( klazz, methodName, arguments );
		if ( foundMethod == null )
			foundMethod = getMethodOrNullRec( klazz.getSuperclass(), methodName, arguments );
		if ( foundMethod == null ) {
			Class[] interfaces = klazz.getInterfaces();
			for ( int i = 0 ; foundMethod == null && i < interfaces.length ; i++ ) {
				foundMethod = getMethodOrNullRec( interfaces[ i ], methodName, arguments );
			}
		}
		return foundMethod;
	}

	private static Field getFieldOrNull( Class klazz, String fieldName, boolean isStatic )
	{
		Field[] fields = klazz.getDeclaredFields();
		Field foundField = null;
		_fieldLoop: for ( int i = 0 ; i < fields.length ; i++ ) {
			Field field = fields[ i ];

			if ( field.getName().equals( fieldName ) ) {
				boolean isFieldStatic = Modifier.isStatic( field.getModifiers() );
				if ( isStatic && isFieldStatic || ! isStatic && ! isFieldStatic ) {
					foundField = field;
					break _fieldLoop;
				}
			}
		}
		return foundField;
	}

	private static Field getFieldOrNullRec( Class klazz, String fieldName, boolean isStatic )
	{
		if ( klazz == null || klazz.equals( Object.class ) || klazz.isInterface() && ! isStatic )
            return null;

		Field foundField = getFieldOrNull( klazz, fieldName, isStatic );
		if ( foundField == null )
			foundField = getFieldOrNullRec( klazz.getSuperclass(), fieldName, isStatic );
		if ( foundField == null ) {
			Class[] interfaces = klazz.getInterfaces();
			for ( int i = 0 ; foundField == null && i < interfaces.length ; i++ ) {
				foundField = getFieldOrNullRec( interfaces[ i ], fieldName, isStatic );
			}
		}
		return foundField;
	}

	public static Object invokeMethod( Method method, Object self, Object... arguments )
	{
		try {
			return method.invoke( self, arguments );
		}
		catch ( IllegalArgumentException e ) {
			throw new RuntimeException( e );
		}
		catch ( IllegalAccessException e ) {
			throw new RuntimeException( e );
		}
		catch ( InvocationTargetException e ) {
			throw new RuntimeException( e.getCause() );
		}
	}

	public static Object invokeMethod( Object self, String methodName, Object... arguments )
	{
		Method method = getMethod( self.getClass(), methodName, getClasses( arguments ) );
		setAccessible(method);
		return invokeMethod( method, self, arguments );
	}

	public static Object invokeStaticMethod( Object self, String methodName, Object... arguments )
	{
		return invokeMethod( self.getClass(), methodName, arguments );
	}

	public static Object invokeStaticMethod( Class clazz, String methodName, Object... arguments )
	{
		Method method = getMethod( clazz, methodName, getClasses( arguments ) );
		setAccessible(method);
		return invokeMethod( method, (Object)null, arguments );
	}

	private static Class[] getClasses( Object[] arguments )
	{
		if ( arguments == null )
			return new Class[ 0 ];
		Class[] result = new Class[ arguments.length ];
		for ( int i = 0 ; i < arguments.length ; i++ )
		    if (arguments [ i ] != null)
		        result[ i ] = arguments[ i ].getClass();

		return result;
	}

	public static Object invokeConstructor( Constructor constructor, Object... arguments )
	{
		try {
			return constructor.newInstance( arguments );
		}
		catch ( IllegalArgumentException e ) {
			throw new RuntimeException( e );
		}
		catch ( InstantiationException e ) {
			throw new RuntimeException( e );
		}
		catch ( IllegalAccessException e ) {
			throw new RuntimeException( e );
		}
		catch ( InvocationTargetException e ) {
			throw new RuntimeException( e.getCause() );
		}
	}

	public static Object invokeConstructor( Class klazz, Object... arguments )
	{
		Constructor constructor = getConstructor( klazz, getClasses( arguments ) );

		return invokeConstructor( constructor, arguments );
	}

	public static Object getFieldValue( Field field, Object self )
	{
		try {
			return field.get( self );
		}
		catch ( IllegalArgumentException e ) {
			throw new RuntimeException( e );
		}
		catch ( IllegalAccessException e ) {
			throw new RuntimeException( e );
		}
	}

	public static void setFieldValue( Field field, Object self, Object value )
	{
		try {
			field.set( self, value );
		}
		catch ( IllegalArgumentException e ) {
			throw new RuntimeException( e );
		}
		catch ( IllegalAccessException e ) {
			throw new RuntimeException( e );
		}
	}

	public static Object getFieldValue( Object self, String fieldName )
	{
		Field field = getField( self.getClass(), fieldName, false );
        setAccessible(field);
		return getFieldValue( field, self );
	}

	public static void setFieldValue( Object self, String fieldName, Object value )
	{
		Field field = getField( self.getClass(), fieldName, false );
        setAccessible(field);
		setFieldValue( field, self, value );
	}

	public static Object getFieldValue( Class klazz, String fieldName )
	{
		Field field = getField( klazz, fieldName, true );
        setAccessible(field);
		return getFieldValue( field, (Object)null );
	}

	public static void setFieldValue( Class klazz, String fieldName, Object value )
	{
		Field field = getField( klazz, fieldName, true );
        setAccessible(field);
		setFieldValue( field, (Object)null, value );
	}

	public static <T extends Annotation> T getAnnotation( Method method, Class<T> annotationClass, boolean doAssert )
	{
		Annotation[] annotations = method.getAnnotations();
		for ( int i = 0 ; i < annotations.length ; i++ ) {
			Annotation annotation = annotations[ i ];
			if ( isAssignableFrom(annotation.getClass(), annotationClass ) )
				return (T) annotation;
		}

		assert (!doAssert) : "Method [" + method + "] does not have the Annotation [" + annotationClass.getName()
			+ "]!";
		return null;

	}

    public static Object newInstance(String className, Object... arguments)
	{
	    try {
	        Class clazz = Thread.currentThread().getContextClassLoader().loadClass(className);

	        return invokeConstructor(clazz, arguments);
	    }
	    catch ( IllegalArgumentException e ) {
			throw new RuntimeException( e );
		}
	    catch ( ClassNotFoundException e ) {
			throw new RuntimeException( e );
		}
	}
}

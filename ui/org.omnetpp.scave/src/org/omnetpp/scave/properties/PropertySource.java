package org.omnetpp.scave.properties;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.PropertyDescriptor;

public abstract class PropertySource implements IPropertySource {

	static Map<Class,Entry> map;

	static class Entry
	{
		IPropertyDescriptor[] descriptors;
		Map<String,PropInfo> infos;
		
		public Entry(IPropertyDescriptor[] descriptors, Map<String,PropInfo> infos)
		{
			this.descriptors = descriptors;
			this.infos = infos;
		}
	}
	
	static class PropInfo
	{
		Method getter;
		Method setter;
		Method defaultGetter;
		IPropertyDescriptor descriptor;
		
		public PropInfo(Method getter, Method setter,
				Method defaultGetter, IPropertyDescriptor descriptor)
		{
			this.getter = getter;
			this.setter = setter;
			this.defaultGetter = defaultGetter;
			this.descriptor = descriptor;
		}
	}
	
	public PropertySource()
	{
		if (map == null)
			map = new HashMap<Class,Entry>();
		if (!map.containsKey(getClass()))
			buildPropInfo();
	}
	
	private PropInfo getInfo(String id)
	{
		Entry entry = map.get(getClass());
		if (entry != null)
			return entry.infos.get(id);
		else
			return null;
	}
	
	private void buildPropInfo()
	{
		ArrayList<IPropertyDescriptor> descriptors = new ArrayList<IPropertyDescriptor>();
		Map<String,PropInfo> infos = new HashMap<String,PropInfo>();

		Class<? extends PropertySource> propClass = getClass();
		for (Method getter : propClass.getMethods())
		{

			if (isPropertyGetter(getter)) {
				try {
					Property propAnnotation = getter.getAnnotation(Property.class);
					String propName = propNameFromGetterName(getter.getName());
					Class propType = getter.getReturnType();
					IPropertyDescriptor descriptor;
					descriptor = createPropertyDescriptor(propAnnotation, propName, propType);
					Method setter = getPropertySetter(propClass, propName, propType);
					Method defaultGetter = getPropertyDefaultGetter(propClass, propName, propType); 
					descriptors.add(descriptor);
					infos.put((String)descriptor.getId(), new PropInfo(getter, setter, defaultGetter, descriptor));
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}
		
		int size = descriptors.size();
		Entry entry = new Entry(descriptors.toArray(new IPropertyDescriptor[size]), infos);
		map.put(getClass(), entry);
	}
	
	private boolean isPropertyGetter(Method method) {
		return method.getName().startsWith("get") &&
			method.getParameterTypes().length == 0 &&
			Modifier.isPublic(method.getModifiers()) &&
			method.isAnnotationPresent(Property.class);
	}
	
	private static Method getPropertySetter(Class propClass, String propName, Class propType) {
		try {
			Method method = propClass.getMethod("set" + propName, propType);
			if (method != null) {
				int modifiers = method.getModifiers();
				if (Modifier.isPublic(modifiers) && !Modifier.isStatic(modifiers))
					return method;
			}
		} catch (Exception e) {}
		return null;
	}
	
	private static Method getPropertyDefaultGetter(Class propClass, String propName, Class<?> propType) {
		try {
			Method method = propClass.getMethod("default" + propName);
			if (method != null) {
				int modifiers = method.getModifiers();
				if (Modifier.isPublic(modifiers) && !Modifier.isStatic(modifiers) &&
						propType.isAssignableFrom(method.getReturnType()))
					return method;
			}
		} catch (Exception e) {}
		return null;
	}
	
	private String propNameFromGetterName(String getterName) {
		return getterName.substring(3);
	}
	
	private IPropertyDescriptor createPropertyDescriptor(Property property, String propName, Class propType)
		throws Exception
	{
		String id = property.id().length() > 0 ? property.id() : propName;
		String displayName = property.displayName().length() > 0 ? property.displayName() : propName;
		String category = property.category();
		String description = property.description();
		Class descriptorClass = property.descriptorClass();
		
		Constructor constructor = descriptorClass.getConstructor(new Class[] {Object.class, String.class});
		PropertyDescriptor descriptor =
			(PropertyDescriptor)constructor.newInstance(new Object[] {id, displayName});
		if (descriptor instanceof EnumPropertyDescriptor)
			((EnumPropertyDescriptor)descriptor).setEnumType(propType);
		if (category.length() > 0)
			descriptor.setCategory(category);
		if (description.length() > 0)
			descriptor.setDescription(description);
		
		return descriptor;
	}
	
	// TODO: this method should exists somewhere else
	public static boolean equals(Object obj1, Object obj2) {
		return obj1 == null && obj2 == null ||
			   obj1 != null && obj1.equals(obj2);
	}

	/*
	 * The rest is the implementation of IPropertySource interface.
	 */
	
	public IPropertyDescriptor[] getPropertyDescriptors() {
		Entry entry = map.get(getClass());
		return entry.descriptors;
	}
	
	public Object getEditableValue() {
		return null;
	}

	public boolean isPropertySet(Object id) {
		PropInfo info = getInfo((String)id);
		if (info.defaultGetter != null) {
			try {
				Object defaultValue = info.defaultGetter.invoke(this);
				Object currentValue = info.getter.invoke(this);
				return !equals(currentValue, defaultValue);
			} catch (Exception e) {}
		}
		return false;
	}
	
	public void resetPropertyValue(Object id) {
		PropInfo info = getInfo((String)id);
		if (info.defaultGetter != null && info.setter != null) {
			try {
				Object defaultValue = info.defaultGetter.invoke(this);
				info.setter.invoke(this, defaultValue);
			} catch (Exception e) {}
		}
	}

	public Object getPropertyValue(Object id) {
		PropInfo info = getInfo((String)id);
		Object value = null;
		try {
			value = info.getter.invoke(this);
			if (info.descriptor instanceof EnumPropertyDescriptor)
				value = ((EnumPropertyDescriptor)info.descriptor).getIndex(value);
		} catch (Exception e) { }
		return value;
	}

	public void setPropertyValue(Object id, Object value) {
		PropInfo info = getInfo((String)id);
		if (info != null && info.setter != null)
			try {
				if (info.descriptor instanceof EnumPropertyDescriptor)
					value = ((EnumPropertyDescriptor)info.descriptor).getValue((Integer)value);
				info.setter.invoke(this, new Object[] {value});
			} catch (Exception e) { }
	}
}	


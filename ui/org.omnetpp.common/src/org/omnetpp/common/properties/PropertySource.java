package org.omnetpp.common.properties;

import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;

public abstract class PropertySource implements IPropertySource2 {

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
		String methodName = method.getName();
		int modifiers = method.getModifiers();
		
		return (methodName.startsWith("get") || methodName.startsWith("is")) &&
			method.getParameterTypes().length == 0 &&
			Modifier.isPublic(modifiers) && !Modifier.isStatic(modifiers) &&
			method.isAnnotationPresent(Property.class);
	}
	
	private String propNameFromGetterName(String getterName) {
		if (getterName.startsWith("get"))
			return getterName.substring(3);
		else
			return getterName.substring(2);
	}
	
	private String displayNameFromPropName(String propName) {
		String[] syllables = syllabize(propName);
		for (int i = 0; i < syllables.length; ++i)
			syllables[i] = syllables[i].toLowerCase();
		return unsplit(syllables, " ");
	}
	
	private static String[] syllabize(String str) {
		int startIndex = 0;
		ArrayList<String> syllables = new ArrayList<String>();
		int len = str.length();
		for (int i = 0; i < len; ++i) {
			if (i > startIndex && Character.isUpperCase(str.charAt(i))) {
				syllables.add(str.substring(startIndex, i));
				startIndex = i;
			}
		}
		if (startIndex < len)
			syllables.add(str.substring(startIndex));
		
		return (String[])syllables.toArray(new String[syllables.size()]);
	}
	
	private static String unsplit(String[] strings, String separator) {
		StringBuffer sb =  new StringBuffer();
		int lastIndex = strings.length - 1;
		for (int i = 0; i < strings.length; ++i) {
			sb.append(strings[i]);
			if (i != lastIndex)
				sb.append(separator);
		}
		return sb.toString();
	}
	
	private static Method getPropertySetter(Class<? extends PropertySource> propClass, String propName, Class propType) {
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
	
	private static Method getPropertyDefaultGetter(Class<? extends PropertySource> propClass, String propName, Class<?> propType) {
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
	
	private IPropertyDescriptor createPropertyDescriptor(Property property, String propName, Class<?> propType)
		throws Exception
	{
		String id = property.id().length() > 0 ? property.id() : propName;
		String displayName = property.displayName().length() > 0 ?
								property.displayName() :
								displayNameFromPropName(propName);
		String category = property.category();
		String description = property.description();
		String[] filterFlags = property.filterFlags();
		Class<? extends PropertyDescriptor> descriptorClass =
			getPropertyDescriptorClass(property.descriptorClass(), propType);
		
		Constructor<? extends PropertyDescriptor> constructor =
			descriptorClass.getConstructor(new Class[] {Object.class, String.class});
		PropertyDescriptor descriptor = constructor.newInstance(new Object[] {id, displayName});
		if (descriptor instanceof EnumPropertyDescriptor)
			((EnumPropertyDescriptor)descriptor).setEnumType(propType);
		if (category.length() > 0)
			descriptor.setCategory(category);
		if (description.length() > 0)
			descriptor.setDescription(description);
		if (filterFlags.length > 0)
			descriptor.setFilterFlags(filterFlags);
		
		return descriptor;
	}
	
	public Class<? extends PropertyDescriptor>
	getPropertyDescriptorClass(Class<? extends PropertyDescriptor> declaredDescriptorClass, Class<?> propType) {
		if (declaredDescriptorClass == PropertyDescriptor.class) {
			if (propType == String.class)
				return TextPropertyDescriptor.class;
			if (propType == Boolean.class || propType == boolean.class)
				return CheckboxPropertyDescriptor.class;
			else if (propType == Double.class || propType == double.class)
				return NumberPropertyDescriptor.class;
			else if (propType == RGB.class)
				return ColorPropertyDescriptor.class;
			else if (propType == FontData.class)
				return FontPropertyDescriptor.class;
			else if (propType.isEnum())
				return EnumPropertyDescriptor.class;
		}
		return declaredDescriptorClass;
	}
	
	// TODO: this method should exists somewhere else
	public static boolean equals(Object obj1, Object obj2) {
		return obj1 == null && obj2 == null ||
			   obj1 != null && obj1.equals(obj2);
	}
	
	@Override
	public String toString() {
		return "";
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
		return true;
	}
	
	public boolean isPropertyResettable(Object id) {
		PropInfo info = getInfo((String)id);
		return info.defaultGetter != null && info.setter != null;
	}
	
	public void resetPropertyValue(Object id) {
		PropInfo info = getInfo((String)id);
		if (info.defaultGetter != null && info.setter != null) {
			try {
				Object defaultValue = info.defaultGetter.invoke(this);
				info.setter.invoke(this, defaultValue);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}

	public Object getPropertyValue(Object id) {
		PropInfo info = getInfo((String)id);
		Object value = null;
		try {
			value = info.getter.invoke(this);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return value;
	}

	public void setPropertyValue(Object id, Object value) {
		PropInfo info = getInfo((String)id);
		if (info != null && info.setter != null)
			try {
				info.setter.invoke(this, new Object[] {value});
			} catch (Exception e) {
				e.printStackTrace();
			}
	}
}	


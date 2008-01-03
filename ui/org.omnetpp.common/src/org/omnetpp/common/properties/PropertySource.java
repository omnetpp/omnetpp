package org.omnetpp.common.properties;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang.ArrayUtils;
import org.apache.commons.lang.ObjectUtils;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.CommonPlugin;
import org.omnetpp.common.util.StringUtils;

/**
 * Abstract base class for PropertySource wrappers.
 *
 * <p> This is a facility to add property source behavior to
 * an existing hierarchy of classes. You can write a subclass of PropertySource
 * for the objects having properties displayed on the property sheet.
 * The subclass has getter/setter/default methods, the getter annotated with Property.
 * The descriptors are generated from the Property annotations on the getters
 * and the corresponding getter, default, or setter method is called when
 * the property value, default value requested or set.
 * The getter/setter/default method calls on the subclasses can be delegated
 * to the wrapped objects.
 *
 * @author tomi
 */
public abstract class PropertySource implements IPropertySource2 {

	static Map<Class<?>,Entry> map;

	// Meta-data for property sources (per class).
	static class Entry
	{
		// Descriptors of a property source.
		public IPropertyDescriptor[] descriptors;
		// Maps property id to property info.
		public Map<String,PropInfo> infos;

		public Entry(IPropertyDescriptor[] descriptors, Map<String,PropInfo> infos)
		{
			this.descriptors = descriptors;
			this.infos = infos;
		}
	}

	// Holds getter/setter/default/descriptor factory methods for a property.
	static class PropInfo
	{
		Method getter;
		Method setter;
		Method defaultGetter;
		Method descriptorFactory;
		IPropertyDescriptor descriptor;

		public PropInfo(Method getter, Method setter,
				Method defaultGetter, Method descriptorFactory,
				IPropertyDescriptor descriptor)
		{
			this.getter = getter;
			this.setter = setter;
			this.defaultGetter = defaultGetter;
			this.descriptorFactory = descriptorFactory;
			this.descriptor = descriptor;
		}
	}

	public PropertySource()
	{
		if (map == null)
			map = new HashMap<Class<?>,Entry>();
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
					Class<?> propType = getter.getReturnType();
					IPropertyDescriptor descriptor;
					Method setter = getPropertySetter(propClass, propName, propType);
					Method defaultGetter = getPropertyDefaultGetter(propClass, propName, propType);
					Method descriptorFactory = getDescriptorFactoryMethod(propClass, propAnnotation, propName);
					descriptor = createPropertyDescriptor(descriptorFactory, propAnnotation, propName, propType);
					descriptors.add(descriptor);
					infos.put((String)descriptor.getId(), new PropInfo(getter, setter, defaultGetter, descriptorFactory, descriptor));
				} catch (Exception e) {
					CommonPlugin.logError(e);
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

		return syllables.toArray(new String[syllables.size()]);
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

	private static Method getPropertySetter(Class<? extends PropertySource> propClass, String propName, Class<?> propType) {
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

	private static Method getDescriptorFactoryMethod(Class<? extends PropertySource> propClass, Property property, String propName) {
		try {
			String methodName = getDescriptorFactoryMethodName(property, propName);
			Method method = propClass.getMethod(methodName, Object.class, String.class);
			if (method != null) {
				int modifiers = method.getModifiers();
				if (Modifier.isPublic(modifiers) && PropertyDescriptor.class.isAssignableFrom(method.getReturnType()))
					return method;
			}
		}
		catch (Exception e) {}
		return null;
	}

	private static String getDescriptorFactoryMethodName(Property property, String propName) {
		return property.descriptorFactoryMethod().length() > 0 ?
			   property.descriptorFactoryMethod() : "create" + propName + "Descriptor";
	}

	private IPropertyDescriptor createPropertyDescriptor(Method descriptorFactory, Property property, String propName, Class<?> propType)
		throws Exception
	{
		String id = property.id().length() > 0 ? property.id() : propName;
		String displayName = property.displayName().length() > 0 ?
								property.displayName() :
								displayNameFromPropName(propName);
		String category = property.category();
		String description = property.description();
		String[] filterFlags = property.filterFlags();
		boolean optional = property.optional();
		PropertyDescriptor descriptor;
		if (descriptorFactory != null) {
			descriptor = (PropertyDescriptor)descriptorFactory.invoke(this, id, displayName);
		}
		else {
			Class<? extends PropertyDescriptor> descriptorClass =
				getPropertyDescriptorClass(property.descriptorClass(), propType);
			Constructor<? extends PropertyDescriptor> constructor =
				descriptorClass.getConstructor(Object.class, String.class);
			descriptor = constructor.newInstance(id, displayName);
			if (descriptor instanceof EnumPropertyDescriptor)
				((EnumPropertyDescriptor)descriptor).setEnumType(propType, optional);
		}
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
			else if (propType == Integer.class || propType == int.class)
				return IntegerPropertyDescriptor.class;
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

	@Override
	public String toString() {
		return "";
	}

	/*
	 * The rest is the implementation of IPropertySource2 interface.
	 */

	public IPropertyDescriptor[] getPropertyDescriptors() {
		Entry entry = map.get(getClass());
		updateDescriptors(entry);
		return entry.descriptors;
	}

	/**
	 * Updates descriptors having a factory method.
	 * XXX: not thread-safe
	 */
	private void updateDescriptors(Entry entry) {
		for (int i = 0; i < entry.descriptors.length; ++i) {
			IPropertyDescriptor descriptor = entry.descriptors[i];
			PropInfo info = entry.infos.get(descriptor.getId());
			entry.descriptors[i] = info.descriptor = updateDescriptor(info, descriptor);
		}
	}

	private IPropertyDescriptor updateDescriptor(PropInfo info, IPropertyDescriptor descriptor) {
		try {
			if (info.descriptorFactory != null) {
				PropertyDescriptor newDescriptor = (PropertyDescriptor)info.descriptorFactory.invoke(this, descriptor.getId(), descriptor.getDisplayName());
				if (StringUtils.isEmpty(newDescriptor.getCategory()))
				    newDescriptor.setCategory(descriptor.getCategory());
				if (ArrayUtils.isEmpty(newDescriptor.getFilterFlags()))
				    newDescriptor.setFilterFlags(descriptor.getFilterFlags());
				if (StringUtils.isEmpty(newDescriptor.getDescription()))
				    newDescriptor.setDescription(descriptor.getDescription());
				return newDescriptor;
			}
		}
		catch (InvocationTargetException e) {}
		catch (IllegalAccessException e) {}

		return descriptor;
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
				return !ObjectUtils.equals(currentValue, defaultValue);
			} catch (Exception e) {}
		}
		return true;
	}

	public boolean isPropertyResettable(Object id) {
		// Note: isPropertyResettable() is supposed to control whether the "Restore Default Value"
		// action is enabled in the property's context menu. However, this is not yet implemented
		// in Eclipse; see comment at top of org.eclipse.ui.views.properties.DefaultsAction:
		// "[Issue: should listen for selection changes in the viewer and set enablement]"
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
				CommonPlugin.logError(e);
			}
		}
	}

	public Object getPropertyValue(Object id) {
		PropInfo info = getInfo((String)id);
		Object value = null;
		try {
			value = info.getter.invoke(this);
		} catch (Exception e) {
			CommonPlugin.logError(e);
		}
		return value;
	}

	public void setPropertyValue(Object id, Object value) {
		PropInfo info = getInfo((String)id);
		if (info != null && info.setter != null)
			try {
				info.setter.invoke(this, new Object[] {value});
			} catch (Exception e) {
				CommonPlugin.logError(e);
			}
	}
}


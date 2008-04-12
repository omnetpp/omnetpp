package org.omnetpp.scave.charting;

import java.util.HashMap;
import java.util.Map;

class PropertyMap<T> {
	
	private static final String KEY_ALL = null;
	private Class<T> clazz;
	private Map<String,T> properties = new HashMap<String,T>();
	
	PropertyMap(Class<T> clazz) {
		this.clazz = clazz;
	}
	
	T getProperties(String key) {
		return (key != null ? properties.get(key) : null);
	}
	
	T getDefaultProperties() {
		return properties.get(KEY_ALL);
	}
	
	T getOrCreateProperties(String key) {
		T props = getProperties(key);
		if (props == null) {
			try {
				props = clazz.newInstance();
				properties.put(key, props);
			} catch (IllegalAccessException e) {
				throw new RuntimeException(e);
			} catch (InstantiationException e) {
				throw new RuntimeException(e);
			}
		}
		return props;
	}
}

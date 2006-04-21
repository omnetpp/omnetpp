package org.omnetpp.common.properties;

import java.io.IOException;

import org.eclipse.jface.resource.StringConverter;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.omnetpp.common.xml.XMLWriter;

public class PropertySourceIO {

	public static void save(IPropertySource propertySource, XMLWriter writer)
	throws IOException {
		for (IPropertyDescriptor descriptor : propertySource.getPropertyDescriptors()) {
			// TODO: do not save default values
			String id = (String)descriptor.getId();
			Object value = propertySource.getPropertyValue(id);
			saveValue(id, value, descriptor, writer);
		}
	}
	
	private static void saveValue(String id, Object value,
			IPropertyDescriptor descriptor, XMLWriter writer)
	throws IOException {
		writer.writeStartElement(id);
		if (value == null) {
			// do nothing
		}
		else if (value instanceof Boolean ||
			value instanceof Number ||
			value instanceof String ||
			value instanceof RGB) {
			writer.writeText(String.valueOf(value));
		}
		else if (descriptor instanceof EnumPropertyDescriptor) {
			writer.writeText(((EnumPropertyDescriptor)descriptor).getName(value));
		}
		else if (value instanceof IPropertySource) {
			IPropertySource propertySource = (IPropertySource)value;
			save(propertySource, writer);
		}
		else if (value instanceof FontData) {
			writer.writeText(StringConverter.asString((FontData)value));
		}
		else {
			writer.writeText(String.valueOf(value));
		}
		writer.writeEndElement(id);
	}
}

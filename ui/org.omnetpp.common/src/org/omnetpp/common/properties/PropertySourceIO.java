/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.properties;

import java.io.IOException;
import java.util.Stack;

import org.eclipse.jface.resource.StringConverter;
import org.eclipse.swt.graphics.FontData;
import org.eclipse.swt.graphics.RGB;
import org.eclipse.ui.views.properties.ColorPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertyDescriptor;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySource2;
import org.eclipse.ui.views.properties.PropertyDescriptor;
import org.eclipse.ui.views.properties.TextPropertyDescriptor;
import org.omnetpp.common.xml.XMLWriter;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class PropertySourceIO {

    public static void save(IPropertySource propertySource, XMLWriter writer)
    throws IOException {
        for (IPropertyDescriptor descriptor : propertySource.getPropertyDescriptors()) {
            String id = (String)descriptor.getId();
            if (!(propertySource instanceof IPropertySource2) ||
                propertySource.isPropertySet(id)) {
                Object value = propertySource.getPropertyValue(id);
                saveValue(id, value, descriptor, writer);
            }
        }
    }

    private static void saveValue(String id, Object value,
            IPropertyDescriptor descriptor, XMLWriter writer) throws IOException {
        writer.writeStartElement(id);
        if (value == null) {
            // empty element
        }
        else if (value instanceof IPropertySource)
            save((IPropertySource)value, writer);
        else
            writer.writeText(toString(value, descriptor));
        writer.writeEndElement(id);
    }

    public static String toString(Object value, IPropertyDescriptor descriptor) {
        String str = null;

        if (descriptor instanceof CheckboxPropertyDescriptor ||
            descriptor instanceof NumberPropertyDescriptor ||
            descriptor instanceof TextPropertyDescriptor) {
            str = String.valueOf(value);
        }
        else if (descriptor instanceof ColorPropertyDescriptor)
            str = (StringConverter.asString((RGB)value));
        else if (descriptor instanceof FontPropertyDescriptor)
            str = (StringConverter.asString((FontData)value));
        else if (descriptor instanceof EnumPropertyDescriptor)
            str = (((EnumPropertyDescriptor)descriptor).getName(value));
        else
            throw new RuntimeException("Unsupported property descriptor: " + descriptor.getClass().getName());

        return str;
    }

    public static Object fromString(String str, IPropertyDescriptor descriptor) {
        Object value = null;

        if (descriptor instanceof CheckboxPropertyDescriptor)
            value = Boolean.parseBoolean(str);
        else if (descriptor instanceof NumberPropertyDescriptor)
            value = Double.parseDouble(str);
        else if (descriptor instanceof TextPropertyDescriptor)
            value = str;
        else if (descriptor instanceof ColorPropertyDescriptor)
            value = StringConverter.asRGB(str);
        else if (descriptor instanceof FontPropertyDescriptor)
            value = StringConverter.asFontData(str);
        else if (descriptor instanceof EnumPropertyDescriptor)
            value = ((EnumPropertyDescriptor)descriptor).getValue(str);
        else
            throw new RuntimeException("Unsupported property descriptor: " + descriptor.getClass().getName());

        return value;
    }

    public static class SAXHandler extends DefaultHandler
    {
        PropertySource propertySource;
        IPropertyDescriptor descriptor;
        Object value;

        Stack<PropertySource> stack = new Stack<PropertySource>();

        public SAXHandler(PropertySource propertySource) {
            this.propertySource = propertySource;
        }

        private IPropertyDescriptor getDescriptor(String id) {
            for (IPropertyDescriptor descriptor : propertySource.getPropertyDescriptors())
                if (id.equals(descriptor.getId()))
                    return descriptor;
            return null;
        }

        @Override
        public void startElement(String uri, String localName, String qName, Attributes attributes) throws SAXException {
            if (stack.isEmpty()) {
                stack.push(propertySource);
                return;
            }

            descriptor = getDescriptor(qName);
            if (descriptor == null)
                throw new SAXException("Unexpected element: " + qName);

            value = null;
            if (descriptor.getClass() == PropertyDescriptor.class) {
                stack.push(propertySource);
                propertySource = (PropertySource)propertySource.getPropertyValue(descriptor.getId());
            }
            else {
                stack.push(null);
            }
        }

        @Override
        public void endElement(String uri, String localName, String qName) throws SAXException {
            PropertySource parentPropertySource = stack.pop();
            if (parentPropertySource != null) {
                value = propertySource;
                propertySource = parentPropertySource;
            }
            propertySource.setPropertyValue(qName, value);
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {
             // ignore whitespace
            if (stack.peek() != null)
                return;

            String str = new String(ch, start, length);
            value = PropertySourceIO.fromString(str, descriptor);
        }
    }
}

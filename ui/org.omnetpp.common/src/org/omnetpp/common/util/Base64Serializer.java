package org.omnetpp.common.util;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import org.eclipse.core.internal.preferences.Base64;

public class Base64Serializer {
	public static String serialize(Object object) throws IOException {
		if (object == null)
			return null;
		else {
			ByteArrayOutputStream data = new ByteArrayOutputStream();
			ObjectOutputStream stream = new ObjectOutputStream(data);
			stream.writeObject(object);
			stream.close();

			return new String(Base64.encode(data.toByteArray()), "ASCII");
		}
	}

	public static Object deserialize(String string) throws IOException, ClassNotFoundException {
		return deserialize(string, null);
	}

	public static Object deserialize(String string, ClassLoader classLoader) throws IOException, ClassNotFoundException {
		if (string == null)
			return null;
		else {
			ByteArrayInputStream data = new ByteArrayInputStream(Base64.decode(string.getBytes("ASCII")));
			ObjectInputStream stream = classLoader == null ? new ObjectInputStream(data) : new ObjectInputStreamWithClassLoader(data, classLoader);
			Object object = stream.readObject();
			stream.close();
			
			return object;
		}
	}
}

package org.omnetpp.common.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.ObjectInputStream;
import java.io.ObjectStreamClass;
import java.io.StreamCorruptedException;
import java.lang.reflect.Array;

class ObjectInputStreamWithClassLoader extends ObjectInputStream {
	private ClassLoader loader;

	public ObjectInputStreamWithClassLoader(InputStream inputstream, ClassLoader classloader)
		throws IOException, StreamCorruptedException
	{
		super(inputstream);

		if (classloader == null) {
			throw new IllegalArgumentException("Illegal null argument to ObjectInputStreamWithLoader");
		}
		else {
			loader = classloader;
			return;
		}
	}

	private Class<?> primitiveType(char c) {
		switch (c) {
			case 66: // 'B'
				return Byte.TYPE;
	
			case 67: // 'C'
				return Character.TYPE;
	
			case 68: // 'D'
				return Double.TYPE;
	
			case 70: // 'F'
				return Float.TYPE;
	
			case 73: // 'I'
				return Integer.TYPE;
	
			case 74: // 'J'
				return Long.TYPE;
	
			case 83: // 'S'
				return Short.TYPE;
	
			case 90: // 'Z'
				return Boolean.TYPE;
	
			case 69: // 'E'
			case 71: // 'G'
			case 72: // 'H'
			case 75: // 'K'
			case 76: // 'L'
			case 77: // 'M'
			case 78: // 'N'
			case 79: // 'O'
			case 80: // 'P'
			case 81: // 'Q'
			case 82: // 'R'
			case 84: // 'T'
			case 85: // 'U'
			case 86: // 'V'
			case 87: // 'W'
			case 88: // 'X'
			case 89: // 'Y'
			default:
				return null;
		}
	}

	protected Class<?> resolveClass(ObjectStreamClass objectstreamclass)
			throws IOException, ClassNotFoundException {
		String s = objectstreamclass.getName();
		if (s.startsWith("[")) {
			int i;
			for (i = 1; s.charAt(i) == '['; i++);
			Class<?> class1;
			if (s.charAt(i) == 'L') {
				class1 = loader.loadClass(s.substring(i + 1, s.length() - 1));
			}
			else {
				if (s.length() != i + 1)
					throw new ClassNotFoundException(s);
				class1 = primitiveType(s.charAt(i));
			}
			int ai[] = new int[i];
			for (int j = 0; j < i; j++)
				ai[j] = 0;

			return Array.newInstance(class1, ai).getClass();
		}
		else {
			return loader.loadClass(s);
		}
	}
}

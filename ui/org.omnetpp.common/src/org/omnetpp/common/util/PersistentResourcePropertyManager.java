package org.omnetpp.common.util;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;

import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;

public class PersistentResourcePropertyManager {
	private String pluginId;

	private ClassLoader classLoader;

	public PersistentResourcePropertyManager(String pluginId) {
		this(pluginId, null);
	}

	public PersistentResourcePropertyManager(String pluginId, ClassLoader classLoader) {
		this.pluginId = pluginId;
		this.classLoader = classLoader;
	}
	
	public boolean hasProperty(IResource resource, String key) {
		String fileName = getPropertyFileName(resource, key);
		File file = new File(fileName);
		
		return file.exists();
	}

	public Object getProperty(IResource resource, String key)
		throws FileNotFoundException, IOException, ClassNotFoundException
	{
		String fileName = getPropertyFileName(resource, key);
		FileInputStream fileStream = new FileInputStream(fileName);
		ObjectInputStream stream = classLoader == null ? new ObjectInputStream(fileStream) : new ObjectInputStreamWithClassLoader(fileStream, classLoader);
		Object object = stream.readObject();
		stream.close();
		
		return object;
	}

	public void setProperty(IResource resource, String key, Object value)
		throws FileNotFoundException, IOException
	{
		String fileName = getPropertyFileName(resource, key);
		ObjectOutputStream stream = new ObjectOutputStream(new FileOutputStream(fileName));
		stream.writeObject(value);
		stream.close();
	}

	public void removeProperty(IResource resource, String key) {
		String fileName = getPropertyFileName(resource, key);
		File file = new File(fileName);

		if (file.exists())
			file.delete();
	}
	
	private String getPropertyFileName(IResource resource, String key) {
		IProject project = resource.getProject();
		String fileNameKey = resource.getFullPath().toPortableString() + "?" + key;

		// TODO: this needs some thinking to be less fragile
		return project.getWorkingLocation(pluginId).append(String.valueOf(fileNameKey.hashCode())).toPortableString();
	}
}

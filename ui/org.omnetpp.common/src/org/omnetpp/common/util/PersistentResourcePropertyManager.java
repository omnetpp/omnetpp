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
		FileInputStream fileStream = null;
		ObjectInputStream stream = null;

		try {
    		fileStream = new FileInputStream(fileName);
    		stream = classLoader == null ? new ObjectInputStream(fileStream) : new ObjectInputStreamWithClassLoader(fileStream, classLoader);
    		Object object = stream.readObject();
            
            return object;
		}
		finally {
		    if (stream != null)
		        stream.close();

            if (fileStream != null)
                fileStream.close();
		}
	}

	public void setProperty(IResource resource, String key, Object value)
		throws FileNotFoundException, IOException
	{
		String fileName = getPropertyFileName(resource, key);
		ObjectOutputStream stream = null;

		try {
    		stream = new ObjectOutputStream(new FileOutputStream(fileName));
    		stream.writeObject(value);
		}
		finally {
		    if (stream != null)
		        stream.close();
		}
	}

	public void removeProperty(IResource resource, String key) {
		String fileName = getPropertyFileName(resource, key);
		File file = new File(fileName);

		if (file.exists() && !file.delete())
			throw new RuntimeException("Unable to delete file: " + fileName);
	}
	
	private String getPropertyFileName(IResource resource, String key) {
		IProject project = resource.getProject();
		String fileNameKey = resource.getFullPath().toPortableString() + "?" + key;

		// TODO: this needs some thinking to be less fragile
		return project.getWorkingLocation(pluginId).append(String.valueOf(fileNameKey.hashCode())).toPortableString();
	}
}

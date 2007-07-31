package org.omnetpp.common.util;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class FileUtils
{
	private FileUtils()
	{
	}
	
	/**
	 * Delete directory and it's contents recursively
	 * 
	 * @param dir
	 */
	public static void deleteDir(File dir)
	{
		if (dir.exists())
		{
			if (dir.isDirectory())
			{
				File[] subs = dir.listFiles();
				
				for (int i = 0; i < subs.length; i++)
				{
					File file = subs[i];
					
					deleteDir(file);
				}
			}

			dir.delete();
		}
	}
	
	/**
	 * Delete the contents of the directory recursively
	 * 
	 * @param dir
	 */
	public static void deleteDirContents(File dir)
	{
		if (dir.exists())
		{
			if (dir.isDirectory())
			{
				File[] subs = dir.listFiles();
				
				for (int i = 0; i < subs.length; i++)
				{
					File file = subs[i];
					
					deleteDir(file);
				}
			}
		}
	}
	
	private static byte[] createBuffer()
	{
		return new byte[4096];
	}
	
	public static void copyFile(File source, File target) throws IOException
	{
		copyFile(source, target, createBuffer());
	}
	
	public static void copyFile(File source, File target, byte[] buffer) throws IOException
	{
		FileInputStream in = new FileInputStream(source);
		
		copy(in, target, buffer);
	}
	
	public static void copy(InputStream in, File target) throws IOException
	{
		copy(in, target, createBuffer());
	}
	
	public static void copy(InputStream in, File target, byte[] buffer) throws IOException
	{
		try
		{
			FileOutputStream out = new FileOutputStream(target);
			try
			{
				int size;
				
				while ((size = in.read(buffer)) > 0)
				{
					out.write(buffer, 0, size);
				}
			}
			finally
			{
				out.close();
			}
		}
		finally
		{
			in.close();
		}
	}
	
	public static void copyFilesOf(File sourceDir, final FileFilter filter, File targetDir, final byte[] buffer) throws IOException
	{
		File[] files = (filter != null) ? sourceDir.listFiles(filter) : sourceDir.listFiles();
		
		for (int i = 0; i < files.length; i++)
		{
			if (targetDir.exists() == false)
				targetDir.mkdir();
			
			File source = files[i];
			File target = new File(targetDir, source.getName());

			if (source.isDirectory())
			{
				copyFilesOf(source, filter, new File(targetDir, source.getName()), buffer);
			}
			else
			{
				copyFile(source, target, buffer);
			}
		}
	}
	
	public static void copyFilesOf(File sourceDir, final FileFilter filter, File targetDir) throws IOException
	{
		copyFilesOf(sourceDir, filter, targetDir, new byte[4096]);
	}
	
	public static void copyFilesOf(File sourceDir, File targetDir) throws IOException
	{
		copyFilesOf(sourceDir, null, targetDir, new byte[4096]);
	}

    public static byte[] readBinaryFile(String fileName) throws IOException
    {
        FileInputStream file = new FileInputStream(fileName);
        DataInputStream in = new DataInputStream(file);
        byte[] b = new byte[in.available()];
        in.readFully(b);

        return b;
    }
	
    public static String readTextFile(String fileName) throws IOException
    {
        return readFile(fileName);
    }
	
    public static String readFile(String fileName) throws IOException
    {
        FileInputStream file = new FileInputStream(fileName);
        DataInputStream in = new DataInputStream(file);
        byte[] b = new byte[in.available()];
        in.readFully(b);
        in.close();
        return new String(b);
    }
}
package org.omnetpp.cdt.wizard.support;

import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.json.ExceptionErrorListener;
import org.omnetpp.common.json.JSONValidatingReader;

import au.com.bytecode.opencsv.CSVReader;

/**
 * Static utility methods for the FreeMarker templates, exposed to the user 
 * via BeansWrapper.
 * 
 * IMPORTANT: This class must be backward compatible across OMNeT++ versions
 * at all times. DO NOT REMOVE FUNCTIONS OR CHANGE THEIR SIGNATURES; add new methods 
 * instead, if needed.
 * 
 * @author Andras
 */
public class FileUtils {
   
    /**
     * Returns true if the given string is syntactically a valid workspace path.
     */
    public static boolean isValidWorkspacePath(String path) {
        return new Path(path).segmentCount() > 0;
    }

    /**
     * Returns true if the given string is syntactically a valid workspace file path.
     * This function does not check whether the file exists, or whether the given path
     * actually already points to a resource of a different type.
     */
    public static boolean isValidWorkspaceFilePath(String path) {
        return new Path(path).segmentCount() > 1;
    }

    /**
     * Returns the handle for the workspace project with the given name.
     * Throws exception if the path is not a valid workspace project path. 
     * This function does not test whether the project exists. To test that, 
     * call the exists() method on the returned handle.
     */
    public static IProject asProject(String path) {
        return ResourcesPlugin.getWorkspace().getRoot().getProject(path);
    }

    /**
     * Returns the handle for the workspace container (i.e. project or folder) with the given name.
     * Throws exception if the path is not a valid workspace container path. 
     * This function does not test whether the container exists. To test that, 
     * call the exists() method on the returned handle.
     */
    public static IContainer asContainer(String path) {
        if (new Path(path).segmentCount() <= 1)
            return ResourcesPlugin.getWorkspace().getRoot().getProject(path); // will fail on empty string -- this is intentional
        else
            return ResourcesPlugin.getWorkspace().getRoot().getFolder(new Path(path));
    }

    /**
     * Returns the handle for the workspace file with the given name.
     * Throws exception if the path is not a valid workspace file path. 
     * This function does not test whether the file exists. To test that, 
     * call the exists() method on the returned handle.
     */
    public static IFile asFile(String path) {
        return ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
    }

	/**
	 * Parse an XML file in the workspace, and return the Document object 
	 * of the resulting DOM tree.
	 * 
	 * @param fileName  the workspace file
	 * @return DOM tree
	 */
	public static org.w3c.dom.Document readXMLFile(String fileName) {
	    IFile file = asFile(fileName);
		return readExternalXMLFile(file.getLocation().toString());
	}

	/**
	 * Parse an XML file, given with its file system path (NOT workspace path),
     * and return the Document object of the resulting DOM tree.
	 * 
	 * @param file  filesystem path of the file
	 * @return DOM tree
	 */
	public static org.w3c.dom.Document readExternalXMLFile(String file) {
		InputStream is = null;
		try {
			is = new FileInputStream(file);
        	DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
			return docBuilder.parse(is);
		} 
		catch (Exception e) {
			throw new RuntimeException("Error XML reading file " + file + ": " + e.getMessage(), e);
		}
		finally {
			if (is != null)
				try { is.close(); } catch (IOException e) { }
		}
	}
	
	/**
	 * Parse a JSON file in the workspace. The result is a Boolean, Integer, 
	 * Double, String, List or Map, or any data structure composed of them. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param fileName  the workspace file
	 * @return JSON tree
	 */
	public static Object readJSONFile(String fileName) {
        IFile file = asFile(fileName);
		return readExternalJSONFile(file.getLocation().toString());
	}

	/**
	 * Parse a JSON file, given with its file system path (NOT workspace path).
	 * The result is a Boolean, Integer, Double, String, List or Map, or any
	 * data structure composed of them. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return JSON tree
	 */
	public static Object readExternalJSONFile(String file) {
		try {
			String contents = readExternalTextFile(file);
			JSONValidatingReader reader = new JSONValidatingReader(new ExceptionErrorListener());
			return reader.read(contents); 
		} catch (IllegalArgumentException e) {
			// thrown from ExceptionErrorListener on parse errors
			throw new RuntimeException("Error parsing JSON file " + file + ": " + e.getMessage(), e);
		}
	}
	
    /**
     * Read a CSV file in the workspace. The result is an array of lines, 
     * where each line is a string array. The file is interpreted in the 
     * Java platform's default encoding. Additional method parameters control
     * whether to discard the first line of the file (which is usually a header
     * line), whether to ignore blank lines, and whether to ignore comment lines
     * (those starting with the # character). Comment lines are not part of
     * the commonly accepted CSV format, but it is supported here nevertheless,
     * due of its usefulness.
	 * 
	 * @param fileName  the workspace file
	 * @param ignoreFirstLine  whether the first line is to be discarded 
	 * @param ignoreBlankLines  whether to ignore blank lines
	 * @param ignoreCommentLines  whether to ignore lines starting with '#'
	 * @return file contents
	 */
	public static String[][] readCSVFile(String fileName, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines) {
        IFile file = asFile(fileName);
		return readExternalCSVFile(file.getLocation().toString(), ignoreFirstLine, ignoreBlankLines, ignoreCommentLines);
	}

	/**
	 * Read a CSV file, given with its file system path (NOT workspace path).
     * The result is an array of lines, where each line is a string array. 
     * The file is interpreted in the Java platform's default encoding. 
     * Additional method parameters control whether to discard the first line
     * of the file (which is usually a header line), whether to ignore blank 
     * lines, and whether to ignore comment lines (those starting with 
     * the # character). Comment lines are not part of the commonly accepted
     * CSV format, but it is supported here nevertheless, due of its usefulness.
     * 
     * @param file  filesystem path of the file
     * @param ignoreFirstLine  whether the first line is to be discarded 
     * @param ignoreBlankLines  whether to ignore blank lines
     * @param ignoreCommentLines  whether to ignore lines starting with '#'
	 * @return file contents
	 */
	public static String[][] readExternalCSVFile(String file, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines) {
		try {
			CSVReader reader = new CSVReader(new FileReader(file));
			List<String[]> tokenizedLines = new ArrayList<String[]>();
			boolean isFirstLine = true;
			String [] nextLine;
			while ((nextLine = reader.readNext()) != null) {
			    if (ignoreFirstLine && isFirstLine) {
			        isFirstLine = false;
			        continue;
			    }
			    if (ignoreBlankLines && (nextLine.length==0 || (nextLine.length==1 && StringUtils.isBlank(nextLine[0]))))
			        continue;
                if (ignoreCommentLines && nextLine.length>0 && !nextLine[0].isEmpty() && nextLine[0].charAt(0)=='#')
                    continue;
				tokenizedLines.add(nextLine);
				isFirstLine = false;
			}
			return tokenizedLines.toArray(new String[][]{});
		} catch (IOException e) {
			throw new RuntimeException("Error reading file " + file + ": " + e.getMessage(), e);
		}
	}

	/**
	 * Parse a Java property file ('key=value' lines) in the workspace. 
     * The result is a Properties object, which is effectively a hash of 
     * key-value pairs.
     * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param fileName  the workspace file
	 * @return key-value pairs in a map
	 */
	public static Properties readPropertyFile(String fileName) {
        IFile file = asFile(fileName);
		return readExternalPropertyFile(file.getLocation().toString());
	}

	/**
	 * Parse a Java property file ('key=value' lines), given with its file 
	 * system path (NOT workspace path). The result is a Properties object, 
	 * which is effectively a hash of key-value pairs. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return key-value pairs in a map
	 */
	public static Properties readExternalPropertyFile(String file) {
		InputStream is = null;
		try {
			is = new FileInputStream(file);
			Properties properties = new Properties();
			properties.load(is);
			return properties;
		} 
		catch (Exception e) {
			throw new RuntimeException("Error reading file " + file + ": " + e.getMessage(), e);
		}
		finally {
			if (is != null)
				try { is.close(); } catch (IOException e) { }
		}
	}
	
	/**
	 * Read a text file in the workspace, and return its contents, split by 
	 * lines, and each line split by whitespace. Comment lines (those starting 
	 * with a hash mark, #) are discarded. The result is an array of lines, 
	 * where each line is a string array. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  the workspace file
	 * @return file contents
	 */
	public static String[][] readSpaceSeparatedTextFile(String fileName) {
        IFile file = asFile(fileName);
		return readExternalSpaceSeparatedTextFile(file.getLocation().toString());
	}

	/**
	 * Read a text file, given with its file system path (NOT workspace path),
	 * and return its contents, split by lines, and each line split by whitespace.
	 * Comment lines (those starting with a hash mark, #) are discarded. 
	 * The result is an array of lines, where each line is a string array. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return file contents
	 */
	public static String[][] readExternalSpaceSeparatedTextFile(String file) {
		String[] lines = readExternalLineOrientedTextFile(file);
		List<String[]> tokenizedLines = new ArrayList<String[]>();
		for (String line : lines)
			tokenizedLines.add(StringUtils.split(line));
		return tokenizedLines.toArray(new String[][]{});
	}

	/**
	 * Read a text file in the workspace, and return its lines. Comment lines
	 * (those starting with a hash mark, #) are discarded. The result is a string array.
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  the workspace file
	 * @return file contents
	 */
	public static String[] readLineOrientedTextFile(String fileName) {
        IFile file = asFile(fileName);
		return readExternalLineOrientedTextFile(file.getLocation().toString());
	}

	/**
	 * Read a text file, given with its file system path (NOT workspace path), 
	 * and return its lines. Comment lines (those starting with a hash mark, #) 
	 * are discarded. The result is a string array.
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return file contents
	 */
	public static String[] readExternalLineOrientedTextFile(String file) {
		String contents = readExternalTextFile(file);
		String[] lines = StringUtils.splitPreserveAllTokens(contents, "\r?\n");
		List<String> noncommentLines = new ArrayList<String>();
		for (String line : lines)
			if (!line.startsWith("#"))
				noncommentLines.add(line);
		return noncommentLines.toArray(new String[]{});
	}

	/**
	 * Reads a text file in the workspace, and returns its contents unchanged 
	 * as a single string. The file is interpreted in the Java platform's 
	 * default encoding.
	 * 
	 * @param file  the workspace file
	 * @return file contents
	 */
	public static String readTextFile(String fileName) {
        IFile file = asFile(fileName);
		return readExternalTextFile(file.getLocation().toString());
	}

	/**
	 * Read a text file, given with its file system path (NOT workspace path), 
	 * and returns its contents unchanged as a single string. The file is 
	 * interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return file contents
	 */
	public static String readExternalTextFile(String file) {
		try {
			return org.omnetpp.common.util.FileUtils.readTextFile(file);
		} catch (IOException e) {
			throw new RuntimeException("Error reading file " + file + ": " + e.getMessage(), e);
		}
	}
	
}

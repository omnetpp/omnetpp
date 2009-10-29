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
import org.eclipse.core.resources.IFile;
import org.omnetpp.common.json.ExceptionErrorListener;
import org.omnetpp.common.json.JSONValidatingReader;
import org.omnetpp.common.util.FileUtils;

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
public class WizardFileUtils {
	/**
	 * Parse an XML file in the workspace, and return the Document object 
	 * of the resulting DOM tree.
	 * 
	 * @param file  the workspace file
	 * @return DOM tree
	 */
	public static org.w3c.dom.Document readXMLFile(IFile file) {
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
	 * @param file  the workspace file
	 * @return JSON tree
	 */
	public static Object readJSONFile(IFile file) {
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
	 * where each line is a string array. The header line is not treated specially. 
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  the workspace file
	 * @return file contents
	 */
	public static String[][] readCSVFile(IFile file) {
		return readExternalCSVFile(file.getLocation().toString());
	}

	/**
	 * Read a CSV file, given with its file system path (NOT workspace path).
	 * The result is an array of lines, where each line is a string array.
	 * The header line is not treated specially.
	 * The file is interpreted in the Java platform's default encoding.
	 * 
	 * @param file  filesystem path of the file
	 * @return file contents
	 */
	public static String[][] readExternalCSVFile(String file) {
		try {
			CSVReader reader = new CSVReader(new FileReader(file));
			List<String[]> tokenizedLines = new ArrayList<String[]>();
			String [] nextLine;
			while ((nextLine = reader.readNext()) != null)
				tokenizedLines.add(nextLine);
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
	 * @param file  the workspace file
	 * @return key-value pairs in a map
	 */
	public static Properties readPropertyFile(IFile file) {
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
	public static String[][] readSpaceSeparatedTextFile(IFile file) {
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
	public static String[] readLineOrientedTextFile(IFile file) {
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
	public static String readTextFile(IFile file) {
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
			return FileUtils.readTextFile(file);
		} catch (IOException e) {
			throw new RuntimeException("Error reading file " + file + ": " + e.getMessage(), e);
		}
	}
	
}

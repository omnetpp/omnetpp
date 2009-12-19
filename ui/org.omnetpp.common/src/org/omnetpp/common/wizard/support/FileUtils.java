package org.omnetpp.common.wizard.support;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.apache.commons.lang.StringUtils;
import org.eclipse.core.resources.IContainer;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IFolder;
import org.eclipse.core.resources.IProject;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.resources.IWorkspaceRoot;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.core.runtime.Path;
import org.omnetpp.common.CommonPlugin;
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
     * Note: getLocation() on the resulting resource may return null!
     */
    public static IProject asProject(String path) {
        return ResourcesPlugin.getWorkspace().getRoot().getProject(path);
    }

    /**
     * Returns the handle for the workspace container (i.e. project or folder) with the given name.
     * Throws exception if the path is not a valid workspace container path.
     * This function does not test whether the container exists. To test that,
     * call the exists() method on the returned handle.
     * Note: getLocation() on the resulting container may return null!
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
     * Note: getLocation() on the resulting file may return null!
     */
    public static IFile asFile(String path) {
        IFile file = ResourcesPlugin.getWorkspace().getRoot().getFile(new Path(path));
        return file;
    }

    /**
     * Returns the handle for the workspace project, folder or file with
     * the given name. If the resource does not exist and the path contains
     * more than one segment (i.e. it cannot be a project), it is returned as
     * a file handle if it has a file extension, and as a folder if it
     * does not.
     * Note: getLocation() on the resulting resource may return null!
     */
    public static IResource asResource(String pathName) {
        Path path = new Path(pathName);
        IWorkspaceRoot root = ResourcesPlugin.getWorkspace().getRoot();
        if (path.segmentCount() == 1)
            return root.getProject(pathName);
        else if (root.getFile(path).exists())
            return root.getFile(path);
        else if (root.getFolder(path).exists())
            return root.getFolder(path);
        else if (path.getFileExtension() != null)
            return root.getFile(path);
        else
            return root.getFolder(path);
    }

    /**
     * Returns a java.io.File object for the given path. The object can be used to
     * access operations provided by the File API, such as exists(), length(), etc.
     *
     * @param path  the file path
     * @return  the File object
     */
    public static File asExternalFile(String path) {
        return new File(path);
    }

    /**
     * Returns the workspace root object. The workspace contains the user's
     * projects.
     */
    public static IWorkspaceRoot getWorkspaceRoot() {
        return ResourcesPlugin.getWorkspace().getRoot();
    }

    /**
     * Copies a workspace resource (file, folder or project) given with its path
     * to the destination path. For projects and folders, it copies recursively
     * (i.e. copies the whole folder tree). From the project root directory it
     * leaves out dot files, hidden files, and team private files.
     */
    public static void copy(String path, String destPath, IProgressMonitor monitor) throws CoreException {
        IResource resource = asResource(path);
        Path destPathPath = new Path(destPath);
        if (resource instanceof IProject && destPathPath.segmentCount()>1) {
            // the normal IResource.copy() insists that a project can only be copied
            // to a project (destPath "must have only one segment"), so here we do the
            // top level by hand. Also, we leave out dot files, hidden files etc.
            if (asContainer(destPath) instanceof IFolder && !asContainer(destPath).exists())
                makeFolder((IFolder)asContainer(destPath), monitor);
            for (IResource child : ((IProject)resource).members())
                if (!child.getName().startsWith(".") && !child.isHidden() && !child.isTeamPrivateMember())
                    child.copy(destPathPath.append(child.getName()), false, monitor);
        }
        else {
            // normal copy is fine
            resource.copy(destPathPath, false, monitor);
        }
    }

    /**
     * Copies the file at the given URL to the given destination workspace file.
     */
    public static void copyURL(String url, String destFilePath, IProgressMonitor monitor) throws CoreException {
        IFile destFile = asFile(destFilePath);
        IContainer destContainer = destFile.getParent();
        if (destContainer instanceof IFolder && !destContainer.exists())
            makeFolder((IFolder)destContainer, monitor);
        try {
            destFile.create(new URL(url).openStream(), false, monitor); // if file already exists, let this method throw a CoreException
        }
        catch (MalformedURLException e) {
            throw CommonPlugin.wrapIntoCoreException("Cannot copy file at URL \""+ url +"\": malformed URL", e);
        }
        catch (IOException e) {
            throw CommonPlugin.wrapIntoCoreException("Cannot copy file at URL \""+ url +"\"", e);
        }
    }

    private static void makeFolder(IFolder folder, IProgressMonitor monitor) throws CoreException {
        if (!folder.exists()) {
            if (folder.getParent() instanceof IFolder)
                makeFolder((IFolder)folder.getParent(), monitor);
            folder.create(false, true, monitor); // let it throw CoreException if not even the project exists
        }
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
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
        return readExternalXMLFile(file.getLocation().toString());
    }

    /**
     * Parse an XML file, given with its file system path (NOT workspace path),
     * and return the Document object of the resulting DOM tree.
     *
     * @param fileName  filesystem path of the file
     * @return DOM tree
     */
    public static org.w3c.dom.Document readExternalXMLFile(String fileName) {
        InputStream is = null;
        try {
            is = new FileInputStream(fileName);
            DocumentBuilder docBuilder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
            return docBuilder.parse(is);
        }
        catch (Exception e) {
            throw new RuntimeException("Error XML reading file " + fileName + ": " + e.getMessage(), e);
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
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
        return readExternalJSONFile(file.getLocation().toString());
    }

    /**
     * Parse a JSON file, given with its file system path (NOT workspace path).
     * The result is a Boolean, Integer, Double, String, List or Map, or any
     * data structure composed of them.
     * The file is interpreted in the Java platform's default encoding.
     *
     * @param fileName  filesystem path of the file
     * @return JSON tree
     */
    public static Object readExternalJSONFile(String fileName) {
        try {
            String contents = readExternalTextFile(fileName);
            JSONValidatingReader reader = new JSONValidatingReader(new ExceptionErrorListener());
            return reader.read(contents);
        } catch (IllegalArgumentException e) {
            // thrown from ExceptionErrorListener on parse errors
            throw new RuntimeException("Error parsing JSON file " + fileName + ": " + e.getMessage(), e);
        }
    }

    /**
     * Read a CSV file in the workspace. The result is an array of lines,
     * where each line is a string array. The file is interpreted in the
     * Java platform's default encoding. Additional method parameters control
     * whether to discard the first line of the file (which is usually a header
     * line), whether to ignore blank lines, and whether to ignore comment lines
     * (those starting with the # character). Comment lines are not part of
     * the commonly accepted CSV format, but they are supported here nevertheless,
     * due to their usefulness.
     *
     * @param fileName  the workspace file
     * @param ignoreFirstLine  whether the first line is to be discarded
     * @param ignoreBlankLines  whether to ignore blank lines
     * @param ignoreCommentLines  whether to ignore lines starting with '#'
     * @return file contents
     */
    public static String[][] readCSVFile(String fileName, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines) {
        IFile file = asFile(fileName);
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
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
     * CSV format, but they are supported here nevertheless, due to their usefulness.
     *
     * @param fileName  filesystem path of the file
     * @param ignoreFirstLine  whether the first line is to be discarded
     * @param ignoreBlankLines  whether to ignore blank lines
     * @param ignoreCommentLines  whether to ignore lines starting with '#'
     * @return file contents
     */
    public static String[][] readExternalCSVFile(String fileName, boolean ignoreFirstLine, boolean ignoreBlankLines, boolean ignoreCommentLines) {
        try {
            CSVReader reader = new CSVReader(new FileReader(fileName));
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
                if (ignoreCommentLines && nextLine.length>0 && nextLine[0].charAt(0)=='#')
                    continue;
                tokenizedLines.add(nextLine);
                isFirstLine = false;
            }
            return tokenizedLines.toArray(new String[][]{});
        } catch (IOException e) {
            throw new RuntimeException("Error reading file " + fileName + ": " + e.getMessage(), e);
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
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
        return readExternalPropertyFile(file.getLocation().toString());
    }

    /**
     * Parse a Java property file ('key=value' lines), given with its file
     * system path (NOT workspace path). The result is a Properties object,
     * which is effectively a hash of key-value pairs.
     * The file is interpreted in the Java platform's default encoding.
     *
     * @param fileName  filesystem path of the file
     * @return key-value pairs in a map
     */
    public static Properties readExternalPropertyFile(String fileName) {
        InputStream is = null;
        try {
            is = new FileInputStream(fileName);
            Properties properties = new Properties();
            properties.load(is);
            return properties;
        }
        catch (Exception e) {
            throw new RuntimeException("Error reading file " + fileName + ": " + e.getMessage(), e);
        }
        finally {
            if (is != null)
                try { is.close(); } catch (IOException e) { }
        }
    }

    /**
     * Read a text file in the workspace, and return its contents, split by
     * lines, and each line split by whitespace. Additional method parameters
     * control whether to ignore blank lines and/or comment lines (those
     * starting with the # character). The result is an array of lines,
     * where each line is a string array of the items on the line.
     * The file is interpreted in the Java platform's default encoding.
     *
     * @param file  the workspace file
     * @return file contents
     */
    public static String[][] readSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines) {
        IFile file = asFile(fileName);
        return readExternalSpaceSeparatedTextFile(file.getLocation().toString(), ignoreBlankLines, ignoreCommentLines);
    }

    /**
     * Read a text file, given with its file system path (NOT workspace path),
     * and return its contents, split by lines, and each line split
     * by whitespace. Additional method parameters control whether
     * to ignore blank lines and/or comment lines (those starting
     * with the # character). The result is an array of lines,
     * where each line is a string array of the items on the line.
     * The file is interpreted in the Java platform's default encoding.
     *
     * @param fileName  filesystem path of the file
     * @param ignoreBlankLines  whether to ignore blank lines
     * @param ignoreCommentLines  whether to ignore lines starting with '#'
     * @return file contents
     */
    public static String[][] readExternalSpaceSeparatedTextFile(String fileName, boolean ignoreBlankLines, boolean ignoreCommentLines) {
        String[] lines = readExternalLineOrientedTextFile(fileName);
        List<String[]> tokenizedLines = new ArrayList<String[]>();
        for (String line : lines) {
            if (ignoreBlankLines && StringUtils.isBlank(line))
                continue;
            if (ignoreCommentLines && line.charAt(0)=='#')
                continue;
            tokenizedLines.add(StringUtils.split(line));
        }
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
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
        return readExternalLineOrientedTextFile(file.getLocation().toString());
    }

    /**
     * Read a text file, given with its file system path (NOT workspace path),
     * and return its lines. Comment lines (those starting with a hash mark, #)
     * are discarded. The result is a string array.
     * The file is interpreted in the Java platform's default encoding.
     *
     * @param fileName  filesystem path of the file
     * @return file contents
     */
    public static String[] readExternalLineOrientedTextFile(String fileName) {
        String contents = readExternalTextFile(fileName);
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
        if (!file.exists())  // ensure file.getLocation()!=null
            throw new RuntimeException("File does not exist: " + fileName);
        return readExternalTextFile(file.getLocation().toString());
    }

    /**
     * Read a text file, given with its file system path (NOT workspace path),
     * and returns its contents unchanged as a single string. The file is
     * interpreted in the Java platform's default encoding.
     *
     * @param fileName  filesystem path of the file
     * @return file contents
     */
    public static String readExternalTextFile(String fileName) {
        try {
            return org.omnetpp.common.util.FileUtils.readTextFile(fileName);
        } catch (IOException e) {
            throw new RuntimeException("Error reading file " + fileName + ": " + e.getMessage(), e);
        }
    }

    /**
     * Writes the given string to a temporary file, and returns the path of the
     * temporary file in the file system. The file will be automatically deleted
     * when the IDE exits, but it can be also deleted earlier via deleteExternalFile().
     *
     * @param content  file content
     * @return  temp file name
     */
    public static String createTempFile(String content) {
        try {
            File tempFile = File.createTempFile("oppwiz", null);
            org.omnetpp.common.util.FileUtils.writeTextFile(tempFile, content);
            tempFile.deleteOnExit();
            return tempFile.getAbsolutePath();
        }
        catch (IOException e) {
            throw new RuntimeException("Error creating temporary file: " + e.getMessage(), e);
        }
    }

    /**
     * Creates a workspaces text file with the given contents, in the platform's default encoding.
     */
    public static void createFile(String fileName, String content) throws CoreException {
        IFile file = asFile(fileName);
        if (!file.exists())
            file.create(new ByteArrayInputStream(content.getBytes()), true, null);
        else
            file.setContents(new ByteArrayInputStream(content.getBytes()), true, true, null);
    }

    /**
     * Creates a text file in the file system with the given contents, in the platform's default encoding.
     */
    public static void createExternalFile(String fileName, String content) {
        try {
            File file = new File(fileName);
            org.omnetpp.common.util.FileUtils.writeTextFile(file, content);
        }
        catch (IOException e) {
            throw new RuntimeException("Error creating file " + fileName + ": " + e.getMessage(), e);
        }
    }

    /**
     * Deletes the given workspace file. It is OK to invoke it on a nonexistent file.
     */
    public static void deleteFile(String fileName) throws CoreException {
        IFile file = asFile(fileName);
        if (file.exists())
            file.delete(true, null);
    }

    /**
     * Deletes the given file from the file system. It is OK to invoke it on a nonexistent file.
     */
    public static void deleteExternalFile(String fileName) {
        File file = new File(fileName);
        if (!file.isFile())
            throw new RuntimeException("deleteExternalFile: " + fileName + " is not a file");
        if (file.exists() && !file.delete())
            throw new RuntimeException("deleteExternalFile: file " + fileName + " could not be deleted");
    }

    /**
     * Creates a workspace folder. The parent must exist.
     */
    public static void createDirectory(String fileName) throws CoreException {
        IContainer container = asContainer(fileName);
        if (!container.exists()) {
            if (container instanceof IFolder)
                ((IFolder)container).create(true, true, null);
            else
                throw new IllegalArgumentException("createDirectory: refusing to create a project: " + fileName);
        }
    }

    /**
     * Creates a directory in the file system. The parent must exist.
     */
    public static void createExternalDirectory(String fileName) {
        if (!new File(fileName).mkdir())
            throw new RuntimeException("createExternalDirectory: directory " + fileName + " could not be created");
    }

    /**
     * Deletes a workspace folder. The folder must be empty. It is OK to invoke
     * it on a nonexistent folder.
     */
    public static void removeDirectory(String fileName) throws CoreException {
        IContainer container = asContainer(fileName);
        if (container.exists()) {
            if (container instanceof IFolder)
                ((IFolder)container).delete(true, null);
            else
                throw new IllegalArgumentException("removeDirectory: refusing to delete a project: " + fileName);
        }
    }

    /**
     * Deletes a directory in the file system. The directory must be empty.
     * It is OK to invoke it on a nonexistent directory.
     */
    public static void removeExternalDirectory(String fileName) {
        File file = new File(fileName);
        if (!file.isDirectory())
            throw new RuntimeException("removeExternalDirectory:" + fileName + " is not a directory");
        if (file.exists() && !file.delete())
            throw new RuntimeException("removeExternalDirectory: directory " + fileName + " could not be deleted");
    }

}
